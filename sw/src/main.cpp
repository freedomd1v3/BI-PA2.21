#include <stdexcept>
#include <iostream>
#include "note_storage.hpp"
#include "menu.hpp"

int main() {
    Note_Storage notes_store;
    Menu menu_emulation(notes_store);
    menu_emulation.print_heading();
    for (;;) {
        Menu::User_Choice request_action;
        try {
            request_action = menu_emulation.ask_user();
        }
        catch (const std::invalid_argument & e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            menu_emulation.add_distance();
            continue;   // We shouln't stop here
        }

        if (!menu_emulation.process_request(request_action)) {
            break;
        }

        menu_emulation.add_distance();
    }
    return 0;
}
