#include "PCH.h"
#include "MannequinInterface.h"
#include "Events.h"
#include "SKEE.h"

using namespace SKSE;

void InitializeHooks()
{
	MannequinInterface::InstallHooks();
	MenuOpenCloseEventHandler::Register();
}

void InitializeMessaging()
{
	if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message* message)	{
		switch (message->type) {
		case MessagingInterface::kDataLoaded:
			InitializeHooks();
			break;
		}
	})) {
		util::report_and_fail("Unable to register message listener");
	}
}

void InitializeLogging()
{
#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::info;
#endif

	auto path = log::log_directory();
	if (!path)
		util::report_and_fail("Unable to lookup SKSE logs directory.");

	*path /= PluginDeclaration::GetSingleton()->GetName();
	*path += L".log";

	std::shared_ptr<spdlog::logger> log;

	if (IsDebuggerPresent()) {
		log = std::make_shared<spdlog::logger>(
			"Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
	} else {
		log = std::make_shared<spdlog::logger>(
			"Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
	}

	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
}

SKSEPluginLoad(const LoadInterface* skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {};
#endif

	InitializeLogging();

	auto* plugin = PluginDeclaration::GetSingleton();
	auto  version = plugin->GetVersion();

	log::info("{} {} is loading...", plugin->GetName(), version);

	Init(skse);
	InitializeMessaging();

	log::info("{} has finished loading.", plugin->GetName());
	return true;
}
