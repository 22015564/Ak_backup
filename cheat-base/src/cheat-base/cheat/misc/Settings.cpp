#include <pch.h>
#include "Settings.h"

#include <cheat-base/cheat/CheatManagerBase.h>
#include <cheat-base/render/renderer.h>
#include <cheat-base/render/gui-util.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cheat-base/util.h>

#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

namespace cheat::feature
{
	Settings::Settings() : Feature(),
		NF(f_MenuKey, u8"��ʾ�˵��İ���", u8"�ܿ���", Hotkey(VK_F12)),
		NF(f_HotkeysEnabled, u8"������ݼ�", u8"�ܿ���", true),
		
		NF(f_StatusMove, u8"�ƶ�״̬����", u8"�ܿ���::״̬����", true),
		NF(f_StatusShow, u8"��ʾ״̬����", u8"�ܿ���::״̬����", true),

		NF(f_InfoMove, u8"�ƶ���Ϣ����", u8"�ܿ���::��Ϣ����", true),
		NF(f_InfoShow, u8"��ʾ��Ϣ����", u8"�ܿ���::��Ϣ����", true),

		NF(f_FpsMove, u8"�ƶ�֡�ʴ���", u8"�ܿ���::FPS", false),
		NF(f_FpsShow, u8"��ʾ֡�ʴ���", u8"�ܿ���::FPS", true),

		NF(f_NotificationsShow, u8"��ʾ֪ͨ", u8"�ܿ���::֪ͨ", true),
		NF(f_NotificationsDelay, u8"֪ͨ�ӳ�", u8"�ܿ���::֪ͨ", 500),

		NF(f_FileLogging, u8"������־", u8"�ܿ���::��־", false),
		NF(f_ConsoleLogging, u8"����̨���", u8"�ܿ���::��־", true),

		NF(f_FastExitEnable, u8"���ٹر���Ϸ", u8"�ܿ���::�����˳�", true),
		NF(f_HotkeyExit, u8"��ݼ�", u8"�ܿ���::�����˳�", Hotkey(VK_F11)),
		
		NF(f_FontSize, u8"�����С", u8"�ܿ���", 16.0f),
		NF(f_ShowStyleEditor, u8"��ʾ�Զ�����ɫ", u8"�ܿ���", false),
		NFS(f_DefaultTheme, u8"����", u8"�ܿ���::��ɫ", "Default"),
		themesDir(util::GetCurrentPath() / "themes")

	{
		renderer::SetGlobalFontSize(static_cast<float>(f_FontSize));
		f_HotkeyExit.value().PressedEvent += MY_METHOD_HANDLER(Settings::OnExitKeyPressed);
		if (!std::filesystem::exists(themesDir))
			std::filesystem::create_directory(themesDir);

	}

	bool inited = false;
	void Settings::Init() {
		if (this->f_DefaultTheme.value() != "Default" && !inited)
		{
			LOG_INFO(u8"��������: %s", themesDir / (f_DefaultTheme.value() + ".json").c_str());
			if (!std::filesystem::exists(themesDir / (f_DefaultTheme.value() + ".json")))
				f_DefaultTheme = "Default";
			else Colors_Import(f_DefaultTheme.value());
			inited = true;
		}
	}

