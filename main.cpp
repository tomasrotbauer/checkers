#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <random>
#include <iterator>

//Global variable declarations
sf::Sprite wht, blk, Wht, Blk, bgnd;
sf::RectangleShape rectangle(sf::Vector2f(112, 112));
sf::RenderWindow window(sf::VideoMode(896, 896), "Tomas' Unbeatable Checkers Game", sf::Style::Close);
static char board[8][8];
static char player, comp;
bool selected = false;
sf::Font font;

struct node {
    node() : parent(nullptr), descr("") {}

    struct node* parent;
    std::string descr;
    int score;
    std::vector<struct node *> children;
    };

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

//Function prototypes
void initBoard();
void drawPieces();
int indexFromCoordinate(int coord);
bool mustJumpRecapture(bool playerTurn, int x, int y, char (*tempBoard)[8][8], struct node* parent, bool update);
bool regularMove(bool playerTurn, int x, int y, const char tempBoard[8][8], struct node* parent);
bool isPlayerMoveValid(int x1, int y1, int x2, int y2);
void updateBoard(char theBoard[8][8], struct node* config);
void cleanUp(struct node * head);
bool gameover(bool playerTurn, struct node* position);
int find_height(struct node * Node);
std::string getDescr(struct node * parent);
int staticEvaluation(struct node* position);
void getChildren(struct node* position, bool playerTurn);
int minimax(struct node* position, bool playerTurn, int depth, int alpha, int beta);

int main() {
    sf::Texture background, start, white, black, White, Black;

    bool playerTurn = false, buttonRelease = true;
    char click = 0;
    int x, y, x1, y1;

    if (!background.loadFromFile("background.png"))
        return EXIT_FAILURE;

        bgnd.setTexture(background);

    if (!white.loadFromFile("checkerwhite.png"))
        return EXIT_FAILURE;

        wht.setTexture(white);

    if (!black.loadFromFile("checkerblack.png"))
        return EXIT_FAILURE;

        blk.setTexture(black);

    if (!White.loadFromFile("checkerwhiteking.png"))
        return EXIT_FAILURE;

        Wht.setTexture(White);

    if (!Black.loadFromFile("checkerblackking.png"))
        return EXIT_FAILURE;

        Blk.setTexture(Black);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if(!selected) {

            if (!start.loadFromFile("start.png"))
                return EXIT_FAILURE;

            if (!font.loadFromFile("arial.ttf"))
                return EXIT_FAILURE;

            sf::Sprite strt;
            strt.setTexture(start);
            window.draw(strt);

            sf::Text colourSel;
            colourSel.setFont(font);
            colourSel.setString("Choose a colour: Press 'b' for black or 'w' for white. (Black goes first)");
            colourSel.setCharacterSize(26);
            colourSel.setFillColor(sf::Color::White);
            colourSel.setStyle(sf::Text::Bold);
            colourSel.setPosition(12,100);
            window.draw(colourSel);

            window.display();

            sf::Text colour;
            colour.setFont(font);
            colour.setCharacterSize(26);
            colour.setFillColor(sf::Color::White);
            colour.setStyle(sf::Text::Bold);
            colour.setPosition(40,350);

            if (event.key.code == sf::Keyboard::B) {
                colour.setString("Black Selelcted");
                window.draw(colour);
                window.display();
                Sleep(500);
                selected = true;
                player = 'b';
                comp = 'w';
                playerTurn = true;
                initBoard();
            }

            else if (event.key.code == sf::Keyboard::W) {
                colour.setString("White Selelcted");
                window.draw(colour);
                window.display();
                Sleep(500);
                selected = true;
                player = 'w';
                comp = 'b';
                playerTurn = false;
                initBoard();
            }
        }

        else if(playerTurn) {   //or player needs to make a play
            if (click == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left) && buttonRelease) {
                sf::Vector2i position = sf::Mouse::getPosition(window);
                x = indexFromCoordinate(position.x);
                y = indexFromCoordinate(position.y);
                if (std::tolower(board[y][x]) == player) {
                    rectangle.setFillColor(sf::Color(255, 255, 20, 150));
                    rectangle.setPosition(x*112, y*112);
                    click = 1;
                    buttonRelease = false;
                    drawPieces();
                }

            }
            else if (click == 1 && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && !buttonRelease)
                        buttonRelease = true;

            else if (click == 1 && buttonRelease && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i position = sf::Mouse::getPosition(window);
                x1 = indexFromCoordinate(position.x);
                y1 = indexFromCoordinate(position.y);

                if (isPlayerMoveValid(x,y,x1,y1)) {
                    if (y1 == 0)
                        board[y1][x1] = (char)std::toupper(player);
                    else
                        board[y1][x1] = board[y][x];
                    board[y][x] = 'v';
                    drawPieces();
                    if (!mustJumpRecapture(true, x1, y1, &board, nullptr, false) || abs(y - y1) == 1 || !y1) {
                        click = 2;
                        buttonRelease = false;
                        playerTurn = false;
                    }
                }
                else if (std::tolower(board[y1][x1]) == player)
                    click = 0;
            }

            else if (click == 2 && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && !buttonRelease) {
                click = 0;
                buttonRelease = true;
            }
        }

        else {
            if(gameover(playerTurn, nullptr)) {
                selected = false;
                Sleep(2000);
                continue;
            }
            struct node* head;
            head = new struct node;
            int nodeScore = minimax(head, false, 3, -1000, 1000);
            std::vector<struct node*> moves;
            for(std::vector<struct node *>::iterator it = std::begin(head -> children); it != std::end(head -> children); ++it) {
                if((*it) -> score == nodeScore)
                    moves.push_back(*it);
            }
            struct node* ptr = *select_randomly(moves.begin(), moves.end());
            updateBoard(board, ptr);
            cleanUp(head);
            drawPieces();
            playerTurn = true;
            if(gameover(playerTurn, nullptr)) {
                selected = false;
                Sleep(2000);
            }

        }
    }

    return 0;
}

