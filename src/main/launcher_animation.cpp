#include "banjo_launcher.h"

struct KeyframeRot {
    float seconds;
    float deg;
};

struct Keyframe2D {
    float seconds;
    float x;
    float y;
};

struct AnimatedSvg {
    recompui::Svg *svg = nullptr;
    std::vector<Keyframe2D> position_keyframes;
    std::vector<Keyframe2D> scale_keyframes;
    std::vector<KeyframeRot> rotation_keyframes;
    uint32_t position_keyframe_index = 0;
    uint32_t scale_keyframe_index = 0;
    uint32_t rotation_keyframe_index = 0;
    float width = 0;
    float height = 0;
};

struct LauncherContext {
    AnimatedSvg banjo_svg;
    AnimatedSvg kazooie_svg;
    AnimatedSvg jiggy_color_svg;
    AnimatedSvg jiggy_shine_svg;
    AnimatedSvg jiggy_hole_svg;
    std::chrono::steady_clock::time_point start_time;
    bool started = false;
} launcher_context;

void calculate_rot_from_keyframes(const std::vector<KeyframeRot> &kf, float seconds, float &deg, uint32_t &kf_index) {
    if (kf.empty()) {
        return;
    }

    while ((kf_index < (kf.size() - 1) && (seconds >= kf[kf_index + 1].seconds))) {
        kf_index++;
    }

    if (kf_index >= (kf.size() - 1)) {
        deg = kf[kf_index].deg;
    }
    else {
        float t = (seconds - kf[kf_index].seconds) / (kf[kf_index + 1].seconds - kf[kf_index].seconds);
        deg = kf[kf_index].deg + (kf[kf_index + 1].deg - kf[kf_index].deg) * t;
    }
}

void calculate_2d_from_keyframes(const std::vector<Keyframe2D> &kf, float seconds, float &x, float &y, uint32_t &kf_index) {
    if (kf.empty()) {
        return;
    }

    while ((kf_index < (kf.size() - 1) && (seconds >= kf[kf_index + 1].seconds))) {
        kf_index++;
    }

    if (kf_index >= (kf.size() - 1)) {
        x = kf[kf_index].x;
        y = kf[kf_index].y;
    }
    else {
        float t = (seconds - kf[kf_index].seconds) / (kf[kf_index + 1].seconds - kf[kf_index].seconds);
        x = kf[kf_index].x + (kf[kf_index + 1].x - kf[kf_index].x) * t;
        y = kf[kf_index].y + (kf[kf_index + 1].y - kf[kf_index].y) * t;
    }
}

AnimatedSvg create_animated_svg(recompui::ContextId context, recompui::Element *parent, const std::string &svg_path, float width, float height) {
    AnimatedSvg animated_svg;
    animated_svg.width = width;
    animated_svg.height = height;
    animated_svg.svg = context.create_element<recompui::Svg>(parent, svg_path);
    animated_svg.svg->set_position(recompui::Position::Absolute);
    animated_svg.svg->set_width(width, recompui::Unit::Dp);
    animated_svg.svg->set_height(height, recompui::Unit::Dp);
    return animated_svg;
}

void update_animated_svg(AnimatedSvg &animated_svg, float seconds, float bg_width, float bg_height) {
    float position_x = 0.0f, position_y = 0.0f;
    float scale_x = 1.0f, scale_y = 1.0f;
    float rotation_degrees = 0.0f;
    calculate_2d_from_keyframes(animated_svg.position_keyframes, seconds, position_x, position_y, animated_svg.position_keyframe_index);
    calculate_2d_from_keyframes(animated_svg.scale_keyframes, seconds, scale_x, scale_y, animated_svg.scale_keyframe_index);
    calculate_rot_from_keyframes(animated_svg.rotation_keyframes, seconds, rotation_degrees, animated_svg.rotation_keyframe_index);
    animated_svg.svg->set_translate_2D(position_x + bg_width / 2.0f - animated_svg.width / 2.0f, position_y + bg_height / 2.0f - animated_svg.height / 2.0f);
    animated_svg.svg->set_scale_2D(scale_x, scale_y);
    animated_svg.svg->set_rotation(rotation_degrees);
}

