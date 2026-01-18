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
    recompui::Element *svg = nullptr;
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
    AnimatedSvg logo_svg;
    std::array<AnimatedSvg, 4> cloud_svgs;
    recompui::Element *wrapper;
    float wrapper_phase = -1.0f;
    std::chrono::steady_clock::time_point last_update_time;
    float seconds = 0.0f;
    bool started = false;
    bool options_enabled = false;
    bool animation_skipped = false;
    std::atomic<bool> skip_animation_next_update = false;
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

bool check_skip_input(SDL_Event* event) {
    switch (event->type) {
    case SDL_KEYDOWN:
        return event->key.keysym.scancode == SDL_SCANCODE_ESCAPE ||
            event->key.keysym.scancode == SDL_SCANCODE_SPACE ||
            (event->key.keysym.scancode == SDL_SCANCODE_RETURN && (event->key.keysym.mod & (KMOD_LALT | KMOD_RALT)) == KMOD_NONE);
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_MOUSEBUTTONDOWN:
        return true;
    default:
        return false;
    }
}

int launcher_event_watch(void* userdata, SDL_Event* event) {
    if (!launcher_context.animation_skipped && check_skip_input(event)) {
        launcher_context.animation_skipped = true;
        launcher_context.skip_animation_next_update = true;
        return 0;
    }
    else {
        return 1;
    }
}

const float jiggy_scale_anim_start = 0.0f;
const float jiggy_scale_anim_length = 0.75f;
const float jiggy_scale_anim_end = jiggy_scale_anim_start + jiggy_scale_anim_length;
const float jiggy_move_over_start = jiggy_scale_anim_end + 0.5f;
const float jiggy_move_over_length = 0.75f;
const float jiggy_move_over_end = jiggy_move_over_start + jiggy_move_over_length;
const float jiggy_shine_start = jiggy_move_over_end + 0.6f;
const float jiggy_shine_length = 0.8f;

const float animation_skip_time = 10.0f;

void banjo::launcher_animation_setup(recompui::LauncherMenu *menu) {
    auto context = recompui::get_current_context();
    recompui::Element *background_container = menu->get_background_container();
    background_container->set_background_color({ 0x1F, 0x63, 0xC2, 0xFF });

    launcher_context.wrapper = context.create_element<recompui::Element>(background_container, 0);
    launcher_context.wrapper->set_position(recompui::Position::Absolute);
    launcher_context.wrapper->set_width(100, recompui::Unit::Percent);
    launcher_context.wrapper->set_height(100, recompui::Unit::Percent);
    launcher_context.wrapper->set_top(0);

    // Disable and hide the options.
    for (auto option : menu->get_game_options_menu()->get_options()) {
        option->set_enabled(false);
        option->set_opacity(0.0f);
    }

    // The creation order of these is important.
    launcher_context.jiggy_color_svg = create_animated_svg(context, launcher_context.wrapper, "JiggyColor.svg", 1054.0f, 1044.0f);
    launcher_context.jiggy_shine_svg = create_animated_svg(context, launcher_context.wrapper, "JiggyShine.svg", 219.0f, 1080.0f);
    launcher_context.jiggy_hole_svg = create_animated_svg(context, launcher_context.wrapper, "JiggyHole.svg", 2180.0f, 2160.0f);
    launcher_context.banjo_svg = create_animated_svg(context, launcher_context.wrapper, "Banjo.svg", 649.0f, 622.0f);
    launcher_context.kazooie_svg = create_animated_svg(context, launcher_context.wrapper, "Kazooie.svg", 626.0f, 774.0f);

    launcher_context.cloud_svgs[0] = create_animated_svg(context, background_container, "Cloud1.svg", 461.0f, 154.0f);
    launcher_context.cloud_svgs[1] = create_animated_svg(context, background_container, "Cloud2.svg", 461.0f, 167.0f);
    launcher_context.cloud_svgs[2] = create_animated_svg(context, background_container, "Cloud3.svg", 295.0f, 167.0f);
    launcher_context.cloud_svgs[3] = create_animated_svg(context, background_container, "Cloud1.svg", 461.0f, 154.0f);

    launcher_context.logo_svg = create_animated_svg(context, background_container, "Logo.svg", 6187.0f * 0.125f, 2625.0f * 0.125f);

    // Animate the jiggy hole.
    launcher_context.jiggy_hole_svg.position_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 3.0f, 0.0f, -5.0f },
        { 6.0f, 0.0f, 5.0f },
        { 9.0f, 0.0f, -5.0f },
    };

    launcher_context.jiggy_hole_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { jiggy_scale_anim_start + 0.0f, 0.0f, 0.0f },
        { jiggy_scale_anim_start + jiggy_scale_anim_length, 1.0f, 1.0f },
    };

    launcher_context.jiggy_hole_svg.rotation_keyframes = {
        { 0.0f, -45.0f },
        { jiggy_scale_anim_start + 0.0f, -45.0f },
        { jiggy_scale_anim_start + jiggy_scale_anim_length, 0.0f },
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
        { jiggy_shine_start, 700.0f, 0.0f },
        { jiggy_shine_start + jiggy_shine_length, -700.0f, 0.0f },
    };

    launcher_context.jiggy_shine_svg.scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { jiggy_shine_start, 0.0f, 0.0f },
        { jiggy_shine_start + 0.01f, 1.0f, 1.0f },
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

    // Animate the logo.
    launcher_context.logo_svg.position_keyframes = {
        { 0.0f, 0.0f, -900.0f },
        { 1.0f, 0.0f, -900.0f },
        { 2.0f, 0.0f, -365.0f },
    };

    launcher_context.logo_svg.position_animation.interpolation_method = InterpolationMethod::Smootherstep;

    // Animate the clouds.
    const float cloud_scale_duration = 0.3f;
    launcher_context.cloud_svgs[0].position_keyframes = {
        { 0.0f, 600.0f, -445.0f },
        { 3.0f, 600.0f, -455.0f },
        { 6.0f, 600.0f, -445.0f },
    };

    launcher_context.cloud_svgs[0].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.0f, 0.0f, 0.0f },
        { 2.0f + cloud_scale_duration, 1.0f, 1.0f },
    };

    launcher_context.cloud_svgs[1].position_keyframes = {
        { 0.0f, -720.0f, 355.0f },
        { 5.0f, -720.0f, 365.0f },
        { 10.0f, -720.0f, 355.0f },
    };

    launcher_context.cloud_svgs[1].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.2f, 0.0f, 0.0f },
        { 2.2f + cloud_scale_duration, 1.0f, 1.0f },
    };

    launcher_context.cloud_svgs[2].position_keyframes = {
        { 0.0f, -600.0f, -295.0f },
        { 2.0f, -600.0f, -305.0f },
        { 4.0f, -600.0f, -295.0f },
    };

    launcher_context.cloud_svgs[2].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.4f, 0.0f, 0.0f },
        { 2.4f + cloud_scale_duration, 1.0f, 1.0f },
    };

    launcher_context.cloud_svgs[3].position_keyframes = {
        { 0.0f, 670.0f, 395.0f },
        { 4.0f, 670.0f, 405.0f },
        { 8.0f, 670.0f, 395.0f },
    };

    launcher_context.cloud_svgs[3].scale_keyframes = {
        { 0.0f, 0.0f, 0.0f },
        { 2.6f, 0.0f, 0.0f },
        { 2.6f + cloud_scale_duration, 1.0f, 1.0f },
    };

    for (size_t i = 0; i < launcher_context.cloud_svgs.size(); i++) {
        launcher_context.cloud_svgs[i].position_animation.loop_keyframe_index = 0;
        launcher_context.cloud_svgs[i].position_animation.interpolation_method = InterpolationMethod::Smootherstep;
    }

    // Install an event watch to skip the launcher animation if a keyboard, mouse or controller input is detected.
    SDL_AddEventWatch(&launcher_event_watch, nullptr);
}

