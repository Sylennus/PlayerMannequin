#pragma once

class MannequinInterface
{
public:

	MannequinInterface();

	bool updateMannequins;
	std::array<RE::TESNPC*, 2> mannequinBases;
	std::chrono::steady_clock::time_point* loadTime;

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
	void UpdateMannequinReferences();

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
