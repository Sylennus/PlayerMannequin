#include "MannequinInterface.h"

MannequinInterface::MannequinInterface()
{
	auto handler = RE::TESDataHandler::GetSingleton();

	mannequinBases[0] = RE::TESForm::LookupByID<RE::TESNPC>(0x89A85);                 // Vanilla Mannequin
	mannequinBases[1] = handler->LookupForm<RE::TESNPC>(0x15D5D, "HearthFires.esm");  // HearthFire Mannequin

	updateMannequins = false;
}

void MannequinInterface::UpdateMannequins()
{
	if (!loadTime)
		return;

	auto currentTime = std::chrono::steady_clock::now();
	auto delay = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - *loadTime).count();

	if (updateMannequins) {
		if (delay >= 3000) {
			UpdateMannequinBases();
			UpdateMannequinReferences();
			updateMannequins = false;
		}
	}
}

void MannequinInterface::UpdateMannequinBases()  // Update mannequin base (face, sex, race, weight, height, and skin color)
{

	RE::TESNPC* playerBase = RE::PlayerCharacter::GetSingleton()->GetActorBase();

	for (int i = 0; i < mannequinBases.size(); i++) {
		mannequinBases[i]->faceNPC = playerBase;
		SetActorBaseDataFlag(mannequinBases[i], RE::ACTOR_BASE_DATA::Flag::kFemale, playerBase->GetSex());
		mannequinBases[i]->race = playerBase->GetRace();
		mannequinBases[i]->weight = playerBase->GetWeight();
		mannequinBases[i]->height = playerBase->GetHeight();  // To improve, doesn't work with RaceMenu Height
		mannequinBases[i]->bodyTintColor = playerBase->bodyTintColor;
		mannequinBases[i]->RemoveChange(RE::TESNPC::ChangeFlags::kGender);
	}
}

void MannequinInterface::UpdateMannequinReferences()  // Check for and update nearby mannequin references
{
	SKSE::GetTaskInterface()->AddTask([&]()
	{
		auto p1 = RE::ProcessLists::GetSingleton();

		RE::NiPointer<RE::Actor> actorPtr;
		RE::Actor* actor = nullptr;

		for (auto& handle : p1->highActorHandles) {
			actorPtr = handle.get();
			if (!actorPtr)
				continue;
			actor = actorPtr.get();
			if (!actor)
				continue;
			if (!actor->Is3DLoaded())
				continue;
			for (int i = 0; i < mannequinBases.size(); i++) {
				if (actor->GetActorBase()->formID == mannequinBases[i]->formID) {
					actor->DoReset3D(true);
					actor->UpdateSkinColor();
				}
			}
		}
	});
}

void SetActorBaseDataFlag(RE::TESActorBaseData* actorBaseData, RE::ACTOR_BASE_DATA::Flag flag, bool enable)
{
	if (actorBaseData) {
		using func_t = decltype(&SetActorBaseDataFlag);
		REL::Relocation<func_t> func{ REL::RelocationID(14261, 14383) };
		return func(actorBaseData, flag, enable);
	}
}
