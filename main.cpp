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
sf::RenderWindow window(sf::VideoMode(896, 896), "Tomas' Unbeatable Checkers Game");
static char board[8][8];
static char player, comp;
bool selected = false;
sf::Font font;

struct node {
    node() : score(999), parent(nullptr){}

    std::string descr;
    int score;
    std::vector<struct node *> children;
    struct node * parent;
    bool playerTurn;
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
void drawPieces();
int indexFromCoordinate(int coord);
bool mustPlayerJump();
bool mustJumpRecapture(bool playerTurn, int x, int y, char (*tempBoard)[8][8], struct node* parent, bool update);
bool regularMove(bool playerTurn, int x, int y, const char tempBoard[8][8], struct node* parent);
bool isPlayerMoveValid(int x1, int y1, int x2, int y2);
void smartMove();
void updateBoard(char theBoard[8][8], struct node* config);
void getTreeLeaves(struct node * parent, std::vector<struct node *>* leaves);
void computeScores(std::vector<struct node *> &current, struct node * head);
void cleanUp(struct node * head);
bool gameover();

int main() {
    sf::Texture background, start, white, black, White, Black;

    bool draw = true, playerTurn = false, buttonRelease = true;
    char click = 0;
    int x, y, x1, y1;

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

        window.clear();

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
            }

            if(selected)
                for(int i = 0; i < 8; ++i)
                    for(int j = 0; j < 8; j++)
                        if((i + j) % 2 == 0)
                            board[i][j] = 'e';  //'e' = empty: the tiles not used during the game
                        else if(i < 3)
                            board[i][j] = comp;  //'b' = black
                        else if(i > 4)
                            board[i][j] = player;  //'w' = white
                        else board[i][j] = 'v'; //'v' = vacant: the tile is available
        }

        else if(draw) {     //board needs to be updated

        if (!background.loadFromFile("background.png"))
                return EXIT_FAILURE;

        window.clear();

        bgnd.setTexture(background);
        window.draw(bgnd);
        drawPieces();
        draw = false;
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
                    if (!mustJumpRecapture(true, x1, y1, &board, nullptr, false) || abs(y - y1) == 1) {
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
            gameover();
            smartMove();
            drawPieces();
            playerTurn = true;
            if(gameover()) {
                selected = false;
                Sleep(2000);
            }

        }
    }

    return 0;
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

//This function finds out if there is a jump to be made. It automatically makes the jump if it's the computer's turn
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

