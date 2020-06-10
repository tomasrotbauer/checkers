#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <windows.h>

//Global variable declarations
char board[8][8];
char player, comp;
sf::Sprite wht, blk, Wht, Blk, bgnd;
sf::RectangleShape rectangle(sf::Vector2f(112, 112));
sf::RenderWindow window(sf::VideoMode(896, 896), "Tomas' Unbeatable Checkers Game");

//Function prototypes
void drawPieces();
int indexFromCoordinate(int coord);
bool mustJump(bool playerTurn);
bool isPlayerMoveValid(int x1, int y1, int x2, int y2);
void makeMove(int x1, int y1, int x2, int y2);
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
                if (board[y][x] == player) {
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
                    board[y][x] = 'v';
                    if (y1 == 0)
                        board[y1][x1] = std::toupper(player);
                    else
                        board[y1][x1] = player;
                    drawPieces();
                    if (!mustJump(true) || y - y1 == 1) {
                        click = 2;
                        buttonRelease = false;
                        playerTurn = false;
                    }
                }
                else if (board[y1][x1] == player)
                    click = 0;
            }

            else if (click == 2 && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && !buttonRelease) {
                click = 0;
                buttonRelease = true;
            }
        }

        else {
            if (mustJump(false)) {
                drawPieces();
                playerTurn = true;
            }
            else {
                smartMove();
                drawPieces();
                playerTurn = true;
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
bool mustJump(bool playerTurn) {
    bool jumped = false;

    if(playerTurn) {
        for(int i = 0; i < 6; ++i)
            for(int j = 0; j < 8; ++j)
                if (board[i][j] == 'v') {
                    if(j > 1) { //left
                        if(board[i+1][j-1] == comp && board[i+2][j-2] == player)
                            return true;
                    }
                    if(j < 6) { //right
                        if(board[i+1][j+1] == comp && board[i+2][j+2] == player)
                            return true;
                    }
                }
    }

    else {
        bool again = true;
        while(again) {
            again = false;
            for(int i = 0; i < 6; ++i)
                for(int j = 0; j < 8; ++j)
                    if(board[i][j] == comp) {
                        if(j > 1) { //left
                            if(board[i+1][j-1] == player && board[i+2][j-2] == 'v') {
                                board[i+1][j-1] = 'v';
                                board[i][j] = 'v';
                                if (i == 5)
                                    board[i+2][j-2] = std::toupper(comp);
                                else
                                    board[i+2][j-2] = comp;
                                jumped = true;
                                again = true;
                            }
                        }
                        if(j < 6) {
                            if(board[i+1][j+1] == player && board[i+2][j+2] == 'v') {
                                board[i+1][j+1] = 'v';
                                board[i][j] = 'v';
                                if (i == 5)
                                    board[i+2][j+2] = std::toupper(comp);
                                else
                                    board[i+2][j+2] = comp;
                                jumped = true;
                                again = true;
                            }
                        }
                    }
        }
    }
    return jumped;
}

//Responsible for making moves within the array
void makeMove(int x1, int y1, int x2, int y2) {
    if(board[x2][y2] == 'v') {
        board[x2][y2] = board[x1][y1];
        board[x1][y1] = 'v';
    }

    else if(board[x2][y2] == 'j') {
        board[x2][y2] = board[x1][y1];
        board[x1][y1] = 'v';
        board[(x1 + x2)/2][(y1 + y2)/2] = 'v';

        for(int i = 0; i < 8; ++i)
            for(int j = 0; j < 8; ++j)
                if(board[i][j] == 'j')
                    board[i][j] = 'v';
    }

    return;
}

bool isPlayerMoveValid(int x1, int y1, int x2, int y2) {
    if (board[y1][x1] == player && board[y2][x2] == 'v' && y1 - y2 == 1 && abs(x2 - x1) == 1 && !mustJump(true))
        return true;
    else if (y1 - y2 == 2 && abs(x2 - x1) == 2) {
        if (board[(y2+y1)/2][(x2+x1)/2] == comp && board[y2][x2] == 'v') {
            board[(y2+y1)/2][(x2+x1)/2] = 'v';
            return true;
        }
    }
    return false;
}

void smartMove() {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            if(board[i][j] == comp) {
                if(j > 1) { //left
                    if(board[i+1][j-1] == 'v') {
                        if (i == 6)
                            board[i+1][j-1] = std::toupper(comp);
                        else
                            board[i+1][j-1] = comp;
                        board[i][j] = 'v';
                        return;
                    }
                }
                if(j < 6) {
                    if(board[i+1][j+1] == 'v') {
                        if (i == 6)
                            board[i+1][j+1] = std::toupper(comp);
                        else
                            board[i+1][j+1] = comp;
                        board[i][j] = 'v';
                        return;
                    }
                }
            }
    return;
}
