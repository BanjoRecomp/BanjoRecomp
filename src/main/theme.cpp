#include "elements/ui_theme.h"
#include "theme.h"

void recomptheme::set_custom_theme() {
    recompui::theme::set_theme_color(recompui::theme::color::Background1, recompui::Color{10, 10, 11, 255});
    recompui::theme::set_theme_color(recompui::theme::color::Background2, recompui::Color{19, 20, 21, 255});
    recompui::theme::set_theme_color(recompui::theme::color::Background3, recompui::Color{27, 27, 29, 255});
    recompui::theme::set_theme_color(recompui::theme::color::BGOverlay, recompui::Color{199, 200, 204, 26}); // actually the overlay for the modal
    recompui::theme::set_theme_color(recompui::theme::color::ModalOverlay, recompui::Color{10, 10, 11, 242}); // actually the background color of the modal itself
    recompui::theme::set_theme_color(recompui::theme::color::BGShadow, recompui::Color{0, 0, 0, 89});
    recompui::theme::set_theme_color(recompui::theme::color::BGShadow2, recompui::Color{10, 10, 11, 184});
    recompui::theme::set_theme_color(recompui::theme::color::Text, recompui::Color{242, 242, 242, 255});
    recompui::theme::set_theme_color(recompui::theme::color::TextActive, recompui::Color{245, 245, 245, 255});
    recompui::theme::set_theme_color(recompui::theme::color::TextDim, recompui::Color{204, 204, 204, 255});
    recompui::theme::set_theme_color(recompui::theme::color::TextInactive, recompui::Color{255, 255, 255, 153});
    recompui::theme::set_theme_color(recompui::theme::color::TextA5, recompui::Color{242, 242, 242, 13});
    recompui::theme::set_theme_color(recompui::theme::color::TextA20, recompui::Color{242, 242, 242, 51});
    recompui::theme::set_theme_color(recompui::theme::color::TextA30, recompui::Color{242, 242, 242, 77});
    recompui::theme::set_theme_color(recompui::theme::color::TextA50, recompui::Color{242, 242, 242, 128});
    recompui::theme::set_theme_color(recompui::theme::color::TextA80, recompui::Color{242, 242, 242, 204});
    recompui::theme::set_theme_color(recompui::theme::color::Primary, recompui::Color{29, 93, 226, 255});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryL, recompui::Color{167, 191, 241, 255});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryD, recompui::Color{0, 38, 117, 255});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryA5, recompui::Color{29, 93, 226, 13});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryA20, recompui::Color{29, 93, 226, 51});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryA30, recompui::Color{29, 93, 226, 77});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryA50, recompui::Color{29, 93, 226, 128});
    recompui::theme::set_theme_color(recompui::theme::color::PrimaryA80, recompui::Color{29, 93, 226, 204});
    recompui::theme::set_theme_color(recompui::theme::color::Secondary, recompui::Color{247, 158, 8, 255});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryL, recompui::Color{255, 215, 148, 255});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryD, recompui::Color{224, 141, 0, 255});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryA5, recompui::Color{247, 158, 8, 13});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryA20, recompui::Color{247, 158, 8, 51});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryA30, recompui::Color{247, 158, 8, 77});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryA50, recompui::Color{247, 158, 8, 128});
    recompui::theme::set_theme_color(recompui::theme::color::SecondaryA80, recompui::Color{247, 158, 8, 204});
    recompui::theme::set_theme_color(recompui::theme::color::Warning, recompui::Color{255, 254, 0, 255});
    recompui::theme::set_theme_color(recompui::theme::color::WarningL, recompui::Color{255, 254, 143, 255});
    recompui::theme::set_theme_color(recompui::theme::color::WarningD, recompui::Color{197, 163, 2, 255});
    recompui::theme::set_theme_color(recompui::theme::color::WarningA5, recompui::Color{255, 254, 0, 13});
    recompui::theme::set_theme_color(recompui::theme::color::WarningA20, recompui::Color{255, 254, 0, 51});
    recompui::theme::set_theme_color(recompui::theme::color::WarningA30, recompui::Color{255, 254, 0, 77});
    recompui::theme::set_theme_color(recompui::theme::color::WarningA50, recompui::Color{255, 254, 0, 128});
    recompui::theme::set_theme_color(recompui::theme::color::WarningA80, recompui::Color{255, 254, 0, 204});
    recompui::theme::set_theme_color(recompui::theme::color::Danger, recompui::Color{255, 53, 31, 255});
    recompui::theme::set_theme_color(recompui::theme::color::DangerL, recompui::Color{255, 149, 138, 255});
    recompui::theme::set_theme_color(recompui::theme::color::DangerD, recompui::Color{163, 16, 0, 255});
    recompui::theme::set_theme_color(recompui::theme::color::DangerA5, recompui::Color{255, 53, 31, 13});
    recompui::theme::set_theme_color(recompui::theme::color::DangerA20, recompui::Color{255, 53, 31, 51});
    recompui::theme::set_theme_color(recompui::theme::color::DangerA30, recompui::Color{255, 53, 31, 77});
    recompui::theme::set_theme_color(recompui::theme::color::DangerA50, recompui::Color{255, 53, 31, 128});
    recompui::theme::set_theme_color(recompui::theme::color::DangerA80, recompui::Color{255, 53, 31, 204});
    recompui::theme::set_theme_color(recompui::theme::color::Success, recompui::Color{40, 238, 32, 255});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessL, recompui::Color{155, 247, 151, 255});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessD, recompui::Color{18, 157, 12, 255});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessA5, recompui::Color{40, 238, 32, 13});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessA20, recompui::Color{40, 238, 32, 51});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessA30, recompui::Color{40, 238, 32, 77});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessA50, recompui::Color{40, 238, 32, 128});
    recompui::theme::set_theme_color(recompui::theme::color::SuccessA80, recompui::Color{40, 238, 32, 204});
    recompui::theme::set_theme_color(recompui::theme::color::Border, recompui::Color{255, 255, 255, 51});
    recompui::theme::set_theme_color(recompui::theme::color::BorderSoft, recompui::Color{255, 255, 255, 26});
    recompui::theme::set_theme_color(recompui::theme::color::BorderHard, recompui::Color{255, 255, 255, 77});
    recompui::theme::set_theme_color(recompui::theme::color::BorderSolid, recompui::Color{255, 255, 255, 153});
    recompui::theme::set_theme_color(recompui::theme::color::Transparent, recompui::Color{0, 0, 0, 0});
    recompui::theme::set_theme_color(recompui::theme::color::A, recompui::Color{51, 51, 255, 255});
    recompui::theme::set_theme_color(recompui::theme::color::AL, recompui::Color{178, 178, 255, 255});
    recompui::theme::set_theme_color(recompui::theme::color::AD, recompui::Color{32, 32, 172, 255});
    recompui::theme::set_theme_color(recompui::theme::color::AA5, recompui::Color{51, 51, 255, 13});
    recompui::theme::set_theme_color(recompui::theme::color::AA20, recompui::Color{51, 51, 255, 51});
    recompui::theme::set_theme_color(recompui::theme::color::AA30, recompui::Color{51, 51, 255, 77});
    recompui::theme::set_theme_color(recompui::theme::color::AA50, recompui::Color{51, 51, 255, 128});
    recompui::theme::set_theme_color(recompui::theme::color::AA80, recompui::Color{51, 51, 255, 204});
    recompui::theme::set_theme_color(recompui::theme::color::White, recompui::Color{255, 255, 255, 255});
    recompui::theme::set_theme_color(recompui::theme::color::WhiteA5, recompui::Color{255, 255, 255, 13});
    recompui::theme::set_theme_color(recompui::theme::color::WhiteA20, recompui::Color{255, 255, 255, 51});
    recompui::theme::set_theme_color(recompui::theme::color::WhiteA30, recompui::Color{255, 255, 255, 77});
    recompui::theme::set_theme_color(recompui::theme::color::WhiteA50, recompui::Color{255, 255, 255, 128});
    recompui::theme::set_theme_color(recompui::theme::color::WhiteA80, recompui::Color{255, 255, 255, 204});
    recompui::theme::set_theme_color(recompui::theme::color::BW05, recompui::Color{13, 13, 13, 255});
    recompui::theme::set_theme_color(recompui::theme::color::BW10, recompui::Color{26, 26, 26, 255});
    recompui::theme::set_theme_color(recompui::theme::color::BW25, recompui::Color{64, 64, 64, 255});
    recompui::theme::set_theme_color(recompui::theme::color::BW50, recompui::Color{128, 128, 128, 255});
    recompui::theme::set_theme_color(recompui::theme::color::BW75, recompui::Color{191, 191, 191, 255});
    recompui::theme::set_theme_color(recompui::theme::color::BW90, recompui::Color{229, 229, 229, 255});

    recompui::theme::border::radius_sm = 12.0f;
    recompui::theme::border::radius_md = 18.0f;
    recompui::theme::border::radius_lg = 24.0f;
};


