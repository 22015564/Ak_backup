#include "pch-il2cpp.h"
#include "ChestTeleport.h"

#include <helpers.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/Chest.h>
#include <cheat/game/util.h>
#include <cheat/teleport/MapTeleport.h>

namespace cheat::feature 
{

    ChestTeleport::ChestTeleport() : ItemTeleportBase("ChestTeleport", u8"宝箱"),
        NF(f_FilterChestLocked    , u8"锁住",       "ChestTeleport", true),
		NF(f_FilterChestInRock    , u8"岩石",      "ChestTeleport", true),
		NF(f_FilterChestFrozen    , u8"冻结",       "ChestTeleport", true),
		NF(f_FilterChestBramble   , u8"荆棘",      "ChestTeleport", true),
		NF(f_FilterChestTrap      , u8"陷阱",         "ChestTeleport", true),

		NF(f_FilterChestCommon    , u8"普通",       "ChestTeleport", true),
		NF(f_FilterChestExquisite , u8"精致",    "ChestTeleport", true),
		NF(f_FilterChestPrecious  , u8"珍贵",     "ChestTeleport", true),
		NF(f_FilterChestLuxurious , u8"华丽",    "ChestTeleport", true),
		NF(f_FilterChestRemarkable, u8"奇馈",   "ChestTeleport", true),

		NF(f_FilterChest          , u8"箱子",       "ChestTeleport", true),
		NF(f_FilterInvestigates   , u8"调查", "ChestTeleport", false),
		NF(f_FilterBookPage       , u8"书页",   "ChestTeleport", false),
		NF(f_FilterBGM            , u8"BGMs",         "ChestTeleport", false),
		NF(f_FilterQuestInt       , u8"探索互动",  "ChestTeleport", false),
		NF(f_FilterFloraChest     , u8"植物箱",  "ChestTeleport", false),

		NF(f_FilterUnknown        , u8"未知",     "ChestTeleport", true)
	{ }


