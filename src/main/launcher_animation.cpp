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

enum class InterpolationMethod {
    Linear,
    Smootherstep
};

struct AnimationData {
    uint32_t keyframe_index = 0;
    uint32_t loop_keyframe_index = UINT32_MAX;
    float seconds = 0.0f;
    InterpolationMethod interpolation_method = InterpolationMethod::Linear;
};

struct AnimatedSvg {
    recompui::Svg *svg = nullptr;
    std::vector<Keyframe2D> position_keyframes;
    std::vector<Keyframe2D> scale_keyframes;
    std::vector<KeyframeRot> rotation_keyframes;
    AnimationData position_animation;
    AnimationData scale_animation;
    AnimationData rotation_animation;
    float width = 0;
    float height = 0;
};

struct LauncherContext {
    AnimatedSvg banjo_svg;
    AnimatedSvg kazooie_svg;
    AnimatedSvg jiggy_color_svg;
    AnimatedSvg jiggy_shine_svg;
    AnimatedSvg jiggy_hole_svg;
    std::chrono::steady_clock::time_point last_update_time;
    bool started = false;
} launcher_context;

float interpolate_value(float a, float b, float t, InterpolationMethod method) {
    switch (method) {
    case InterpolationMethod::Smootherstep:
        return a + (b - a) * (t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f));
    case InterpolationMethod::Linear:
    default:
        return a + (b - a) * t;
    }
}

void calculate_rot_from_keyframes(const std::vector<KeyframeRot> &kf, AnimationData &an, float delta_time, float &deg) {
    if (kf.empty()) {
        return;
    }

    an.seconds += delta_time;

    while ((an.keyframe_index < (kf.size() - 1) && (an.seconds >= kf[an.keyframe_index + 1].seconds))) {
        an.keyframe_index++;
    }

    if (an.keyframe_index >= (kf.size() - 1)) {
        deg = kf[an.keyframe_index].deg;
    }
    else {
        float t = (an.seconds - kf[an.keyframe_index].seconds) / (kf[an.keyframe_index + 1].seconds - kf[an.keyframe_index].seconds);
        deg = interpolate_value(kf[an.keyframe_index].deg, kf[an.keyframe_index + 1].deg, t, an.interpolation_method);
    }
}

