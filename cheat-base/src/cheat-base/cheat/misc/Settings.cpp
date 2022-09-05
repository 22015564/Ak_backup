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
		NF(f_MenuKey, u8"显示菜单的按键", u8"总开关", Hotkey(VK_F12)),
		NF(f_HotkeysEnabled, u8"开启快捷键", u8"总开关", true),
		
		NF(f_StatusMove, u8"移动状态窗口", u8"总开关::状态窗口", true),
		NF(f_StatusShow, u8"显示状态窗口", u8"总开关::状态窗口", true),

		NF(f_InfoMove, u8"移动信息窗口", u8"总开关::信息窗口", true),
		NF(f_InfoShow, u8"显示信息窗口", u8"总开关::信息窗口", true),

		NF(f_FpsMove, u8"移动帧率窗口", u8"总开关::FPS", false),
		NF(f_FpsShow, u8"显示帧率窗口", u8"总开关::FPS", true),

		NF(f_NotificationsShow, u8"显示通知", u8"总开关::通知", true),
		NF(f_NotificationsDelay, u8"通知延迟", u8"总开关::通知", 500),

		NF(f_FileLogging, u8"保存日志", u8"总开关::日志", false),
		NF(f_ConsoleLogging, u8"控制台输出", u8"总开关::日志", true),

		NF(f_FastExitEnable, u8"快速关闭游戏", u8"总开关::快速退出", true),
		NF(f_HotkeyExit, u8"快捷键", u8"总开关::快速退出", Hotkey(VK_F11)),
		
		NF(f_FontSize, u8"字体大小", u8"总开关", 16.0f),
		NF(f_ShowStyleEditor, u8"显示自定义颜色", u8"总开关", false),
		NFS(f_DefaultTheme, u8"主题", u8"总开关::颜色", "Default"),
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
			LOG_INFO(u8"加载主题: %s", themesDir / (f_DefaultTheme.value() + ".json").c_str());
			if (!std::filesystem::exists(themesDir / (f_DefaultTheme.value() + ".json")))
				f_DefaultTheme = "Default";
			else Colors_Import(f_DefaultTheme.value());
			inited = true;
		}
	}

	const FeatureGUIInfo& Settings::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", u8"设置", false };
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

		ImGui::BeginGroupPanel(u8"总开关");
		{
			ConfigWidget(f_MenuKey, false,
				u8"呼出主菜单的按键. 不能设置成空.\n"\
				u8"如果你忘记了, 你可以在配置文件中查看和修改.");
			ConfigWidget(f_HotkeysEnabled, "Enable hotkeys.");
					}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel("Logging");
		{
			bool consoleChanged = ConfigWidget(f_ConsoleLogging,
				u8"启用控制台以记录信息 (更改将在重新启动后生效)");
			if (consoleChanged && !f_ConsoleLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::ConsoleLogger);
			}

			bool fileLogging = ConfigWidget(f_FileLogging,
				u8"开启保存日志到文件 (更改将在重新启动后生效).\n" \
				u8"日志目录将放在程序同一目录.");
			if (fileLogging && !f_FileLogging)
			{
				Logger::SetLevel(Logger::Level::None, Logger::LoggerType::FileLogger);
			}
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"状态窗口");
		{
			ConfigWidget(f_StatusShow);
			ConfigWidget(f_StatusMove, u8"允许移动 '状态' 窗口.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"信息窗口");
		{
			ConfigWidget(f_InfoShow);
			ConfigWidget(f_InfoMove, u8"允许移动 '信息' 窗口.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"FPS 窗口");
		{
			ConfigWidget(f_FpsShow);
			ConfigWidget(f_FpsMove, u8"允许移动 'FPS' 窗口.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"显示通知");
		{
			ConfigWidget(f_NotificationsShow, u8"通知将在游戏窗口右下角显示.");
			ConfigWidget(f_NotificationsDelay, 1, 1, 10000, u8"通知显示的延迟（毫秒）.");
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"快速退出游戏");
		{
			ConfigWidget(u8"开启",
				f_FastExitEnable,
				u8"开启快速关闭游戏.\n"
			);
			if (!f_FastExitEnable)
				ImGui::BeginDisabled();

			ConfigWidget(u8"按键", f_HotkeyExit, true,
				u8"快速退出游戏的键.");

			if (!f_FastExitEnable)
				ImGui::EndDisabled();
		}
		ImGui::EndGroupPanel();

		ImGui::BeginGroupPanel(u8"界面定制");
		{
			if (ConfigWidget(f_FontSize, 1, 8, 64, u8"调整界面字体大小."))
			{
				f_FontSize = std::clamp(f_FontSize.value(), 8, 64);
				renderer::SetGlobalFontSize(static_cast<float>(f_FontSize));
			}
			ImGui::Spacing();

			ConfigWidget(f_ShowStyleEditor, u8"在自定义窗口中显示颜色.");
			ImGui::Spacing();

			ImGui::Text(u8"保存自定义颜色");
			static std::string nameBuffer_;
			ImGui::InputText(u8"颜色名", &nameBuffer_);
			if (ImGui::Button(u8"保存"))
				Colors_Export(nameBuffer_);
				ImGui::SameLine();

			if (std::filesystem::exists(themesDir / (nameBuffer_ + ".json")))
			{
				if (this->f_DefaultTheme.value() != nameBuffer_)
				{
					if (ImGui::Button(u8"设置为默认"))
					{
						f_DefaultTheme = nameBuffer_;
					}
					ImGui::SameLine();
					if (ImGui::Button(u8"加载"))
					{
						Colors_Import(nameBuffer_);
					}
				}
			}
			else
			{
				ImGui::Text(u8"颜色不存在.");
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
