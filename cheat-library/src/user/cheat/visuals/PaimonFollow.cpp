#include "pch-il2cpp.h"
#include "PaimonFollow.h"
#include <helpers.h>
#include <cheat/events.h>


namespace cheat::feature
{
    namespace GameObject {
        app::GameObject* Paimon = nullptr;
        app::GameObject* ProfileLayer = nullptr;
    }

    PaimonFollow::PaimonFollow() : Feature(),
        NFEX(f_Enabled, u8"���ɸ���", "PaimonFollow", u8"ͼ����Ⱦ", false, false)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(PaimonFollow::OnGameUpdate);
    }

    const FeatureGUIInfo& PaimonFollow::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"���ɸ���", u8"ͼ����Ⱦ", true };
        return info;
    }

    void PaimonFollow::DrawMain()
    {
        ConfigWidget(f_Enabled, u8"Ҫ��ʾ���ɣ���򿪸ù��ܣ���ESC�˵�������ر�. \n" \
            u8"��������ڴ��ͺ���ʧ��, ��Ҫ���øù���, ���´�ESC�˵����ر�.");
    }

    bool PaimonFollow::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void PaimonFollow::DrawStatus()
    {
        ImGui::Text(u8"���ɸ���");
    }

    PaimonFollow& PaimonFollow::GetInstance()
    {
        static PaimonFollow instance;
        return instance;
    }

    void PaimonFollow::OnGameUpdate()
    {
        UPDATE_DELAY(100);

        if (f_Enabled)
        {
            GameObject::Paimon = app::GameObject_Find(string_to_il2cppi("/EntityRoot/OtherGadgetRoot/NPC_Guide_Paimon(Clone)"), nullptr);
            if (GameObject::Paimon == nullptr)
                return;
            
            GameObject::ProfileLayer = app::GameObject_Find(string_to_il2cppi("/Canvas/Pages/PlayerProfilePage"), nullptr);
            if (GameObject::ProfileLayer == nullptr)
                return;

            if (GameObject::Paimon->fields._.m_CachedPtr != nullptr && GameObject::ProfileLayer->fields._.m_CachedPtr != nullptr)
            {
                auto ProfileOpen = app::GameObject_get_active(GameObject::ProfileLayer, nullptr);

                if (ProfileOpen)
                    app::GameObject_set_active(GameObject::Paimon, false, nullptr);
                else
                    app::GameObject_set_active(GameObject::Paimon, true, nullptr);
            }
        }
        else
        {
            GameObject::Paimon == nullptr;
            GameObject::ProfileLayer == nullptr;
        }
    }
}