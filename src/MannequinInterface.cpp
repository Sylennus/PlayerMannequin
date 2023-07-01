#include "MannequinInterface.h"
#include "Events.h"
#include "SKEE.h"
#include "SKEEInterface.cpp"

#include <future>

MannequinInterface::MannequinInterface()
{
	updateMannequins = false;

	auto mannequinMsg = RE::TESForm::LookupByID<RE::BGSMessage>(0xD056B);	// "You can only place Armor on the Mannequin."
	mannequinMsg->flags.reset(RE::BGSMessage::MessageFlag::kMessageBox);	// Change MessageBox to Notification to avoid spam when other mods add objects to mannequins
	mannequinMsg->displayTime = 2;

	auto handler = RE::TESDataHandler::GetSingleton();

	mannequinBases[0] = RE::TESForm::LookupByID<RE::TESNPC>(0x89A85);                 // Vanilla Mannequin
	mannequinBases[1] = handler->LookupForm<RE::TESNPC>(0x15D5D, "HearthFires.esm");  // HearthFire Mannequin

	loadTime = std::chrono::steady_clock::now();
	currentTime = std::chrono::steady_clock::now();
	delay = std::chrono::seconds(0);

	iMap = SKEE::GetInterfaceMap();	// Check if SKEE is installed

	if (iMap) {

		auto bodyMorph = SKEE::GetBodyMorphInterface(iMap);

		if (bodyMorph) {
			auto version = bodyMorph->GetVersion();
			if (version >= 4) {
				iBodyMorph = bodyMorph;
			} else {
				logger::warn("SKEE BodyMorph Interface is too old. Version must be 4 or greater (v{} currently installed)", version);
			}
		} else {
			logger::warn("Couldn't get SKEE BodyMorph Interface");
		}

		auto niTransform = SKEE::GetNiTransformInterface(iMap);

		if (niTransform) {
			auto version = niTransform->GetVersion();
			if (version >= 3) {
				iNiTransform = niTransform;
			} else {
				logger::warn("SKEE NiTransform Interface is too old. Version must be 3 or greater (v{} currently installed)", version);
			}
		} else {
			logger::warn("Couldn't get SKEE NiTransform Interface");
		}

		auto overlay = SKEE::GetOverlayInterface(iMap);

		if (overlay)
			iOverlay = overlay;
		else
			logger::warn("Couldn't get SKEE Overlay Interface");

	} else {
		logger::warn("Couldn't SKEE Interface Map");
	}
}

void MannequinInterface::UpdateMannequins()
{
	if (!updateMannequins)
		return;

	currentTime = std::chrono::steady_clock::now();
	delay = std::chrono::duration_cast<std::chrono::seconds>(currentTime - loadTime);

	if (delay.count() < 2)	// Delay to let SKEE properly load overlays & colors
		return;

	updateMannequins = false;

	std::future<void> futureA = std::async(std::launch::deferred, &MannequinInterface::UpdateMannequinBases, this);
	std::future<void> futureB = std::async(std::launch::deferred, &MannequinInterface::RegisterPlayerMorphs, this);
	std::future<void> futureC = std::async(std::launch::deferred, &MannequinInterface::RegisterPlayerNodes, this);
	std::future<void> futureD = std::async(std::launch::deferred, &MannequinInterface::RegisterPlayerOverlays, this);
	futureA.get();
	futureB.get();
	futureC.get();
	futureD.get();
	FindMannequinReferences();
}

void MannequinInterface::UpdateMannequinBases()	// Update mannequin base (face, sex, race, weight, skin color, etc.)
{
	auto player = RE::PlayerCharacter::GetSingleton();
	auto playerBase = player->GetActorBase();
	auto playerSex = playerBase->GetSex();
	auto playerRace = playerBase->GetRace();
	auto playerHeight = playerBase->height;	// ?
	auto playerWeight = playerBase->GetWeight();
	RE::Color playerBodyTint = playerBase->bodyTintColor;

	for (auto& mannequinBase : mannequinBases) {
		
		SetActorBaseDataFlag(mannequinBase, RE::ACTOR_BASE_DATA::Flag::kFemale, playerSex);
		mannequinBase->RemoveChange(RE::TESNPC::ChangeFlags::kGender);	// To avoid baking sex into save
		mannequinBase->race = playerRace;
		mannequinBase->height = playerHeight;	// ?
		mannequinBase->weight = playerWeight;
		mannequinBase->bodyTintColor = playerBodyTint;
		mannequinBase->faceNPC = playerBase;

		for (int i = 0; i < playerBase->numHeadParts; i++) {
			if (playerBase->headParts && playerBase->headParts[i] && playerBase->headParts[i]->type == RE::BGSHeadPart::HeadPartType::kHair)
				mannequinBase->ChangeHeadPart(playerBase->headParts[i]);	// Replace hair HDPT to avoid hair clipping through face revealing helmets
		}
	}
}

