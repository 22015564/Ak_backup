#include "pch-il2cpp.h"
#include "GameSpeed.h"

#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature
{
    GameSpeed::GameSpeed() : Feature(),
        NF(f_Enabled, u8"游戏速度启用", u8"游戏速度", false),
        NF(f_Hotkey, u8"游戏速度快捷键", u8"游戏速度", Hotkey(VK_CAPITAL)),
        NF(f_Speed, u8"游戏速度乘数", u8"游戏速度", 5.0f)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(GameSpeed::OnGameUpdate);
    }

    const FeatureGUIInfo& GameSpeed::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"游戏速度", u8"世界", true };
        return info;
    }

    void GameSpeed::DrawMain()
    {
        ConfigWidget(u8"游戏速度启用", f_Enabled, u8"使用快捷键加速游戏");
        if (f_Enabled)
        {
            ConfigWidget(u8"游戏速度快捷键", f_Hotkey);
            ConfigWidget(f_Speed, 1.0f, 0.0f, 20.0f, u8"设置游戏速度乘数\n" \
                u8"不要在开放世界中使用，仅在菜单/等中使用, 极度危险！！！");
        }
    }

    bool GameSpeed::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void GameSpeed::DrawStatus()
    {
        ImGui::Text(u8"游戏加速");
    }

    GameSpeed& GameSpeed::GetInstance()
    {
        static GameSpeed instance;
        return instance;
    }

    void GameSpeed::OnGameUpdate()
    {
        static bool isSpeed = false;
        float currentSpeed = app::Time_get_timeScale(nullptr);

        if (f_Enabled)
        {
            if (f_Hotkey.value().IsPressed() && !isSpeed)
            {
                if (currentSpeed == 1.0f)
                {
                    app::Time_set_timeScale(f_Speed, nullptr);
                    isSpeed = true;
                }
            }

            if (!f_Hotkey.value().IsPressed() && isSpeed)
            {
                if (currentSpeed != 1.0f)
                {
                    app::Time_set_timeScale(1.0f, nullptr);
                    isSpeed = false;
                }
            }
        }
        else
        {
            // Aditional check if user is still pressing key and they decide to disable the feature
            if (isSpeed)
            {
                if (currentSpeed != 1.0f)
                {
                    app::Time_set_timeScale(1.0f, nullptr);
                    isSpeed = false;
                }
            }
        }
    }
}
