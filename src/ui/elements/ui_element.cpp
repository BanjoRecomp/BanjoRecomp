#include "RmlUi/Core/StringUtilities.h"

#include "overloaded.h"
#include "recomp_ui.h"
#include "ui_element.h"
#include "../core/ui_context.h"

#include <cassert>

namespace recompui {

Element *Element::get_nav_parent() {
    Element *cur = parent;
    while (cur != nullptr) {
        if (cur->is_nav_container) {
            return cur;
        }
        cur = cur->parent;
    }
    return nullptr;
}

void Element::set_as_navigation_container(NavigationType nav_type) {
    is_nav_container = true;
    this->nav_type = nav_type;

    // set_border_left_width(1.0f);
    // set_border_left_color(theme::color::Warning);
    Element *parent_nav = get_nav_parent();
    if (parent_nav != nullptr) {
        parent_nav->nav_children.push_back(this);
    }
}

void Element::set_as_primary_focus(bool is_primary_focus) {
    this->is_primary_focus = is_primary_focus;
    // if (is_primary_focus) {
    //     set_border_right_width(1.0f);
    //     set_border_right_color(theme::color::SuccessD);
    // } else {
    //     set_border_right_width(1.0f);
    //     set_border_right_color(theme::color::WarningD);
    // }
}

struct RmlPosSize {
    Rml::Vector2f position;
    Rml::Vector2f size;
    Rml::Vector2f center;

    RmlPosSize(Rml::Vector2f position, const Rml::Box& box) : position(position), size(box.GetSize()) {
        center = position + (size * 0.5f);
    }

    float get_distance_sq(const RmlPosSize& other) const {
        auto delta = center - other.center;
        return delta.SquaredMagnitude();
    }

    bool can_navigate_vertical(const RmlPosSize& other, int dir) const {
        if (position.x + size.x < other.position.x + 1) {
            return false;
        }
        if (position.x + 1 > other.position.x + other.size.x) {
            return false;
        }

        // bool aligns_horizontally = (
        //     std::abs(position.x - other.position.x) < 5 &&
        //     std::abs((position.x + size.x) - (other.position.x + other.size.x)) < 5
        // );
        // bool aligns_horizontally = (
        //     // left side of this box is to the left of the other
        //     position.x < other.position.x + other.size.x &&
        //     // right side of this box is to the right of the other
        //     position.x + size.x > other.position.x
        // );

        // if (!aligns_horizontally) {
        //     return false;
        // }

        if (dir == 1) {
            // Navigating down
            return is_above(other);
        } else {
            // Navigating up
            return is_below(other);
        }
    }

    bool can_navigate_horizontal(const RmlPosSize& other, int dir) const {
        if (std::abs(position.x - other.position.x) < 5) {
            return false;
        }

        bool aligns_vertically = (
            // top side of this box is above the other
            position.y < other.position.y + other.size.y &&
            // bottom side of this box is below the other
            position.y + size.y > other.position.y
        );

        if (!aligns_vertically) {
            return false;
        }

        if (dir == 1) {
            // Navigating right
            return is_left_of(other);
        } else {
            // Navigating left
            return is_right_of(other);
        }
    }

