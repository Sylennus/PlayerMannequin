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

	//TESNPC::UpdateNeck(BSFaceGenNiNode* a_faceNode)
}

int updateMannequinRef()
{
	int ret = 0;

	SKSE::GetTaskInterface()->AddTask([&]() {

		auto p1 = RE::ProcessLists::GetSingleton();

		for (auto& handle : p1->highActorHandles) {
			auto actorPtr = handle.get();
			if (!actorPtr)
				continue;
			RE::Actor* actor = actorPtr.get();
			if (!actor)
				continue;
			if (!actor->Is3DLoaded())
				continue;
			if (!actor->GetRace())
				continue;
			if (actor->GetRace()->formID == 0x10760A)  //Manakin Race (to improve)
			{
				// actor->DoReset3D(true); // doesn't update
				actor->Update3DModel();
				actor->UpdateSkinColor();
				ret++;
			}
		}
	});

	//const auto cell = RE::PlayerCharacter::GetSingleton()->GetParentCell();
	//cell->ForEachReference([&](RE::TESObjectREFR& a_ref) {
	//	if (a_ref.Is(RE::FormType::NPC) && a_ref.GetBaseObject()->As<RE::TESNPC>()->GetRace()->formID == 0x10760A) {  // ManakinRace
	//		// do your thing
	//	}
	//})

	return ret;
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
