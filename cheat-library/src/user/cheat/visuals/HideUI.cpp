#include "pch-il2cpp.h"
#include "HideUI.h"

#include <helpers.h>
#include <cheat/events.h>

namespace cheat::feature
{
    app::GameObject* ui_camera{};

    HideUI::HideUI() : Feature(),
		NFEX(f_Enabled, u8"隐藏 UI", "HideUI", u8"图像渲染", false, false)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(HideUI::OnGameUpdate);
    }

    const FeatureGUIInfo& HideUI::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"隐藏UI", u8"图像渲染", false };
        return info;
    }

    void HideUI::DrawMain()
    {
        ConfigWidget(f_Enabled, u8"隐藏游戏 UI.\n切换到另一场景（例如，进入副本）时，关闭并重新启用此功能");
    }

    bool HideUI::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void HideUI::DrawStatus()
    {
        ImGui::Text(u8"隐藏UI");
    }

    HideUI& HideUI::GetInstance()
    {
        static HideUI instance;
        return instance;
    }

    void HideUI::OnGameUpdate()
    {
        if (f_Enabled)
        {
            if (ui_camera == nullptr)
                ui_camera = app::GameObject_Find(string_to_il2cppi("/UICamera"), nullptr);
  
            if (ui_camera->fields._.m_CachedPtr != nullptr)
                app::GameObject_SetActive(ui_camera, false, nullptr);
        }
        else
        {
            if (ui_camera)
            {
                if (ui_camera->fields._.m_CachedPtr != nullptr)
                    app::GameObject_SetActive(ui_camera, true, nullptr);

                ui_camera = nullptr;
            }
        }
    }
}