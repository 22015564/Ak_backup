#include "pch-il2cpp.h"
#include "ItemTeleportBase.h"

#include <helpers.h>
#include <cheat/teleport/MapTeleport.h>
#include <cheat-base/cheat/CheatManagerBase.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat/GenshinCM.h>

namespace cheat::feature
{
	ItemTeleportBase::ItemTeleportBase(const std::string& section, const std::string& name) : Feature(),
		NF(f_Key, u8"传送最近一个", section, Hotkey()),
		NF(f_ShowInfo, u8"显示信息", section, true),
		section(section), name(name)
	{
		f_Key.value().PressedEvent += MY_METHOD_HANDLER(ItemTeleportBase::OnTeleportKeyPressed);
	}

	void ItemTeleportBase::DrawMain()
	{
		auto desc = util::string_format(u8"按下按键时, 将传送到最近的 %s 如果存在.", name.c_str());
		ConfigWidget(f_Key, desc.c_str());

		DrawFilterOptions();

		DrawItems();
	}

	void ItemTeleportBase::DrawItems()
	{
		auto nodeName = util::string_format("%s list", name.c_str());
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			DrawEntities();
			ImGui::TreePop();
		}
	}

	bool ItemTeleportBase::NeedInfoDraw() const
	{
		return f_ShowInfo;
	}

	void ItemTeleportBase::DrawInfo()
	{
		DrawNearestEntityInfo();
	}

	void ItemTeleportBase::OnTeleportKeyPressed()
	{
		if (GenshinCM::IsMenuShowed())
			return;

		auto entity = game::FindNearestEntity(*this);
		if (entity != nullptr)
		{
			MapTeleport& mapTeleport = MapTeleport::GetInstance();
			mapTeleport.TeleportTo(entity->absolutePosition());
		}
	}

	void ItemTeleportBase::DrawEntityInfo(game::Entity* entity)
	{
		if (entity == nullptr)
		{
			ImGui::Text(name.c_str()); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.1f, 0.1f, 1.0f), u8"未找到");
			return;
		}

		auto& manager = game::EntityManager::instance();
		ImGui::Text(name.c_str()); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "%.3fm", manager.avatar()->distance(entity));
	}

	void ItemTeleportBase::DrawNearestEntityInfo()
	{
		auto nearestEntity = game::FindNearestEntity(*this);
		DrawEntityInfo(nearestEntity);
	}

	void ItemTeleportBase::DrawEntities()
	{
		auto& manager = game::EntityManager::instance();
		auto entities = manager.entities(*this);
		if (entities.size() == 0)
		{
			ImGui::Text(u8"未找到.");
			return;
		}

		for (const auto& entity : entities)
		{
			ImGui::Text("Dist %.03fm", manager.avatar()->distance(entity));
			ImGui::SameLine();
			auto label = util::string_format("Teleport ## %p", entity);
			if (ImGui::Button(label.c_str()))
			{
				MapTeleport& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(entity->absolutePosition());
			}
		}
	}



}

