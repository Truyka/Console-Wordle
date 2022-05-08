#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <fstream>
#include <random>
#include <time.h>

class RNG
{
public:
    static int rollInt(int a, int b)
    {
        std::uniform_int_distribution<> dist(a, b);
        return dist(generator);
    }

    static std::mt19937 generator;
};

std::mt19937 RNG::generator(time(NULL));



HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

void setCursorPosition(int x, int y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(hOut, c);
}

void setColor(int fg, int bg)
{
    if(fg >= 16) fg = fg % 16;
    if(bg >= 16) bg = bg % 16;
    const int color = fg + bg * 16;
    SetConsoleTextAttribute(hOut, color);
}

void drawBoard(const std::vector<std::string>& board, int sx, int sy)
{
    for(int y = 0; y < board.size(); y++)
    {
        for(int x = 0; x < board[0].length(); x++)
        {
            setCursorPosition(sx + x, sy + y);
            printf("%c", board[y][x]);
        }
    }
}

std::string createRow(int w, char first, char middle, char last)
{
    std::string row(w, middle);
    row.front() = first;
    row.back()  = last;
    return row;
}

std::vector<std::string> createBoard(int w, int h)
{
    const int total_w = w + 2;

    std::vector<std::string> board;
    board.push_back(createRow(total_w, 201, 205, 187));
    for(int y = 0; y < h - 1; y++)
    {
        board.push_back(createRow(total_w, 186,  32, 186));
        board.push_back(createRow(total_w, 204, 205, 185));
    }
    board.push_back(createRow(total_w, 186,  32, 186));
    board.push_back(createRow(total_w, 200, 205, 188));
    return board;
}

std::size_t countAllRows(std::fstream& file)
{
    std::streampos old_pos = file.tellg();
    file.seekg(0);

    size_t nLines = 0;
    std::string temp;
    while(std::getline(file, temp)) nLines++;

    file.clear();
    file.seekg(old_pos);
    return nLines;
}

std::string getRandomRow(std::fstream& file)
{
    std::size_t numRows = countAllRows(file);
    if(numRows == 0) return "";
    
    int index = RNG::rollInt(0, numRows-1);

    std::streampos old_pos = file.tellg();
    file.seekg(0);

    int i = 0;
    std::string str;
    while(std::getline(file, str))
    {
        if(i++ == index) break;
    }

    file.clear();
    file.seekg(old_pos);
    return str;
}

std::string& strToLower(std::string& str)
{
    for(auto& character : str)
        character = tolower(character);
    return str;
}

bool fileContainsRow(std::fstream& file, std::string row)
{
    bool contains = false;
    std::streampos old_pos = file.tellg();
    file.seekg(0);

    strToLower(row);
    std::string str;
    while(std::getline(file, str))
    {
        if(strToLower(str) == row)
        {
            contains = true;
            break;
        }
    }

    file.clear();
    file.seekg(old_pos);

    return contains;
}

int main()
{
    const int word_length = 5;
    const int tries = 6;
    const int max_height = 1 + tries * 2;

    std::fstream file("words.txt", std::ios::in);
    if(!file.good()) 
    {
        std::cout << "Error! Could not load \'words.txt\'\n";
        return -1;
    }

    std::string random_word = getRandomRow(file);
    strToLower(random_word);

    bool victoryFlag = false;
    drawBoard(createBoard(word_length*2-1, tries), 0, 0);
    for(int t = 0; t < tries && !victoryFlag; t++)
    {
        // Take input
        std::string input;
        do
        {
            setCursorPosition(0, max_height + 1);
            printf("\t\t\t");
            setCursorPosition(0, max_height + 1);
            std::cin >> input;
        }
        while(!fileContainsRow(file, input));
        strToLower(input);

        victoryFlag = true;
        // Check if equals
        for(int i = 0; i < word_length; i++)
        {
            setCursorPosition(1 + i * 2, 1 + t * 2);
            if(input[i] == random_word[i]) 
                setColor(10, 0);
            else 
            {
                victoryFlag = false;
                if(random_word.find(input[i], 0) != std::string::npos) 
                    setColor(14, 0);
                else 
                    setColor(12, 0);
            }

            std::cout << input[i];
        }
        setColor(7, 0);
    }

    setCursorPosition(0, max_height + 1);
    std::cout << (victoryFlag ? "Your guess was right! Good job!\n" : "Game Over - You failed!\n");
    std::cout << "The word to guess was: " << random_word << std::endl;

    file.close();

    std::cin.ignore();
    std::cin.get();
    return 0;
}