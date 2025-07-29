#pragma once

#include "recomp_input.h"
#include "elements/ui_element.h"
#include "elements/ui_svg.h"
#include "elements/ui_button.h"
#include "elements/ui_select.h"

namespace recompui {

// player index, profile index
using on_select_player_profile_callback = std::function<void(int, int)>;
// player index
using on_edit_player_profile_callback = std::function<void(int)>;

class PlayerCard : public Element {
protected:
    bool is_open = false;
    Element *card = nullptr;
    Svg* icon = nullptr;
    Select *profile_select = nullptr;
    int player_index = -1;
    bool is_assignment_card = false;

    on_select_player_profile_callback on_select_profile_callback;
    on_edit_player_profile_callback on_edit_profile_callback;

    std::string_view get_type_name() override { return "PlayerCard"; }
private:
    void on_select_player_profile(int profile_index);
    void on_edit_profile();
public:
    PlayerCard(Element *parent, int player_index, bool is_assignment_card = false);
    virtual ~PlayerCard();
    void update_assignment_player_card();
    void update_player_card_icon();
    void set_on_select_profile_callback(on_select_player_profile_callback callback) {
        on_select_profile_callback = std::move(callback);
    }
    void set_on_edit_profile_callback(on_edit_player_profile_callback callback) {
        on_edit_profile_callback = std::move(callback);
    }
};

} // namespace recompui
