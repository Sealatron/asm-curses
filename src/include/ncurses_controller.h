#ifndef NCURSES_CONTROLLER_H
#define NCURSES_CONTROLLER_H

#include <curses.h>

namespace nc{

    enum BorderType{
        E_NO_BORDER = 0x0,
        //Edge Masks
        E_SINGLE_BORDER = 0x1,
        E_DOUBLE_BORDER = 0x2,
        //Corner Masks
        E_CROSS_CORNER = 0x10,
    };

    enum WindowPosition{
        E_TOP,
        E_BOTTOM,
        E_LEFT,
        E_RIGHT
    };

    class Window{
        public:
            Window(unsigned int ax=0,
                    unsigned int ay=0,
                    unsigned int arows=10,
                    unsigned int acolumns=10,
                    unsigned int aborder=(E_SINGLE_BORDER|E_CROSS_CORNER));
            Window(WINDOW* win);
            ~Window();
            void refresh();
            void Put(char character);
            int Get();
            void Clear();
            void Print(const std::string& text);
            void setBorder(unsigned int type);
            std::shared_ptr<Window> Split(float ratio, WindowPosition position);
            inline void setTitle(std::string new_title) {title = new_title;};
            void showTitle();
            bool move(unsigned int new_x, unsigned int new_y);
            bool resize(unsigned int new_rows, unsigned int new_columns);
            void MoveCursor(unsigned int new_x, unsigned int new_y);
            std::shared_ptr<Window> GetNeighbour(WindowPosition position);
            void SetNeighbour(std::shared_ptr<Window> neighbour, WindowPosition position);

        private:
            WINDOW* handle;
            unsigned int x;
            unsigned int y;
            unsigned int rows;
            unsigned int columns;
            std::string title;
            unsigned int border_type;
            std::shared_ptr<Window> neighbours[4];
    };

    class Environment{
        public:
            Environment();
            ~Environment();
            std::shared_ptr<Window> getScreen();
    };

};
#endif //NCURSES_CONTROLLER_H
