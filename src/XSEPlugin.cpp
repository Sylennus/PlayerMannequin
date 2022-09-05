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
		case MessagingInterface::kPostLoad:
		{
			auto map = SKEE::GetInterfaceMap();
			if (!map) {
				log::critical("Couldn't get the SKEE InterfaceMap!");
				return;
			}

			auto transform = SKEE::GetNiTransformInterface(map);
			if (!transform) {
				log::critical("Couldn't get the SKEE NiTransform Interface!");
				return;
			}

			auto version = transform->GetVersion();
			log::info("Version: {}", version);
			if (version < 3) {
				log::critical("SKEE NiTransform Interface is too old. Version must be 3 or greater (v{} currently installed)", version);
				return;
			}
			break;
		}

		case MessagingInterface::kDataLoaded:
			InitializeHooks();
			break;

		case MessagingInterface::kPostLoadGame:
			auto currentTime = std::chrono::steady_clock::now();
			MannequinInterface::GetSingleton()->loadTime = &currentTime;
			MannequinInterface::GetSingleton()->updateMannequins = true;
			break;
		}
	})) {
		util::report_and_fail("Unable to register message listener.");
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
