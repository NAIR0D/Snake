#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void draw_logo();
void game();
char dir_to_char(int dir);

int main(void)
{
    clear();
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
    nodelay(stdscr, TRUE);

    // Field params
    char field[20][40][2] = {' '}; // The 3rd dimension represents direction
    int n_rows=(sizeof(field)/sizeof(field[0]));
    int n_cols=(sizeof(field)/sizeof(field[0][0]))/n_rows;

    int col_start = COLS/2 - n_cols/2 - 1; // Left border
    int row_start = LINES/2 - n_rows/2 - 1; // Upper border

    // Snake position limits
    int col_min = col_start + 1, col_max = col_start + n_cols;
    int row_min = row_start + 1, row_max = row_start + n_rows;

    // Draw borders
    attron(A_STANDOUT);
    for(int col = col_start; col < col_max + 2; col++)
        mvprintw(row_start, col, "-");
    for(int col = col_start; col < col_max + 2; col++)
        mvprintw(row_start + n_rows + 1, col, "-");
    for(int row = row_start + 1; row < row_max + 1; row++)
        mvprintw(row, col_start, "|");
    for(int row = row_start + 1; row < row_max + 1; row++)
        mvprintw(row, col_start + n_cols + 1, "|");
    attroff(A_STANDOUT);

    // Print info
    char buffer[] = "Press Q to quit.";
    mvprintw(LINES - 1, 0, "%s", buffer);

    // Snake current position
    int x = n_cols/4, y= n_rows/2;
    int x_tail = x - 2, y_tail = y;

    for(int i = 0; i < 3; i++)
    {
        field[y][x - i][0] = '0';
        field[y][x - i][1] = 'R';
    }

    // Variables
    char head = '>', body = '0';
    int key;
    int dir = KEY_RIGHT;
    bool game_over = false;

    // Game loop
    do{
        key = getch();

        if(
        (key == KEY_LEFT && field[y][x-1][0] != body)
        || (key == KEY_UP && field[y-1][x][0] != body)
        || (key == KEY_RIGHT && field[y][x+1][0] != body)
        || (key == KEY_DOWN && field[y+1][x][0] != body)
        )
            dir = key;

        field[y][x][0] = body; // Previous head becomes body
        field[y][x][1] = dir_to_char(dir);

        switch(dir){
            case KEY_LEFT: head = '<';
                           x = (x > 0) ? x - 1 : n_cols - 1;
                           break;
            case KEY_RIGHT: head = '>';
                            x = (x < n_cols - 1) ? x + 1 : 0;
                            break;
            case KEY_UP: head = '^';
                         y = (y > 0) ? y - 1 : n_rows - 1;
                         break;
            case KEY_DOWN: head = 'v';
                           y = (y < n_rows - 1) ? y + 1 : 0;
                           break;
        }

        if(field[y][x][0] == body)
        {
            game_over = true;
            break;
        }

        // Reassign head position
        field[y][x][0] = head;

        // Reassign tail position
        field[y_tail][x_tail][0] = ' ';
        switch(field[y_tail][x_tail][1])
        {
            case 'L':
                field[y_tail][x_tail][1] = ' ';
                x_tail = (x_tail > 0) ? x_tail - 1 : n_cols - 1;
                break;
            case 'U':
                field[y_tail][x_tail][1] = ' ';
                y_tail = (y_tail > 0) ? y_tail - 1 : n_rows - 1;
                break;
            case 'R':
                field[y_tail][x_tail][1] = ' ';
                x_tail = (x_tail < n_cols - 1) ? x_tail + 1 : 0;
                break;
            case 'D':
                field[y_tail][x_tail][1] = ' ';
                y_tail = (y_tail < n_rows - 1) ? y_tail + 1 : 0;
                break;
            default:
                break;
        }

        mvprintw(0, 0, "Y : %d", y);
        mvprintw(1, 0, "X : %d", x);

        // Draw field
        for(int y = 0; y < n_rows; y++)
            for(int x = 0; x < n_cols; x++)
                mvprintw(row_min + y, col_min + x, "%c", field[y][x][0]);

        refresh();
        usleep(500000);

    } while(key != 'q' && key != 'Q');
    clear();
}

char dir_to_char(int dir)
{
    switch(dir)
    {
        case KEY_LEFT: return 'L';
        case KEY_RIGHT: return 'R';
        case KEY_UP: return 'U';
        case KEY_DOWN: return 'D';
        default: return ' ';
    }
}
