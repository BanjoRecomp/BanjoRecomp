#include <array>
#include "ui_theme.h"

using ThemeColor = recompui::theme::color;
using ThemeColorArray = std::array<recompui::Color, (std::size_t)(ThemeColor::size)>;
using ThemeColorNameArray = std::array<const char *, (std::size_t)(ThemeColor::size)>;

float recompui::theme::border::radius_sm = 8.0f;
float recompui::theme::border::radius_md = 12.0f;
float recompui::theme::border::radius_lg = 16.0f;
float recompui::theme::border::width = 1.1f;

constexpr ThemeColorArray get_default_theme_color_array() {
    ThemeColorArray colors;

    colors[(std::size_t)ThemeColor::Background1] = recompui::Color{2, 7, 18, 255};
    colors[(std::size_t)ThemeColor::Background2] = recompui::Color{7, 15, 34, 255};
    colors[(std::size_t)ThemeColor::Background3] = recompui::Color{18, 24, 38, 255};
    colors[(std::size_t)ThemeColor::BGOverlay] = recompui::Color{182, 194, 221, 26};
    colors[(std::size_t)ThemeColor::ModalOverlay] = recompui::Color{2, 7, 18, 229};

    colors[(std::size_t)ThemeColor::BGShadow] = recompui::Color{0, 0, 0, 89};
    colors[(std::size_t)ThemeColor::BGShadow2] = recompui::Color{2, 7, 18, 184};

    colors[(std::size_t)ThemeColor::Text] = recompui::Color{242, 242, 242, 255};
    colors[(std::size_t)ThemeColor::TextActive] = recompui::Color{245, 245, 245, 255};
    colors[(std::size_t)ThemeColor::TextDim] = recompui::Color{204, 204, 204, 255};
    colors[(std::size_t)ThemeColor::TextInactive] = recompui::Color{255, 255, 255, 153};
    colors[(std::size_t)ThemeColor::TextA5] = recompui::Color{242, 242, 242, 13};
    colors[(std::size_t)ThemeColor::TextA20] = recompui::Color{242, 242, 242, 51};
    colors[(std::size_t)ThemeColor::TextA30] = recompui::Color{242, 242, 242, 77};
    colors[(std::size_t)ThemeColor::TextA50] = recompui::Color{242, 242, 242, 128};
    colors[(std::size_t)ThemeColor::TextA80] = recompui::Color{242, 242, 242, 204};

    colors[(std::size_t)ThemeColor::Primary] = recompui::Color{29, 93, 226, 255};
    colors[(std::size_t)ThemeColor::PrimaryL] = recompui::Color{167, 191, 241, 255};
    colors[(std::size_t)ThemeColor::PrimaryD] = recompui::Color{0, 38, 117, 255};
    colors[(std::size_t)ThemeColor::PrimaryA5] = recompui::Color{29, 93, 226, 13};
    colors[(std::size_t)ThemeColor::PrimaryA20] = recompui::Color{29, 93, 226, 51};
    colors[(std::size_t)ThemeColor::PrimaryA30] = recompui::Color{29, 93, 226, 77};
    colors[(std::size_t)ThemeColor::PrimaryA50] = recompui::Color{29, 93, 226, 128};
    colors[(std::size_t)ThemeColor::PrimaryA80] = recompui::Color{29, 93, 226, 204};

    colors[(std::size_t)ThemeColor::Secondary] = recompui::Color{247, 158, 8, 255};
    colors[(std::size_t)ThemeColor::SecondaryL] = recompui::Color{255, 215, 148, 255};
    colors[(std::size_t)ThemeColor::SecondaryD] = recompui::Color{224, 141, 0, 255};
    colors[(std::size_t)ThemeColor::SecondaryA5] = recompui::Color{247, 158, 8, 13};
    colors[(std::size_t)ThemeColor::SecondaryA20] = recompui::Color{247, 158, 8, 51};
    colors[(std::size_t)ThemeColor::SecondaryA30] = recompui::Color{247, 158, 8, 77};
    colors[(std::size_t)ThemeColor::SecondaryA50] = recompui::Color{247, 158, 8, 128};
    colors[(std::size_t)ThemeColor::SecondaryA80] = recompui::Color{247, 158, 8, 204};

    colors[(std::size_t)ThemeColor::Warning] = recompui::Color{255, 254, 0, 255};
    colors[(std::size_t)ThemeColor::WarningL] = recompui::Color{255, 254, 143, 255};
    colors[(std::size_t)ThemeColor::WarningD] = recompui::Color{197, 163, 2, 255};
    colors[(std::size_t)ThemeColor::WarningA5] = recompui::Color{255, 254, 0, 13};
    colors[(std::size_t)ThemeColor::WarningA20] = recompui::Color{255, 254, 0, 51};
    colors[(std::size_t)ThemeColor::WarningA30] = recompui::Color{255, 254, 0, 77};
    colors[(std::size_t)ThemeColor::WarningA50] = recompui::Color{255, 254, 0, 128};
    colors[(std::size_t)ThemeColor::WarningA80] = recompui::Color{255, 254, 0, 204};

    colors[(std::size_t)ThemeColor::Danger] = recompui::Color{255, 53, 31, 255};
    colors[(std::size_t)ThemeColor::DangerL] = recompui::Color{255, 149, 138, 255};
    colors[(std::size_t)ThemeColor::DangerD] = recompui::Color{163, 16, 0, 255};
    colors[(std::size_t)ThemeColor::DangerA5] = recompui::Color{255, 53, 31, 13};
    colors[(std::size_t)ThemeColor::DangerA20] = recompui::Color{255, 53, 31, 51};
    colors[(std::size_t)ThemeColor::DangerA30] = recompui::Color{255, 53, 31, 77};
    colors[(std::size_t)ThemeColor::DangerA50] = recompui::Color{255, 53, 31, 128};
    colors[(std::size_t)ThemeColor::DangerA80] = recompui::Color{255, 53, 31, 204};

    colors[(std::size_t)ThemeColor::Success] = recompui::Color{40, 238, 32, 255};
    colors[(std::size_t)ThemeColor::SuccessL] = recompui::Color{155, 247, 151, 255};
    colors[(std::size_t)ThemeColor::SuccessD] = recompui::Color{18, 157, 12, 255};
    colors[(std::size_t)ThemeColor::SuccessA5] = recompui::Color{40, 238, 32, 13};
    colors[(std::size_t)ThemeColor::SuccessA20] = recompui::Color{40, 238, 32, 51};
    colors[(std::size_t)ThemeColor::SuccessA30] = recompui::Color{40, 238, 32, 77};
    colors[(std::size_t)ThemeColor::SuccessA50] = recompui::Color{40, 238, 32, 128};
    colors[(std::size_t)ThemeColor::SuccessA80] = recompui::Color{40, 238, 32, 204};

    colors[(std::size_t)ThemeColor::Border] = recompui::Color{255, 255, 255, 51};
    colors[(std::size_t)ThemeColor::BorderSoft] = recompui::Color{255, 255, 255, 26};
    colors[(std::size_t)ThemeColor::BorderHard] = recompui::Color{255, 255, 255, 77};
    colors[(std::size_t)ThemeColor::BorderSolid] = recompui::Color{255, 255, 255, 153};

    colors[(std::size_t)ThemeColor::Transparent] = recompui::Color{0, 0, 0, 0};

    colors[(std::size_t)ThemeColor::A] = recompui::Color{51, 51, 255, 255};
    colors[(std::size_t)ThemeColor::AL] = recompui::Color{178, 178, 255, 255};
    colors[(std::size_t)ThemeColor::AD] = recompui::Color{32, 32, 172, 255};
    colors[(std::size_t)ThemeColor::AA5] = recompui::Color{51, 51, 255, 13};
    colors[(std::size_t)ThemeColor::AA20] = recompui::Color{51, 51, 255, 51};
    colors[(std::size_t)ThemeColor::AA30] = recompui::Color{51, 51, 255, 77};
    colors[(std::size_t)ThemeColor::AA50] = recompui::Color{51, 51, 255, 128};
    colors[(std::size_t)ThemeColor::AA80] = recompui::Color{51, 51, 255, 204};

    colors[(std::size_t)ThemeColor::White] = recompui::Color{255, 255, 255, 255};
    colors[(std::size_t)ThemeColor::WhiteA5] = recompui::Color{255, 255, 255, 13};
    colors[(std::size_t)ThemeColor::WhiteA20] = recompui::Color{255, 255, 255, 51};
    colors[(std::size_t)ThemeColor::WhiteA30] = recompui::Color{255, 255, 255, 77};
    colors[(std::size_t)ThemeColor::WhiteA50] = recompui::Color{255, 255, 255, 128};
    colors[(std::size_t)ThemeColor::WhiteA80] = recompui::Color{255, 255, 255, 204};

    colors[(std::size_t)ThemeColor::BW05] = recompui::Color{13, 13, 13, 255};
    colors[(std::size_t)ThemeColor::BW10] = recompui::Color{26, 26, 26, 255};
    colors[(std::size_t)ThemeColor::BW25] = recompui::Color{64, 64, 64, 255};
    colors[(std::size_t)ThemeColor::BW50] = recompui::Color{128, 128, 128, 255};
    colors[(std::size_t)ThemeColor::BW75] = recompui::Color{191, 191, 191, 255};
    colors[(std::size_t)ThemeColor::BW90] = recompui::Color{229, 229, 229, 255};

    colors[(std::size_t)ThemeColor::Player1] = recompui::Color{0, 185, 253, 255};
    colors[(std::size_t)ThemeColor::Player2] = recompui::Color{253, 76, 0, 255};
    colors[(std::size_t)ThemeColor::Player3] = recompui::Color{253, 194, 0, 255};
    colors[(std::size_t)ThemeColor::Player4] = recompui::Color{147, 253, 0, 255};
    colors[(std::size_t)ThemeColor::Player5] = recompui::Color{0, 253, 164, 255};
    colors[(std::size_t)ThemeColor::Player6] = recompui::Color{253, 0, 189, 255};
    colors[(std::size_t)ThemeColor::Player7] = recompui::Color{140, 74, 255, 255};
    colors[(std::size_t)ThemeColor::Player8] = recompui::Color{255, 186, 118, 255};

    return colors;
}

