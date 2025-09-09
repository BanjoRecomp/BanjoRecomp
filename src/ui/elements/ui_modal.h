#pragma once

#include "recomp_ui.h"
#include "ui_element.h"
#include "ui_config_page.h"
#include "ui_tab_set.h"

namespace recompui {

    class Modal;

    enum class ModalType {
        Fullscreen,
        Prompt
    };

    // Function in Modal::set_first_focusable_below_tabs, reports the element to focus on when navigating down from the tabs
    using set_first_focusable_below_tabs_t = std::function<void(Element*)>;

    class TabContext {
    private:
        Element *upper_focusable = nullptr;
        Modal *parent_modal = nullptr;
    protected:
        set_first_focusable_below_tabs_t set_first_focusable_below_tabs = nullptr;
    public:
        std::string tab_name;
        TabContext(std::string name) : tab_name(name) {};
        virtual ~TabContext() = default;
        virtual Element* create_tab_contents(recompui::ContextId context, Element* parent) = 0;
        virtual bool can_be_closed() { return true; }
        virtual void on_tab_close() {}
        virtual void on_init() {}

        virtual void set_nav_up(Element *element) { upper_focusable = element; }
        Element* get_nav_up() const { return upper_focusable; }
        Modal *get_parent_modal() const { return parent_modal; }
        void set_parent_modal(Modal *modal) { parent_modal = modal; }

        void apply_set_first_focusable_below_tabs_callback();
    };

    class Modal : public Element {
    protected:
        bool is_open = false;
        Element *modal_element = nullptr;
        ConfigHeaderFooter *header = nullptr;
        Element *body = nullptr;
        ModalType modal_type;
        std::vector<TabContext*> tab_contexts;
        std::function<void()> on_close_callback;
        std::unordered_map<MenuAction, std::function<void()>> menu_action_callbacks;
        TabSet *tabs = nullptr;
        int previous_tab_index = -1;
        int current_tab_index = -1;
        
        virtual void process_event(const Event &e) override;
        std::string_view get_type_name() override { return "Modal"; }
        void on_tab_change(int tab_index);
        void navigate_tab_direction(int direction);
    public:
        recompui::ContextId modal_root_context;
        Modal(Element *parent, recompui::ContextId modal_root_context, ModalType modal_type);
        static Modal *create_modal(ModalType modal_type = ModalType::Fullscreen);
        virtual ~Modal();
        void open();
        void close();
        bool is_open_now() const { return is_open; }

        void add_tab(TabContext *tab_context);
        void set_selected_tab(int tab_index);

        ConfigHeaderFooter *get_header() { return header; }
        Element *get_body() { return body; }
        Element *get_active_tab() { return tabs != nullptr ? static_cast<Tab*>(tabs->get_active_tab_element()) : nullptr; }
        // set_first_focusable_below_tabs_t
        void set_first_focusable_below_tabs(Element *element);
        void set_on_close_callback(std::function<void()> callback);
        void set_menu_action_callback(MenuAction action, std::function<void()> callback);
    };
} // namespace recompui
