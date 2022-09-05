#include "pch-il2cpp.h"
#include "About.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>

namespace cheat::feature 
{
    const FeatureGUIInfo& About::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ "", u8"关于", false };
        return info;
    }

    void About::DrawMain()
    {
        // Advertisement. To change/delete contact with Callow#1429
        ImGui::TextColored(ImColor(28, 202, 214, 255), u8"原作者Akebi的github地址:");
        TextURL("Github link", "https://github.com/Akebi-Group/Akebi-GC", true, false);

		ImGui::TextColored(ImColor(28, 202, 214, 255), u8"Akebi的discord链接:");
		TextURL("Discord invite link", "https://discord.com/invite/MmV8hNZB9S", true, false);

        ImGui::Text(u8"创始人:");
        ImGui::SameLine();
        ImGui::TextColored(ImColor(0, 102, 255, 255), "Callow");
        
		ImGui::Text(u8"主要开发人员和更新人员:");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(0, 102, 255, 255), "Taiga");

        ImGui::Text(u8"功能开发人员和Acai创始人:");
		ImGui::SameLine();
		ImGui::TextColored(ImColor(0, 102, 255, 255), "RyujinZX");

		ImGui::Text(u8"主要贡献者:");
		ImGui::TextColored(ImColor(0, 102, 255, 255), "WitchGod, m0nkrel, Shichiha, harlanx, andiabrudan, hellomykami");

		ImGui::Text(u8"完整贡献者名单:");
		TextURL("Github link", "https://github.com/Akebi-Group/Akebi-GC/graphs/contributors", true, false);

        ImGui::TextColored(ImColor(0, 102, 255, 255), u8"本汉化版本交流群:");
        TextURL(u8"点此加群811979687", "https://jq.qq.com/?_wv=1027&k=wHVLvIn3", true, false);
    }

    About& About::GetInstance()
    {
        static About instance;
        return instance;
    }
}