constexpr ThemeColorNameArray get_default_theme_color_names() {
    ThemeColorNameArray names = {};
    names[(std::size_t)ThemeColor::Background1] = "Background1";
    names[(std::size_t)ThemeColor::Background2] = "Background2";
    names[(std::size_t)ThemeColor::Background3] = "Background3";
    names[(std::size_t)ThemeColor::BGOverlay] = "BGOverlay";
    names[(std::size_t)ThemeColor::ModalOverlay] = "ModalOverlay";
    names[(std::size_t)ThemeColor::BGShadow] = "BGShadow";
    names[(std::size_t)ThemeColor::BGShadow2] = "BGShadow2";
    names[(std::size_t)ThemeColor::Text] = "Text";
    names[(std::size_t)ThemeColor::TextActive] = "TextActive";
    names[(std::size_t)ThemeColor::TextDim] = "TextDim";
    names[(std::size_t)ThemeColor::TextInactive] = "TextInactive";
    names[(std::size_t)ThemeColor::TextA5] = "TextA5";
    names[(std::size_t)ThemeColor::TextA20] = "TextA20";
    names[(std::size_t)ThemeColor::TextA30] = "TextA30";
    names[(std::size_t)ThemeColor::TextA50] = "TextA50";
    names[(std::size_t)ThemeColor::TextA80] = "TextA80";
    names[(std::size_t)ThemeColor::Primary] = "Primary";
    names[(std::size_t)ThemeColor::PrimaryL] = "PrimaryL";
    names[(std::size_t)ThemeColor::PrimaryD] = "PrimaryD";
    names[(std::size_t)ThemeColor::PrimaryA5] = "PrimaryA5";
    names[(std::size_t)ThemeColor::PrimaryA20] = "PrimaryA20";
    names[(std::size_t)ThemeColor::PrimaryA30] = "PrimaryA30";
    names[(std::size_t)ThemeColor::PrimaryA50] = "PrimaryA50";
    names[(std::size_t)ThemeColor::PrimaryA80] = "PrimaryA80";
    names[(std::size_t)ThemeColor::Secondary] = "Secondary";
    names[(std::size_t)ThemeColor::SecondaryL] = "SecondaryL";
    names[(std::size_t)ThemeColor::SecondaryD] = "SecondaryD";
    names[(std::size_t)ThemeColor::SecondaryA5] = "SecondaryA5";
    names[(std::size_t)ThemeColor::SecondaryA20] = "SecondaryA20";
    names[(std::size_t)ThemeColor::SecondaryA30] = "SecondaryA30";
    names[(std::size_t)ThemeColor::SecondaryA50] = "SecondaryA50";
    names[(std::size_t)ThemeColor::SecondaryA80] = "SecondaryA80";
    names[(std::size_t)ThemeColor::Warning] = "Warning";
    names[(std::size_t)ThemeColor::WarningL] = "WarningL";
    names[(std::size_t)ThemeColor::WarningD] = "WarningD";
    names[(std::size_t)ThemeColor::WarningA5] = "WarningA5";
    names[(std::size_t)ThemeColor::WarningA20] = "WarningA20";
    names[(std::size_t)ThemeColor::WarningA30] = "WarningA30";
    names[(std::size_t)ThemeColor::WarningA50] = "WarningA50";
    names[(std::size_t)ThemeColor::WarningA80] = "WarningA80";
    names[(std::size_t)ThemeColor::Danger] = "Danger";
    names[(std::size_t)ThemeColor::DangerL] = "DangerL";
    names[(std::size_t)ThemeColor::DangerD] = "DangerD";
    names[(std::size_t)ThemeColor::DangerA5] = "DangerA5";
    names[(std::size_t)ThemeColor::DangerA20] = "DangerA20";
    names[(std::size_t)ThemeColor::DangerA30] = "DangerA30";
    names[(std::size_t)ThemeColor::DangerA50] = "DangerA50";
    names[(std::size_t)ThemeColor::DangerA80] = "DangerA80";
    names[(std::size_t)ThemeColor::Success] = "Success";
    names[(std::size_t)ThemeColor::SuccessL] = "SuccessL";
    names[(std::size_t)ThemeColor::SuccessD] = "SuccessD";
    names[(std::size_t)ThemeColor::SuccessA5] = "SuccessA5";
    names[(std::size_t)ThemeColor::SuccessA20] = "SuccessA20";
    names[(std::size_t)ThemeColor::SuccessA30] = "SuccessA30";
    names[(std::size_t)ThemeColor::SuccessA50] = "SuccessA50";
    names[(std::size_t)ThemeColor::SuccessA80] = "SuccessA80";
    names[(std::size_t)ThemeColor::Border] = "Border";
    names[(std::size_t)ThemeColor::BorderSoft] = "BorderSoft";
    names[(std::size_t)ThemeColor::BorderHard] = "BorderHard";
    names[(std::size_t)ThemeColor::BorderSolid] = "BorderSolid";
    names[(std::size_t)ThemeColor::Transparent] = "Transparent";
    names[(std::size_t)ThemeColor::A] = "A";
    names[(std::size_t)ThemeColor::AL] = "AL";
    names[(std::size_t)ThemeColor::AD] = "AD";
    names[(std::size_t)ThemeColor::AA5] = "AA5";
    names[(std::size_t)ThemeColor::AA20] = "AA20";
    names[(std::size_t)ThemeColor::AA30] = "AA30";
    names[(std::size_t)ThemeColor::AA50] = "AA50";
    names[(std::size_t)ThemeColor::AA80] = "AA80";
    names[(std::size_t)ThemeColor::White] = "White";
    names[(std::size_t)ThemeColor::WhiteA5] = "WhiteA5";
    names[(std::size_t)ThemeColor::WhiteA20] = "WhiteA20";
    names[(std::size_t)ThemeColor::WhiteA30] = "WhiteA30";
    names[(std::size_t)ThemeColor::WhiteA50] = "WhiteA50";
    names[(std::size_t)ThemeColor::WhiteA80] = "WhiteA80";
    names[(std::size_t)ThemeColor::BW05] = "BW05";
    names[(std::size_t)ThemeColor::BW10] = "BW10";
    names[(std::size_t)ThemeColor::BW25] = "BW25";
    names[(std::size_t)ThemeColor::BW50] = "BW50";
    names[(std::size_t)ThemeColor::BW75] = "BW75";
    names[(std::size_t)ThemeColor::BW90] = "BW90";
    names[(std::size_t)ThemeColor::Player1] = "Player1";
    names[(std::size_t)ThemeColor::Player2] = "Player2";
    names[(std::size_t)ThemeColor::Player3] = "Player3";
    names[(std::size_t)ThemeColor::Player4] = "Player4";
    names[(std::size_t)ThemeColor::Player5] = "Player5";
    names[(std::size_t)ThemeColor::Player6] = "Player6";
    names[(std::size_t)ThemeColor::Player7] = "Player7";
    names[(std::size_t)ThemeColor::Player8] = "Player8";
    return names;
}

