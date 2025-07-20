#pragma once

#include "recomp_input.h"
#include "elements/ui_element.h"

namespace recompui {

class AssignPlayersModal : public Element {
protected:
    bool is_open = false;
    Element* player_elements_wrapper = nullptr;
    std::vector<Element*> player_elements = {};

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