void MannequinInterface::RegisterPlayerMorphs()
{
	if (!playerMorphs.empty())
		playerMorphs.clear();

	if (iBodyMorph) {
		auto        player = RE::PlayerCharacter::GetSingleton();
		NameVisitor visitor;
		KeyVisitor  keyVisitor;

		iBodyMorph->VisitMorphs(player, visitor);

		for (auto& name : visitor.morphList) {
			iBodyMorph->VisitKeys(player, name.c_str(), keyVisitor);

			for (auto& [key, value] : keyVisitor.morphKeyList) {
				auto morphValue = iBodyMorph->GetMorph(player, name.c_str(), key.c_str());
				if (abs(morphValue) > 0.001f)
					playerMorphs.push_back(Morph{ name, key, morphValue });
			}
		}
	}
}

void MannequinInterface::RegisterPlayerNodes()
{
	if (!playerNodes.empty())
		playerNodes.clear();

	if (iNiTransform) {
		auto player = RE::PlayerCharacter::GetSingleton();
		NodeScaleVisitor nodeVisitor;

		iNiTransform->VisitNodes(player, false, player->GetActorBase()->GetSex(), nodeVisitor);
	}
}

void MannequinInterface::RegisterPlayerOverlays()
{
	if (!playerOverlays.empty()) {
		playerOverlays.clear();
	}

	auto playerModel = RE::PlayerCharacter::GetSingleton()->Get3D(false);

	RE::BSVisit::TraverseScenegraphGeometries(playerModel, [&](RE::BSGeometry* geometry) -> RE::BSVisit::BSVisitControl {
		if (auto effect = geometry->GetGeometryRuntimeData().properties[RE::BSGeometry::States::State::kEffect].get()) {

			if (auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect)) {
				const auto material = static_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);

				if (material->materialAlpha > 0.0039f) {  // Exclude textures with alpha below 0.0039 (~1/256)
					auto diffuse = material->textureSet.get()->GetTexturePath(RE::BSTextureSet::Textures::kDiffuse);

					if (diffuse) {
						std::string dstring = diffuse;

						for (int i = 0; i < dstring.size(); i++)
							dstring[i] = std::tolower(dstring[i]);

						if (dstring.contains("overlays") && !dstring.contains("\\default.dds")) {  // Check if the material is an overlay. To improve

							std::string name = geometry->name.c_str();
							playerOverlays.push_back(Overlay{
								name,
								material 
							});
						}
					}
				}
			}
		}
		return RE::BSVisit::BSVisitControl::kContinue;
	});
}

void MannequinInterface::FindMannequinReferences()  // Update nearby mannequin references
{
	SKSE::GetTaskInterface()->AddTask([this]()
	{
		RE::NiPointer<RE::Actor> actorPtr;
		RE::Actor* actor = nullptr;

		auto p1 = RE::ProcessLists::GetSingleton();

		for (auto& handle : p1->highActorHandles) {
			
			actorPtr = handle.get(); 
			if (!actorPtr)
				continue;

			actor = actorPtr.get();
			if (!actor)
				continue;

			if (!actor->Is3DLoaded())
				continue;

			for (auto& mannequinBase : mannequinBases) {
				if (actor->GetActorBase()->formID == mannequinBase->formID) {

					std::jthread t(&MannequinInterface::UpdateMannequinReference, this, actor->formID);  // RaceMenu Overlays require the actor to be disabled then enabled, and a delay is necessary between the 2
					t.detach();
				}
			}
		}
	});
}

