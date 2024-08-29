#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <unordered_map>
#include "mine.cpp"
#include <stack>
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <map>
#include <cmath>
#include <queue>
#include <termios.h>
#include <unistd.h> // For STDIN_FILENO
#include <locale>
#include <codecvt>
#include <cassert>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::seconds

const std::string BOMB_CHAR = "\033[31mB\033[0m";
const std::string RESET_COLOR = "\033[0m";

const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

const std::string BRIGHT_BLACK = "\033[90m";
const std::string BRIGHT_RED = "\033[91m";
const std::string BRIGHT_GREEN = "\033[92m";
const std::string BRIGHT_YELLOW = "\033[93m";
const std::string BRIGHT_BLUE = "\033[94m";
const std::string BRIGHT_MAGENTA = "\033[95m";
const std::string BRIGHT_CYAN = "\033[96m";
const std::string BRIGHT_WHITE = "\033[97m";
enum MineType
{
    UNKNOWN = 0,
    BOMB = 1,
    SAFE = 2,
};


// Function to configure terminal to read input without Enter
void enableRawMode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);          // Get current terminal attributes
    term.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term); // Set terminal attributes
}

// Function to restore terminal settings
void disableRawMode()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);          // Get current terminal attributes
    term.c_lflag |= (ICANON | ECHO);         // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &term); // Set terminal attributes
}


struct Mine
{
    Mine() : actual_type(UNKNOWN), perceived_type(UNKNOWN), adjacent_bombs(0)
    {
        adjacent_mines.resize(8);
    }
    std::pair<int, int> coords;
    MineType actual_type;
    MineType perceived_type;
    int adjacent_bombs;                 // <=10 for bombs
    std::vector<Mine *> adjacent_mines; // 0 1 2
                                        // 7   3
                                        // 6 5 4
};