void initBoard() {
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; j++)
            if((i + j) % 2 == 0)
                board[i][j] = 'e';  //'e' = empty: the tiles not used during the game
            else if(i < 3)
                board[i][j] = comp;  //'b' = black
            else if(i > 4)
                board[i][j] = player;  //'w' = white
            else board[i][j] = 'v'; //'v' = vacant: the tile is available

    window.draw(bgnd);
    drawPieces();
}

void drawPieces() {
    window.draw(bgnd);

    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            if(board[i][j] == 'b') {
                blk.setPosition(112*j + 6, 112*i + 6);
                window.draw(blk);
            }

            else if(board[i][j] == 'w') {
                wht.setPosition(112*j + 6, 112*i + 6);
                window.draw(wht);
            }

            else if(board[i][j] == 'W') {
                Wht.setPosition(112*j + 6, 112*i + 6);
                window.draw(Wht);
            }

            else if(board[i][j] == 'B') {
                Blk.setPosition(112*j + 6, 112*i + 6);
                window.draw(Blk);
            }
    window.draw(rectangle);

    window.display();
    return;
}

int indexFromCoordinate(int coord) {
    int index = -1;

    while(coord >= 0) {
        coord -= 112;
        index ++;
    }

    return index;
}

//This function finds out if there is a jump to be made
bool mustPlayerJump() {
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            if (board[i][j] == 'v') {
                if(j > 1) { //right
                    if(i < 6) { //up
                        if(std::tolower(board[i+1][j-1]) == comp && std::tolower(board[i+2][j-2]) == player)
                            return true;
                    }
                    if(i > 1) { //down
                        if(std::tolower(board[i-1][j-1]) == comp && board[i-2][j-2] == (char)std::toupper(player))
                            return true;
                    }
                }
                if(j < 6) { //left
                    if(i < 6) { //up
                        if(std::tolower(board[i+1][j+1]) == comp && std::tolower(board[i+2][j+2]) == player)
                            return true;
                    }
                    if(i > 1) { //down
                        if(std::tolower(board[i-1][j+1]) == comp && board[i-2][j+2] == (char)std::toupper(player))
                            return true;
                    }
                }
            }
    return false;
}


