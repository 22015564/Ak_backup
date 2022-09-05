#include "pch-il2cpp.h"
#include "AutoDestroy.h"

#include <helpers.h>
#include <algorithm>

#include <cheat/events.h>
#include <cheat/game/SimpleFilter.h>
#include <cheat/game/EntityManager.h>
#include <cheat/world/AutoChallenge.h>
#include <cheat/game/filters.h>

namespace cheat::feature
{
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method);

	AutoDestroy::AutoDestroy() : Feature(),
		NF(f_Enabled, u8"自动破坏", u8"自动破坏", false),
		NF(f_DestroyOres, u8"破坏矿物", u8"自动破坏", false),
		NF(f_DestroyShields, u8"破坏盾牌", u8"自动破坏", false),
		NF(f_DestroyDoodads, u8"破坏装饰物", u8"自动破坏", false),
		NF(f_DestroyPlants, u8"破坏植物", u8"自动破坏", false),
		NF(f_DestroySpecialObjects, u8"破坏特殊物品", u8"自动破坏", false),
		NF(f_DestroySpecialChests, u8"破坏特殊箱子", u8"自动破坏", false),
		NF(f_Range, u8"范围", u8"自动破坏", 10.0f)
	{
		HookManager::install(app::MoleMole_LCAbilityElement_ReduceModifierDurability, LCAbilityElement_ReduceModifierDurability_Hook);
	}

	const FeatureGUIInfo& AutoDestroy::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"自动破坏物品", u8"世界", true };
		return info;
	}

	void AutoDestroy::DrawMain()
	{
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"注意. 该功能并未进行全面测试.\n"
			u8"不建议用于主帐户或开启高数值使用.");

		ConfigWidget(u8"开启", f_Enabled, u8"立即摧毁射程内的物体.");
		ImGui::Indent();
		ConfigWidget(u8"矿物", f_DestroyOres, "矿石和变体，如电晶体等.");
		ConfigWidget(u8"盾牌", f_DestroyShields, u8"深渊法师/矮人/黏液护盾.");
		ConfigWidget(u8"装饰物", f_DestroyDoodads, u8"桶、箱子、花瓶等.");
		ConfigWidget(u8"植物", f_DestroyPlants, u8"蒲公英种子、樱花花等.");
		ConfigWidget(u8"特别物品", f_DestroySpecialObjects, u8"摧毁古老的雾凇、大小岩石堆");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"风险未知!");
		ConfigWidget(u8"特别箱子", f_DestroySpecialChests, u8"用荆棘、冰冻或岩石破坏箱子");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"风险未知!");
		ImGui::Unindent();
		ConfigWidget(u8"范围 (m)", f_Range, 0.1f, 1.0f, 15.0f);
	}

	bool AutoDestroy::NeedStatusDraw() const
	{
		return f_Enabled;
	}

	void AutoDestroy::DrawStatus()
	{
		ImGui::Text(u8"破坏 [%.01fm%s%s%s%s%s%s%s]",
			f_Range.value(),
			f_DestroyOres || f_DestroyShields || f_DestroyDoodads || f_DestroyPlants || f_DestroySpecialObjects || f_DestroySpecialChests ? "|" : "",
			f_DestroyOres ? u8"矿" : "",
			f_DestroyShields ? u8"盾" : "",
			f_DestroyDoodads ? u8"饰" : "",
			f_DestroyPlants ? u8"植" : "",
			f_DestroySpecialObjects ? u8"特殊" : "",
			f_DestroySpecialChests ? u8"箱" : "");
	}

	AutoDestroy& AutoDestroy::GetInstance()
	{
		static AutoDestroy instance;
		return instance;
	}

	// Thanks to @RyujinZX
	// Every ore has ability element component
	// Durability of ability element is a ore health
	// Every tick ability element check reducing durability, for ore in calm state `reduceDurability` equals 0, means HP don't change
	// We need just change this value to current durability or above to destroy ore
	// This function also can work with some types of shields (TODO: improve killaura with this function)
	static void LCAbilityElement_ReduceModifierDurability_Hook(app::LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, app::Nullable_1_Single_ deltaTime, MethodInfo* method)
	{
		auto& manager = game::EntityManager::instance();
		auto& autoDestroy = AutoDestroy::GetInstance();
		auto& autoChallenge = AutoChallenge::GetInstance();
		auto entity = __this->fields._._._entity;
		// call origin ReduceModifierDurability without correct modifierDurabilityIndex will coz game crash.
		// so use this hook function to destroy challenge's bombbarrel
		if (autoChallenge.f_Enabled && autoChallenge.f_BombDestroy &&
			autoChallenge.f_Range > manager.avatar()->distance(entity) &&
			game::filters::puzzle::Bombbarrel.IsValid(manager.entity(entity))
			)
		{
			reduceDurability = 1000.f;
		}
		if (autoDestroy.f_Enabled &&
			autoDestroy.f_Range > manager.avatar()->distance(entity) &&
			(
				(autoDestroy.f_DestroyOres && game::filters::combined::Ores.IsValid(manager.entity(entity))) ||
				(autoDestroy.f_DestroyDoodads && (game::filters::combined::Doodads.IsValid(manager.entity(entity)) || game::filters::chest::SBramble.IsValid(manager.entity(entity)))) ||
				(autoDestroy.f_DestroyShields && !game::filters::combined::MonsterBosses.IsValid(manager.entity(entity)) && (
					game::filters::combined::MonsterShielded.IsValid(manager.entity(entity)) ||											// For shields attached to monsters, e.g. abyss mage shields.
					game::filters::combined::MonsterEquips.IsValid(manager.entity(entity)))) ||											// For shields/weapons equipped by monsters, e.g. rock shield.
					(autoDestroy.f_DestroyPlants && game::filters::combined::PlantDestroy.IsValid(manager.entity(entity))) ||			// For plants e.g dandelion seeds.
				(autoDestroy.f_DestroySpecialObjects && game::filters::combined::BreakableObjects.IsValid(manager.entity(entity))) ||	// For Breakable Objects e.g Ancient Rime, Large and Small Rock Piles.
				(autoDestroy.f_DestroySpecialChests && game::filters::combined::Chests.IsValid(manager.entity(entity)))					// For Special Chests e.g Brambles, Frozen, Encased in Rock.
				)
			)
		{
			// This value always above any ore durability
			reduceDurability = 1000;
		}
		CALL_ORIGIN(LCAbilityElement_ReduceModifierDurability_Hook, __this, modifierDurabilityIndex, reduceDurability, deltaTime, method);
	}

}

