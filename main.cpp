#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include <string>

//Global variable declarations
sf::Sprite wht, blk, Wht, Blk, bgnd;
sf::RectangleShape rectangle(sf::Vector2f(112, 112));
sf::RenderWindow window(sf::VideoMode(896, 896), "Tomas' Unbeatable Checkers Game");
static char board[8][8];
static char player, comp;

struct node {
    std::string descr;
    int score;
    std::vector<struct node *> children;
    struct node * parent;
    };

//Function prototypes
void drawPieces();
int indexFromCoordinate(int coord);
bool mustPlayerJump();
bool mustJump(struct node& parent, int tempBoard[8][8], bool playerTurn);
std::string mustRecapture(bool playerTurn, int x, int y, char tempBoard[8][8]);
bool isPlayerMoveValid(int x1, int y1, int x2, int y2);
void smartMove();

int main() {
    sf::Texture background, start, white, black, White, Black;

    bool selected = false, draw = true, playerTurn = false, buttonRelease = true;
    char click = 0;
    int x, y, x1, y1;
    sf::Font font;

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
                        board[y1][x1] = std::toupper(player);
                    else
                        board[y1][x1] = board[y][x];
                    board[y][x] = 'v';
                    drawPieces();
                    if (!(mustRecapture(true, x1, y1, board).length()) || abs(y - y1) == 1) {
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
            smartMove();
            drawPieces();
            playerTurn = true;
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
                        if(std::tolower(board[i-1][j-1]) == comp && board[i-2][j-2] == std::toupper(player))
                            return true;
                    }
                }
                if(j < 6) { //left
                    if(i < 6) { //up
                        if(std::tolower(board[i+1][j+1]) == comp && std::tolower(board[i+2][j+2]) == player)
                            return true;
                    }
                    if(i > 1) { //down
                        if(std::tolower(board[i-1][j+1]) == comp && board[i-2][j+2] == std::toupper(player))
                            return true;
                    }
                }
            }
    return false;
}

