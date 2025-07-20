#pragma once

#include "ui_element.h"

namespace recompui {
    class ConfigHeaderFooter : public Element {
    protected:
        Element *left;
        Element *right;
        bool is_header;

        std::string_view get_type_name() override { return "ConfigHeaderFooter"; }
    public:
        ConfigHeaderFooter(Element *parent, bool is_header);
        Element *get_left() { return left; }
        Element *get_right() { return right; }
    };

    class ConfigBody : public Element {
    protected:
        Element *left;
        Element *right;

        std::string_view get_type_name() override { return "ConfigBody"; }
    public:
        ConfigBody(Element *parent);
        Element *get_left() { return left; }
        Element *get_right() { return right; }
    };


    class ConfigPage : public Element {
    protected:
        ConfigHeaderFooter *header = nullptr;
        ConfigBody *body;
        ConfigHeaderFooter *footer = nullptr;

        std::string_view get_type_name() override { return "ConfigPage"; }
    public:
        ConfigPage(Element *parent);
        ConfigHeaderFooter *add_header();
        ConfigHeaderFooter *add_footer();
        ConfigHeaderFooter *get_header() { return header; };
        ConfigBody *get_body() { return body; };
        ConfigHeaderFooter *get_footer() { return footer; };
    private:
    };

} // namespace recompui
