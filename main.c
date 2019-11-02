#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

bool start();
void menu();
void game();
char dir_to_char(int dir);

int main(void)
{
    // Initialisation
    clear();
    initscr();
    srand(time(NULL));

    // Settings
    cbreak();
    noecho();
    keypad(stdscr,TRUE);
    curs_set(FALSE);

    // start screen
    //if(start())
        //menu();
    game(1);

    endwin();

    return 0;
}

bool start()
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

    char text[] = "Press ENTER to continue.";
    mvprintw(start_y + n_rows + 3, COLS/2 - strlen(text)/2, "%s", text);

    int key = 0;
    do
    {
        key = getch();
    }while(key != 10 && key != 27);

    if(key == 10)
        return true;

    return false;
}

void menu()
{
    char choice_1[] = "Level 1";
    char choice_2[] = "Level 2";
    char choice_3[] = "Level 3";
    char choice_4[] =  "Exit";

    int selection = 1;
    int key = 0;

    do
    {
        clear();

        mvprintw(LINES/2 - 3, COLS/2 - strlen(choice_1)/2, "%s", choice_1);
        mvprintw(LINES/2 - 1, COLS/2 - strlen(choice_2)/2, "%s", choice_2);
        mvprintw(LINES/2 + 1, COLS/2 - strlen(choice_3)/2, "%s", choice_3);
        mvprintw(LINES/2 + 3, COLS/2 - strlen(choice_4)/2, "%s", choice_4);

        switch(selection)
        {
            case 1:
                mvprintw(LINES/2 - 3, COLS/2 - strlen(choice_1)/2 - 5, "==>");
                break;
            case 2:
                mvprintw(LINES/2 - 1, COLS/2 - strlen(choice_2)/2 - 5, "==>");
                break;
            case 3:
                mvprintw(LINES/2 + 1, COLS/2 - strlen(choice_3)/2 - 5, "==>");
                break;
            case 4:
                mvprintw(LINES/2 + 3, COLS/2 - strlen(choice_4)/2 - 5, "==>");
                break;
            default:
                break;
        }
        refresh();

        key = getch();

        if(key == KEY_UP && selection > 1)
            selection--;
        else if(key == KEY_DOWN && selection < 4)
            selection++;
        else if(key == 10 && selection < 4)
            game(selection);

    }while((key != 10 || selection != 4) && key != 27); // 10 = "Enter" ; 27 = "ESC"
}

void game(int level)
{
    nodelay(stdscr, TRUE);
    clear();

    // Field params
    char field[15][45][2] = {' '}; // The 3rd dimension represents direction
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
    mvprintw(LINES - 2, 0, "P : Pause");
    mvprintw(LINES - 1, 0, "ESC : Exit");

    // Snake current position
    int x = n_cols/4, y= n_rows/2;
    int x_tail = x - 2, y_tail = y;

    for(int i = 0; i < 3; i++)
    {
        field[y][x - i][0] = '0';
        field[y][x - i][1] = 'R';
    }

    // Variables
    char head = '>', body = '0', food = '*';
    int  y_food = -1, x_food = -1;
    bool got_food = false;
    int key, key_2;
    int dir = KEY_RIGHT;
    int score = 0;
    bool game_over = false;

    // Game loop
    do{
        key = getch();
        key_2 = 0;

        // Pause with P
        if(key == 'p' || key == 'P')
            do{
                key_2 = getch();
            }while(key_2 != 'p' && key_2 != 'P' && key_2 != 27);
        if(key_2 == 27)
            break;

        // Change direction
        if( // Cannot change direction to opposite
        (key == KEY_LEFT && dir != KEY_RIGHT)
        || (key == KEY_UP && dir != KEY_DOWN)
        || (key == KEY_RIGHT && dir != KEY_LEFT)
        || (key == KEY_DOWN && dir != KEY_UP)
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

        // Move head position
        field[y][x][0] = head;

        // Move tail position
        if(y != y_food || x != x_food) // If on food, the tail does not move
        {
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
        }

        // Update score
        if(y == y_food && x == x_food)
        {
            score++;
            y_food = -1;
            x_food = -1;
        }

        // Drop food randomely
        if(y_food == -1 || x_food == -1)
            do
            {
                y_food = rand() % n_rows;
                x_food = rand() % n_cols;
            }while(field[y_food][x_food][0] == ' '
                   && (y - y_food + x - x_food < level*5)); // Food pops further depending on the level

        field[y_food][x_food][0] = food;

        // Draw field
        for(int y = 0; y < n_rows; y++)
            for(int x = 0; x < n_cols; x++)
                mvprintw(row_min + y, col_min + x, "%c", field[y][x][0]);

        //Print score
        attron(A_DIM);
        mvprintw(row_start - 2, col_start, "SCORE : %d", score);
        attroff(A_DIM);

        refresh();

        //Pause
        int delay = 1000000 * 0.5 / level; // Speed increases with level
        usleep(delay);

    } while(key != 27);

    nodelay(stdscr, FALSE);
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