void MannequinInterface::UpdateMannequinReference(RE::FormID mannequinRefID)
{
	auto actor = RE::TESForm::LookupByID<RE::Actor>(mannequinRefID);

	if (!actor) {
		logger::error("Failed to access mannequin (RefID {:x})", mannequinRefID);
		return;
	}

	auto actorSex = actor->GetActorBase()->GetSex();

	if (iBodyMorph)
		iBodyMorph->ClearMorphs(actor);

	for (auto& [name, key, value] : playerMorphs)
		iBodyMorph->SetMorph(actor, name.c_str(), key.c_str(), value);

	for (auto& [name, key, scale, scaleMode] : playerNodes) {
		iNiTransform->RemoveNodeTransformScaleMode(actor, false, actorSex, name.c_str(), key.c_str());
		iNiTransform->RemoveNodeTransformScale(actor, false, actorSex, name.c_str(), key.c_str());
		iNiTransform->AddNodeTransformScaleMode(actor, false, actorSex, name.c_str(), key.c_str(), scaleMode);
		iNiTransform->AddNodeTransformScale(actor, false, actorSex, name.c_str(), key.c_str(), scale);
		iNiTransform->UpdateNodeTransforms(actor, false, actorSex, name.c_str());
	}

	if (iOverlay) {
		iOverlay->RemoveOverlays(actor);
		iOverlay->AddOverlays(actor);
	}

	while (!actor->Get3D(false))  // After enabling an actor their 3D model isn't accessible immediately (?). A hook would do the same job but would be more complex
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	SKSE::GetTaskInterface()->AddTask([this, actor] {
		actor->DoReset3D(true);  // Force references to update their model
		actor->UpdateSkinColor();
		UpdateMannequinOverlays(actor->formID);
	});
}

void MannequinInterface::UpdateMannequinOverlays(RE::FormID mannequinRefID)
{
	auto actor = RE::TESForm::LookupByID<RE::Actor>(mannequinRefID);

	if (!actor) {
		logger::error("Failed to access mannequin (RefID {:x})", mannequinRefID);
		return;
	}
	
	auto actorModel = actor->Get3D(false);

	if (!actorModel) {
		logger::error("Failed to access 3D model of mannequin (RefID {:x})", mannequinRefID);
		return;
	}
		
	for (auto& [sourceName, sourceMaterial] : playerOverlays) {

		if (auto object = actorModel->GetObjectByName(sourceName)) {

			if (auto geometry = object->AsGeometry()) {

				if (auto effect = geometry->GetGeometryRuntimeData().properties[RE::BSGeometry::States::State::kEffect].get()) {

					if (auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect)) {

						const auto material = static_cast<RE::BSLightingShaderMaterialBase*>(lightingShader->material);

						auto diffuse = material->textureSet.get()->GetTexturePath(RE::BSTextureSet::Textures::kDiffuse);

						if (diffuse) {

							std::string dstring = diffuse;

							if (dstring.contains("\\default.dds"))	// Replace default overlay by the one from the player
								material->CopyMembers(sourceMaterial);
						}
					}
				}
			}
		}
	}
}

void SetActorBaseDataFlag(RE::TESActorBaseData* actorBaseData, RE::ACTOR_BASE_DATA::Flag flag, bool enable)
{
	if (actorBaseData) {
		using func_t = decltype(&SetActorBaseDataFlag);
		REL::Relocation<func_t> func{ REL::RelocationID(14261, 14383) };
		return func(actorBaseData, flag, enable);
	}
}

//void RE::TESObjectREFR::Enable(bool a_resetInventory) // To remove once CLib-NG adds it
//{
//	using func_t = decltype(&RE::TESObjectREFR::Enable);
//	REL::Relocation<func_t> func{ REL::RelocationID(19373, 19800) };
//	return func(this, a_resetInventory);
//}

//void RE::InventoryChanges::RemoveAllItems(TESObjectREFR* a_ref, TESObjectREFR* a_moveToRef, bool a_arg4, bool a_keepOwnership, bool a_arg6)  // To remove once CLib-NG adds it
//{
//	using func_t = decltype(&RE::InventoryChanges::RemoveAllItems);
//	REL::Relocation<func_t> func{ REL::RelocationID(15878, 16118) };
//	return func(this, a_ref, a_moveToRef, a_arg4, a_keepOwnership, a_arg6);
//}