	const FeatureGUIInfo& Settings::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", u8"����", false };
		return info;
	}

	void Settings::Colors_Export(std::string name)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto colors = style.Colors;

		nlohmann::json json;
		for (int i = 0; i < ImGuiCol_COUNT; i++)
			json[ImGui::GetStyleColorName((ImGuiCol)i)] = { colors[i].x, colors[i].y, colors[i].z, colors[i].w };
		std::ofstream file(themesDir / (name + ".json"));
		file << std::setw(4) << json << std::endl;
	}

	void Settings::Colors_Import(std::string name)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto colors = style.Colors;
		nlohmann::json json;
		std::ifstream file(themesDir / (name + ".json"));
		file >> json;
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			auto color = json[ImGui::GetStyleColorName((ImGuiCol)i)];
			colors[i].x = color[0];
			colors[i].y = color[1];
			colors[i].z = color[2];
			colors[i].w = color[3];
		}
	}

	void Settings::DrawMain()
	{

		ImGui::BeginGroupPanel(u8"�ܿ���");
		{
			ConfigWidget(f_MenuKey, false,
				u8"�������˵��İ���. �������óɿ�.\n"\
				u8"�����������, ������������ļ��в鿴���޸�.");
			ConfigWidget(f_HotkeysEnabled, "Enable hotkeys.");
					}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Logging");
		{
			bool consoleChanged = ConfigWidget(f_ConsoleLogging,
				u8"���ÿ���̨�Լ�¼��Ϣ (���Ľ���������������Ч)");
			if (consoleChanged && !f_ConsoleLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::ConsoleLogger);
			}

			bool fileLogging = ConfigWidget(f_FileLogging,
				u8"����������־���ļ� (���Ľ���������������Ч).\n" \
				u8"��־Ŀ¼�����ڳ���ͬһĿ¼.");
			if (fileLogging && !f_FileLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::FileLogger);
			}
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"״̬����");
		{
			ConfigWidget(f_StatusShow);
			ConfigWidget(f_StatusMove, u8"�����ƶ� '״̬' ����.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"��Ϣ����");
		{
			ConfigWidget(f_InfoShow);
			ConfigWidget(f_InfoMove, u8"�����ƶ� '��Ϣ' ����.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"FPS ����");
		{
			ConfigWidget(f_FpsShow);
			ConfigWidget(f_FpsMove, u8"�����ƶ� 'FPS' ����.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"��ʾ֪ͨ");
		{
			ConfigWidget(f_NotificationsShow, u8"֪ͨ������Ϸ�������½���ʾ.");
			ConfigWidget(f_NotificationsDelay, 1, 1, 10000, u8"֪ͨ��ʾ���ӳ٣����룩.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"�����˳���Ϸ");
		{
			ConfigWidget(u8"����",
				f_FastExitEnable,
				u8"�������ٹر���Ϸ.\n"
			);
			if (!f_FastExitEnable)
				ImGui::BeginDisabled();

			ConfigWidget(u8"����", f_HotkeyExit, true,
				u8"�����˳���Ϸ�ļ�.");

			if (!f_FastExitEnable)
				ImGui::EndDisabled();
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"���涨��");
		{
			if (ConfigWidget(f_FontSize, 1, 8, 64, u8"�������������С."))
			{
				f_FontSize = std::clamp(f_FontSize.value(), 8, 64);
				renderer::SetGlobalFontSize(static_cast<float>(f_FontSize));
			}
			ImGui::Spacing();

			ConfigWidget(f_ShowStyleEditor, u8"���Զ��崰������ʾ��ɫ.");
			ImGui::Spacing();

			ImGui::Text(u8"�����Զ�����ɫ");
			static std::string nameBuffer_;
			ImGui::InputText(u8"��ɫ��", &nameBuffer_);
			if (ImGui::Button(u8"����"))
				Colors_Export(nameBuffer_);
				ImGui::SameLine();

			if (std::filesystem::exists(themesDir / (nameBuffer_ + ".json")))
			{
				if (this->f_DefaultTheme.value() != nameBuffer_)
				{
					if (ImGui::Button(u8"����ΪĬ��"))
					{
						f_DefaultTheme = nameBuffer_;
					}
					ImGui::SameLine();
					if (ImGui::Button(u8"����"))
					{
						Colors_Import(nameBuffer_);
					}
				}
			}
			else
			{
				ImGui::Text(u8"��ɫ������.");
			}
		}
		ImGui::EndGroupPanel();
	}

	Settings& Settings::GetInstance()
	{
		static Settings instance;
		return instance;
	}

	void Settings::OnExitKeyPressed()
	{
		if (!f_FastExitEnable || CheatManagerBase::IsMenuShowed())
			return;

		ExitProcess(0);
	}
}
