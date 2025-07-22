#pragma once

#include "recomp_input.h"
#include "elements/ui_element.h"
#include "elements/ui_svg.h"
#include "elements/ui_button.h"

namespace recompui {

class PlayerCard : public Element {
protected:
    bool is_open = false;
    Svg* icon = nullptr;
    int player_index = -1;
    bool is_assignment_card = false;

    std::string_view get_type_name() override { return "PlayerCard"; }
public:
    PlayerCard(Element *parent, int player_index, bool is_assignment_card = false);
    virtual ~PlayerCard();
    void update_assignment_player_card();
    void update_player_card_icon();
};

} // namespace recompui