void banjo::launcher_animation_update(recompui::LauncherMenu *menu) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    float delta_time = launcher_context.started ? std::chrono::duration_cast<std::chrono::milliseconds>(now - launcher_context.last_update_time).count() / 1000.0f : 0.0f;
    if (launcher_context.skip_animation_next_update) {
        delta_time = std::max(animation_skip_time - launcher_context.seconds, 0.0f);
        launcher_context.skip_animation_next_update = false;
    }

    launcher_context.seconds += delta_time;
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
    update_animated_svg(launcher_context.logo_svg, delta_time, bg_width, bg_height);

    for (size_t i = 0; i < launcher_context.cloud_svgs.size(); i++) {
        update_animated_svg(launcher_context.cloud_svgs[i], delta_time, bg_width, bg_height);
    }

    float wrapper_phase = std::clamp((launcher_context.seconds - jiggy_move_over_start) / (jiggy_move_over_end - jiggy_move_over_start), 0.0f, 1.0f);
    if (wrapper_phase != launcher_context.wrapper_phase) {
        float x_translation = interpolate_value(0, 1440 * -0.2f, wrapper_phase, InterpolationMethod::Smootherstep);
        launcher_context.wrapper->set_translate_2D(x_translation, 0, recompui::Unit::Dp);
        float y_translation = interpolate_value(0, launcher_options_top_offset, wrapper_phase, InterpolationMethod::Smootherstep);
        launcher_context.wrapper->set_top(y_translation);
        float scale = interpolate_value(1, 0.666f, wrapper_phase, InterpolationMethod::Smootherstep);
        launcher_context.wrapper->set_scale_2D(scale, scale);

        float game_option_menu_opacity = interpolate_value(0, 1.0f, wrapper_phase, InterpolationMethod::Smootherstep);
        for (auto option : menu->get_game_options_menu()->get_options()) {
            option->set_opacity(game_option_menu_opacity);
        }

        float game_option_menu_right = interpolate_value(launcher_options_right_position_start, launcher_options_right_position_end, wrapper_phase, InterpolationMethod::Smootherstep);
        menu->get_game_options_menu()->set_right(game_option_menu_right);

        launcher_context.wrapper_phase = wrapper_phase;
    }

    if (!launcher_context.options_enabled && launcher_context.seconds >= jiggy_move_over_end) {
        SDL_DelEventWatch(&launcher_event_watch, nullptr);

        for (auto option : menu->get_game_options_menu()->get_options()) {
            option->set_enabled(true);
            option->set_opacity(1.0f);
        }

        launcher_context.options_enabled = true;
    }
}