bool isPlayerMoveValid(int x1, int y1, int x2, int y2) {
    if (std::tolower(board[y1][x1]) == player && board[y2][x2] == 'v' &&
    (y1 - y2 == 1 || (std::isupper(board[y1][x1]) && y2 - y1 == 1))
    && abs(x2 - x1) == 1 && !mustPlayerJump())
        return true;

    else if ((y1 - y2 == 2 || (std::isupper(board[y1][x1]) && y2 - y1 == 2)) && abs(x2 - x1) == 2) {
        if (std::tolower(board[(y2+y1)/2][(x2+x1)/2]) == comp && board[y2][x2] == 'v') {
            board[(y2+y1)/2][(x2+x1)/2] = 'v';
            return true;
        }
    }
    return false;
}

bool mustJumpRecapture(bool playerTurn, int x, int y, char (*tempBoard)[8][8], struct node* parent, bool update) {
    bool jumped = false;
    char (*tempBoard2)[8][8] = tempBoard;

    if(update) {
        char tempBoard1[8][8];
        updateBoard(tempBoard1, parent);
        tempBoard2 = &tempBoard1;
    }

    if(playerTurn) {
        if(x > 1) { //left
            if(y > 1) { //up
                if(std::tolower((*tempBoard2)[y-1][x-1]) == comp && (*tempBoard2)[y-2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x-1);

                    if(y == 2 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-2) + std::to_string(x-2);

                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x-2);
                        mustJumpRecapture(playerTurn, x-2, y-2, tempBoard2, newnode, true);
                    }
                }
            }
            if(y < 6) { //down
                if((*tempBoard2)[y][x] == std::toupper(player) && std::tolower((*tempBoard2)[y+1][x-1]) == comp && (*tempBoard2)[y+2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x-1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x-2);

                    mustJumpRecapture(playerTurn, x-2, y+2, tempBoard2, newnode, true);
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if(std::tolower((*tempBoard2)[y-1][x+1]) == comp && (*tempBoard2)[y-2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x+1);

                    if(y == 2 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-2) + std::to_string(x+2);
                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x+2);
                        mustJumpRecapture(playerTurn, x+2, y-2, tempBoard2, newnode, true);
                    }
                }
            }
            if(y < 6) { //down
                if((*tempBoard2)[y][x] == std::toupper(player) && std::tolower((*tempBoard2)[y+1][x+1]) == comp && (*tempBoard2)[y+2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x+1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x+2);

                    mustJumpRecapture(playerTurn, x+2, y+2, tempBoard2, newnode, true);
                }
            }
        }
    }
    else {
        if(x > 1) { //left
            if(y > 1) { //up
                if((*tempBoard2)[y][x] == std::toupper(comp) && std::tolower((*tempBoard2)[y-1][x-1]) == player && (*tempBoard2)[y-2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x-1)
                                     + (char)std::toupper(comp) + std::to_string(y-2) + std::to_string(x-2);

                    mustJumpRecapture(playerTurn, x-2, y-2, tempBoard2, newnode, true);
                }
            }
            if(y < 6) { //down
                if(std::tolower((*tempBoard2)[y+1][x-1]) == player && (*tempBoard2)[y+2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x-1);

                    if(y == 5 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(comp) + std::to_string(y+2) + std::to_string(x-2);
                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x-2);
                        mustJumpRecapture(playerTurn, x-2, y+2, tempBoard2, newnode, true);
                    }
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if((*tempBoard2)[y][x] == std::toupper(comp) && std::tolower((*tempBoard2)[y-1][x+1]) == player && (*tempBoard2)[y-2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x+1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x+2);

                    mustJumpRecapture(playerTurn, x+2, y-2, tempBoard2, newnode, true);
                }
            }
            if(y < 6) { //down
                if(std::tolower((*tempBoard2)[y+1][x+1]) == player && (*tempBoard2)[y+2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    if(update)
                        newnode = parent;
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x+1);

                    if(y == 5 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(comp) + std::to_string(y+2) + std::to_string(x+2);
                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x+2);
                        mustJumpRecapture(playerTurn, x+2, y+2, tempBoard2, newnode, true);
                    }
                }
            }
        }
    }
    return jumped;
}

