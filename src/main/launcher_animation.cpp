#include "banjo_launcher.h"

constexpr float banjo_apsect_ratio = 1.0434f;
constexpr float banjo_base_width = 649.0f;
constexpr float banjo_base_height = 622.0f;
constexpr float banjo_scale = 1.0f;
constexpr float banjo_width = banjo_base_width * banjo_scale;
constexpr float banjo_height = banjo_base_height * banjo_scale;
constexpr float kazooie_base_width = 626.0f;
constexpr float kazooie_base_height = 774.0f;
constexpr float kazooie_scale = 1.0f;
constexpr float kazooie_width = kazooie_base_width * kazooie_scale;
constexpr float kazooie_height = kazooie_base_height * kazooie_scale;

struct LauncherContext {
    recompui::Svg* banjo_svg;
    recompui::Svg* kazooie_svg;
} launcher_context;

recompui::Svg* create_absolute_positioned_svg(recompui::ContextId context, recompui::Element* parent, const std::string& svg_path, float width, float height) {
    recompui::Svg* ret = context.create_element<recompui::Svg>(parent, svg_path);

    ret->set_position(recompui::Position::Absolute);
    ret->set_height(width, recompui::Unit::Dp);
    ret->set_width(height, recompui::Unit::Dp);
    ret->set_translate_2D(-width / 2, -height / 2, recompui::Unit::Dp);

    return ret;
}

void banjo::launcher_animation_setup(recompui::LauncherMenu *menu) {
    auto context = recompui::get_current_context();
    recompui::Element* background_container = menu->get_background_container();

    launcher_context.banjo_svg = create_absolute_positioned_svg(context, background_container, "Banjo.svg",
        banjo_width, banjo_height);
    launcher_context.banjo_svg->set_top(50.0f, recompui::Unit::Percent);
    launcher_context.banjo_svg->set_left(50.0f, recompui::Unit::Percent);
    launcher_context.banjo_svg->set_translate_2D(-500.0f - banjo_width / 2, -banjo_height / 2);

    launcher_context.kazooie_svg = create_absolute_positioned_svg(context, background_container, "Kazooie.svg",
        kazooie_width, kazooie_height);
    launcher_context.kazooie_svg->set_top(50.0f, recompui::Unit::Percent);
    launcher_context.kazooie_svg->set_left(50.0f, recompui::Unit::Percent);
    launcher_context.kazooie_svg->set_translate_2D(500.0f - kazooie_width / 2, -kazooie_height / 2);
}

void banjo::launcher_animation_update(recompui::LauncherMenu *menu) {
    launcher_context.banjo_svg->set_translate_2D(-500.0f - banjo_width / 2, -banjo_height / 2);    
    launcher_context.kazooie_svg->set_translate_2D(500.0f - kazooie_width / 2, -kazooie_height / 2);
}