    void cheat::feature::ChestTeleport::DrawFilterOptions()
    {
		ConfigWidget(f_ShowInfo, u8"在信息窗口中显示最近箱子的简短信息.");

		if (ImGui::TreeNode(u8"过滤器"))
		{

			ImGui::Text(u8"种类过滤");

			ConfigWidget(f_FilterChest, u8"开启箱子检测和过滤.");
			ConfigWidget(f_FilterInvestigates);
			ConfigWidget(f_FilterBookPage);
			ConfigWidget(f_FilterBGM);
			ConfigWidget(f_FilterQuestInt);
			ConfigWidget(f_FilterFloraChest);
			ConfigWidget(f_FilterUnknown, u8"开启未知项目检测.\n您可以在下面看到这些项目（如果存在）.");

			ImGui::Spacing();

			if (!f_FilterChest)
				ImGui::BeginDisabled();

			if (ImGui::BeginTable(u8"箱子过滤", 2, ImGuiTableFlags_NoBordersInBody))
			{
				ImGui::TableNextColumn();
				ImGui::Text(u8"稀有度过滤");

				ImGui::TableNextColumn();
				ImGui::Text(u8"状态过滤");

				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestCommon);

				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestLocked);


				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestExquisite);

				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestInRock);


				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestPrecious);

				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestFrozen);


				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestLuxurious);

				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestBramble);


				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestRemarkable);

				ImGui::TableNextColumn();
				ConfigWidget(f_FilterChestTrap);

				ImGui::EndTable();
			}

			if (!f_FilterChest)
				ImGui::EndDisabled();

			ImGui::TreePop();
		}
    }

	const FeatureGUIInfo& ChestTeleport::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ u8"箱子传送", u8"传送", true };
		return info;
	}

    ChestTeleport& ChestTeleport::GetInstance()
	{
		static ChestTeleport instance;
		return instance;
	}

	bool ChestTeleport::IsValid(game::Entity* entity) const
	{
		if (!entity->isChest())
			return false;

		auto chest = reinterpret_cast<game::Chest*>(entity);
		auto filterResult = FilterChest(chest);
		return filterResult == ChestTeleport::FilterStatus::Valid ||
			(filterResult == ChestTeleport::FilterStatus::Unknown && f_FilterUnknown);
	}

	cheat::feature::ChestTeleport::FilterStatus ChestTeleport::FilterChest(game::Chest* entity) const
	{
		auto itemType = entity->itemType();
		switch (itemType)
		{
		case game::Chest::ItemType::Chest:
		{
			if (!f_FilterChest)
				return FilterStatus::Invalid;
			
			auto chestRarity = entity->chestRarity();
			if (chestRarity == game::Chest::ChestRarity::Unknown)
				return FilterStatus::Unknown;
			
			bool rarityValid = (chestRarity == game::Chest::ChestRarity::Common && f_FilterChestCommon) ||
				(chestRarity == game::Chest::ChestRarity::Exquisite  && f_FilterChestExquisite) ||
				(chestRarity == game::Chest::ChestRarity::Precious   && f_FilterChestPrecious) ||
				(chestRarity == game::Chest::ChestRarity::Luxurious  && f_FilterChestLuxurious) ||
				(chestRarity == game::Chest::ChestRarity::Remarkable && f_FilterChestRemarkable);

			if (!rarityValid)
				return FilterStatus::Invalid;

			auto chestState = entity->chestState();
			if (chestState == game::Chest::ChestState::Invalid)
				return FilterStatus::Invalid;

			bool chestStateValid = chestState == game::Chest::ChestState::None ||
				(chestState == game::Chest::ChestState::Locked  && f_FilterChestLocked) ||
				(chestState == game::Chest::ChestState::InRock  && f_FilterChestInRock) ||
				(chestState == game::Chest::ChestState::Frozen  && f_FilterChestFrozen) ||
				(chestState == game::Chest::ChestState::Bramble && f_FilterChestBramble) ||
				(chestState == game::Chest::ChestState::Trap    && f_FilterChestTrap);

			if (!chestStateValid)
				return FilterStatus::Invalid;

			return FilterStatus::Valid;
		}
		case game::Chest::ItemType::Investigate:
			return f_FilterInvestigates ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::BookPage:
			return f_FilterBookPage ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::BGM:
			return f_FilterBGM ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::QuestInteract:
			return f_FilterQuestInt ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::Flora:
			return f_FilterFloraChest ? FilterStatus::Valid : FilterStatus::Invalid;
		case game::Chest::ItemType::None:
		default:
			return FilterStatus::Unknown;
		}

		return FilterStatus::Unknown;
	}

	void ChestTeleport::DrawItems()
	{
		DrawUnknowns();
		DrawChests();
	}

	bool ChestTeleport::NeedInfoDraw() const
	{
		return true;
	}

	void ChestTeleport::DrawInfo()
	{
		auto entity = game::FindNearestEntity(*this);
		auto chest = reinterpret_cast<game::Chest*>(entity);

		DrawEntityInfo(entity);
		if (entity == nullptr)
			return;
		ImGui::SameLine();

		ImGui::TextColored(chest->chestColor(), "%s", chest->minName().c_str());
	}

	void ChestTeleport::DrawChests()
	{	
		if (!ImGui::TreeNode(u8"项目"))
			return;

		auto& manager = game::EntityManager::instance();
		auto entities = manager.entities(*this);

		ImGui::BeginTable("ChestsTable", 2);
		for (auto& entity : entities)
		{
			ImGui::PushID(entity);
			auto chest = reinterpret_cast<game::Chest*>(entity);

			ImGui::TableNextColumn();
			if (chest->itemType() == game::Chest::ItemType::Chest)
			{
				ImGui::TextColored(chest->chestColor(), "%s [%s] [%s] at %0.3fm", 
					magic_enum::enum_name(chest->itemType()).data(),
					magic_enum::enum_name(chest->chestRarity()).data(),
					magic_enum::enum_name(chest->chestState()).data(),
					manager.avatar()->distance(entity));
			}
			else
			{
				ImGui::TextColored(chest->chestColor(), "%s at %0.3fm", magic_enum::enum_name(chest->itemType()).data(), 
					manager.avatar()->distance(entity));
			}

			ImGui::TableNextColumn();

			if (ImGui::Button("Teleport"))
			{
				auto& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(chest->absolutePosition());
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}

	static bool ChestUnknownFilter(game::Entity* entity)
	{
		if (!entity->isChest())
			return false;

		auto chest = reinterpret_cast<game::Chest*>(entity);
		auto& chestTp = ChestTeleport::GetInstance();
		return chestTp.FilterChest(chest) == ChestTeleport::FilterStatus::Unknown;
	}

	void ChestTeleport::DrawUnknowns()
	{
		auto& manager = game::EntityManager::instance();
		auto unknowns = manager.entities(ChestUnknownFilter);
		if (unknowns.empty())
			return;

		ImGui::TextColored(ImColor(255, 165, 0, 255), u8"哇，你发现了一个未知的箱子。这意味着这种箱子类型还没有过滤器.");
		TextURL("请为GitHub上的问题做出贡献", "https://github.com/CallowBlack/genshin-cheat/issues/48", false, false);

		if (ImGui::Button(u8"复制到剪贴板"))
		{
			ImGui::LogToClipboard();

			ImGui::LogText(u8"未知名字:\n");

			for (auto& entity : unknowns)
				ImGui::LogText(u8"%s; 位置: %s; 场景: %u\n", entity->name().c_str(),
					il2cppi_to_string(entity->relativePosition()).c_str(), game::GetCurrentPlayerSceneID());

			ImGui::LogFinish();
		}

		if (!ImGui::TreeNode(u8"未知项目"))
			return;

		ImGui::BeginTable(u8"未知的", 2);

		for (auto& entity : unknowns)
		{
			ImGui::PushID(entity);

			ImGui::TableNextColumn();
			ImGui::Text("%s. Dist %0.3f", entity->name().c_str(), manager.avatar()->distance(entity));

			ImGui::TableNextColumn();
			if (ImGui::Button(u8"传送"))
			{
				auto& mapTeleport = MapTeleport::GetInstance();
				mapTeleport.TeleportTo(entity->absolutePosition());
			}

			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}



}

