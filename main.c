#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void draw_logo();
void game();

int main(void)
{
    // Initialisation
    initscr();

    // Settings
    cbreak();
    noecho();
    keypad(stdscr,TRUE);
    curs_set(FALSE);

    // Logo page
    //draw_logo();

    // Start game
    game();

    endwin();

    return 0;
}

void draw_logo()
{
    clear();
    char logo[5][32] = {
        {" _____ _____ _____ _____ _____ "},
        {"|  ___|   | |  _  |  |  |  ___|"},
        {"|___  | | | |     |    -|  ___|"},
        {"|_____|_|___|__|__|__|__|_____|"}
    };

    int n_rows=(sizeof(logo)/sizeof(logo[0]));
    int n_cols=(sizeof(logo)/sizeof(logo[0][0]))/n_rows;

    int start_y = LINES/2 - n_rows/2;
    int start_x = COLS/2 - n_cols/2;

    for(int x = start_x; x < start_x + n_cols; x++)
    {
        for(int y = start_y; y < start_y + n_rows; y++)
        {
            move(y, x);
            printw("%c", logo[y - start_y][x - start_x]);
        }
        refresh();
        usleep(1 * 100000);
    }

    char buffer[] = "Press a key.";
    mvprintw(LINES - 1, 0, "%s", buffer);

    getch();
    clear();
}

int select_level()
{
    int selection = 0;

    return selection;
}

void game()
{
    int y = LINES / 2;
    int x = COLS / 2;

    char buffer[] = "Press q/Q to quit. Move by arrows.";
    mvprintw(LINES - 1, 0, "%s", buffer);

    char player = '>', previous = ' ';
    mvprintw(y, x, "%c", player);

    refresh();

    int c;

    do{
        c = getch();
        previous = (y == LINES - 1 && x < 34) ? buffer[x] : ' ';
        if(c == KEY_LEFT || c == KEY_RIGHT || c == KEY_UP || c == KEY_DOWN){
            mvprintw(y, x, "%c", previous);
        }
        switch(c){
            case KEY_LEFT: player = '<';
                           x = (x > 0) ? x - 1 : COLS - 1;
                           break;
            case KEY_RIGHT: player = '>';
                            x = (x < COLS - 1) ? x + 1 : 0;
                            break;
            case KEY_UP: player = '^';
                         y = (y > 0) ? y - 1 : LINES - 1;
                         break;
            case KEY_DOWN: player = 'v';
                           y = (y < LINES - 1) ? y + 1 : 0;
                           break;
            default: continue; break;
        }
        mvprintw(y, x, "%c", player);
        refresh();
    } while(c != 'q' && c != 'Q');
}
