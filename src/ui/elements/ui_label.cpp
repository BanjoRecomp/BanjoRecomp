#include "ui_label.h"
#include "ui_theme.h"

#include <cassert>

namespace recompui {

    Label::Label(Element *parent, LabelStyle label_style) : Element(parent, 0U, "div", true) {
        switch (label_style) {
        case LabelStyle::Annotation:
            set_typography(theme::Typography::LabelXS);
            break;
        case LabelStyle::Small:
            set_typography(theme::Typography::Body);
            break;
        case LabelStyle::Normal:
            set_typography(theme::Typography::LabelMD);
            break;
        case LabelStyle::Large:
            set_typography(theme::Typography::Header3);
            break;
        }
    }

    Label::Label(Element *parent, const std::string &text, LabelStyle label_style) : Label(parent, label_style) {
        set_text(text);
    }

    Label::Label(Element *parent, theme::Typography typography) : Element(parent, 0U, "div", true) {
        set_typography(typography);
    }

    Label::Label(Element *parent, const std::string &text, theme::Typography typography) : Label(parent, typography) {
        set_text(text);
    }

};
