#pragma once

#include "SKEE.h"

class MannequinInterface
{
public:
	MannequinInterface();

	struct Morph
	{
		std::string name;
		std::string key;
		float       value;
	};

	struct Node
	{
		std::string node;
		std::string key;
		float       scale;
		uint32_t    scaleMode = 0;
	};

	struct Overlay
	{
		std::string                       nodeName;
		RE::BSLightingShaderMaterialBase* material;
	};

	bool                                  updateMannequins;
	std::array<RE::TESNPC*, 2>            mannequinBases;
	std::chrono::steady_clock::time_point loadTime;
	std::chrono::steady_clock::time_point currentTime;
	std::chrono::seconds                  delay;
	std::vector<Morph>                    playerMorphs;
	std::vector<Node>                     playerNodes;
	std::vector<Overlay>                  playerOverlays;
	SKEE::IInterfaceMap*                  iMap = nullptr;
	SKEE::IBodyMorphInterface*            iBodyMorph = nullptr;
	SKEE::INiTransformInterface*          iNiTransform = nullptr;
	SKEE::IOverlayInterface*              iOverlay = nullptr;

	static void InstallHooks()
	{
		Hooks::Install();
	}

	static MannequinInterface* GetSingleton()
	{
		static MannequinInterface singleton;
		return &singleton;
	}

	void UpdateMannequins();
	void UpdateMannequinBases();
	void RegisterPlayerMorphs();
	void RegisterPlayerNodes();
	void RegisterPlayerOverlays();
	void FindMannequinReferences();
	void UpdateMannequinReference(RE::FormID mannequinRefID);
	void UpdateMannequinOverlays(RE::FormID mannequinRefID);

protected:
	struct Hooks
	{
		struct MainUpdate_Nullsub
		{
			static void thunk()
			{
				func();
				GetSingleton()->UpdateMannequins();
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct Character_SaveGame
		{
			static void thunk(RE::Character* a_this, RE::BGSSaveFormBuffer* a_buf)
			{
				if (a_this)	{
					if (a_this->GetActorBase())	{
						for (int i = 0; i < GetSingleton()->mannequinBases.size(); i++) {
							if (a_this->GetActorBase()->formID != GetSingleton()->mannequinBases[i]->formID)
								continue;

							a_this->GetActorBase()->weight = 50.0;
							func(a_this, a_buf);
							a_this->GetActorBase()->weight = RE::PlayerCharacter::GetSingleton()->GetActorBase()->GetWeight();

							return;
						}
					}
				}
				func(a_this, a_buf);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			stl::write_thunk_call<MainUpdate_Nullsub>(REL::RelocationID(35565, 36564).address() + REL::Relocate(0x748, 0xC26));
			stl::write_vfunc<RE::Character, 0x00E, Character_SaveGame>();
		}
	};
};

void SetActorBaseDataFlag(RE::TESActorBaseData* actorBaseData, RE::ACTOR_BASE_DATA::Flag flag, bool enable);
