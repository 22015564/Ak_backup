#include "pch-il2cpp.h"
#include "DumbEnemies.h"

#include <helpers.h>

namespace cheat::feature
{
    static void VCMonsterAIController_TryDoSkill_Hook(void* __this, uint32_t skillID, MethodInfo* method);

    DumbEnemies::DumbEnemies() : Feature(),
        NF(f_Enabled, u8"无视敌人", u8"无视敌人", false)
    {
        HookManager::install(app::MoleMole_VCMonsterAIController_TryDoSkill, VCMonsterAIController_TryDoSkill_Hook);
    }

    const FeatureGUIInfo& DumbEnemies::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", u8"世界", false };
        return info;
    }

    void DumbEnemies::DrawMain()
    {
        ConfigWidget(f_Enabled, u8"敌人不会攻击或使用异能攻击玩家. \n"
            u8"可能无法对某些敌人或敌人技能起作用.");
    }

    bool DumbEnemies::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void DumbEnemies::DrawStatus()
    {
        ImGui::Text(u8"无视敌人");
    }

    DumbEnemies& DumbEnemies::GetInstance()
    {
        static DumbEnemies instance;
        return instance;
    }

    // Raised when monster trying to do skill. Attack also is skill.
    // We just block if dumb mob enabled, so mob will not attack player.
    static void VCMonsterAIController_TryDoSkill_Hook(void* __this, uint32_t skillID, MethodInfo* method)
    {
        DumbEnemies& dumbEnemies = DumbEnemies::GetInstance();
        if (dumbEnemies.f_Enabled)
            return;
        CALL_ORIGIN(VCMonsterAIController_TryDoSkill_Hook, __this, skillID, method);
    }
}

