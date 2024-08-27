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

enum MineType
{
    UNKNOWN = 0,
    BOMB = 1,
    SAFE = 2,
};

struct Mine
{
    Mine() : actual_type(UNKNOWN), perceived_type(UNKNOWN), adjacent_bombs(0), above(nullptr), below(nullptr), left(nullptr), right(nullptr) {}
    MineType actual_type;
    MineType perceived_type;
    int adjacent_bombs; // 10 for bombs
    Mine *above;
    Mine *below;
    Mine *left;
    Mine *right;
};

struct Board
{
    Board(int size) : mine_field(size, std::vector<Mine>(size)), board_size(size)
    {
        diffifulty = 4.72 * std::pow(size, 0.243) / 100;
    }
    void get_first_square(std::pair<int, int> &coord)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                if (std::abs(row - coord.first) > 1 || std::abs(col - coord.second) > 1)
                {
                    double rand = dis(gen);
                    if (rand < diffifulty)
                    {
                        mine_field[row][col].actual_type = BOMB;
                    }
                    else
                    {
                        mine_field[row][col].actual_type = SAFE;
                    }
                }
                mine_field[row][col].actual_type = SAFE;
                mine_field[row][col].above = (row == 0) ? nullptr : &mine_field[row - 1][col];
                mine_field[row][col].below = (row == board_size - 1) ? nullptr : &mine_field[row + 1][col];
                mine_field[row][col].left = (col == 0) ? nullptr : &mine_field[row][col - 1];
                mine_field[row][col].right = (col == board_size - 1) ? nullptr : &mine_field[row][col + 1];
            }
        }
        for (std::vector<Mine> &row : mine_field)
        {
            for (Mine &mine : row)
            {
                if (mine.actual_type == BOMB)
                {
                    for (Mine *m : {mine.above, mine.below, mine.left, mine.right})
                    {
                        if (m != nullptr)
                        {
                            m->adjacent_bombs++;
                        }
                    }
                }
            }
        }
        std::deque<Mine *> to_be_revealed = {&mine_field[coord.first][coord.second]};
        if (mine_field[coord.first][coord.second].adjacent_bombs == 0)
        {
            Mine *curr_mine = to_be_revealed.front();
            to_be_revealed.pop_front();
            if (curr_mine->adjacent_bombs == 0)
            {

                for (Mine *m : {curr_mine->above, curr_mine->below, curr_mine->right, curr_mine->left})
                {
                    if (m != nullptr)
                    {
                        to_be_revealed.push_back(m);
                    }
                }
            }
            curr_mine->perceived_type=SAFE;
        }
    }
    bool update_square(std::pair<int, int> &coord, int action)
    { // 1 means reveal, 2 means cover up
    }
    std::vector<std::vector<Mine>> mine_field;
    int board_size;
    double diffifulty; // Maybe change later
    int num_mines;
    int perceived_num_mines_left;
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
void print_all_mines(Board &b)
{
}

int main()
{
    int board_size = 15;
    // std::cout << "What size board would you like? (2-130)\nBoard size: ";
    // std::cin >> board_size;
    // while (board_size < 2 || board_size > 130)
    // {
    //     if (board_size < 2)
    //     {
    //         std::cout << board_size << " is too small, choose a number between 2 and 130.\nBoard size: ";
    //     }
    //     else
    //     {
    //         std::cout << board_size << " is too small, choose a number between 2 and 130.\nBoard size: ";
    //     }
    //     std::cin >> board_size;
    // }
    Board b(board_size);

    print_all_mines(b);
};