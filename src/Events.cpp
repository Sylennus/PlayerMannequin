#include "Events.h"
#include "MannequinInterface.h"

MenuOpenCloseEventHandler* MenuOpenCloseEventHandler::GetSingleton()
{
	static MenuOpenCloseEventHandler singleton;
	return &singleton;
}

void MenuOpenCloseEventHandler::Register()
{
	RE::UI::GetSingleton()->AddEventSink(GetSingleton());
}

RE::BSEventNotifyControl MenuOpenCloseEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
{
	if (a_event) {
		if (a_event->menuName == "RaceSex Menu" && !a_event->opening) {
			MannequinInterface::GetSingleton()->updateMannequins = true;
		}
	}
	return RE::BSEventNotifyControl::kContinue;
}
