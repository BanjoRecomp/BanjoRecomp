#include "ui_config_page_example.h"
#include "elements/ui_button.h"
#include "elements/ui_icon_button.h"
#include "elements/ui_label.h"

namespace recompui {

ElementConfigPageExample::ElementConfigPageExample(const Rml::String& tag) : Rml::Element(tag) {
    SetProperty(Rml::PropertyId::Display, Rml::Style::Display::Block);
    SetProperty("width", "100%");
    SetProperty("height", "100%");
    
    recompui::Element this_compat(this);
    recompui::ContextId context = get_current_context();

    auto config_page = context.create_element<ConfigPage>(&this_compat);

    auto body = config_page->get_body();
    {
        auto body_left = body->get_left();
        context.create_element<Label>(body_left, "First", LabelStyle::Normal);
        context.create_element<Label>(body_left, "Second", LabelStyle::Normal);
        context.create_element<Label>(body_left, "Third", LabelStyle::Normal);
        context.create_element<Label>(body_left, "Fourth", LabelStyle::Normal);
        context.create_element<Label>(body_left, "Fifth", LabelStyle::Normal);
        context.create_element<Label>(body_left, "Sixth", LabelStyle::Normal);
    }
    {
        auto body_right = body->get_right();
        auto right_p = context.create_element<recompui::Element>(body_right, 0, "p", true);
        right_p->set_text("Testing a really long string here that should probably wrap if its actually cool... but it might not be cool? im not sure. but we will need some extra formatting which im forgetting about right now so i guess it'll have to use set inner html or something like that idk.");
    }

    auto header = config_page->add_header();
    {
        auto header_left = header->get_left();
        context.create_element<Button>(header_left, "Hello", ButtonStyle::Primary);
        context.create_element<Button>(header_left, "Second button", ButtonStyle::Secondary);
    }
    {
        auto header_right = header->get_right();
        context.create_element<IconButton>(header_right, "icons/Arrow.svg", ButtonStyle::Tertiary, IconButtonSize::Small);
        context.create_element<IconButton>(header_right, "icons/Reset.svg", ButtonStyle::Danger, IconButtonSize::XLarge);
    }

    auto footer = config_page->add_footer();
    {
        auto footer_left = footer->get_left();
        context.create_element<Button>(footer_left, "Goodbye", ButtonStyle::Warning);
    }
    {
        auto footer_right = footer->get_right();
        context.create_element<Button>(footer_right, "Last button", ButtonStyle::Success);
        context.create_element<IconButton>(footer_right, "icons/X.svg", ButtonStyle::Basic, IconButtonSize::Medium);
    }
}

ElementConfigPageExample::~ElementConfigPageExample() {
}

} // namespace recompui
