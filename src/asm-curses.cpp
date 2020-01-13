#include <asm-curses.h>

nc::Environment::Environment()
{
    std::cout << "[asm-curses] Setting up environment..." << std::endl;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    std::cout << "[asm-curses] Environment setup complete!" << std::endl;
}

nc::Environment::~Environment()
{
    endwin();
    std::cout << "[asm-curses] Environment cleared!" << std::endl;
}

std::shared_ptr<nc::Window> nc::Environment::getScreen()
{
    unsigned int x = 0, y = 0, rows = 0, columns = 0;

    getbegyx(stdscr, y, x);
    getmaxyx(stdscr, rows, columns);

    std::shared_ptr<nc::Window> result = std::make_shared<nc::Window>(x, y, rows, columns, (E_SINGLE_BORDER|E_CROSS_CORNER));

    return result;        
}

nc::Window::Window(unsigned int ax,
                   unsigned int ay,
                   unsigned int arows,
                   unsigned int acolumns,
                   unsigned int aborder) : x(ax), y(ay), rows(arows), columns(acolumns), border_type(aborder)
{
    if(E_NO_BORDER != aborder)
    {
        border_win = newwin(rows,columns,y,x);
        setBorder(border_type);

        //Create a second window inside the first, with 2 less rows/columns and shifted down one row/column.
        handle = newwin(rows-2,columns-2,y+1,x+1);
    }
    else
    {
        handle = newwin(rows,columns,y,x);
    }

    keypad(handle, TRUE);
}

nc::Window::Window(WINDOW* win) : handle(win)
{
    if(win)
    {
        getbegyx(win, y, x);
        getmaxyx(win, columns, rows);
    }
}

nc::Window::~Window()
{
    if(handle)
    {
        delwin(handle);
        std::cout << "[asm-curses] Window destroyed!" << std::endl;
    }
}

void nc::Window::Put(char character)
{
    waddch(handle,character);
}

void nc::Window::Print(const std::string& text)
{
    wprintw(handle, text.c_str());
}

void nc::Window::setBorder(unsigned int type)
{
    char ls=' ', rs=' ', ts=' ', bs=' ', tl=' ', tr=' ', bl=' ', br= ' ';

    if((type & E_SINGLE_BORDER) == E_SINGLE_BORDER)
    {
        ts = '-';
        bs = '-';
        ls = '|';
        rs = '|';
    }
    else if((type & E_DOUBLE_BORDER) == E_DOUBLE_BORDER)
    {
        ts = '=';
        bs = '=';
        ls = 'H';
        rs = 'H';
    }

    if((type & E_CROSS_CORNER) == E_CROSS_CORNER)
    {
        tl = '+';
        tr = '+';
        bl = '+';
        br = '+';
    }

    border_type = type;

    wborder(border_win, ls, rs, ts, bs, tl, tr, bl, br);
    showTitle();
}

std::shared_ptr<nc::Window> nc::Window::Split(float ratio, WindowPosition position)
{
    unsigned int split_x;
    unsigned int split_y;
    unsigned int split_rows;
    unsigned int split_cols;

    if(E_BOTTOM == position)
    {
        //Work out the new window height
        split_rows  = rows * ratio;
        split_cols  = columns;

        resize(rows - split_rows, columns); //Will modify 'rows'
        split_x    = x;
        split_y    = rows;

        setBorder(border_type); //refresh the window border
    }
    else if(E_TOP == position)
    {
        //Work out the new window height
        split_rows  = rows * ratio;
        split_cols  = columns;

        resize(rows - split_rows, columns); //Will modify 'rows'
        split_x    = x;
        split_y    = y;

        move(x, split_rows);

        setBorder(border_type); //refresh the window border
    }
    else if(E_LEFT == position)
    {
        //Work out the new window height
        split_rows  = rows;
        split_cols  = columns * ratio;
        resize(rows, columns - split_cols);

        split_x    = x;
        split_y    = y;

        move(split_cols, y);

        setBorder(border_type); //refresh the window border
    }
    else //Default to right of screen if any other value is provided
    {
        //Work out the new window height
        split_rows  = rows;
        split_cols  = columns * ratio;
        resize(rows, columns - split_cols);

        setBorder(border_type); //refresh the window border

        split_x    = columns;
        split_y    = y;
    }

    std::shared_ptr<Window> split = std::make_shared<nc::Window>(split_x,
            split_y,
            split_rows,
            split_cols,
            (E_SINGLE_BORDER|E_CROSS_CORNER));

    //The splits should initially have the same neighbours as one another
    for(unsigned int index = 0; index < 4; ++index)
    {
        split->SetNeighbour(this->GetNeighbour(static_cast<WindowPosition>(index)), static_cast<WindowPosition>(index));
    }

    split->Clear();
    split->setBorder((E_SINGLE_BORDER|E_CROSS_CORNER));
    split->MoveCursor(1,1);

    return split;
}

void nc::Window::showTitle()
{
    if(0 != title.length())
    {
        unsigned int cur_x = 0, cur_y = 0;
        getyx(handle, cur_y, cur_x);
        mvwprintw(handle,0,4, title.c_str());
        wmove(handle, cur_y, cur_x);
    }
}

bool nc::Window::move(unsigned int new_x, unsigned int new_y)
{
    mvwin(handle, new_y, new_x);
    x = new_x;
    y = new_y;

    return true;
}

bool nc::Window::resize(unsigned int new_rows, unsigned int new_columns)
{
    wresize(handle, new_rows, new_columns);
    rows = new_rows;
    columns = new_columns;

    return true;
}

void nc::Window::refresh()
{
    if(border_type != E_NO_BORDER)
        wrefresh(border_win);

    wrefresh(handle);
}

int nc::Window::Get()
{
    return wgetch(handle);
}

void nc::Window::Clear()
{
    wclear(handle);
    setBorder(border_type);
}

void nc::Window::MoveCursor(unsigned int new_x, unsigned int new_y)
{
    wmove(handle,new_x,new_y);
}

std::shared_ptr<nc::Window> nc::Window::GetNeighbour(WindowPosition position)
{
    if(NULL != neighbours[position])
    {
        return neighbours[position];
    }
    else
    {
        return NULL;
    }
}

void nc::Window::SetNeighbour(std::shared_ptr<nc::Window> neighbour, WindowPosition position)
{
    neighbours[position] = neighbour;
}