std::string mustRecapture(bool playerTurn, int x, int y, char tempBoard[8][8]) {
    if(y == 0)
        return "";

    if(playerTurn) {
        if(x > 1) { //left
            if(y > 1) { //up
                if(std::tolower(tempBoard[y-1][x-1]) == comp && tempBoard[y-2][x-2] == 'v') {
                    tempBoard[y-1][x-1] = 'v';
                    tempBoard[y-2][x-2] = y == 2 ? std::toupper(tempBoard[y][x]) : tempBoard[y][x];
                    tempBoard[y][x] = 'v';

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y-1)
                            + std::to_string(x-1) + tempBoard[y-2][x-2] + std::to_string(y-2)
                            + std::to_string(x-2) + mustRecapture(true, x-2, y-2, tempBoard);
                }
            }
            if(y < 6) { //down
                if(tempBoard[y][x] == std::toupper(player) && std::tolower(tempBoard[y+1][x-1]) == comp && tempBoard[y+2][x-2] == 'v') {
                    tempBoard[y][x] = 'v';
                    tempBoard[y+1][x-1] = 'v';
                    tempBoard[y+2][x-2] = std::toupper(player);

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y+1)
                            + std::to_string(x-1) + std::to_string(std::toupper(player)) + std::to_string(y+2) + std::to_string(x-2)
                            + mustRecapture(true, x-2, y+2, tempBoard);
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if(std::tolower(tempBoard[y-1][x+1]) == comp && tempBoard[y-2][x+2] == 'v') {
                    tempBoard[y-1][x+1] = 'v';
                    tempBoard[y-2][x+2] = y == 2 ? std::toupper(tempBoard[y][x]) : tempBoard[y][x];
                    tempBoard[y][x] = 'v';

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y-1)
                            + std::to_string(x+1) + tempBoard[y-2][x+2] + std::to_string(y-2)
                            + std::to_string(x+2) + mustRecapture(true, x+2, y-2, tempBoard);
                }
            }
            if(y < 6) { //down
                if(tempBoard[y][x] == std::toupper(player) && std::tolower(tempBoard[y+1][x+1]) == comp && tempBoard[y+2][x+2] == 'v') {
                    tempBoard[y][x] = 'v';
                    tempBoard[y+1][x+1] = 'v';
                    tempBoard[y+2][x+2] = std::toupper(player);

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y+1)
                            + std::to_string(x+1) + std::to_string(std::toupper(player)) + std::to_string(y+2) + std::to_string(x+2)
                            + mustRecapture(true, x+2, y+2, tempBoard);
                }
            }
        }
    }
    else {
        if(x > 1) { //left
            if(y > 1) { //up
                if(tempBoard[y][x] == std::toupper(comp) && tempBoard[y-1][x-1] == player && tempBoard[y-2][x-2] == 'v') {
                    tempBoard[y-1][x-1] = 'v';
                    tempBoard[y-2][x-2] = tempBoard[y][x];
                    tempBoard[y][x] = 'v';

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y-1)
                            + std::to_string(x-1) + std::to_string(std::toupper(comp)) + std::to_string(y-2)
                            + std::to_string(x-2) + mustRecapture(true, x-2, y-2, tempBoard);
                }
            }
            if(y < 6) { //down
                if(std::tolower(tempBoard[y+1][x-1]) == player && tempBoard[y+2][x-2] == 'v') {
                    tempBoard[y+1][x-1] = 'v';
                    tempBoard[y+2][x-2] = y == 5 ? std::toupper(tempBoard[y][x]) : tempBoard[y][x];
                    tempBoard[y][x] = 'v';

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y+1)
                            + std::to_string(x-1) + tempBoard[y+2][x-2] + std::to_string(y+2) + std::to_string(x-2)
                            + mustRecapture(true, x-2, y+2, tempBoard);
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if(tempBoard[y][x] == std::toupper(comp) && tempBoard[y-1][x+1] == player && tempBoard[y-2][x+2] == 'v') {
                    tempBoard[y-1][x+1] = 'v';
                    tempBoard[y-2][x+2] = tempBoard[y][x];
                    tempBoard[y][x] = 'v';

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y-1)
                            + std::to_string(x+1) + std::to_string(std::toupper(comp)) + std::to_string(y-2)
                            + std::to_string(x+2) + mustRecapture(true, x+2, y-2, tempBoard);
                }
            }
            if(y < 6) { //down
                if(std::tolower(tempBoard[y+1][x+1]) == player && tempBoard[y+2][x+2] == 'v') {
                    tempBoard[y+1][x+1] = 'v';
                    tempBoard[y+2][x+2] = y == 5 ? std::toupper(tempBoard[y][x]) : tempBoard[y][x];
                    tempBoard[y][x] = 'v';

                    return 'v' + std::to_string(y) + std::to_string(x) + 'v' + std::to_string(y+1)
                            + std::to_string(x+1) + tempBoard[y+2][x+2] + std::to_string(y+2) + std::to_string(x+2)
                            + mustRecapture(true, x+2, y+2, tempBoard);
                }
            }
        }
    }
    return "";
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

void smartMove() {
    struct node head;

    if (1/*!mustJump(false)*/) { //implement mustJump
        std::vector<int> possibleMoves;
        for (int i = 0; i < 8; ++i)
            for (int j = 0; j < 8; ++j)
                if(std::tolower(board[i][j]) == comp) {
                    if(j > 0 && i > 0) { //left, up
                        if(std::isupper(board[i][j]) && board[i-1][j-1] == 'v') {
                            ;
                        }
                    }
                    if(j > 0 && i < 7) {
                        if(board[i+1][j-1] == 'v') { //left, down
                            ;
                        }
                    }
                    if(j < 7 && i > 0) { //right, up
                        if(std::isupper(board[i][j]) && board[i-1][j+1] == 'v') {
                            ;
                        }
                    }
                    if(j < 7 && i < 7) { //right, down
                        if(board[i+1][j+1] == 'v') {
                            ;
                        }
                    }
                }

    }
    return;
}

//Purpose is to update the parent node reference based on whether or not jumps must be made.
//mustrecapture is theoretically fully functional.