//Everything makes logical sense, just make sure you create a new node each time you uncover a recapture.
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
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x-1);

                    if(y == 2 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-2) + std::to_string(x-2);

                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x-2);
                        if(mustJumpRecapture(playerTurn, x-2, y-2, tempBoard2, newnode, true))
                            newnode -> playerTurn = playerTurn;
                    }
                }
            }
            if(y < 6) { //down
                if((*tempBoard2)[y][x] == std::toupper(player) && std::tolower((*tempBoard2)[y+1][x-1]) == comp && (*tempBoard2)[y+2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x-1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x-2);

                    if(mustJumpRecapture(playerTurn, x-2, y+2, tempBoard2, newnode, true))
                        newnode -> playerTurn = playerTurn;
                    else
                        newnode -> playerTurn = !playerTurn;
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
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x+1);

                    if(y == 2 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-2) + std::to_string(x+2);
                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x+2);
                        if(mustJumpRecapture(playerTurn, x+2, y-2, tempBoard2, newnode, true))
                            newnode -> playerTurn = playerTurn;
                    }
                }
            }
            if(y < 6) { //down
                if((*tempBoard2)[y][x] == std::toupper(player) && std::tolower((*tempBoard2)[y+1][x+1]) == comp && (*tempBoard2)[y+2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    jumped = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x+1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x+2);

                    if(mustJumpRecapture(playerTurn, x+2, y+2, tempBoard2, newnode, true))
                        newnode -> playerTurn = playerTurn;
                    else
                        newnode -> playerTurn = !playerTurn;
                }
            }
        }
    }
    else {
        if(x > 1) { //left
            if(y > 1) { //up
                if((*tempBoard2)[y][x] == std::toupper(comp) && std::tolower((*tempBoard2)[y-1][x-1]) == player && (*tempBoard2)[y-2][x-2] == 'v') {
                    jumped = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x-1)
                                     + (char)std::toupper(comp) + std::to_string(y-2) + std::to_string(x-2);

                    if(mustJumpRecapture(playerTurn, x-2, y-2, tempBoard2, newnode, true))
                        newnode -> playerTurn = playerTurn;
                    else
                        newnode -> playerTurn = !playerTurn;
                }
            }
            if(y < 6) { //down
                if(std::tolower((*tempBoard2)[y+1][x-1]) == player && (*tempBoard2)[y+2][x-2] == 'v') {
                    jumped = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x-1);

                    if(y == 5 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(comp) + std::to_string(y+2) + std::to_string(x-2);
                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x-2);
                        if(mustJumpRecapture(playerTurn, x-2, y+2, tempBoard2, newnode, true))
                            newnode -> playerTurn = playerTurn;
                    }
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if((*tempBoard2)[y][x] == std::toupper(comp) && std::tolower((*tempBoard2)[y-1][x+1]) == player && (*tempBoard2)[y-2][x+2] == 'v') {
                    jumped = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x+1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x+2);

                    if(mustJumpRecapture(playerTurn, x+2, y-2, tempBoard2, newnode, true))
                        newnode -> playerTurn = playerTurn;
                    else
                        newnode -> playerTurn = !playerTurn;
                }
            }
            if(y < 6) { //down
                if(std::tolower((*tempBoard2)[y+1][x+1]) == player && (*tempBoard2)[y+2][x+2] == 'v') {
                    jumped = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x+1);

                    if(y == 5 && std::islower((*tempBoard2)[y][x]))
                        newnode -> descr += (char)std::toupper(comp) + std::to_string(y+2) + std::to_string(x+2);
                    else {
                        newnode -> descr += (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x+2);
                        if(mustJumpRecapture(playerTurn, x+2, y+2, tempBoard2, newnode, true))
                            newnode -> playerTurn = playerTurn;
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
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x);

                    if(y == 1 && std::islower(tempBoard[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-1) + std::to_string(x-1);
                    else
                        newnode -> descr += (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x-1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y][x] == (char)std::toupper(player) && tempBoard[y+1][x-1] == 'v') {
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y+1) + std::to_string(x-1);
                }
            }
        }
        if(x < 7) { //right
            if(y > 0) { //up
                if(tempBoard[y-1][x+1] == 'v') {
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x);

                    if(y == 1 && std::islower(tempBoard[y][x]))
                        newnode -> descr += (char)std::toupper(player) + std::to_string(y-1) + std::to_string(x+1);
                    else
                        newnode -> descr += (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x+1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y][x] == std::toupper(player) && tempBoard[y+1][x+1] == 'v') {
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y+1) + std::to_string(x+1);
                }
            }
        }
    }
    else {
        if(x > 0) { //left
            if(y > 0) { //up
                if(tempBoard[y][x] == std::toupper(comp) && tempBoard[y-1][x-1] == 'v') {
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x-1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y+1][x-1] == 'v') {
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x);

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
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + (char)tempBoard[y][x] + std::to_string(y-1) + std::to_string(x+1);
                }
            }
            if(y < 7) { //down
                if(tempBoard[y+1][x+1] == 'v') {
                    moved = true;
                    struct node * newnode;
                    newnode = new struct node;
                    parent -> children.push_back(newnode);
                    newnode -> parent = parent;
                    newnode -> playerTurn = !playerTurn;
                    newnode -> descr = parent -> descr;
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x);

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

void smartMove() {
    struct node* head;
    head = new struct node;
    head -> playerTurn = false;

    char tempBoard[8][8];
    bool playerTurn = false, jump, moved;

    std::vector<struct node *> current, next;
    current.push_back(head);


    //std::cout<<"-----------------------"<<std::endl;
    for (int d = 0; d < 6; ++d) {
        for(std::vector<struct node *>::iterator it = std::begin(current); it != std::end(current); ++it) {
            updateBoard(tempBoard, *it);
            jump = false;
            moved = false;

            for(int y = 0; y < 8; ++y)
                for(int x = 0; x < 8; ++x) {
                    if (!((playerTurn && std::tolower(tempBoard[y][x]) == player) || (!playerTurn && std::tolower(tempBoard[y][x]) == comp)))
                        continue;
                    if (mustJumpRecapture(playerTurn, x, y, &tempBoard, *it, false))
                        jump = true;
                }

            if(jump) //append children into next vector
                getTreeLeaves(*it, &next);

            else {
                for(int y = 0; y < 8; ++y)
                    for(int x = 0; x < 8; ++x) {
                        if (!((playerTurn && std::tolower(tempBoard[y][x]) == player) || (!playerTurn && std::tolower(tempBoard[y][x]) == comp)))
                            continue;
                        if(regularMove(playerTurn, x, y, tempBoard, *it))
                            moved = true;
                    }

                if(moved)
                    next.insert(next.end(), (*it) -> children.begin(), (*it) -> children.end());
            }

        }
        current.clear();
        if(next.empty())
            break;
        current = next;
        next.clear();
        playerTurn = !playerTurn;
    }

    struct node * ptr;
    computeScores(current, head);
    int max_score = head -> children.at(0) -> score;
    std::cout<<"children: ";
    for(std::vector<struct node *>::iterator it = std::begin(head -> children); it != std::end(head -> children); ++it) {
        std::cout<<(*it)->score<<' ';
        if((*it) -> score >= max_score) {
            if((*it) -> score > max_score)
                next.clear();
            max_score = (*it) -> score;
            ptr = (*it);
            while(!(ptr -> playerTurn))
                for(std::vector<struct node *>::iterator iter = std::begin((*it) -> children); iter != std::end((*it) -> children); ++iter)
                    if ((*iter) -> score == max_score) {
                        ptr = (*iter);
                        break;
                }
            next.push_back(ptr);
        }
    }
    std::cout<<"head: ";
    std::cout<<head->score<<" # of children found: "<<next.size()<< " score of child selected: ";
    ptr = *select_randomly(next.begin(), next.end());
    std::cout<< ptr -> score<<std::endl;
//    if(ptr -> score == 100) {
//        selected = false;
//        Sleep(2000);
//    }
    updateBoard(board, ptr);
    next.clear();
    cleanUp(head);

    return;
}

void updateBoard(char theBoard[8][8], struct node* config) {
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            theBoard[i][j] = board[i][j];

    if(config)
        for (unsigned i = 0; i < config -> descr.length(); i += 3) {
            int row = config -> descr[i+1] - '0';
            int col = config -> descr[i+2] - '0';
            if(theBoard[row][col] == 'e')
                std::cout<<config -> descr;
            theBoard[row][col] = config -> descr[i];
        }
    return;
}

void getTreeLeaves(struct node * parent, std::vector<struct node *>* leaves) {
    if(parent -> children.empty()) {
        leaves -> push_back(parent);
        return;
    }
    for(std::vector<struct node *>::iterator it = std::begin(parent -> children); it != std::end(parent -> children); ++it)
        getTreeLeaves(*it, leaves);

    return;
}

void computeScores(std::vector<struct node *> &current, struct node * head) {
    std::vector<struct node *> next;
    char tempBoard[8][8];
    int score;
    bool win = true;
    for(std::vector<struct node *>::iterator it = std::begin(current); it != std::end(current); ++it) {
        updateBoard(tempBoard, (*it));
        win = true;
        score = 0;
        for(int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j) {
                if(tempBoard[i][j] == player) {
                    score -= 1;
                    win = false;
                }
                else if(tempBoard[i][j] == (char)std::toupper(player)) {
                    score -= 3;
                    win = false;
                }
                else if(tempBoard[i][j] == comp)
                    score += 1;
                else if(tempBoard[i][j] == (char)std::toupper(comp))
                    score += 3;
                else;
            }
        (*it) -> score = score;
        if(win)
            (*it) -> score += 500;
    }
    while(!current.empty()) {
        for(std::vector<struct node *>::iterator it = std::begin(current); it != std::end(current); ++it) {
            if ((*it) -> parent) {
                if ((*it) -> parent -> score == 999) {
                    (*it) -> parent -> score = (*it) -> score;
                    next.push_back((*it) -> parent);
                }

                else if ((*it) -> parent -> playerTurn && (*it) -> parent -> score > (*it) -> score)
                    (*it) -> parent -> score = (*it) -> score;

                else if (!((*it) -> parent -> playerTurn) && (*it) -> parent -> score < (*it) -> score)
                    (*it) -> parent -> score = (*it) -> score;

            }
        }
        current = next;
        next.clear();
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

bool gameover() {
    bool p_alive = false, c_alive = false;
    for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            if(std::tolower(board[i][j]) == player)
                p_alive = true;
            else if(std::tolower(board[i][j]) == comp)
                c_alive = true;

    return !(p_alive && c_alive);
}
