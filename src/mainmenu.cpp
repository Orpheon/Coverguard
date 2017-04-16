#include "mainmenu.h"
#include "global.h"

#define LEFT_MOUSE_BUTTON 1
#define RIGHT_MOUSE_BUTTON 2

MainMenu::MainMenu(ALLEGRO_DISPLAY *display) : spriteloader(false)
{
    // Create an event queue, and error if it fails
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not create event queue");
    }

    // Connect the window, keyboard and mouse events to this event queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
}

MainMenu::~MainMenu()
{
    al_destroy_event_queue(event_queue);
}

bool MainMenu::run(ALLEGRO_DISPLAY *display)
{
    ALLEGRO_EVENT event;
    bool finished = false;
    // Capture events first
    while (al_get_next_event(event_queue, &event))
    {
        switch (event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // Deliberate closing, not an error
                throw 0;

            case ALLEGRO_EVENT_KEY_DOWN:
                switch (event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_W:
//                        pressed_keys.JUMP = true;
                        break;
                    case ALLEGRO_KEY_S:
//                        pressed_keys.CROUCH = true;
                        break;

                    case ALLEGRO_KEY_ESCAPE:
                        // Exit menu
                        finished = true;
                }

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                switch (event.mouse.button)
                {
                    case LEFT_MOUSE_BUTTON:
//                        pressed_keys.PRIMARY_FIRE = true;
                        break;
                    case RIGHT_MOUSE_BUTTON:
//                        pressed_keys.SECONDARY_FIRE = true;
                        break;
                }
        }
    }

    ALLEGRO_MOUSE_STATE mousestate;
    al_get_mouse_state(&mousestate);

    // use mousestate.x or .y for position of mouse on screen

    // Draw
//    al_draw_bitmap(spriteloader.requestsprite("mainmenu/background"), 0, 0);

    return not finished;
}
