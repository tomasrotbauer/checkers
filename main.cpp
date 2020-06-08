#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>

//Global variable declarations
char board[8][8];
sf::Sprite wht, blk, bgnd;
sf::RectangleShape rectangle(sf::Vector2f(112, 112));
sf::RenderWindow window(sf::VideoMode(896, 896), "Tomas' Unbeatable Checkers Game");

//Function prototypes
void drawPieces();
int indexFromCoordinate(int coord);
bool mustJump(char player, char comp, bool playerTurn);
void makeMove(int x1, int y1, int x2, int y2);

int main() {
    sf::Texture background, start, white, black;

    bool selected = false, draw = true, playerTurn = false, buttonRelease = true;
    char player, comp, click = 0;
    int x, y, x1, y1;
    sf::Font font;

    if (!white.loadFromFile("checkerwhite.png"))
        return EXIT_FAILURE;

        wht.setTexture(white);

    if (!black.loadFromFile("checkerblack.png"))
        return EXIT_FAILURE;

        blk.setTexture(black);

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
                rectangle.setFillColor(sf::Color(255, 255, 20, 150));
                rectangle.setPosition(x*112, y*112);
                click = 1;
                buttonRelease = false;
                drawPieces();

            }
            else if (click == 1 && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && !buttonRelease)
                        buttonRelease = true;

            else if (click == 1 && buttonRelease && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i position = sf::Mouse::getPosition(window);
                x1 = indexFromCoordinate(position.x);
                y1 = indexFromCoordinate(position.y);
                click = 2;
                buttonRelease = false;
                board[y][x] = 'v';
                board[y1][x1] = player;
                drawPieces();
            }

            else if (click == 2 && !sf::Mouse::isButtonPressed(sf::Mouse::Left) && !buttonRelease) {
                click = 0;
                buttonRelease = true;
            }
            //highlight selected tile if valid
            //wait for next click
            //either make move or highlight a different tile
        }

        else {      //else computer needs to make a move
            //algorithm
            //make move
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

//This function finds out if there is a jump to be made. It automatically makes the jump if its the computer's turn
bool mustJump(char player, char comp, bool playerTurn) {
    bool mustJump = false;

    if(playerTurn) {
        for(int i = 0; i < 8; ++i)
            for(int j = 0; j < 8; ++j)
                if(board[i][j] == player) {
                    if(i > 1 && j > 1)
                        if((board[i-1][j-1] == comp || board[i-1][j-1] == (char)toupper(comp))
                           && board[i-2][j-2] == 'v') {
                            mustJump = true;
                            board[i-2][j-2] = 'j';   //j = player must jump here
                        }
                    if(i > 1 && j < 6)
                        if((board[i-1][j+1] == comp || board[i-1][j+1] == (char)toupper(comp))
                           && board[i-2][j+2] == 'v') {
                            mustJump = true;
                            board[i-2][j+2] = 'j';   //j = player must jump here
                        }
                }

                else if(board[i][j] == (char)toupper(player)) {
                    if(i > 1 && j > 1)
                        if((board[i-1][j-1] == comp || board[i-1][j-1] == (char)toupper(comp))
                           && board[i-2][j-2] == 'v') {
                            mustJump = true;
                            board[i-2][j-2] = 'j';   //j = player must jump here
                        }
                    if(i > 1 && j < 6)
                        if((board[i-1][j+1] == comp || board[i-1][j+1] == (char)toupper(comp))
                           && board[i-2][j+2] == 'v') {
                            mustJump = true;
                            board[i-2][j+2] = 'j';   //j = player must jump here
                        }
                    if(i < 6 && j > 1)
                        if((board[i+1][j-1] == comp || board[i+1][j-1] == (char)toupper(comp))
                           && board[i+2][j-2] == 'v') {
                            mustJump = true;
                            board[i-2][j-2] = 'j';   //j = player must jump here
                        }
                    if(i < 6 && j < 6)
                        if((board[i+1][j+1] == comp || board[i+1][j+1] == (char)toupper(comp))
                            && board[i+2][j+2] == 'v') {
                            mustJump = true;
                            board[i+2][j+2] = 'j';   //j = player must jump here
                        }
                }
    }

    else {
        for(int i = 0; i < 8; ++i)
            for(int j = 0; j < 8; ++j)
                if(board[i][j] == comp) {
                    if(i < 6 && j > 1)
                        if((board[i+1][j-1] == player || board[i+1][j-1] == (char)toupper(player))
                           && board[i+2][j-2] == 'v') {
                            mustJump = true;
                            board[i-2][j-2] = 'j';   //j = player must jump here
                        }
                    if(i < 6 && j < 6)
                        if((board[i+1][j+1] == player || board[i+1][j+1] == (char)toupper(player))
                            && board[i+2][j+2] == 'v') {
                            mustJump = true;
                            board[i+2][j+2] = 'j';   //j = player must jump here
                        }
                }

                else if(board[i][j] == (char)toupper(comp)) {
                    if(i > 1 && j > 1)
                        if((board[i-1][j-1] == player || board[i-1][j-1] == (char)toupper(player))
                           && board[i-2][j-2] == 'v') {
                            mustJump = true;
                            board[i-2][j-2] = 'j';   //j = player must jump here
                        }
                    if(i > 1 && j < 6)
                        if((board[i-1][j+1] == player || board[i-1][j+1] == (char)toupper(player))
                           && board[i-2][j+2] == 'v') {
                            mustJump = true;
                            board[i-2][j+2] = 'j';   //j = player must jump here
                        }
                    if(i < 6 && j > 1)
                        if((board[i+1][j-1] == player || board[i+1][j-1] == (char)toupper(player))
                           && board[i+2][j-2] == 'v') {
                            mustJump = true;
                            board[i-2][j-2] = 'j';   //j = player must jump here
                        }
                    if(i < 6 && j < 6)
                        if((board[i+1][j+1] == player || board[i+1][j+1] == (char)toupper(player))
                            && board[i+2][j+2] == 'v') {
                            mustJump = true;
                            board[i+2][j+2] = 'j';   //j = player must jump here
                        }
                }
    }

    return mustJump;
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
