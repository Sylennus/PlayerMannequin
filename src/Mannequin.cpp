#include "Mannequin.h"

void UpdateMannequinBases()  // Update mannequin base (face, sex, race, weight, height, and skin color)
{
	auto handler = RE::TESDataHandler::GetSingleton();
	
	RE::TESNPC* playerBase = RE::PlayerCharacter::GetSingleton()->GetActorBase();
	std::array<RE::TESNPC*, 2> mannequinBases;
	mannequinBases[0] = RE::TESForm::LookupByID<RE::TESNPC>(0x89A85); // Vanilla Mannequin
	mannequinBases[1] = handler->LookupForm<RE::TESNPC>(0x15D5D, "HearthFires.esm");  // HearthFire Mannequin

	for (int i = 0; i < mannequinBases.size(); i++)
	{
		mannequinBases[i]->faceNPC = playerBase;
		SetActorBaseDataFlag(mannequinBases[i], RE::ACTOR_BASE_DATA::Flag::kFemale, playerBase->GetSex());
		mannequinBases[i]->race = playerBase->GetRace();
		mannequinBases[i]->weight = playerBase->GetWeight();
		mannequinBases[i]->height = playerBase->GetHeight();  // To improve, doesn't work with RaceMenu Height
		mannequinBases[i]->bodyTintColor = playerBase->bodyTintColor;
	}
}

void UpdateMannequinReferences() // Check for and update nearby mannequin references
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
			if (actor->GetActorBase()->formID == 0x89A85)  //PlayerHouseMannequin Base ID
			{
				actor->DoReset3D(true);
				actor->UpdateSkinColor();
			}
		}
	});
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