bool regularMove(bool playerTurn, int x, int y, const char tempBoard[8][8], struct node* parent) {
    bool moved = false;

    if(playerTurn) {
        if(x > 0) { //left
            if(y > 0) { //up
                if(tempBoard[y-1][x-1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x);

                    if(y == 1 && std::islower(tempBoard[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-1) + std::to_string(x-1);
                    else
                        newnode -> descr += (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x-1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y][x] == (char)std::toupper(player) && tempBoard[y+1][x-1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y+1) + std::to_string(x-1);
                }
            }
        }
        if(x < 7) { //right
            if(y > 0) { //up
                if(tempBoard[y-1][x+1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x);

                    if(y == 1 && std::islower(tempBoard[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-1) + std::to_string(x+1);
                    else
                        newnode -> descr += (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x+1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y][x] == std::toupper(player) && tempBoard[y+1][x+1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y+1) + std::to_string(x+1);
                }
            }
        }
    }
    else {
        if(x > 0) { //left
            if(y > 0) { //up
                if(tempBoard[y][x] == std::toupper(comp) && tempBoard[y-1][x-1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x-1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y+1][x-1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x);

                    if(y == 6 && std::islower(tempBoard[y][x]))
                        newnode -> descr += (char)std::toupper(comp) + std::to_string(y+1) + std::to_string(x-1);
                    else
                        newnode -> descr += (char)tempBoard[y][x] + std::to_string(y+1) + std::to_string(x-1);
                }
            }
        }
        if(x < 7) { //right
            if(y > 0) { //up
                if(tempBoard[y][x] == std::toupper(comp) && tempBoard[y-1][x+1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x+1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y+1][x+1] == 'v') {
                    if(!parent)
                        return true;
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = 'v' + std::to_string(y) + std::to_string(x);

                    if(y == 6 && std::islower(tempBoard[y][x]))
                        newnode -> descr += (char)std::toupper(comp) + std::to_string(y+1) + std::to_string(x+1);
                    else
                        newnode -> descr += (char)tempBoard[y][x] + std::to_string(y+1) + std::to_string(x+1);
                }
            }
        }
    }
    return moved;
}

int minimax(struct node* position, bool playerTurn, int depth, int alpha, int beta) {
    if(!depth || gameover(playerTurn, position))
        return staticEvaluation(position);

    if(!playerTurn) {
        int maxEval = -1000, eval;
        getChildren(position, playerTurn);
        for(std::vector<struct node *>::iterator it = std::begin(position -> children); it != std::end(position -> children); ++it) {
            eval = minimax(*it, !playerTurn, depth - 1, alpha, beta);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        position -> score = maxEval;
        return maxEval;
    }

    else {
        int minEval = 1000, eval;
        getChildren(position, playerTurn);
        for(std::vector<struct node *>::iterator it = std::begin(position -> children); it != std::end(position -> children); ++it) {
            eval = minimax(*it, !playerTurn, depth - 1, alpha, beta);
            minEval = std::min(beta, eval);
            if(beta <= alpha)
                break;
        }
        position -> score = minEval;
        return minEval;
    }

}

void updateBoard(char theBoard[8][8], struct node* config) {
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            theBoard[i][j] = board[i][j];

    if(config) {
        std::string descr = getDescr(config);
        for (unsigned i = 0; i < descr.length(); i += 3) {
            int row = descr[i+1] - '0';
            int col = descr[i+2] - '0';
            theBoard[row][col] = descr[i];
        }
    }
    return;
}

void cleanUp(struct node * head)  {
    if(head -> children.empty()) {
        delete head;
        return;
    }

    for(std::vector<struct node *>::iterator it = std::begin(head -> children); it != std::end(head -> children); ++it)
        cleanUp(*it);

    delete head;
    return;
}

bool gameover(bool playerTurn, struct node* position) {
    char Board[8][8];
    updateBoard(Board, position);
    bool p_alive = false, c_alive = false;
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            if(std::tolower(Board[i][j]) == player)
                p_alive = true;
            else if(std::tolower(Board[i][j]) == comp)
                c_alive = true;

    if (!(p_alive && c_alive))
        return true;

    for(int y = 0; y < 8; ++y)
        for(int x = 0; x < 8; ++x)
            if ((playerTurn && std::tolower(Board[y][x]) == player) || (!playerTurn && std::tolower(Board[y][x]) == comp)) {
                if(mustJumpRecapture(playerTurn, x, y, &board, nullptr, false) || regularMove(playerTurn, x, y, board, nullptr))
                    return false;
            }

    return true;
}

int find_height(struct node * Node) {
    int height = 0;
    while(Node -> parent) {
        Node = Node -> parent;
        height++;
    }
    return height;
}

std::string getDescr(struct node * parent) {
    if(parent -> parent)
        return getDescr(parent -> parent) + parent -> descr;
    else
        return parent -> descr;
}

int staticEvaluation(struct node* position) {
    char tempBoard[8][8];
    updateBoard(tempBoard, position);
    int score = 0;
    bool win = true, lose = true;
    for(int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
            if(tempBoard[i][j] == player) {
                if(i == 7)
                    score -= 2;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score -= 3;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score -= 3;
                }
                score -= 10;
                win = false;
            }
            else if(tempBoard[i][j] == (char)std::toupper(player)) {
                if(i == 7)
                    score -= 2;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score -= 3;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score -= 3;
                }
                score -= 15;
                win = false;
            }
            else if(tempBoard[i][j] == comp) {
                if(!i)
                    score += 2;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score += 3;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score += 3;
                }
                score += 10;
                lose = false;
            }
            else if(tempBoard[i][j] == (char)std::toupper(comp)) {
                if(!i)
                    score += 2;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score += 3;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score += 3;
                }
                score += 15;
                lose = false;
            }
        }

    if(win) {
        int height = find_height(position);
        if(!height)//better safe than sorry
            height = 1;
        score += (int)(500/height);
    }
    else if(lose) {
        int height = find_height(position);
        if(!height)//better safe than sorry
            height = 1;
        score -= (int)(500/height);
    }
    position -> score = score;
    return score;
}

void getChildren(struct node* position, bool playerTurn) {
    char tempBoard[8][8];
    updateBoard(tempBoard, position);
    bool jump = false;
    std::vector<struct node*> next;

    for(int y = 0; y < 8; ++y)
        for(int x = 0; x < 8; ++x) {
            if ((playerTurn && std::tolower(tempBoard[y][x]) == player) || (!playerTurn && std::tolower(tempBoard[y][x]) == comp))
                if (mustJumpRecapture(playerTurn, x, y, &tempBoard, position, false))
                    jump = true;
        }

    if(jump)
        return;

    else
        for(int y = 0; y < 8; ++y)
            for(int x = 0; x < 8; ++x)
                if ((playerTurn && std::tolower(tempBoard[y][x]) == player) || (!playerTurn && std::tolower(tempBoard[y][x]) == comp))
                    regularMove(playerTurn, x, y, tempBoard, position);
}
