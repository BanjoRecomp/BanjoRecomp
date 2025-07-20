#pragma once

#include "ui_types.h"

namespace recompui {
    namespace theme {
        enum class color {
            Background1,
            Background2,
            Background3,
            BGOverlay,
            ModalOverlay,
            BGShadow,
            BGShadow2,
            Text,
            TextActive,
            TextDim,
            TextInactive,
            TextA5,
            TextA20,
            TextA30,
            TextA50,
            TextA80,
            Primary,
            PrimaryL,
            PrimaryD,
            PrimaryA5,
            PrimaryA20,
            PrimaryA30,
            PrimaryA50,
            PrimaryA80,
            Secondary,
            SecondaryL,
            SecondaryD,
            SecondaryA5,
            SecondaryA20,
            SecondaryA30,
            SecondaryA50,
            SecondaryA80,
            Warning,
            WarningL,
            WarningD,
            WarningA5,
            WarningA20,
            WarningA30,
            WarningA50,
            WarningA80,
            Danger,
            DangerL,
            DangerD,
            DangerA5,
            DangerA20,
            DangerA30,
            DangerA50,
            DangerA80,
            Success,
            SuccessL,
            SuccessD,
            SuccessA5,
            SuccessA20,
            SuccessA30,
            SuccessA50,
            SuccessA80,
            Border,
            BorderSoft,
            BorderHard,
            BorderSolid,
            Transparent,
            A,
            AL,
            AD,
            AA5,
            AA20,
            AA30,
            AA50,
            AA80,
            White,
            WhiteA5,
            WhiteA20,
            WhiteA30,
            WhiteA50,
            WhiteA80,
            BW05,
            BW10,
            BW25,
            BW50,
            BW75,
            BW90,
            Player1,
            Player2,
            Player3,
            Player4,
            Player5,
            Player6,
            Player7,
            Player8,

            size,
        };

        namespace border {
            extern float radius_sm;
            extern float radius_md;
            extern float radius_lg;
            extern float width;
        }

        const char *get_theme_color_name(theme::color color);
        void set_theme_color(theme::color color, const recompui::Color &value);
        const recompui::Color &get_theme_color(theme::color color);

    } // namespace theme
} // namespace recompui
