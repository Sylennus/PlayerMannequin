#include "Mannequin.h"

void updateMannequinBase()  // Update mannequin base (sex, race, weight, height, and skin color)
{
	RE::TESNPC* playerBase = RE::PlayerCharacter::GetSingleton()->GetActorBase();
	RE::TESNPC* mannequinBase = RE::TESForm::LookupByID<RE::TESNPC>(0x89A85);

	SetActorBaseDataFlag(mannequinBase, RE::ACTOR_BASE_DATA::Flag::kFemale, playerBase->GetSex());
	mannequinBase->race = playerBase->GetRace();
	mannequinBase->weight = playerBase->GetWeight();
	mannequinBase->height = playerBase->GetHeight();
	mannequinBase->bodyTintColor = playerBase->bodyTintColor;
}

void updateMannequinRef() // Check for and update nearby mannequin references
{
	SKSE::GetTaskInterface()->AddTask([&]()
	{
		auto p1 = RE::ProcessLists::GetSingleton();

		for (auto& handle : p1->highActorHandles)
		{
			auto actorPtr = handle.get();
			if (!actorPtr)
				continue;
			RE::Actor* actor = actorPtr.get();
			if (!actor)
				continue;
			if (!actor->Is3DLoaded())
				continue;
			if (actor->GetActorBase()->formID == 0x89A85)  //PlayerHouseMannequin record
			{
				actor->DoReset3D(true);
				// actor->Update3DModel(); 
				// actor->UpdateSkinColor();
			}
		}
	});

	// Other method
	//const auto cell = RE::PlayerCharacter::GetSingleton()->GetParentCell();
	//cell->ForEachReference([&](RE::TESObjectREFR& a_ref) {
	//	if (a_ref.Is(RE::FormType::NPC) && a_ref.GetBaseObject()->As<RE::TESNPC>()->GetRace()->formID == 0x10760A) {  // ManakinRace
	//		// do stuff
	//	}
	//})
}


void SetActorBaseDataFlag(RE::TESActorBaseData* actorBaseData, RE::ACTOR_BASE_DATA::Flag flag, bool enable)
{
	if (actorBaseData)
	{
		using func_t = decltype(&SetActorBaseDataFlag);
		REL::Relocation<func_t> func{ REL::RelocationID(14261, 14383) };
		return func(actorBaseData, flag, enable);
	}
}
