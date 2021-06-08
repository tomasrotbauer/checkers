#include "utils.h"

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

//During recursion, if more than one jumps are possible, need seperate nodes

bool mustJumpRecapture(bool playerTurn, int x, int y, char (*tempBoard)[8][8], struct node* parent, bool update) {
    bool jumped = false;
    std::string description;
    char (*tempBoard2)[8][8] = tempBoard;

    if(update) {
        char tempBoard1[8][8];
        updateBoard(tempBoard1, parent);
        tempBoard2 = &tempBoard1;
        description = parent -> descr;
    }

    if(playerTurn) {
        if(x > 1) { //left
            if(y > 1) { //up
                if(std::tolower((*tempBoard2)[y-1][x-1]) == comp && (*tempBoard2)[y-2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
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
                    jumped = true;
                }
            }
            if(y < 6) { //down
                if((*tempBoard2)[y][x] == std::toupper(player) && std::tolower((*tempBoard2)[y+1][x-1]) == comp && (*tempBoard2)[y+2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x-1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x-2);

                    mustJumpRecapture(playerTurn, x-2, y+2, tempBoard2, newnode, true);
                    jumped = true;
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if(std::tolower((*tempBoard2)[y-1][x+1]) == comp && (*tempBoard2)[y-2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
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
                    jumped = true;
                }
            }
            if(y < 6) { //down
                if((*tempBoard2)[y][x] == std::toupper(player) && std::tolower((*tempBoard2)[y+1][x+1]) == comp && (*tempBoard2)[y+2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y+1) + std::to_string(x+1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y+2) + std::to_string(x+2);

                    mustJumpRecapture(playerTurn, x+2, y+2, tempBoard2, newnode, true);
                    jumped = true;
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
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x-1)
                                     + (char)std::toupper(comp) + std::to_string(y-2) + std::to_string(x-2);

                    mustJumpRecapture(playerTurn, x-2, y-2, tempBoard2, newnode, true);
                    jumped = true;
                }
            }
            if(y < 6) { //down
                if(std::tolower((*tempBoard2)[y+1][x-1]) == player && (*tempBoard2)[y+2][x-2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
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
                    jumped = true;
                }
            }
        }
        if(x < 6) { //right
            if(y > 1) { //up
                if((*tempBoard2)[y][x] == std::toupper(comp) && std::tolower((*tempBoard2)[y-1][x+1]) == player && (*tempBoard2)[y-2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
                    else {
                        newnode = new struct node;
                        parent -> children.push_back(newnode);
                        newnode -> parent = parent;
                    }
                    newnode -> descr += 'v' + std::to_string(y) + std::to_string(x)
                                     + 'v' + std::to_string(y-1) + std::to_string(x+1)
                                     + (char)(*tempBoard2)[y][x] + std::to_string(y-2) + std::to_string(x+2);

                    mustJumpRecapture(playerTurn, x+2, y-2, tempBoard2, newnode, true);
                    jumped = true;
                }
            }
            if(y < 6) { //down
                if(std::tolower((*tempBoard2)[y+1][x+1]) == player && (*tempBoard2)[y+2][x+2] == 'v') {
                    if(!parent)
                        return true;
                    struct node * newnode;
                    if(update) {
                        if(!jumped)
                            newnode = parent;
                        else {
                            newnode = new struct node;
                            newnode -> descr = description;
                            newnode -> parent = parent -> parent;
                            parent -> parent -> children.push_back(newnode);
                        }
                    }
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
                    jumped = true;
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
        int maxEval = -1000;
        getChildren(position, playerTurn);
        for(std::vector<struct node *>::iterator it = std::begin(position -> children); it != std::end(position -> children); ++it) {
            (*it) -> score = minimax(*it, !playerTurn, depth - 1, alpha, beta);
            maxEval = std::max(maxEval, (*it) -> score);
            alpha = std::max(alpha, maxEval);
            if (beta <= alpha)
                return 999;
        }
        return maxEval;
    }

    else {
        int minEval = 1000;
        getChildren(position, playerTurn);
        for(std::vector<struct node *>::iterator it = std::begin(position -> children); it != std::end(position -> children); ++it) {
            (*it) -> score = minimax(*it, !playerTurn, depth - 1, alpha, beta);
            minEval = std::min(minEval, (*it) -> score);
            beta = std::min(beta, minEval);
            if(beta <= alpha)
                return -999;
        }
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
                if(mustJumpRecapture(playerTurn, x, y, &Board, nullptr, false) || regularMove(playerTurn, x, y, Board, nullptr))
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
                    score -= 1;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score -= 2;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score -= 2;
                }
                score -= 10;
                win = false;
            }
            else if(tempBoard[i][j] == (char)std::toupper(player)) {
                if(i == 7)
                    score -= 1;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score -= 2;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score -= 2;
                }
                score -= 15;
                win = false;
            }
            else if(tempBoard[i][j] == comp) {
                if(!i)
                    score += 1;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score += 2;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score += 2;
                }
                score += 10;
                lose = false;
            }
            else if(tempBoard[i][j] == (char)std::toupper(comp)) {
                if(!i)
                    score += 1;
                else if(i == 3) {
                    if(j == 2 || j == 4)
                        score += 2;
                }
                else if(i == 4) {
                    if(j == 3 || j == 5)
                        score += 2;
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
