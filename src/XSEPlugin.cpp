#include "PCH.h"
#include "Plugin.h"
#include "MannequinInterface.h"
#include "Events.h"
#include "SKEE.h"
#include "SKEEInterface.cpp"

using namespace SKSE;
using namespace Plugin;

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
	auto path = logger::log_directory();
	if (!path)
		util::report_and_fail("Failed to find standard logging directory"sv);

	*path /= std::format("{}.log"sv, PluginName);

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	const auto level = spdlog::level::info;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
}

SKSEPluginInfo(
	.Version = PluginVersion,
	.Name = PluginName)

SKSEPluginLoad(const LoadInterface* skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {};
#endif

	InitializeLogging();

	logger::info("Skyrim v{}", REL::Module::get().version());
	logger::info("{} {} is loading", PluginName, PluginVersion);

	Init(skse);
	InitializeMessaging();

	logger::info("{} has finished loading", PluginName);
	return true;
}
