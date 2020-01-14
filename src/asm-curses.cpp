#include <asm-curses.h>

void print_win_numbers(WINDOW* win)

{
	int x, y, columns, rows = 0;
	getbegyx(win, y, x);
	getmaxyx(win, columns, rows);

	std::cout << "x = " << x << "; ";
	std::cout << "y = " << y << "; ";
	std::cout << "r = " << rows << "; ";
	std::cout << "c = " << columns << std::endl;
}


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
	border_win = newwin(rows, columns, y, x);
	setBorder(border_type);

	//Create a second window inside the first, with 2 less rows/columns and shifted down one row/column.
	inner_win = newwin(rows - 2, columns - 2, y + 1, x + 1);

    keypad(inner_win, TRUE);
}

nc::Window::Window(WINDOW* win) : inner_win(win)
{
    if(win)
    {
        getbegyx(win, y, x);
        getmaxyx(win, columns, rows);
    }
}

nc::Window::~Window()
{
    if(inner_win)
    {
        delwin(inner_win);
        std::cout << "[asm-curses] Window destroyed!" << std::endl;
    }
}

void nc::Window::Put(char character)
{
    waddch(inner_win,character);
}

void nc::Window::Print(const std::string& text)
{
    wprintw(inner_win, text.c_str());
}

void nc::Window::setBorder(unsigned int type)
{
    border_type = type;
}

void nc::Window::drawBorder()
{
    char ls=' ', rs=' ', ts=' ', bs=' ', tl=' ', tr=' ', bl=' ', br= ' ';

    if((border_type & E_SINGLE_BORDER) == E_SINGLE_BORDER)
    {
        ts = '-';
        bs = '-';
        ls = '|';
        rs = '|';
    }
	else if((border_type & E_DOUBLE_BORDER) == E_DOUBLE_BORDER)
    {
        ts = '=';
        bs = '=';
        ls = 'H';
        rs = 'H';
    }

    if((border_type & E_CROSS_CORNER) == E_CROSS_CORNER)
    {
        tl = '+';
        tr = '+';
        bl = '+';
        br = '+';
    }

    wborder(border_win, ls, rs, ts, bs, tl, tr, bl, br);
}

void nc::Window::drawTitle()
{
    if(0 != title.length())
    {
        mvwprintw(border_win,0,4, title.c_str());
    }
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

    refresh();

    return split;
}

bool nc::Window::move(unsigned int new_x, unsigned int new_y)
{
    mvwin(border_win, new_y, new_x);
    mvwin(inner_win, new_y+1, new_x+1);
    x = new_x;
    y = new_y;

	std::cout << "Move Border: ";
	print_win_numbers(border_win);
	std::cout << "Move Inner: ";
	print_win_numbers(inner_win);

    return true;
}

bool nc::Window::resize(unsigned int new_rows, unsigned int new_columns)
{
    wresize(border_win, new_rows, new_columns);
    wresize(inner_win, new_rows-2, new_columns-2);
    rows = new_rows;
    columns = new_columns;

	std::cout << "Resize Border: ";
	print_win_numbers(border_win);
	std::cout << "Resize Inner: ";
	print_win_numbers(inner_win);

    return true;
}

void nc::Window::refresh()
{
    drawBorder();
    drawTitle();
	wrefresh(border_win);
	wrefresh(inner_win);
}

int nc::Window::Get()
{
    return wgetch(inner_win);
}

void nc::Window::clear()
{
    wclear(inner_win);
}

void nc::Window::MoveCursor(unsigned int new_x, unsigned int new_y)
{
    wmove(inner_win,new_x,new_y);
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