void calculate_2d_from_keyframes(const std::vector<Keyframe2D> &kf, AnimationData &an, float delta_time, float &x, float &y) {
    if (kf.empty()) {
        return;
    }

    an.seconds += delta_time;

    while ((an.keyframe_index < (kf.size() - 1) && (an.seconds >= kf[an.keyframe_index + 1].seconds))) {
        an.keyframe_index++;
    }

    if ((an.loop_keyframe_index != UINT32_MAX) && (an.keyframe_index >= (kf.size() - 1))) {
        an.seconds = kf[an.loop_keyframe_index].seconds + (an.seconds - kf[an.keyframe_index].seconds);
        an.keyframe_index = an.loop_keyframe_index;
    }

    if (an.keyframe_index >= (kf.size() - 1)) {
        x = kf[an.keyframe_index].x;
        y = kf[an.keyframe_index].y;
    }
    else {
        float t = (an.seconds - kf[an.keyframe_index].seconds) / (kf[an.keyframe_index + 1].seconds - kf[an.keyframe_index].seconds);
        x = interpolate_value(kf[an.keyframe_index].x, kf[an.keyframe_index + 1].x, t, an.interpolation_method);
        y = interpolate_value(kf[an.keyframe_index].y, kf[an.keyframe_index + 1].y, t, an.interpolation_method);
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

void update_animated_svg(AnimatedSvg &animated_svg, float delta_time, float bg_width, float bg_height) {
    float position_x = 0.0f, position_y = 0.0f;
    float scale_x = 1.0f, scale_y = 1.0f;
    float rotation_degrees = 0.0f;
    calculate_2d_from_keyframes(animated_svg.position_keyframes, animated_svg.position_animation, delta_time, position_x, position_y);
    calculate_2d_from_keyframes(animated_svg.scale_keyframes, animated_svg.scale_animation, delta_time, scale_x, scale_y);
    calculate_rot_from_keyframes(animated_svg.rotation_keyframes, animated_svg.rotation_animation, delta_time, rotation_degrees);
    animated_svg.svg->set_translate_2D(position_x + bg_width / 2.0f - animated_svg.width / 2.0f, position_y + bg_height / 2.0f - animated_svg.height / 2.0f);
    animated_svg.svg->set_scale_2D(scale_x, scale_y);
    animated_svg.svg->set_rotation(rotation_degrees);
}

void banjo::launcher_animation_setup(recompui::LauncherMenu *menu) {
    auto context = recompui::get_current_context();
    recompui::Element *background_container = menu->get_background_container();
    background_container->set_background_color({ 0x1A, 0x56, 0x98, 0xFF });

    // The creation order of these is important.
    launcher_context.jiggy_color_svg = create_animated_svg(context, background_container, "JiggyColor.svg", 1054.0f, 1044.0f);
    launcher_context.jiggy_shine_svg = create_animated_svg(context, background_container, "JiggyShine.svg", 219.0f, 1080.0f);
    launcher_context.jiggy_hole_svg = create_animated_svg(context, background_container, "JiggyHole.svg", 2180.0f, 2160.0f);
    launcher_context.banjo_svg = create_animated_svg(context, background_container, "Banjo.svg", 649.0f, 622.0f);
    launcher_context.kazooie_svg = create_animated_svg(context, background_container, "Kazooie.svg", 626.0f, 774.0f);

    // Animate the jiggy hole.
    launcher_context.jiggy_hole_svg.position_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 3.0f, 0.0f, -5.0f },
        { 6.0f, 0.0f, 5.0f },
        { 9.0f, 0.0f, -5.0f },
    };

    launcher_context.jiggy_hole_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 2.2f, 1.0f, 1.0f },
    };

    launcher_context.jiggy_hole_svg.rotation_keyframes = {
        { 0.0f, -45.0f },
        { 1.0f, -45.0f },
        { 2.2f, 0.0f },
    };

    launcher_context.jiggy_hole_svg.position_animation.loop_keyframe_index = 1;
    launcher_context.jiggy_hole_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.jiggy_hole_svg.scale_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.jiggy_hole_svg.rotation_animation.interpolation_method = InterpolationMethod::Smootherstep;

    // Copy keyframes from the hole to the color.
    launcher_context.jiggy_color_svg.position_keyframes = launcher_context.jiggy_hole_svg.position_keyframes;
    launcher_context.jiggy_color_svg.position_animation = launcher_context.jiggy_hole_svg.position_animation;
    launcher_context.jiggy_color_svg.scale_keyframes = launcher_context.jiggy_hole_svg.scale_keyframes;
    launcher_context.jiggy_color_svg.scale_animation = launcher_context.jiggy_hole_svg.scale_animation;
    launcher_context.jiggy_color_svg.rotation_keyframes = launcher_context.jiggy_hole_svg.rotation_keyframes;
    launcher_context.jiggy_color_svg.rotation_animation = launcher_context.jiggy_hole_svg.rotation_animation;

    // Animate the jiggy shine.
    launcher_context.jiggy_shine_svg.position_keyframes = {
        { 0.0f, 700.0f, 0.0f },
        { 2.0f, 700.0f, 0.0f },
        { 3.0f, -700.0f, 0.0f },
    };

    launcher_context.jiggy_shine_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.0f, 0.0f, 0.0f },
        { 2.0f, 1.0f, 1.0f },
    };

    launcher_context.jiggy_shine_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;

    // Animate Banjo.
    launcher_context.banjo_svg.position_keyframes = {
        { 0.0f, -1200.0f, 0.0f },
        { 1.0f, -220.0f, 0.0f },
        { 2.0f, -220.0f, -5.0f },
        { 5.0f, -220.0f, 5.0f },
        { 8.0f, -220.0f, -5.0f },
    };

    launcher_context.banjo_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 0.1f, 1.0f, 1.0f },
    };

    launcher_context.banjo_svg.rotation_keyframes = {
        { 0.0f, -20.0f },
        { 1.0f, 0.0f },
    };

    launcher_context.banjo_svg.position_animation.loop_keyframe_index = 2;
    launcher_context.banjo_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    launcher_context.banjo_svg.rotation_animation.interpolation_method = InterpolationMethod::Smootherstep;

    // Animate Kazooie. Mirror all of Banjo's keyframes except for the scale.
    launcher_context.kazooie_svg.position_keyframes = launcher_context.banjo_svg.position_keyframes;
    launcher_context.kazooie_svg.position_animation = launcher_context.banjo_svg.position_animation;
    launcher_context.kazooie_svg.scale_keyframes = launcher_context.banjo_svg.scale_keyframes;
    launcher_context.kazooie_svg.scale_animation = launcher_context.banjo_svg.scale_animation;
    launcher_context.kazooie_svg.rotation_keyframes = launcher_context.banjo_svg.rotation_keyframes;
    launcher_context.kazooie_svg.rotation_animation = launcher_context.banjo_svg.rotation_animation;

    for (auto &kf : launcher_context.kazooie_svg.position_keyframes) {
        kf.x = -kf.x;
    }

    launcher_context.kazooie_svg.position_keyframes[2].seconds += 1.5f;
    launcher_context.kazooie_svg.position_keyframes[3].seconds += 1.5f;
    launcher_context.kazooie_svg.position_keyframes[4].seconds += 1.5f;

    for (auto &kf : launcher_context.kazooie_svg.rotation_keyframes) {
        kf.deg = -kf.deg;
    }
}

void banjo::launcher_animation_update(recompui::LauncherMenu *menu) {
    std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
    float delta_time = launcher_context.started ? std::chrono::duration_cast<std::chrono::milliseconds>(now - launcher_context.last_update_time).count() / 1000.0f : 0.0f;
    launcher_context.last_update_time = now;
    launcher_context.started = true;

    recompui::Element *background_container = menu->get_background_container();
    float dp_to_pixel_ratio = background_container->get_dp_to_pixel_ratio();
    float bg_width = background_container->get_client_width() / dp_to_pixel_ratio;
    float bg_height = background_container->get_client_height() / dp_to_pixel_ratio;
    update_animated_svg(launcher_context.banjo_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.kazooie_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.jiggy_color_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.jiggy_shine_svg, delta_time, bg_width, bg_height);
    update_animated_svg(launcher_context.jiggy_hole_svg, delta_time, bg_width, bg_height);
}
