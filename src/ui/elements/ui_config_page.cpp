#include "ui_config_page.h"
#include "ui_theme.h"
#include "ui_container.h"


namespace recompui {
    static const float headerFooterPaddingVert = 20.0f;
    static const float headerFooterPaddingHorz = 20.0f;
    
    static void set_header_footer_side_styles(Element *el) {
        el->set_align_items(AlignItems::Center);
        // el->set_width(100.0f, Unit::Percent);
        el->set_width_auto();
        el->set_height_auto();
        el->set_flex_basis_auto();
        el->set_gap(8.0f);
    }

    ConfigHeaderFooter::ConfigHeaderFooter(Element *parent, bool is_header) : Element(parent, 0, "div", false) {
        set_display(Display::Flex);
        set_position(Position::Relative);
        set_flex_direction(FlexDirection::Row);
        set_align_items(AlignItems::Center);
        set_justify_content(JustifyContent::SpaceBetween);
        set_width(100.0f, Unit::Percent);
        set_height_auto();

        set_padding_top(headerFooterPaddingVert);
        set_padding_bottom(headerFooterPaddingVert);
        set_padding_left(headerFooterPaddingHorz);
        set_padding_right(headerFooterPaddingHorz);

        set_background_color(theme::color::BGShadow);

        const float border_width = theme::border::width;
        const recompui::theme::color border_color = theme::color::BorderSoft;
        if (is_header) {
            set_border_bottom_width(border_width);
            set_border_bottom_color(border_color);
        } else {
            set_border_top_width(border_width);
            set_border_top_color(border_color);
            set_border_bottom_left_radius(theme::border::radius_lg);
            set_border_bottom_right_radius(theme::border::radius_lg);
        }

        ContextId context = get_current_context();
        left = context.create_element<Container>(this, FlexDirection::Row, JustifyContent::FlexStart, 0);
        set_header_footer_side_styles(left);

        right = context.create_element<Container>(this, FlexDirection::Row, JustifyContent::FlexEnd, 0);
        set_header_footer_side_styles(right);
    }

    void ConfigHeaderFooter::hide() {
        set_display(Display::None);
    }

    void ConfigHeaderFooter::show() {
        set_display(Display::Flex);
    }
    
    static void set_config_body_side_styles(Element *el) {
        el->set_flex_grow(1.0f);
        el->set_flex_shrink(1.0f);
        el->set_flex_basis(100.0f, Unit::Percent);
        el->set_width_auto();
        el->set_height_auto();
        el->set_padding(16);
    }

    ConfigBody::ConfigBody(Element *parent) : Element(parent, 0, "div", false) {
        set_display(Display::Flex);
        set_position(Position::Relative);
        set_flex_grow(1.0f);
        set_flex_shrink(1.0f);
        set_flex_basis_auto();
        set_flex_direction(FlexDirection::Row);
        set_width(100.0f, Unit::Percent);

        ContextId context = get_current_context();
        left = context.create_element<Element>(this, 0, "div", false);
        set_config_body_side_styles(left);

        right = context.create_element<Element>(this, 0, "div", false);
        set_config_body_side_styles(right);
    }

    ConfigPage::ConfigPage(Element *parent, uint32_t events_enabled) : Element(parent, events_enabled, "div", false) {
        set_display(Display::Flex);
        set_position(Position::Relative);

        set_flex_grow(1.0f);
        set_flex_shrink(1.0f);
        set_flex_basis(100.0f, Unit::Percent);

        set_flex_direction(FlexDirection::Column);
        set_justify_content(JustifyContent::SpaceBetween);

        set_width(100.0f, Unit::Percent);
        set_height(100.0f, Unit::Percent);

        ContextId context = get_current_context();

        header = context.create_element<ConfigHeaderFooter>(this, true);
        header->hide();

        body = context.create_element<ConfigBody>(this);
        set_border_bottom_left_radius(theme::border::radius_lg);
        set_border_bottom_right_radius(theme::border::radius_lg);

        footer = context.create_element<ConfigHeaderFooter>(this, false);
        footer->hide();
    }

    ConfigHeaderFooter* ConfigPage::add_header() {
        header->show();
        return header;
    }

    void ConfigPage::hide_header() {
        header->hide();
    }
    
    ConfigHeaderFooter* ConfigPage::add_footer() {
        footer->show();
        set_border_bottom_left_radius(0);
        set_border_bottom_right_radius(0);
        return footer;
    }

    void ConfigPage::hide_footer() {
        footer->hide();
        set_border_bottom_left_radius(theme::border::radius_lg);
        set_border_bottom_right_radius(theme::border::radius_lg);
    }

} // namespace recompui
