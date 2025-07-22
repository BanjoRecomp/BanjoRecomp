#pragma once

#include "recomp_input.h"
#include "elements/ui_element.h"
#include "elements/ui_svg.h"
#include "elements/ui_button.h"

namespace recompui {

class AssignPlayerCard : public Element {
protected:
    bool is_open = false;
    Svg* icon = nullptr; 

    std::string_view get_type_name() override { return "AssignPlayerCard"; }
public:
    AssignPlayerCard(Element *parent);
    virtual ~AssignPlayerCard();
    void update_player_card(int player_index);
};

class AssignPlayersModal : public Element {
protected:
    bool is_open = false;
    bool was_assigning = false;
    Element* player_elements_wrapper = nullptr;
    Element* fake_focus_button = nullptr;
    std::vector<AssignPlayerCard*> player_elements = {};

    Button* close_button = nullptr;
    Button* retry_button = nullptr;
    Button* confirm_button = nullptr;

    virtual void process_event(const Event &e) override;
    std::string_view get_type_name() override { return "AssignPlayersModal"; }
private:
    void create_player_elements();
public:
    AssignPlayersModal(Element *parent);
    virtual ~AssignPlayersModal();
    void open();
    void close();
};

extern AssignPlayersModal *assign_players_modal;

void init_assign_players_modal();

} // namespace recompui
