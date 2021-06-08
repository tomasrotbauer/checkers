#include <SFML/Graphics.hpp>
#pragma once

static char board[8][8];
static char player, comp;
sf::Sprite wht, blk, Wht, Blk, bgnd;
sf::RectangleShape rectangle(sf::Vector2f(112, 112));
sf::RenderWindow window(sf::VideoMode(896, 896), "Tomas' Unbeatable Checkers Game", sf::Style::Close);
bool selected = false;
sf::Font font;

struct node {
    node() : parent(nullptr), descr("") {}

    struct node* parent;
    std::string descr;
    int score;
    std::vector<struct node *> children;
    };

void initBoard();
void drawPieces();
int indexFromCoordinate(int coord);
bool mustPlayerJump();
bool isPlayerMoveValid(int x1, int y1, int x2, int y2);
bool mustJumpRecapture(bool playerTurn, int x, int y, char (*tempBoard)[8][8], struct node* parent, bool update);
bool regularMove(bool playerTurn, int x, int y, const char tempBoard[8][8], struct node* parent);
int minimax(struct node* position, bool playerTurn, int depth, int alpha, int beta);
void updateBoard(char theBoard[8][8], struct node* config);
void cleanUp(struct node * head);
bool gameover(bool playerTurn, struct node* position);
int find_height(struct node * Node);
std::string getDescr(struct node * parent);
int staticEvaluation(struct node* position);
void getChildren(struct node* position, bool playerTurn);