bool mustJump(struct node& parent, char tempBoard[8][8], bool playerTurn) {
    bool mustjump = false;
    if(playerTurn) {
        for(int i = 0; i < 8; ++i)
            for(int j = 0; j < 8; ++j)
                if (tempBoard[i][j] == 'v') {
                    if(j > 1) { //right
                        if(i < 6) { //up
                            if(std::tolower(tempBoard[i+1][j-1]) == comp && std::tolower(tempBoard[i+2][j-2]) == player) {
                                mustjump = true;
                                struct node * newnode;
                                newnode = new struct node;
                                parent.children.push_back(newnode);
                                newnode -> parent = &parent;
                                newnode -> descr = parent.descr;
                                newnode -> descr += !i ? std::toupper(tempBoard[i+2][j-2]) : tempBoard[i+2][j-2];
                                newnode -> descr += std::to_string(i) + std::to_string(j)
                                                + 'v' + std::to_string(i+1) + std::to_string(j-1)
                                                + 'v' + std::to_string(i+2) + std::to_string(j-2)
                                                + mustRecapture(true, j, i, );
                            }
                        }
                        if(i > 1) { //down
                            if(std::tolower(tempBoard[i-1][j-1]) == comp && tempBoard[i-2][j-2] == std::toupper(player)) {
                                mustjump = true;
                                struct node * newnode;
                                newnode = new struct node;
                                parent.children.push_back(newnode);
                                newnode->parent = &parent;
                                newnode -> descr = parent.descr;
                                newnode -> descr += !i ? std::toupper(player) : player;
                                newnode -> descr += std::to_string(i) + std::to_string(j)
                                                + 'v' + std::to_string(i-1) + std::to_string(j-1)
                                                + 'v' + std::to_string(i-2) + std::to_string(j-2);
                            }
                        }
                    }
                    if(j < 6) { //left
                        if(i < 6) { //up
                            if(std::tolower(tempBoard[i+1][j+1]) == comp && std::tolower(tempBoard[i+2][j+2]) == player) {
                                mustjump = true;
                                struct node * newnode;
                                newnode = new struct node;
                                parent.children.push_back(newnode);
                                newnode->parent = &parent;
                                newnode -> descr = parent.descr;
                                newnode -> descr += !i ? std::toupper(player) : player;
                                newnode -> descr += std::to_string(i) + std::to_string(j)
                                                + 'v' + std::to_string(i+1) + std::to_string(j+1)
                                                + 'v' + std::to_string(i+2) + std::to_string(j+2);
                            }
                        }
                        if(i > 1) { //down
                            if(std::tolower(tempBoard[i-1][j+1]) == comp && tempBoard[i-2][j+2] == std::toupper(player)) {
                                mustjump = true;
                                struct node * newnode;
                                newnode = new struct node;
                                parent.children.push_back(newnode);
                                newnode->parent = &parent;
                                newnode -> descr = parent.descr;
                                newnode -> descr += !i ? std::toupper(player) : player;
                                newnode -> descr += std::to_string(i) + std::to_string(j)
                                                + 'v' + std::to_string(i-1) + std::to_string(j+1)
                                                + 'v' + std::to_string(i-2) + std::to_string(j+2);
                            }
                        }
                    }
                }
    }

    else {
        for(int i = 0; i < 8; ++i)
            for(int j = 0; j < 8; ++j)
                if(std::tolower(board[i][j]) == comp) {
                    if(j > 1) { //left
                        if(i < 6) { //down
                            if(std::tolower(board[i+1][j-1]) == player && board[i+2][j-2] == 'v') {
                                board[i+1][j-1] = 'v';
                                if (i == 5) {
                                    board[i+2][j-2] = std::toupper(comp);
                                    board[i][j] = 'v';
                                    return true;
                                }
                                board[i+2][j-2] = board[i][j];
                                board[i][j] = 'v';
                                mustRecapture(false, j-2, i+2);
                                return true;
                            }
                        }
                        if(i > 1 && board[i][j] == std::toupper(comp)) { //up
                            if(std::tolower(board[i-1][j-1]) == player && board[i-2][j-2] == 'v') {
                                board[i-1][j-1] = 'v';
                                board[i-2][j-2] = board[i][j];
                                board[i][j] = 'v';
                                mustRecapture(false, j-2, i-2);
                                return true;
                            }
                        }
                    }
                    if(j < 6) { //right
                        if(i < 6) { //down
                            if(std::tolower(board[i+1][j+1]) == player && board[i+2][j+2] == 'v') {
                                board[i+1][j+1] = 'v';
                                if (i == 5) {
                                    board[i+2][j+2] = std::toupper(comp);
                                    board[i][j] = 'v';
                                    return true;
                                }
                                board[i+2][j+2] = board[i][j];
                                board[i][j] = 'v';
                                mustRecapture(false, j+2, i+2);
                                return true;
                            }
                        }
                        if(i > 1 && board[i][j] == std::toupper(comp)) { //up
                            if(std::tolower(board[i-1][j+1]) == player && board[i-2][j+2] == 'v') {
                                board[i-1][j+1] = 'v';
                                board[i-2][j+2] = board[i][j];
                                board[i][j] = 'v';
                                mustRecapture(false, j+2, i-2);
                                return true;
                            }
                        }
                    }
                }
    }
    return false;
}
