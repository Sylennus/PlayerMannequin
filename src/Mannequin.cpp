#include "Mannequin.h"

void updateMannequinBase()  // change sex, race, weight, height, and skin color
{
	RE::TESNPC* playerBase = RE::PlayerCharacter::GetSingleton()->GetActorBase();
	RE::TESNPC* mannequinBase = RE::TESForm::LookupByID<RE::TESNPC>(0x89A85);

	SetActorBaseDataFlag(mannequinBase, RE::ACTOR_BASE_DATA::Flag::kFemale, playerBase->GetSex());
	mannequinBase->race = playerBase->GetRace();
	mannequinBase->weight = playerBase->GetWeight();
	mannequinBase->height = playerBase->GetHeight();
	mannequinBase->bodyTintColor = playerBase->bodyTintColor;

	//Actor::DoReset3D(bool a_updateWeight) ?
	//Actor::UpdateSkinColor() ?
	//TESNPC::UpdateNeck(BSFaceGenNiNode* a_faceNode)
}

void updateMannequinRef()
{
	auto p1 = RE::ProcessLists::GetSingleton();

	SKSE::GetTaskInterface()->AddTask([&]() {
		for (auto& handle : p1->highActorHandles) {
			RE::Actor* actor = handle.get().get();
			if (!actor)
				continue;
			if (!actor->GetRace())
				continue;
			if (actor->GetRace()->formID == 0x10760A)  //Manakin Race (to improve)
			{
				actor->DoReset3D(true);
			}
		}
	});
}


void SetActorBaseDataFlag(RE::TESActorBaseData* actorBaseData, RE::ACTOR_BASE_DATA::Flag flag, bool enable)
{
	if (actorBaseData) {
		using func_t = decltype(&SetActorBaseDataFlag);
		REL::Relocation<func_t> func{ REL::ID(14261) };
		return func(actorBaseData, flag, enable);
	}
}
