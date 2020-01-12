#include "std-headers.h"
#include <ncurses_controller.h>

int main(int argc, char** argv)
{
    nc::Environment ui;

    std::shared_ptr<nc::Window> main_window = ui.getScreen();
    main_window->setTitle("main");
    main_window->setBorder(nc::E_SINGLE_BORDER);
    main_window->showTitle();
    main_window->Print("Hello Main!");
    main_window->refresh();

    std::shared_ptr<nc::Window> browser = main_window->Split(0.3f, nc::E_LEFT);
    browser->SetNeighbour(main_window, nc::E_RIGHT);
    main_window->SetNeighbour(browser, nc::E_LEFT);
    browser->setTitle("browser");
    browser->showTitle();
    browser->setBorder(nc::E_DOUBLE_BORDER);
    browser->Print("Hello Browser!");
    browser->refresh();

    std::shared_ptr<nc::Window> console = main_window->Split(0.3f, nc::E_BOTTOM);
    console->SetNeighbour(main_window, nc::E_TOP);
    main_window->SetNeighbour(console, nc::E_BOTTOM);
    console->setTitle("console");
    console->showTitle();
    console->Print("Hello Console!");
    console->refresh();

    std::shared_ptr<nc::Window> active_window = main_window;

    int a;
    while((a = active_window->Get()) != 'q')
    {
        switch(a)
        {
            case KEY_UP:
                if(NULL != active_window->GetNeighbour(nc::E_TOP))
                {
                    active_window = active_window->GetNeighbour(nc::E_TOP);
                }
                break;
            case KEY_DOWN:
                if(NULL != active_window->GetNeighbour(nc::E_BOTTOM))
                {
                    active_window = active_window->GetNeighbour(nc::E_BOTTOM);
                }
                break;
            case KEY_LEFT:
                if(NULL != active_window->GetNeighbour(nc::E_LEFT))
                {
                    active_window = active_window->GetNeighbour(nc::E_LEFT);
                }
                break;
            case KEY_RIGHT:
                if(NULL != active_window->GetNeighbour(nc::E_RIGHT))
                {
                    active_window = active_window->GetNeighbour(nc::E_RIGHT);
                }
                break;
            default:
                active_window->Put(a);
                break;
        }
    }

    return 0;
}
