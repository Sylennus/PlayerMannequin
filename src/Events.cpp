#include "Events.h"
#include "Mannequin.h"

RE::BSEventNotifyControl MenuOpenCloseEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
{
	if (a_event) {
		if (a_event->menuName == "RaceSex Menu" && !a_event->opening)
		{
			updateMannequinBase();
			updateMannequinRef();
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}

MenuOpenCloseEventHandler* MenuOpenCloseEventHandler::GetSingleton()
{
	static MenuOpenCloseEventHandler singleton;
	return std::addressof(singleton);
}

void MenuOpenCloseEventHandler::Register()
{
	auto ui = RE::UI::GetSingleton();
	ui->AddEventSink(GetSingleton());
}
