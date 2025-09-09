#include "ui_modal.h"
#include "ui_element.h"

namespace recompui {

class ModalOverlay : public Element {
protected:
    Modal *parent_modal;

    void process_event(const Event &e) override {
        if (e.type != EventType::Click) {
            return;
        }

        // parent_modal->close();
    }
    std::string_view get_type_name() override { return "ModalOverlay"; }
public:
    ModalOverlay(Modal *parent) : Element(parent, Events(EventType::Click, EventType::Hover, EventType::Enable, EventType::Focus), "div", false) {
        this->parent_modal = parent;

        set_display(Display::Block);
        set_position(Position::Absolute);
        set_top(0);
        set_right(0);
        set_bottom(0);
        set_left(0);
        set_background_color(theme::color::BGOverlay);
    }

    ~ModalOverlay() {
    }
};

constexpr float modal_page_padding = 64.0f;
constexpr float modal_page_base_height = 1080.0f;
constexpr float modal_height = modal_page_base_height - (modal_page_padding * 2.0f);
constexpr float modal_width = modal_page_base_height * 16.0f / 9.0f;

Modal::Modal(
    Element *parent,
    recompui::ContextId modal_root_context,
    ModalType modal_type
) : Element(parent, Events(EventType::MenuAction), "div", false)
{
    this->modal_root_context = modal_root_context;
    this->modal_type = modal_type;
    recompui::ContextId context = modal_root_context;

    set_display(Display::None);
    set_position(Position::Absolute);
    set_top(0);
    set_right(0);
    set_bottom(0);
    set_left(0);

    ModalOverlay* modal_overlay = context.create_element<ModalOverlay>(this);

    Element* modal_whole_page_wrapper = context.create_element<Element>(modal_overlay);
    modal_whole_page_wrapper->set_display(Display::Flex);
    modal_whole_page_wrapper->set_position(Position::Absolute);
    modal_whole_page_wrapper->set_top(0);
    modal_whole_page_wrapper->set_right(0);
    modal_whole_page_wrapper->set_bottom(0);
    modal_whole_page_wrapper->set_left(0);
    modal_whole_page_wrapper->set_padding(modal_page_padding);
    modal_whole_page_wrapper->set_align_items(AlignItems::Center);
    modal_whole_page_wrapper->set_justify_content(JustifyContent::Center);

    modal_element = context.create_element<Element>(modal_whole_page_wrapper);
    modal_element->set_display(Display::Flex);
    modal_element->set_position(Position::Relative);
    modal_element->set_flex(1.0f, 1.0f);
    modal_element->set_flex_basis(100, Unit::Percent);
    modal_element->set_flex_direction(FlexDirection::Column);
    modal_element->set_width(100, Unit::Percent);
    modal_element->set_max_width(modal_width);
    modal_element->set_height(100, Unit::Percent);
    modal_element->set_margin_auto();
    modal_element->set_border_width(theme::border::width);
    modal_element->set_border_radius(theme::border::radius_lg);
    modal_element->set_border_color(theme::color::Border);
    modal_element->set_background_color(theme::color::ModalOverlay);

    header = context.create_element<ConfigHeaderFooter>(modal_element, true);
    header->set_padding_top(0.0f);
    header->set_padding_bottom(0.0f);
    header->set_padding_left(8.0f);
    header->set_padding_right(8.0f);
    header->set_border_top_left_radius(theme::border::radius_lg);
    header->set_border_top_right_radius(theme::border::radius_lg);

    body = context.create_element<Element>(modal_element);
    body->set_display(Display::Flex);
    body->set_position(Position::Relative);
    body->set_flex_grow(1.0f);
    body->set_flex_shrink(1.0f);
    body->set_flex_basis_auto();
    body->set_flex_direction(FlexDirection::Row);
    body->set_width(100.0f, Unit::Percent);
}

Modal::~Modal() {
}

void Modal::open() {
    if (!recompui::is_context_shown(modal_root_context)) {
        recompui::show_context(modal_root_context, "");
    }

    is_open = true;
    set_display(Display::Block);
    // queue_update();
    if (tabs != nullptr) {
        tabs->focus_on_active_tab();
        on_tab_change(tabs->get_active_tab());
    }
}

void Modal::close() {
    if (current_tab_index < tab_contexts.size() && current_tab_index >= 0) {
        if (tab_contexts[current_tab_index]->can_be_closed()) {
            tab_contexts[current_tab_index]->on_tab_close();
        } else {
            return;
        }
    }

    if (recompui::is_context_shown(modal_root_context)) {
        set_display(Display::None);
        recompui::hide_context(modal_root_context);
    }

    is_open = false;

    if (on_close_callback != nullptr) {
        on_close_callback();
    }
}

void Modal::process_event(const Event &e) {
    switch (e.type) {
        case EventType::Update: {
            if (previous_tab_index != current_tab_index) {
                body->clear_children();
                Element* tab_contents = tab_contexts[current_tab_index]->create_tab_contents(modal_root_context, body);
                previous_tab_index = current_tab_index;
            }
            queue_update();
            break;
        }
        case EventType::MenuAction: {
            auto action = std::get<EventMenuAction>(e.variant).action;
            switch (action) {
                case MenuAction::Accept:
                    break;
                case MenuAction::Apply:
                    break;
                case MenuAction::Back: {
                    if (tabs != nullptr && tabs->get_active_tab_element() != nullptr) {
                        tabs->get_active_tab_element()->focus();
                    }
                    break;
                }
                case MenuAction::Toggle: {
                    close();
                    break;
                }
                case MenuAction::TabLeft: {
                    navigate_tab_direction(-1);
                    break;
                }
                case MenuAction::TabRight: {
                    navigate_tab_direction(1);
                    break;
                }
            }
            break;
        }
    }
}

void Modal::navigate_tab_direction(int direction) {
    if (tab_contexts.size() == 0) {
        return;
    }
    int next_index = current_tab_index + direction;
    if (next_index < 0) {
        next_index = tab_contexts.size() - 1;
    } else if (next_index >= tab_contexts.size()) {
        next_index = 0;
    }
    set_selected_tab(next_index);
    if (tabs != nullptr && tabs->get_active_tab_element() != nullptr) {
        tabs->get_active_tab_element()->focus();
    }
}

void Modal::set_menu_action_callback(MenuAction action, std::function<void()> callback) {
    menu_action_callbacks[action] = callback;
}

void Modal::on_tab_change(int tab_index) {
    if (current_tab_index < tab_contexts.size() && current_tab_index >= 0) {
        if (tab_contexts[current_tab_index]->can_be_closed()) {
            tab_contexts[current_tab_index]->on_tab_close();
            current_tab_index = tab_index;
        } else if (tabs != nullptr) {
            tabs->set_active_tab(current_tab_index);
        }
    } else {
        current_tab_index = tab_index;
    }

    if (current_tab_index < tab_contexts.size() && current_tab_index >= 0 && tabs != nullptr) {
        tab_contexts[current_tab_index]->set_nav_up(tabs->get_active_tab_element());
    }

    // body->clear_children();
    // Element* tab_contents = tab_contexts[tab_index]->create_tab_contents(modal_root_context, body);
}

void Modal::set_selected_tab(int tab_index) {
    if (tabs != nullptr) {
        // calls Modal::on_tab_change internally
        tabs->set_active_tab(tab_index);
    }
}

void Modal::add_tab(TabContext *tab_context) {
    tab_context->set_parent_modal(this);
    tab_context->on_init();
    tab_contexts.push_back(tab_context);
    ContextId context = get_current_context();
    if (tabs == nullptr) {
        header->set_padding_left(0.0f); // tabs hug to left side
        tabs = context.create_element<TabSet>(header->get_left());
        tabs->set_change_tab_callback([this](int tab_index) {
            this->on_tab_change(tab_index);
        });
    }
    tabs->add_tab(tab_context->tab_name);
}

Modal *Modal::create_modal(ModalType modal_type) {
    ContextId new_context = recompui::create_context();
    new_context.open();
    auto doc = new_context.get_root_element();
    Modal *modal = new_context.create_element<Modal>(doc, new_context, modal_type);
    new_context.close();
    return modal;
}

void Modal::set_first_focusable_below_tabs(Element *element) {
    if (tabs != nullptr) {
        tabs->set_nav_down(element);
    }
}

void Modal::set_on_close_callback(std::function<void()> callback) {
    on_close_callback = callback;
}

void TabContext::apply_set_first_focusable_below_tabs_callback() {
    set_first_focusable_below_tabs = [this](Element *element) {
        if (this->parent_modal != nullptr) {
            this->parent_modal->set_first_focusable_below_tabs(element);
        }
    };
}

} // namespace recompui