void banjo::launcher_animation_setup(recompui::LauncherMenu *menu) {
    auto context = recompui::get_current_context();
    recompui::Element *background_container = menu->get_background_container();
    background_container->set_background_color({ 0x1A, 0x56, 0x98, 0xFF });

    // The creation order of these is important.
    launcher_context.jiggy_color_svg = create_animated_svg(context, background_container, "JiggyColor.svg", 1054.0f * 0.75f, 1044.0f * 0.75f);
    launcher_context.jiggy_shine_svg = create_animated_svg(context, background_container, "JiggyShine.svg", 219.0f * 0.75f, 1080.0f * 0.75f);
    launcher_context.jiggy_hole_svg = create_animated_svg(context, background_container, "JiggyHole.svg", 1090.0f * 0.75f, 1080.0f * 0.75f);
    launcher_context.banjo_svg = create_animated_svg(context, background_container, "Banjo.svg", 649.0f * 0.75f, 622.0f * 0.75f);
    launcher_context.kazooie_svg = create_animated_svg(context, background_container, "Kazooie.svg", 626.0f * 0.75f, 774.0f * 0.75f);

    // Animate the jiggy hole.
    launcher_context.jiggy_hole_svg.position_keyframes = {
        { 0.0f, 0.0f, 0.0f },
    };

    launcher_context.jiggy_hole_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 0.0f },
        { 1.3f, 1.0f, 1.0f },
        { 1.4f, 1.1f, 1.1f },
        { 1.5f, 1.0f, 1.0f },
    };

    launcher_context.jiggy_hole_svg.rotation_keyframes = {
        { 0.0f, -45.0f },
        { 0.5f, -45.0f },
        { 1.3f, 0.0f },
        { 1.4f, 5.0f },
        { 1.5f, 0.0f },
    };

    // Copy keyframes from the hole to the color.
    launcher_context.jiggy_color_svg.position_keyframes = launcher_context.jiggy_hole_svg.position_keyframes;
    launcher_context.jiggy_color_svg.scale_keyframes = launcher_context.jiggy_hole_svg.scale_keyframes;
    launcher_context.jiggy_color_svg.rotation_keyframes = launcher_context.jiggy_hole_svg.rotation_keyframes;

    // Animate the jiggy shine.
    launcher_context.jiggy_shine_svg.position_keyframes = {
        { 0.0f, 320.0f, 0.0f },
        { 1.5f, 320.0f, 0.0f },
        { 1.8f, -320.0f, 0.0f },
    };

    launcher_context.jiggy_shine_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 1.5f, 0.0f, 0.0f },
        { 1.5f, 1.0f, 1.0f },
        { 1.8f, 1.0f, 1.0f },
        { 1.8f, 0.0f, 0.0f },
    };

    // Animate Banjo.
    launcher_context.banjo_svg.position_keyframes = {
        { 0.0f, -1200.0f, 0.0f },
        { 0.6f, -165.0f, 0.0f },
        { 0.65f, -150.0f, 0.0f },
        { 0.8f, -165.0f, 0.0f },
    };

    launcher_context.banjo_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 0.1f, 1.0f, 1.0f },
        { 0.6f, 1.0f, 1.0f },
        { 0.65f, 0.9f, 1.0f },
        { 0.8f, 1.0f, 1.0f },
    };

    launcher_context.banjo_svg.rotation_keyframes = {
        { 0.0f, -15.0f },
        { 0.6f, 0.0f },
    };

    // Animate Kazooie.
    launcher_context.kazooie_svg.position_keyframes = {
       { 0.0f, 1200.0f, 0.0f },
       { 0.6f, 165.0f, 0.0f },
       { 0.65f, 150.0f, 0.0f },
       { 0.8f, 165.0f, 0.0f },
    };

    launcher_context.kazooie_svg.rotation_keyframes = {
        { 0.0f, 15.0f },
        { 0.6f, 0.0f },
    };

    launcher_context.kazooie_svg.scale_keyframes = launcher_context.banjo_svg.scale_keyframes;
}

void banjo::launcher_animation_update(recompui::LauncherMenu *menu) {
    if (!launcher_context.started) {
        launcher_context.start_time = std::chrono::high_resolution_clock::now();
        launcher_context.started = true;
    }

    recompui::Element *background_container = menu->get_background_container();
    float dp_to_pixel_ratio = background_container->get_dp_to_pixel_ratio();
    float bg_width = background_container->get_client_width() / dp_to_pixel_ratio;
    float bg_height = background_container->get_client_height() / dp_to_pixel_ratio;
    float seconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - launcher_context.start_time).count() / 1000.0f;
    update_animated_svg(launcher_context.banjo_svg, seconds, bg_width, bg_height);
    update_animated_svg(launcher_context.kazooie_svg, seconds, bg_width, bg_height);
    update_animated_svg(launcher_context.jiggy_color_svg, seconds, bg_width, bg_height);
    update_animated_svg(launcher_context.jiggy_shine_svg, seconds, bg_width, bg_height);
    update_animated_svg(launcher_context.jiggy_hole_svg, seconds, bg_width, bg_height);
}