static ThemeColorArray theme_colors = get_default_theme_color_array();
static ThemeColorNameArray theme_color_names = get_default_theme_color_names();

void recompui::theme::set_theme_color(recompui::theme::color color, const recompui::Color &value) {
    theme_colors[(std::size_t)color] = value;
}

const recompui::Color &recompui::theme::get_theme_color(recompui::theme::color color) {
    return theme_colors[(std::size_t)color];
}

const char *recompui::theme::get_theme_color_name(recompui::theme::color color) {
    return theme_color_names[(std::size_t)color];
}

using TypographyPreset = recompui::theme::TypographyPreset;

constexpr TypographyPreset create_typography_preset(float font_size, float letter_spacing_percentage, uint32_t font_weight = 400, recompui::FontStyle font_style = recompui::FontStyle::Normal) {
    TypographyPreset preset;
    preset.font_size = font_size;
    preset.line_height = font_size;
    preset.letter_spacing = font_size * letter_spacing_percentage;
    preset.font_weight = font_weight;
    preset.font_style = font_style;
    return preset;
}

using TypographyArray = std::array<TypographyPreset, (std::size_t)(recompui::theme::Typography::size)>;
constexpr TypographyArray get_default_typography_array() {
    TypographyArray typography = {};

    typography[(std::size_t)recompui::theme::Typography::Header1] = create_typography_preset(68.0f, 0.07f, 700);
    typography[(std::size_t)recompui::theme::Typography::Header2] = create_typography_preset(52.0f, 0.07f, 700);
    typography[(std::size_t)recompui::theme::Typography::Header3] = create_typography_preset(36.0f, 0.07f, 700);
    typography[(std::size_t)recompui::theme::Typography::LabelLG] = create_typography_preset(36.0f, 0.11f, 700);
    typography[(std::size_t)recompui::theme::Typography::LabelMD] = create_typography_preset(28.0f, 0.11f, 700);
    typography[(std::size_t)recompui::theme::Typography::LabelSM] = create_typography_preset(20.0f, 0.14f, 700);
    typography[(std::size_t)recompui::theme::Typography::LabelXS] = create_typography_preset(18.0f, 0.14f);
    typography[(std::size_t)recompui::theme::Typography::Body]    = create_typography_preset(20.0f, 0);

    return typography;
}

static TypographyArray typography_presets = get_default_typography_array();

void recompui::theme::set_typography_preset(Typography type, float font_size, float letter_spacing_percentage, uint32_t font_weight, recompui::FontStyle font_style) {
    typography_presets[(std::size_t)type] = create_typography_preset(font_size, letter_spacing_percentage, font_weight, font_style);
}

TypographyPreset &recompui::theme::get_typography_preset(recompui::theme::Typography type) {
    return typography_presets[(std::size_t)type];
}
