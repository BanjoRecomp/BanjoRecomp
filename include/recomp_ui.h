#ifndef __RECOMP_UI__
#define __RECOMP_UI__

#include <memory>
#include <string>
#include <string_view>
#include <list>

// TODO move this file into src/ui

#include "SDL.h"
#include "RmlUi/Core.h"
#include "recomp_input.h"

#include "../src/ui/util/hsv.h"
#include "../src/ui/util/bem.h"
#include "../src/ui/elements/ui_button.h"
#include "../src/ui/elements/ui_theme.h"
#include "../src/ui/elements/ui_types.h"

#include "../src/ui/core/ui_context.h"

namespace Rml {
    class ElementDocument;
    class EventListenerInstancer;
    class Context;
    class Event;
}

namespace recompui {
    class UiEventListenerInstancer;

    // TODO remove this once the UI has been ported over to the new system.
    class MenuController {
    public:
        virtual ~MenuController() {}
        virtual void load_document() = 0;
        virtual void register_events(UiEventListenerInstancer& listener) = 0;
        virtual void make_bindings(Rml::Context* context) = 0;
    };

    std::unique_ptr<MenuController> create_launcher_menu();
    std::unique_ptr<MenuController> create_config_menu();

    using event_handler_t = void(const std::string& param, Rml::Event&);

    void queue_event(const SDL_Event& event);
    bool try_deque_event(SDL_Event& out);

    std::unique_ptr<UiEventListenerInstancer> make_event_listener_instancer();
    void register_event(UiEventListenerInstancer& listener, const std::string& name, event_handler_t* handler);

    void show_context(ContextId context, std::string_view param);
    void hide_context(ContextId context);
    void hide_all_contexts();
    bool is_context_shown(ContextId context);
    bool is_context_capturing_input();
    bool is_context_capturing_mouse();
    bool is_any_context_shown();
    ContextId try_close_current_context();

    ContextId get_launcher_context_id();
    ContextId get_config_context_id();
    ContextId get_config_sub_menu_context_id();

    enum class ConfigTabId {
        General,
        Controls,
        Graphics,
        Sound,
        Mods,
        Debug,
    };

    void set_config_tab(ConfigTabId tab);
    int config_tab_to_index(ConfigTabId tab);
    Rml::ElementTabSet* get_config_tabset();
    Rml::Element* get_mod_tab();
    void set_config_tabset_mod_nav();
    void focus_mod_configure_button();

    void init_styling(const std::filesystem::path& rcss_file);
    void init_prompt_context();
    void open_choice_prompt(
        const std::string& header_text,
        const std::string& content_text,
        const std::string& confirm_label_text,
        const std::string& cancel_label_text,
        std::function<void()> confirm_action,
        std::function<void()> cancel_action,
        ButtonStyle confirm_variant = ButtonStyle::Success,
        ButtonStyle cancel_variant = ButtonStyle::Danger,
        bool focus_on_cancel = true,
        const std::string& return_element_id = ""
    );
    void open_info_prompt(
        const std::string& header_text,
        const std::string& content_text,
        const std::string& okay_label_text,
        std::function<void()> okay_action,
        ButtonStyle okay_variant = ButtonStyle::Danger,
        const std::string& return_element_id = ""
    );
    void open_notification(
        const std::string& header_text,
        const std::string& content_text,
        const std::string& return_element_id = ""
    );
    void close_prompt();
    bool is_prompt_open();
    void update_mod_list(bool scan_mods = true);
    void process_game_started();

    void apply_color_hack();
    void get_window_size(int& width, int& height);
    void set_cursor_visible(bool visible);
    void update_supported_options();
    void toggle_fullscreen();

    bool get_cont_active(void);
    void set_cont_active(bool active);
    void activate_mouse();

    void message_box(const char* msg);

    void set_render_hooks();

    Rml::ElementPtr create_custom_element(Rml::Element* parent, std::string tag);
    Rml::ElementDocument* load_document(const std::filesystem::path& path);
    Rml::ElementDocument* create_empty_document();
    Rml::Element* get_child_by_tag(Rml::Element* parent, const std::string& tag);

    void queue_image_from_bytes_rgba32(const std::string &src, const std::vector<char> &bytes, uint32_t width, uint32_t height);
    void queue_image_from_bytes_file(const std::string &src, const std::vector<char> &bytes);
    void release_image(const std::string &src);

    void drop_files(const std::list<std::filesystem::path> &file_list);
    void report_removed_element(Rml::Element* element);

    namespace menu_action_mapping {
        struct key_map {
            // End result menu action
            const MenuAction action;
            // Mapped input from controller
            const recomp::GameInput input;
            // SDL key code from controller -> keyboard
            const int sdl;
            // RML key identifier passed to rmlui
            const Rml::Input::KeyIdentifier rml;
        };

        static const key_map accept =    { MenuAction::Accept,   recomp::GameInput::ACCEPT_MENU,    SDLK_RETURN, Rml::Input::KI_RETURN };
        static const key_map apply =     { MenuAction::Apply,    recomp::GameInput::APPLY_MENU,     SDLK_f,      Rml::Input::KI_F };
        static const key_map back =      { MenuAction::Back,     recomp::GameInput::BACK_MENU,      SDLK_F15,    Rml::Input::KI_F15 };
        static const key_map toggle =    { MenuAction::Toggle,   recomp::GameInput::TOGGLE_MENU,    SDLK_ESCAPE, Rml::Input::KI_ESCAPE };
        static const key_map tab_left =  { MenuAction::TabLeft,  recomp::GameInput::TAB_LEFT_MENU,  SDLK_F16,    Rml::Input::KI_F16 };
        static const key_map tab_right = { MenuAction::TabRight, recomp::GameInput::TAB_RIGHT_MENU, SDLK_F17,    Rml::Input::KI_F17 };

        static const std::unordered_map<Rml::Input::KeyIdentifier, key_map> rml_key_to_action {
            { accept.rml, accept },
            { apply.rml, apply },
            { back.rml, back },
            { toggle.rml, toggle },
            { tab_left.rml, tab_left },
            { tab_right.rml, tab_right }
        };

        MenuAction menu_action_from_rml_key(const Rml::Input::KeyIdentifier& key);
    };

    // Constant that represents an input that doesn't have a promptfont icon associated with it.
    extern const std::string unknown_input;
}

#endif