struct Board
{
    Board(int size) : mine_field(size, std::vector<Mine>(size)), board_size(size)
    {
        diffifulty = 0.21;
    }
    void get_first_square(int start_row, int start_col)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                if (std::abs(row - start_row) > 1 || std::abs(col - start_col) > 1)
                {
                    double rand = dis(gen);
                    if (rand < diffifulty)
                    {
                        mine_field[row][col].actual_type = BOMB;
                        mine_field[row][col].adjacent_bombs = 10;
                    }
                    else
                    {
                        mine_field[row][col].actual_type = SAFE;
                        mine_field[row][col].adjacent_bombs = 0;
                    }
                }
                else
                {
                    mine_field[row][col].actual_type = SAFE;
                }
                mine_field[row][col].coords = {row, col};
                mine_field[row][col].adjacent_mines[0] = (row == 0 || col == 0) ? nullptr : &mine_field[row - 1][col - 1];
                mine_field[row][col].adjacent_mines[1] = (row == 0) ? nullptr : &mine_field[row - 1][col];
                mine_field[row][col].adjacent_mines[2] = (row == 0 || col == board_size - 1) ? nullptr : &mine_field[row - 1][col + 1];
                mine_field[row][col].adjacent_mines[3] = (col == board_size - 1) ? nullptr : &mine_field[row][col + 1];
                mine_field[row][col].adjacent_mines[4] = (row == board_size - 1 || col == board_size - 1) ? nullptr : &mine_field[row + 1][col + 1];
                mine_field[row][col].adjacent_mines[5] = (row == board_size - 1) ? nullptr : &mine_field[row + 1][col];
                mine_field[row][col].adjacent_mines[6] = (row == board_size - 1 || col == 0) ? nullptr : &mine_field[row + 1][col - 1];
                mine_field[row][col].adjacent_mines[7] = (col == 0) ? nullptr : &mine_field[row][col - 1];
            }
        }
        for (std::vector<Mine> &row : mine_field)
        {
            for (Mine &mine : row)
            {
                if (mine.actual_type == BOMB)
                {
                    for (Mine *m : mine.adjacent_mines)
                    {
                        if (m != nullptr)
                        {
                            m->adjacent_bombs++;
                        }
                    }
                }
            }
        }
        for (int row = 0; row < board_size - 2; row++)
        {
            for (int col = 0; col < board_size - 2; col++)
            {
                fix_potential_right_diagonal_zero(row, col);
            }
        }
        for (int row = 1; row < board_size - 1; row++)
        {
            for (int col = 0; col < board_size - 2; col++)
            {
                fix_potential_left_diagonal_zero(row, col);
            }
        }
        for (int row = std::max(0, start_row - 2); row < std::min(board_size - 1, start_row + 2); row++)
        {
            for (int col = std::max(0, start_col - 2); col < std::min(board_size - 1, start_col + 2); col++)
            {
                if (mine_field[row][col].actual_type == BOMB)
                {
                    mine_field[row][col].actual_type = SAFE;
                    mine_field[row][col].adjacent_bombs -= 10;
                    for (Mine *m : mine_field[row][col].adjacent_mines)
                    {
                        if (m != nullptr)
                        {
                            m->adjacent_bombs--;
                        }
                    }
                }
            }
        }
        std::deque<Mine *> to_be_revealed = {&mine_field[start_row][start_col]};
        while (!to_be_revealed.empty())
        {
            Mine *curr_mine = to_be_revealed.front();
            curr_mine->perceived_type = SAFE;
            to_be_revealed.pop_front();
            if (curr_mine->adjacent_bombs == 0)
            {
                for (Mine *m : curr_mine->adjacent_mines)
                {
                    if (m != nullptr && m->perceived_type != SAFE)
                    {
                        to_be_revealed.push_back(m);
                    }
                }
            }
        }
        for (std::vector<Mine> &row : mine_field)
        {
            for (Mine &m : row)
            {
                if (m.actual_type == BOMB && m.perceived_type != SAFE)
                {
                    this->perceived_num_bombs_left++;
                }
                if (m.actual_type == SAFE && m.perceived_type != SAFE)
                {
                    perceived_num_safe_squares++;
                }
            }
        }
        make_string_mine_field();
    }
    void fix_potential_right_diagonal_zero(int row, int col)
    {
        if (mine_field[row][col].adjacent_bombs == 0 && mine_field[row + 1][col + 1].adjacent_bombs == 0 && mine_field[row][col + 1].adjacent_bombs != 0 && mine_field[row + 1][col].adjacent_bombs != 0)
        {
            mine_field[row + 1][col + 1].adjacent_bombs += 10;
            mine_field[row + 1][col + 1].actual_type = BOMB;
            for (Mine *m : mine_field[row + 1][col + 1].adjacent_mines)
            {
                if (m != nullptr)
                {
                    m->adjacent_bombs++;
                }
            }
        }
    }
    void fix_potential_left_diagonal_zero(int row, int col)
    {
        if (mine_field[row][col].adjacent_bombs == 0 && mine_field[row - 1][col + 1].adjacent_bombs == 0 && mine_field[row - 1][col].adjacent_bombs != 0 && mine_field[row][col + 1].adjacent_bombs != 0)
        {
            mine_field[row - 1][col + 1].adjacent_bombs += 10;
            mine_field[row - 1][col + 1].actual_type = BOMB;
            for (Mine *m : mine_field[row - 1][col + 1].adjacent_mines)
            {
                if (m != nullptr)
                {
                    m->adjacent_bombs++;
                }
            }
        }
    }
    void make_string_mine_field()
    {
        string_mine_field += std::string(int(149 / 2.0 - 5.5), ' ') + "MINESWEEPER\n";
        string_mine_field += std::string(int(149 / 2.0 - board_size / 2.0), ' ') + std::string(board_size, '-') + '\n';
        for (std::vector<Mine> &row : mine_field)
        {
            string_mine_field += std::string(int(149 / 2.0 - board_size / 2), ' ');
            for (Mine &m : row)
            {
                if (m.adjacent_bombs == 0 && m.perceived_type == SAFE)
                {
                    string_mine_field += ' ';
                }
                else if (m.perceived_type == SAFE)
                {
                    string_mine_field += std::to_string(m.adjacent_bombs);
                    if (std::to_string(m.adjacent_bombs).size() != 1)
                    {
                        assert(false);
                    }
                }
                else
                {
                    string_mine_field += '?';
                }
            }
            string_mine_field += '\n';
        }
        int string_size = 12 + std::log10(perceived_num_bombs_left);
        last_line = std::string(string_size, ' ') + "Bombs Left: " + std::to_string(perceived_num_bombs_left) + "\nSquare: ";
    }
    void move_blinker()
    {
    }
    bool make_move(std::pair<int, int> coord, char action)
    { // 1 means reveal, 2 means cover up
        Mine &m = mine_field[coord.first][coord.second];
        int string_inx = int(164 + (149 - board_size) / 2 + (151 + board_size) / 2 * int(coord.first) + int(coord.second));
        if (action == 'd')
        {
            string_mine_field[string_inx] = 'B';
            perceived_num_bombs_left--;
            mine_field[coord.first][coord.second].perceived_type = BOMB;
            int string_size = 12 + std::log10(perceived_num_bombs_left);
            last_line = std::string((149 - string_size) / 2, ' ') + "Bombs Left: " + std::to_string(perceived_num_bombs_left) + "\nSquare: ";
        }
        else
        {
            if (mine_field[coord.first][coord.second].actual_type == BOMB)
            {
                mine_field[coord.first][coord.second].perceived_type = SAFE;
                print_game_losing_message();
                return true;
            }
            else
            {
                mine_field[coord.first][coord.second].perceived_type = SAFE;
                string_mine_field[string_inx] = mine_field[coord.first][coord.second].adjacent_bombs;
                this->perceived_num_safe_squares--;
            }
        }
        return false;
    }
    void print_mine_field()
    {
        std::cout << string_mine_field;
        std::cout << last_line;
    }
    void print_game_losing_message()
    {
        // for (size_t row = board_size - 1; row != 0; row--)
        // {
        //     for (size_t col = board_size - 1; col != 0; col--)
        //     {
        //         int string_inx = int(164 + (149 - board_size) / 2 + (151 + board_size) / 2 * int(row) + int(col));
        //         if (mine_field[row][col].actual_type == BOMB && mine_field[row][col].perceived_type == SAFE)
        //         {
        //             string_mine_field.erase(string_inx);
        //             string_mine_field.insert(string_inx, BOMB_CHAR);
        //         }
        //         else if (mine_field[row][col].actual_type == BOMB && mine_field[row][col].perceived_type == UNKNOWN) {
        //             string_mine_field.erase(string_inx);
        //             std::string replacement = YELLOW + "B" + RESET_COLOR;
        //             string_mine_field.insert(string_inx, replacement);
        //         }
        //         else if (mine_field[row][col].actual_type == BOMB && mine_field[row][col].perceived_type == BOMB) {
        //             string_mine_field.erase(string_inx);
        //             std::string replacement = RED + "B" + RESET_COLOR;
        //             string_mine_field.insert(string_inx, replacement);
        //         } else if (mine_field[row][col].actual_type == SAFE && mine_field[row][col].perceived_type == BOMB) {
        //             string_mine_field.erase(string_inx);
        //             std::string replacement = GREEN + "B" + RESET_COLOR;
        //             string_mine_field.insert(string_inx,  replacement);
        //         }
        //     }
        // }
        std::cout << string_mine_field;
    }
    void play_game(int &curr_row, int &curr_col)
    {
        enableRawMode();
        int important_inx = int(164 + (149 - board_size) / 2 + (151 + board_size) / 2 * int(curr_row) + int(curr_col));
        char &to_store = string_mine_field[important_inx];
        char to_store_copy = to_store;
        char c;
        ssize_t nread = read(STDIN_FILENO, &c, 1);
        int counter = 0;
        while (this->perceived_num_safe_squares != 0)
        {
            counter++;
            counter %= 5000;
            if (counter > 2500)
            {
                to_store = '#';
            }
            else
            {
                to_store = to_store_copy;
            }

            nread = read(STDIN_FILENO, &c, 1);
            if (nread == -1)
                continue;
            else if (c == '\x1b')
            { // Escape character detected
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 0)
                    continue;
                if (read(STDIN_FILENO, &seq[1], 1) == 0)
                    continue;
                if (seq[0] == '[')
                {
                    switch (seq[1])
                    {
                    case 'A':
                        curr_row = std::max(curr_row - 1, 0);
                        break;
                    case 'B':
                        curr_row = std::min(board_size - 1, curr_row + 1);
                        break;
                    case 'C':
                        curr_col = std::min(board_size - 1, curr_col + 1);
                        break;
                    case 'D':
                        curr_col = std::max(0, curr_col - 1);
                        break;
                    default:
                        assert(false);
                    }
                }
                else if (c == 'd')
                {
                    make_move({curr_row, curr_col}, 'd');
                }
                else if (c == 's')
                {
                    make_move({curr_row, curr_col}, 's');
                }
            }
        }
        disableRawMode();
    }
    std::vector<std::vector<Mine>> mine_field;
    int board_size;
    double diffifulty; // Maybe change later
    int perceived_num_safe_squares;
    int perceived_num_bombs_left;
    std::string string_mine_field;
    std::string last_line;
};

int main()
{
    std::wcout.imbue(std::locale("en_US.UTF-8"));
    std::cout << "\033[1m";
    int board_size;
    int curr_row = 0;
    int curr_col = 0;
    std::cout << "What size board would you like? (2-130)\nBoard size: ";
    std::cin >> board_size;
    while (board_size < 2 || board_size > 130)
    {
        if (board_size < 2)
        {
            std::cout << board_size << " is too small, choose a number between 2 and 130.\nBoard size: ";
        }
        else
        {
            std::cout << board_size << " is too small, choose a number between 2 and 130.\nBoard size: ";
        }
        std::cin >> board_size;
    }
    Board b(board_size);
    std::cout << "\n\nChoose your starting square: ";
    std::cin >> curr_row >> curr_col;
    b.get_first_square(curr_row, curr_col);
    b.print_mine_field();
    std::cout << "\nPick a square and an action: ";
    char action;
    std::cin >> curr_row >> curr_col >> action;
    b.make_move({curr_row, curr_col}, action);
    // b.start_game();
    std::cout << "\033[0m";
    return 0;
};