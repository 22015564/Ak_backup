#include "pch-il2cpp.h"
#include "CustomWeather.h"
#include <helpers.h>
#include <cheat/events.h>
#include <cheat/game/EntityManager.h>
#include <cheat/game/util.h>
#include <cheat/game/filters.h>

namespace cheat::feature
{
    const char* WeatherType[]{ u8"晴天", u8"多云", u8"雾天", u8"暴风雨", u8"大雨", u8"喷泉雨", u8"雪光", u8"东海岸" };
    std::string CustomWeather::GetWeather() {
        switch (current_weather)
        {
        case 0:
            return "Data/Environment/Weather/BigWorld/Weather_ClearSky";

        case 1:
            return "Data/Environment/Weather/BigWorld/Weather_Cloudy";

        case 2:
            return "Data/Environment/Weather/BigWorld/Weather_Foggy";

        case 3:
            return "Data/Environment/Weather/BigWorld/Weather_Storm";

        case 4:
            return "Data/Environment/Weather/BigWorld/Weather_Dq_Tabeisha_Rain_Heavy";

        case 5:
            return "Data/Environment/Weather/BigWorld/Weather_LY_Fountain_Rain";

        case 6:
            return "Data/Environment/Weather/BigWorld/Weather_Snowmountain_Snow_Light";

        case 7:
            return "Data/Environment/Weather/BigWorld/Weather_Snowmountain_EastCoast";

        default:
            return "Data/Environment/Weather/BigWorld/Weather_ClearSky";
        }
    }

    CustomWeather::CustomWeather() : Feature(),
        NF(f_Enabled, u8"自定义天气", u8"世界", false),
        NF(f_Lightning, u8"闪电", u8"世界", false),
        toBeUpdate(), nextUpdate(0)
    {
        events::GameUpdateEvent += MY_METHOD_HANDLER(CustomWeather::OnGameUpdate);
    }

    const FeatureGUIInfo& CustomWeather::GetGUIInfo() const
    {
        static const FeatureGUIInfo info{ u8"自定义天气", u8"图像渲染", true };
        return info;
    }

    void CustomWeather::DrawMain()
    {
        ConfigWidget(f_Enabled, u8"自定义天气.");
        if (f_Enabled) {
            ImGui::Combo(("Weather Type"), &current_weather, WeatherType, ARRAYSIZE(WeatherType));
        }
        ConfigWidget(f_Lightning, u8"闪电目标敌人，在大雨天气下工作.");
    }

    bool CustomWeather::NeedStatusDraw() const
    {
        return f_Enabled;
    }

    void CustomWeather::DrawStatus()
    {
        ImGui::Text(u8"自定义天气");
        if (f_Lightning)
            ImGui::Text(u8"闪电");
    }

    CustomWeather& CustomWeather::GetInstance()
    {
        static CustomWeather instance;
        return instance;
    }

    void CustomWeather::OnGameUpdate()
    {
        if (!f_Enabled)
            return;

        auto currentTime = util::GetCurrentTimeMillisec();
        if (currentTime < nextUpdate)
            return;

        auto Enviro = app::EnviroSky_get_Instance(nullptr);
        if (Enviro != nullptr) {
            app::EnviroSky_ChangeWeather(Enviro, string_to_il2cppi(GetWeather()), 1, 1, nullptr);

            if (f_Lightning && current_weather == 4) {
                auto& manager = game::EntityManager::instance();
                
                for (auto& Monsters : manager.entities(game::filters::combined::Monsters)) {
                    if (manager.avatar()->distance(Monsters) >= 30)
                        continue;

                    for (auto& entity : manager.entities(game::filters::combined::Lightning)) {
                        entity->setRelativePosition(Monsters->relativePosition());
                    } 
                }
            }
        }
        
        nextUpdate = currentTime + (int)f_DelayUpdate;
    }
}