    bool is_left_of(const RmlPosSize& other) const {
        return position.x <= other.position.x;
    }
    bool is_right_of(const RmlPosSize& other) const {
        return position.x >= other.position.x;
    }
    bool is_above(const RmlPosSize& other) const {
        return position.y <= other.position.y;
    }
    bool is_below(const RmlPosSize& other) const {
        return position.y >= other.position.y;
    }
    float vertical_distance(const RmlPosSize& other) const {
        return std::min(
            std::abs(position.y - (other.position.y + other.size.y)),
            std::abs((position.y + size.y) - other.position.y)
        );
    }
    float horizontal_distance(const RmlPosSize& other) const {
        return std::min(
            std::abs(position.x - (other.position.x + other.size.x)),
            std::abs((position.x + size.x) - other.position.x)
        );
    }
};

Element::CanFocus Element::is_focusable() {
    if (!base->IsVisible()) {
        return CanFocus::NoAndNoChildren;
    }

    if (enabled == false) {
        return CanFocus::NoAndNoChildren;
    }

    const Rml::ComputedValues& computed = base->GetComputedValues();

    if (computed.focus() == Rml::Style::Focus::None) {
        return CanFocus::NoAndNoChildren;
    }

    if (computed.tab_index() == Rml::Style::TabIndex::Auto) {
        return CanFocus::Yes;
    }

    return CanFocus::No;
}
Element *Element::get_first_focusable_child() {
    for (auto child : children) {
        CanFocus res = child->is_focusable();
        if (res == CanFocus::Yes) {
            return child;
        } else if (res == CanFocus::NoAndNoChildren) {
            continue; // Skip this child, it has no focusable children.
        } else {
            Element *focusable_child = child->get_first_focusable_child();
            if (focusable_child != nullptr) {
                return focusable_child;
            }
        }
    }

    return nullptr;
}

int get_element_index(Element *el, std::vector<Element *> elements) {
    for (int i = 0; i < static_cast<int>(elements.size()); i++) {
        if (el == elements[i]) {
            return i;
        }
    }
    return -1;
}

Element *Element::try_grid_navigation(
    int nav_dir,
    int cur_element_index
) {
    auto *grid_parent = get_nav_parent();
    NavigationType parent_check_type = nav_type == NavigationType::GridCol ? NavigationType::GridRow : NavigationType::GridCol;
    if (grid_parent == nullptr || grid_parent->nav_type != parent_check_type) {
        return nullptr;
    }

    int parent_element_index = get_element_index(this, grid_parent->nav_children);
    if (parent_element_index < 0) {
        return nullptr;
    }

    int next_grid_index = parent_element_index + nav_dir;
    if (next_grid_index >= 0 && next_grid_index < grid_parent->nav_children.size()) {
        Element *adjacent = grid_parent->nav_children[next_grid_index];
        if (cur_element_index >= 0 && cur_element_index < adjacent->nav_children.size()) {
            return adjacent->nav_children[cur_element_index];
        }
    }

    return nullptr;
}

Element *Element::try_get_nav_direction(
    int vertical_nav,
    int horizontal_nav,
    Element *cur_nav_child
) {

    int cur_element_index = get_element_index(cur_nav_child, nav_children);
    if (cur_element_index == -1) {
        printf("ERROR: could not find nav child!\n");
        return nullptr;
    }

    int num_nav_children = static_cast<int>(nav_children.size());
    int next_nav_index = -1;

    switch (nav_type) {
        case NavigationType::Auto:
            break;
        case NavigationType::GridCol:
            if (horizontal_nav != 0) {
                return try_grid_navigation(horizontal_nav, cur_element_index);
            }
            // fallthrough
        case NavigationType::Vertical:
            if (vertical_nav == 0) {
                return nullptr;
            }
            cur_element_index += vertical_nav;
            if (cur_element_index >= 0 && cur_element_index < num_nav_children) {
                return nav_children[cur_element_index];
            }
            return nullptr;
        case NavigationType::GridRow:
            if (vertical_nav != 0) {
                return try_grid_navigation(vertical_nav, cur_element_index);
            }
            // fallthrough
        case NavigationType::Horizontal:
            if (horizontal_nav == 0) {
                return nullptr;
            }
            cur_element_index += horizontal_nav;
            if (cur_element_index >= 0 && cur_element_index < num_nav_children) {
                return nav_children[cur_element_index];
            }
            return nullptr;
    }

    auto focused_box = cur_nav_child->base->GetBox();
    RmlPosSize this_pos_size(cur_nav_child->base->GetAbsoluteOffset(), focused_box);

    Element *next_focus = nullptr;
    if (nav_children.size() > 0) {
        float best_dist = std::numeric_limits<float>::max();
        for (auto &nav_sibling : nav_children) {
            if (nav_children[cur_element_index] == nav_sibling) {
                continue;
            }

            RmlPosSize sibling_pos_size(nav_sibling->base->GetAbsoluteOffset(), nav_sibling->base->GetBox());
            if (vertical_nav != 0 && this_pos_size.can_navigate_vertical(sibling_pos_size, vertical_nav)) {
                float dist = this_pos_size.vertical_distance(sibling_pos_size);
                if (dist < best_dist) {
                    best_dist = dist;
                    next_focus = nav_sibling;
                }
            } else if (horizontal_nav != 0 && this_pos_size.can_navigate_horizontal(sibling_pos_size, horizontal_nav)) {
                float dist = this_pos_size.horizontal_distance(sibling_pos_size);
                if (dist < best_dist) {
                    best_dist = dist;
                    next_focus = nav_sibling;
                }
            }
        }
    }

    return next_focus;
}

void Element::get_all_focusable_children(Element *nav_parent) {
    for (auto child : children) {
        CanFocus res = child->is_focusable();
        if (res == CanFocus::Yes) {
            nav_parent->nav_children.push_back(child);
        } else if (res == CanFocus::NoAndNoChildren) {
            continue; // Skip this child, it has no focusable children.
        } else {
            child->get_all_focusable_children(nav_parent);
        }
    }
}

void Element::build_navigation(Element *nav_parent, Element *cur_focus_element) {
    if (!base->IsVisible()) {
        return;
    }

    for (auto &child : children) {
        if (child == cur_focus_element) {
            nav_parent->nav_children.push_back(child);
            continue;
        }
        if (!child->base->IsVisible() || !child->enabled) {
            continue;
        }

        if (child->is_focusable() == CanFocus::Yes) {
            // End of the line because it is focusable itself
            nav_parent->nav_children.push_back(child);
        } else if (child->is_nav_container) {
            nav_parent->nav_children.push_back(child);
            child->nav_children.clear();
            child->build_navigation(child, cur_focus_element);
            // didn't find any nav children, check for focus elements
            if (child->nav_children.size() == 0) {
                child->get_all_focusable_children(child);
            }
            // didn't find any focus elements
            if (child->nav_children.size() == 0) {
                nav_parent->nav_children.pop_back();
            }
        } else {
            child->build_navigation(nav_parent, cur_focus_element);
        }
    }
}

Element *Element::get_closest_element(std::vector<Element *> &elements) {
    if (elements.empty()) {
        return nullptr;
    }

    auto this_box = base->GetBox();
    RmlPosSize this_pos_size(base->GetAbsoluteOffset(), this_box);
    Element *closest = nullptr;
    float closest_distance = std::numeric_limits<float>::max();
    for (auto &element : elements) {
        auto element_box = element->base->GetBox();
        RmlPosSize element_pos_size(element->base->GetAbsoluteOffset(), element_box);
        float distance = this_pos_size.get_distance_sq(element_pos_size);
        if (distance < closest_distance) {
            closest_distance = distance;
            closest = element;
        }
    }

    return closest;
}

bool Element::handle_navigation_event(Rml::Event &event) {
    if (
        is_root_document &&
        event.GetId() == Rml::EventId::Keydown &&
        event.GetPhase() == Rml::EventPhase::Bubble &&
        event.IsPropagating()
    ) {
        int key_identifier = event.GetParameter<int>("key_identifier", Rml::Input::KI_UNKNOWN);
        if (
            key_identifier != Rml::Input::KI_LEFT &&
            key_identifier != Rml::Input::KI_RIGHT &&
            key_identifier != Rml::Input::KI_UP &&
			key_identifier != Rml::Input::KI_DOWN
        ) {
            return false;
        }
        std::string nav_key_name;
        switch (key_identifier) {
            case Rml::Input::KI_UP:    nav_key_name = "up";    break;
            case Rml::Input::KI_DOWN:  nav_key_name = "down";  break;
            case Rml::Input::KI_LEFT:  nav_key_name = "left";  break;
            case Rml::Input::KI_RIGHT: nav_key_name = "right"; break;
        }
        printf("Navigation key pressed: %-5s (%d)\n", nav_key_name.c_str(), key_identifier);

        int vertical_nav = 0;
        int horizontal_nav = 0;
        switch (key_identifier) {
            case Rml::Input::KI_UP:    vertical_nav   = -1; break;
            case Rml::Input::KI_DOWN:  vertical_nav   =  1; break;
            case Rml::Input::KI_LEFT:  horizontal_nav = -1; break;
            case Rml::Input::KI_RIGHT: horizontal_nav =  1; break;
        }

        auto ctx = get_current_context();
        auto navigation_from_element = ctx.get_focused_element();
        if (navigation_from_element == nullptr) {
            printf("    No focused element, cannot navigate.\n");
            return false;
        }
        nav_children.clear();
        build_navigation(this, navigation_from_element);
        printf("ROOT\n");
        print_nav_hierarchy(0);

        Element *nav_parent = navigation_from_element->get_nav_parent();
        while (nav_parent != nullptr) {
            Element *next_focus = nav_parent->try_get_nav_direction(
                vertical_nav,
                horizontal_nav,
                navigation_from_element
            );

            // If found, dive into the next focus element until finding the bottom most nav container
            if (next_focus != nullptr) {
                while (next_focus->nav_children.size() > 0) {
                    bool found_primary = false;
                    for (auto &child : next_focus->nav_children) {
                        if (child->is_primary_focus) {
                            next_focus = child;
                            found_primary = true;
                            printf("    Found primary focus child: %s\n", child->get_id().c_str());
                            break;
                        }
                    }
                    if (!found_primary) {
                        next_focus = navigation_from_element->get_closest_element(next_focus->nav_children);
                    }
                }
            }

            if (next_focus != nullptr) {
                event.StopPropagation();
                next_focus->focus();
                printf("    Navigated to next focus element: %s\n", next_focus->get_id().c_str());
                return true;
            }

            navigation_from_element = nav_parent;
            nav_parent = nav_parent->get_nav_parent();
            printf("    No next focus found, moving up to parent: %s\n", navigation_from_element->get_id().c_str());
        }

        printf("    No next focus found, reached root document.\n");

        return false;
    } else {
        return false;
    }
}

Element::Element(Rml::Element *base) {
    assert(base != nullptr);

    this->base = base;
    this->base_owning = {};
    this->shim = true;
}

Element::Element(Element* parent, uint32_t events_enabled, Rml::String base_class, bool can_set_text) : can_set_text(can_set_text) {
    ContextId context = get_current_context();
    base_owning = context.get_document()->CreateElement(base_class);

    if (parent != nullptr) {
        base = parent->base->AppendChild(std::move(base_owning));
        parent->add_child(this);
        this->parent = parent;
    }
    else {
        base = base_owning.get();
    }

    set_display(Display::Block);
    set_property(Rml::PropertyId::BoxSizing, Rml::Style::BoxSizing::BorderBox);

    register_event_listeners(events_enabled);
}

Element::~Element() {
    if (!shim) {
        clear_children();
        if (!base_owning) {
            base->GetParentNode()->RemoveChild(base);
        }
    }
}

void Element::add_child(Element *child) {
    assert(child != nullptr);
    if (can_set_text) {
        assert(false && "Elements with settable text cannot have children");
        return;
    }

    children.emplace_back(child);

    if (shim) {
        ContextId context = get_current_context();
        context.add_loose_element(child);
    }
}

void Element::set_parent(Element *new_parent) {
    if (parent != nullptr) {
        parent->remove_child(this, false);
        base_owning = parent->base->RemoveChild(base);

        parent = new_parent;
    
        base = parent->base->AppendChild(std::move(base_owning), true);
        parent->add_child(this);
    }

}

void Element::set_property(Rml::PropertyId property_id, const Rml::Property &property) {
    assert(base != nullptr);

    base->SetProperty(property_id, property);
    Style::set_property(property_id, property);
}

void Element::register_event_listeners(uint32_t events_enabled) {
    assert(base != nullptr);

    this->events_enabled = events_enabled;

    if (events_enabled & Events(EventType::Click)) {
        base->AddEventListener(Rml::EventId::Click, this);
    }

    if (events_enabled & Events(EventType::MouseButton)) {
        base->AddEventListener(Rml::EventId::Mousedown, this);
        base->AddEventListener(Rml::EventId::Mouseup, this);
    }

    if (events_enabled & Events(EventType::Focus)) {
        base->AddEventListener(Rml::EventId::Focus, this);
        base->AddEventListener(Rml::EventId::Blur, this);
    }

    if (events_enabled & Events(EventType::Hover)) {
        base->AddEventListener(Rml::EventId::Mouseover, this);
        base->AddEventListener(Rml::EventId::Mouseout, this);
    }

    if (events_enabled & Events(EventType::Drag)) {
        base->AddEventListener(Rml::EventId::Drag, this);
        base->AddEventListener(Rml::EventId::Dragstart, this);
        base->AddEventListener(Rml::EventId::Dragend, this);
    }

    if (events_enabled & Events(EventType::Text)) {
        base->AddEventListener(Rml::EventId::Change, this);
    }

    if (events_enabled & Events(EventType::Navigate, EventType::MenuAction)) {
        base->AddEventListener(Rml::EventId::Keydown, this);
    }
}

void Element::apply_style(Style *style) {
    for (auto it : style->property_map) {
        // Skip redundant SetProperty calls to prevent dirtying unnecessary state.
        // This avoids expensive layout operations when a simple color-only style is applied.
        const Rml::Property* cur_value = base->GetLocalProperty(it.first);
        if (cur_value == nullptr || *cur_value != it.second) {
            base->SetProperty(it.first, it.second);
        }
    }
}

void Element::apply_styles() {
    apply_style(this);

    for (size_t i = 0; i < styles_counter.size(); i++) {
        if (styles_counter[i] == 0) {
            apply_style(styles[i]);
        }
    }
}

void Element::propagate_disabled(bool disabled) {
    disabled_from_parent = disabled;

    bool attribute_state = disabled_from_parent || !enabled;
    if (disabled_attribute != attribute_state) {
        disabled_attribute = attribute_state;
        if (disabled_attribute) {
            base->SetAttribute("disabled", true);
        } else {
            base->RemoveAttribute("disabled");
        }

        if (events_enabled & Events(EventType::Enable)) {
            handle_event(Event::enable_event(!attribute_state));
        }

        for (auto &child : children) {
            child->propagate_disabled(attribute_state);
        }
    }
}

void Element::handle_event(const Event& event) {
    for (const auto& callback : callbacks) {
        recompui::queue_ui_callback(resource_id, event, callback);
    }

    process_event(event);
}

void Element::set_id(const std::string& new_id) {
    id = new_id;
    base->SetId(new_id);
}

recompui::MouseButton convert_rml_mouse_button(int button) {
    switch (button) {
        case 0:
            return recompui::MouseButton::Left;
        case 1:
            return recompui::MouseButton::Right;
        case 2:
            return recompui::MouseButton::Middle;
        default:
            return recompui::MouseButton::Count;
    }
}

void Element::ProcessEvent(Rml::Event &event) {
    ContextId prev_context = recompui::try_close_current_context();
    ContextId context = ContextId::null();
    Rml::ElementDocument* doc = event.GetTargetElement()->GetOwnerDocument();
    if (doc != nullptr) {
        context = get_context_from_document(doc);
    }

    bool did_open = false;

    // TODO disallow null contexts once the entire UI system has been migrated.
    if (context != ContextId::null()) {
        did_open = context.open_if_not_already();
    }

    // Events that are processed during any phase.
    switch (event.GetId()) {
    case Rml::EventId::Click:
        handle_event(Event::click_event(event.GetParameter("mouse_x", 0.0f), event.GetParameter("mouse_y", 0.0f)));
        break;
    case Rml::EventId::Mousedown:
        {
            MouseButton mouse_button = convert_rml_mouse_button(event.GetParameter("button", 3));
            if (mouse_button != MouseButton::Count) {
                handle_event(Event::mousebutton_event(event.GetParameter("mouse_x", 0.0f), event.GetParameter("mouse_y", 0.0f), mouse_button, true));
            }
        }
        break;
    case Rml::EventId::Mouseup:
        {
            MouseButton mouse_button = convert_rml_mouse_button(event.GetParameter("button", 3));
            if (mouse_button != MouseButton::Count) {
                handle_event(Event::mousebutton_event(event.GetParameter("mouse_x", 0.0f), event.GetParameter("mouse_y", 0.0f), mouse_button, false));
            }
        }
        break;
    case Rml::EventId::Keydown: {
        auto rml_key = (Rml::Input::KeyIdentifier)event.GetParameter<int>("key_identifier", 0);
        if (events_enabled & Events(EventType::Navigate)) {
            if (handle_navigation_event(event)) {
                break;
            }

            switch (rml_key) {
                case Rml::Input::KeyIdentifier::KI_LEFT:
                    handle_event(Event::navigate_event(NavDirection::Left));
                    break;
                case Rml::Input::KeyIdentifier::KI_UP:
                    handle_event(Event::navigate_event(NavDirection::Up));
                    break;
                case Rml::Input::KeyIdentifier::KI_RIGHT:
                    handle_event(Event::navigate_event(NavDirection::Right));
                    break;
                case Rml::Input::KeyIdentifier::KI_DOWN:
                    handle_event(Event::navigate_event(NavDirection::Down));
                    printf("Navigate down\n");
                    break;
            }
        }

        if (events_enabled & Events(EventType::MenuAction)) {
            MenuAction action = menu_action_mapping::menu_action_from_rml_key(rml_key);
            if (action != MenuAction::None) {
                handle_event(Event::menu_action_event(action));
            }
        }

        break;
    }
    case Rml::EventId::Drag:
        handle_event(Event::drag_event(event.GetParameter("mouse_x", 0.0f), event.GetParameter("mouse_y", 0.0f), DragPhase::Move));
        break;
    default:
        break;
    }

    // Events that are only processed during the Target phase.
    if (event.GetPhase() == Rml::EventPhase::Target) {
        switch (event.GetId()) {
        case Rml::EventId::Mouseover:
            handle_event(Event::hover_event(true));
            break;
        case Rml::EventId::Mouseout:
            handle_event(Event::hover_event(false));
            break;
        case Rml::EventId::Focus:
            handle_event(Event::focus_event(true));
            break;
        case Rml::EventId::Blur:
            handle_event(Event::focus_event(false));
            break;
        case Rml::EventId::Dragstart:
            handle_event(Event::drag_event(event.GetParameter("mouse_x", 0.0f), event.GetParameter("mouse_y", 0.0f), DragPhase::Start));
            break;
        case Rml::EventId::Dragend:
            handle_event(Event::drag_event(event.GetParameter("mouse_x", 0.0f), event.GetParameter("mouse_y", 0.0f), DragPhase::End));
            break;
        case Rml::EventId::Change: {
            if (events_enabled & Events(EventType::Text)) {
                Rml::Variant *value_variant = base->GetAttribute("value");
                if (value_variant != nullptr) {
                    handle_event(Event::text_event(value_variant->Get<std::string>()));
                }
            }

            break;
        }
        default:
            break;
        }
    }

    if (context != ContextId::null() && did_open) {
        context.close();
    }

    if (prev_context != ContextId::null()) {
        prev_context.open();
    }
}

void Element::set_as_root_document(NavigationType nav_type) {
    is_root_document = true;
    is_nav_container = true;
    this->nav_type = nav_type;

    register_event_listeners(recompui::Events(recompui::EventType::Navigate, recompui::EventType::Focus));
}

void Element::set_attribute(const Rml::String &attribute_key, const Rml::String &attribute_value) {
    base->SetAttribute(attribute_key, attribute_value);
}

void Element::process_event(const Event &) {
    // Does nothing by default.
}

void Element::enable_focus() {
    set_tab_index_auto();
    set_focusable(true);
    set_nav_auto(NavDirection::Up);
    set_nav_auto(NavDirection::Down);
    set_nav_auto(NavDirection::Left);
    set_nav_auto(NavDirection::Right);
}

void Element::clear_children() {
    if (children.empty()) {
        return;
    }
    ContextId context = get_current_context();

    // Remove the children from the context.
    for (Element* child : children) {
        context.destroy_resource(child);
    }

    // Clear the child list.
    children.clear();
}

bool Element::remove_child(ResourceId child, bool remove_from_context) {
    bool found = false;

    ContextId context = get_current_context();

    for (auto it = children.begin(); it != children.end(); ++it) {
        Element* cur_child = *it;
        if (cur_child->get_resource_id() == child) {
            children.erase(it);
            if (remove_from_context) {
                context.destroy_resource(cur_child);
            }
            found = true;
            break;
        }
    }

    return found;
}

void Element::add_style(Style *style, const std::string_view style_name) {
    add_style(style, { style_name });
}

void Element::add_style(Style *style, const std::initializer_list<std::string_view> &style_names) {
    for (const std::string_view &style_name : style_names) {
        style_name_index_map.emplace(style_name, styles.size());
    }

    styles.emplace_back(style);

    uint32_t initial_style_counter = style_names.size();
    for (const std::string_view &style_name : style_names) {
        if (style_active_set.find(style_name) != style_active_set.end()) {
            initial_style_counter--;
        }
    }

    styles_counter.push_back(initial_style_counter);
}

void Element::set_enabled(bool enabled) {
    this->enabled = enabled;

    propagate_disabled(disabled_from_parent);
}

bool Element::is_enabled() const {
    return enabled && !disabled_from_parent;
}

// Adapted from RmlUi's `EncodeRml`.
std::string escape_rml(std::string_view string)
{
	std::string result;
	result.reserve(string.size());
	for (char c : string)
	{
		switch (c)
		{
		case '<': result += "&lt;"; break;
		case '>': result += "&gt;"; break;
		case '&': result += "&amp;"; break;
		case '"': result += "&quot;"; break;
        case '\n': result += "<br/>"; break;
		default: result += c; break;
		}
	}
	return result;
}

void Element::set_text(std::string_view text) {
    if (can_set_text) {
        // Queue the text update. If it's applied immediately, it might happen
        // while the document is being updated or rendered. This can cause a crash
        // due to the child elements being deleted while the document is being updated.
        // Queueing them defers it to the update thread, which prevents that issue.
        // Escape the string into Rml to prevent element injection.
        get_current_context().queue_set_text(this, escape_rml(text));
    }
    else {
        assert(false && "Attempted to set text of an element that cannot have its text set.");
    }
}

void Element::set_text_unsafe(std::string_view text) {
    if (can_set_text) {
        get_current_context().queue_set_text(this, std::string(text));
    }
    else {
        assert(false && "Attempted to set text of an element that cannot have its text set.");
    }
}

std::string Element::get_input_text() {
    return base->GetAttribute("value", std::string{});
}

void Element::set_input_text(std::string_view val) {
    base->SetAttribute("value", std::string{ val });
}

void Element::set_src(std::string_view src) {
    base->SetAttribute("src", std::string(src));
}

void Element::set_style_enabled(std::string_view style_name, bool enable) {
    if (enable && style_active_set.find(style_name) == style_active_set.end()) {
        // Style was disabled and will be enabled.
        style_active_set.emplace(style_name);

    }
    else if (!enable && style_active_set.find(style_name) != style_active_set.end()) {
        // Style was enabled and will be disabled.
        style_active_set.erase(style_name);
    }
    else {
        // Do nothing.
        return;
    }

    auto range = style_name_index_map.equal_range(style_name);
    for (auto it = range.first; it != range.second; it++) {
        if (enable) {
            styles_counter[it->second]--;
        }
        else {
            styles_counter[it->second]++;
        }
    }

    apply_styles();
}

bool Element::is_style_enabled(std::string_view style_name) {
    return style_active_set.contains(style_name);
}

float Element::get_absolute_left() {
    return base->GetAbsoluteLeft();
}

float Element::get_absolute_top() {
    return base->GetAbsoluteTop();
}

float Element::get_client_left() {
    return base->GetClientLeft();
}

float Element::get_client_top() {
    return base->GetClientTop();
}

float Element::get_client_width() {
    return base->GetClientWidth();
}

float Element::get_client_height() {
    return base->GetClientHeight();
}

uint32_t Element::get_input_value_u32() {
    ElementValue value = get_element_value();
    
    return std::visit(overloaded {
        [](double d) { return (uint32_t)d; },
        [](float f) { return (uint32_t)f; },
        [](uint32_t u) { return u; },
        [](std::monostate) { return 0U; }
    }, value);
}

float Element::get_input_value_float() {
    ElementValue value = get_element_value();
    
    return std::visit(overloaded {
        [](double d) { return (float)d; },
        [](float f) { return f; },
        [](uint32_t u) { return (float)u; },
        [](std::monostate) { return 0.0f; }
    }, value);
}

double Element::get_input_value_double() {
    ElementValue value = get_element_value();
    
    return std::visit(overloaded {
        [](double d) { return d; },
        [](float f) { return (double)f; },
        [](uint32_t u) { return (double)u; },
        [](std::monostate) { return 0.0; }
    }, value);
}

void Element::focus() {
    base->Focus();
}

void Element::blur() {
    base->Blur();
}

void Element::queue_update() {
    ContextId cur_context = get_current_context();

    // TODO disallow null contexts once the entire UI system has been migrated.
    if (cur_context == ContextId::null()) {
        return;
    }    

    cur_context.queue_element_update(resource_id);
}

void Element::register_callback(ContextId context, PTR(void) callback, PTR(void) userdata) {
    callbacks.emplace_back(UICallback{.context = context, .callback = callback, .userdata = userdata});
}

Element *Element::select_add_option(std::string_view text, std::string_view value) {
    if (base->GetTagName() != "select") {
        return nullptr;
    }

    Rml::ElementFormControlSelect* select = (Rml::ElementFormControlSelect *)(base);
    if (!select) {
        return nullptr;
    }

    ContextId context = get_current_context();
    Element *option_element = context.create_element<Element>(this, 0, "option", true);
    option_element->set_text(text);
    option_element->set_input_text(value);

    return option_element;
}

void Element::select_set_selection(std::string_view option_value) {
    if (base->GetTagName() != "select") {
        return;
    }

    Rml::ElementFormControlSelect* select = (Rml::ElementFormControlSelect *)(base);
    if (!select) {
        return;
    }
    select->SetValue(std::string(option_value));
}

Element Element::get_element_with_tag_name(std::string_view tag_name) {
    for (int i = 0; i < base->GetNumChildren(true); i++) {
        Rml::Element* child = base->GetChild(i);
        if (child->GetTagName() == tag_name) {
            return Element(child);
        }
    }
    throw std::runtime_error("Select element has no child with the specified tag name");
}

bool Element::is_pseudo_class_set(Rml::String pseudo_class) {
    return base->IsPseudoClassSet(pseudo_class);
}

void Element::scroll_into_view() {
    if (base == nullptr) {
        return;
    }

    Rml::ScrollIntoViewOptions options;
    options.vertical = Rml::ScrollAlignment::Nearest;

    base->ScrollIntoView(options);
}

} // namespace recompui
