#include <string.h>
#include <time.h>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief DEBUG AREA
 */
// #define PRINT_PATH
// #define PRINT_ANCESTER

/**
 * @brief VARIABLE ELEMENTS
 */
#define CHAOS_FLOOR 7
#define ORDER_FLOOR 6
#define CHAOS_START 25
#define CHAOS_END 40
#define ORDER_FIRST 10
#define ORDER_SECOND 27
#define ORDER_THIRD 42
#define STEPS 1
#define NUMS 10

/**
 * @brief INVARIABLE VALUE
 */
#define CHAOS 1
#define ORDER 2
#define TRIVIAL -1

int board[7][7] = {0};
int colors[7] = {0};
int chess_num = 0;

class Detail;
class Node;
class Tree;

Detail* (*algorithm_chao)(int);
Detail* (*algorithm_order)();
Detail* (*tree_chao)(int);
Detail* (*tree_order)();

Node* benefit;

class Board {
   public:
    int Score();
    void Add(int c, int r, int color);
    void Delete(int c, int r);
    void Change(int c1, int r1, int c2, int r2);
    void Move(int c, int r, int* result);
    void Show();
    friend class Tree;
};

int Board::Score() {
    int score = 0;
    int target[3] = {0};
    // row_score
    for (int k = 2; k <= 7; k++) {
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 8 - k; j++) {
                if (k <= 3) {
                    target[0] = board[i][j];
                    if (target[0] == 0)
                        continue;
                    if (board[i][j + 1 + (k % 2)] == target[0]) {
                        score += k;
                    }
                } else if (k <= 5) {
                    target[0] = board[i][j];
                    target[1] = board[i][j + 1];
                    if (target[0] == 0 || target[1] == 0)
                        continue;
                    if (board[i][j + 2 + (k % 2)] == target[1] &&
                        board[i][j + 3 + (k % 2)] == target[0]) {
                        score += k;
                    }
                } else {
                    target[0] = board[i][j];
                    target[1] = board[i][j + 1];
                    target[2] = board[i][j + 2];
                    if (target[0] == 0 || target[1] == 0 || target[2] == 0)
                        continue;
                    if (board[i][j + 3 + (k % 2)] == target[2] &&
                        board[i][j + 4 + (k % 2)] == target[1] &&
                        board[i][j + 5 + (k % 2)] == target[0]) {
                        score += k;
                    }
                }
            }
        }
    }

    // column_score
    for (int k = 2; k <= 7; k++) {
        for (int j = 0; j < 7; j++) {
            for (int i = 0; i < 8 - k; i++) {
                // if(k%2==1 && board[i+(k/2)][j]==0)continue;
                if (k <= 3) {
                    target[0] = board[i][j];
                    if (target[0] == 0)
                        continue;
                    if (board[i + 1 + (k % 2)][j] == target[0]) {
                        score += k;
                    }
                } else if (k <= 5) {
                    target[0] = board[i][j];
                    target[1] = board[i + 1][j];
                    if (target[0] == 0 || target[1] == 0)
                        continue;
                    if (board[i + 2 + (k % 2)][j] == target[1] &&
                        board[i + 3 + (k % 2)][j] == target[0]) {
                        score += k;
                    }
                } else {
                    target[0] = board[i][j];
                    target[1] = board[i + 1][j];
                    target[2] = board[i + 2][j];
                    if (target[0] == 0 || target[1] == 0 || target[2] == 0)
                        continue;
                    if (board[i + 3 + (k % 2)][j] == target[2] &&
                        board[i + 4 + (k % 2)][j] == target[1] &&
                        board[i + 5 + (k % 2)][j] == target[0]) {
                        score += k;
                    }
                }
            }
        }
    }
    return score;
}

void Board::Add(int c, int r, int color) {
    board[c][r] = color;
    colors[color - 1]++;
}

void Board::Delete(int c, int r) {
    colors[board[c][r] - 1]--;
    board[c][r] = 0;
}

void Board::Change(int c1, int r1, int c2, int r2) {
    if (c1 == c2 && r1 == r2)
        return;
    else {
        board[c2][r2] = board[c1][r1];
        board[c1][r1] = 0;
    }
}

void Board::Move(int c, int r, int* result) {
    int score = 0;
    int max_score = Score();
    int max_c = c;
    int max_r = r;

    // right
    for (int i = 1; i < 7 - r; i++) {
        if (board[c][r + i] == 0) {
            Change(c, r, c, r + i);
            score = Score();
            if (score > max_score) {
                max_score = score;
                max_c = c;
                max_r = r + i;
            }
            Change(c, r + i, c, r);
        } else {
            break;
        }
    }
    // left
    for (int i = 1; i <= r; i++) {
        if (board[c][r - i] == 0) {
            Change(c, r, c, r - i);
            score = Score();
            if (score > max_score) {
                max_score = score;
                max_c = c;
                max_r = r - i;
            }
            Change(c, r - i, c, r);
        } else {
            break;
        }
    }
    // up
    for (int i = 1; i <= c; i++) {
        if (board[c - i][r] == 0) {
            Change(c, r, c - i, r);
            score = Score();
            if (score > max_score) {
                max_score = score;
                max_c = c - i;
                max_r = r;
            }
            Change(c - i, r, c, r);
        } else {
            break;
        }
    }
    // down
    for (int i = 1; i < 7 - c; i++) {
        if (board[c + i][r] == 0) {
            Change(c, r, c + i, r);
            score = Score();
            if (score > max_score) {
                max_score = score;
                max_c = c + i;
                max_r = r;
            }
            Change(c + i, r, c, r);
        } else {
            break;
        }
    }
    result[0] = c;
    result[1] = r;
    result[2] = max_c;
    result[3] = max_r;
}

void Board::Show() {
    fprintf(stderr, "     ");
    for (int i = 0; i < 7; i++) {
        int x = 'a' + i;
        fprintf(stderr, "%2c ", (char)x);
    }
    fprintf(stderr, "\n");

    for (int i = 0; i < 7; i++) {
        int x = 'A' + i;
        fprintf(stderr, "%2c ", (char)x);
        for (int j = 0; j < 7; j++) {
            fprintf(stderr, "%2d ", board[i][j]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

class Detail {
   public:
    int player;
    int color;
    int c, r, mc, mr;
    Detail() : player(0), color(0), c(0), r(0), mc(0), mr(0){};
    Detail(int a, int b, int c, int d, int e, int f)
        : player(a), color(b), c(c), r(d), mc(e), mr(f){};
    friend class Node;
};

class Node {
   public:
    Detail* info;
    int height;
    int score;
    Node* parent;
    vector<Node*> children;
    Node() : info(0), height(0), score(0), parent(0), children(0){};
    Node(Detail* a) : info(a), height(0), score(0), parent(0), children(0){};
    friend class Tree;
};

class Tree {
   public:
    Node* root;
    Tree() : root(0){};
    void Init();
    void Insert(Detail* info, Node* p);
    void Show(Node* current);
    Node* Compare(Node* a, Node* b, int player);
    void Search_chaos(Node* current, int i, int floor, int color, int player);
    void Search_order(Node* current, int i, int floor, int player);
    Node* Search(Node* current, int floor, int color, int player);
    friend class Board;
};

Tree T;
Board B;

void Tree::Init() {
    root = new Node();
    root->info = new Detail();
}

void Tree::Insert(Detail* info, Node* p) {
    Node* insert = new Node(info);
    if (p->height == 0) {
        insert->parent = insert;
    } else {
        insert->parent = p->parent;
    }
    insert->height = p->height + 1;
    insert->score = B.Score();
    p->children.push_back(insert);
}

void Tree::Show(Node* current) {
    fprintf(stderr, "Height: %d\n", current->height);
    fprintf(stderr, "Player: %s\n",
            current->info->player == 1   ? "Choas"
            : current->info->player == 2 ? "ORDER"
                                         : "Root");
    if (current->info->player == 1) {
        fprintf(stderr, "put color %d at [%d, %d]\n", current->info->color,
                current->info->c, current->info->r);
        fprintf(stderr, "score: %d\n\n", current->score);
    } else if (current->info->player == 2) {
        fprintf(stderr, "move [%d, %d] to [%d, %d]\n", current->info->c,
                current->info->r, current->info->mc, current->info->mr);
        fprintf(stderr, "score: %d\n\n", current->score);
    } else {
        fprintf(stderr, "start\n\n");
    }
}

Node* Tree::Compare(Node* a, Node* b, int player) {
    return player == CHAOS ? (a->score <= b->score ? a : b)
                           : (a->score >= b->score ? a : b);
}

Detail* greedy_chao(int color) {
    int pick_board[7][7] = {0};
    int score = 0;
    int original_score = B.Score();
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 7; j++) {
            if (board[i][j] != 0)
                pick_board[i][j] = 1000;
            else
                pick_board[i][j] = 0;
        }
    }
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 7; j++) {
            if (board[i][j] == 0) {
                B.Add(i, j, color);
                score = B.Score();
                B.Delete(i, j);
                // fprintf(stderr,"score: %d\n",score);
                if (score > original_score) {
                    // up
                    int k = 0;
                    while (pick_board[i - k][j] != 1000 && i >= k) {
                        pick_board[i - k][j]++;
                        k++;
                    }
                    // down
                    k = 0;
                    while (pick_board[i + k][j] != 1000 && i + k < 7) {
                        pick_board[i + k][j]++;
                        k++;
                    }
                    // left
                    k = 0;
                    while (pick_board[i][j - k] != 1000 && j >= k) {
                        pick_board[i][j - k]++;
                        k++;
                    }
                    // right
                    k = 0;
                    while (pick_board[i][j + k] != 1000 && j + k < 7) {
                        pick_board[i][j + k]++;
                        k++;
                    }
                }
            }
        }
    }
    int find_min = 10000;
    int min_c = 0;
    int min_r = 0;

    // central
    for (size_t i = 2; i < 5; i++) {
        for (size_t j = 2; j < 5; j++) {
            if (pick_board[i][j] < find_min) {
                find_min = pick_board[i][j];
                min_c = i;
                min_r = j;
            }
        }
    }
    // central left
    for (size_t i = 2; i < 5; i++) {
        for (size_t j = 0; j < 2; j++) {
            if (pick_board[i][j] < find_min) {
                find_min = pick_board[i][j];
                min_c = i;
                min_r = j;
            }
        }
    }
    // central right
    for (size_t i = 2; i < 5; i++) {
        for (size_t j = 5; j < 7; j++) {
            if (pick_board[i][j] < find_min) {
                find_min = pick_board[i][j];
                min_c = i;
                min_r = j;
            }
        }
    }
    // up
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 7; j++) {
            if (pick_board[i][j] < find_min) {
                find_min = pick_board[i][j];
                min_c = i;
                min_r = j;
            }
        }
    }
    // down
    for (size_t i = 5; i < 7; i++) {
        for (size_t j = 0; j < 7; j++) {
            if (pick_board[i][j] < find_min) {
                find_min = pick_board[i][j];
                min_c = i;
                min_r = j;
            }
        }
    }

    Detail* result = new Detail(CHAOS, color, min_c, min_r, TRIVIAL, TRIVIAL);
    return result;
}

Detail* greedy_order() {
    int score = 0;
    int max_score = -1;
    int max_c = 0;
    int max_r = 0;
    int max_move_c = 0;
    int max_move_r = 0;
    int tmp[4] = {0};
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";
    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 7; j++) {
            if (board[i][j] != 0) {
                B.Move(i, j, tmp);
                B.Change(tmp[0], tmp[1], tmp[2], tmp[3]);
                score = B.Score();
                if (score > max_score) {
                    max_score = score;
                    max_c = tmp[0];
                    max_r = tmp[1];
                    max_move_c = tmp[2];
                    max_move_r = tmp[3];
                }
                B.Change(tmp[2], tmp[3], tmp[0], tmp[1]);
            }
        }
    }

    Detail* result =
        new Detail(CHAOS, TRIVIAL, max_c, max_r, max_move_c, max_move_r);
    return result;
}

/*Tingggggggggg*/
Detail* winner_chao(int color) {
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";
    int min_c = 0;
    int min_r = 0;
    int min = 5000;
    // 大十字，提高優先權
    int score[7][7] = {
        0,   0, 0, -10, 0,   0,   0,   0,   0,   0,   -10, 0,   0, 0, 0,   0, 0,
        -10, 0, 0, 0,   -10, -10, -10, -20, -10, -10, -10, 0,   0, 0, -10, 0, 0,
        0,   0, 0, 0,   -10, 0,   0,   0,   0,   0,   0,   -10, 0, 0, 0,
    };
    int up_left = 0;
    int up_right = 0;
    int down_left = 0;
    int down_right = 0;

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j] != 0) {
                if (i <= 2 and j <= 2)
                    up_left++;
                else if (i <= 2 and j >= 4)
                    up_right++;
                else if (i >= 4 and j <= 2)
                    down_left++;
                else if (i >= 4 and j >= 4)
                    down_right++;
            }
        }
    }
    int order_may_move[7][7] = {0};
    int tmp[4];
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (board[i][j] != 0) {
                // 有棋子的地方分數設1000
                score[i][j] = 10000;
                // 尋找該棋子最可能的下一步
                B.Move(i, j, tmp);
                B.Change(tmp[0], tmp[1], tmp[2], tmp[3]);
                // 減少下一步位置chaos下的score(增加優先值)
                score[tmp[2]][tmp[3]] -= B.Score() * 10;
                // fprintf(stderr, "1. %d%d = %d\n", tmp[2], tmp[3],
                // score[tmp[2]][tmp[3]] );
                B.Change(tmp[2], tmp[3], tmp[0], tmp[1]);
                if (board[i][j] == color) {
                    int i2 = 40, i1 = 30, i0 = 30, i1_ = 30, i2_ = 40;
                    int j2 = 40, j1 = 30, j0 = 30, j1_ = 30, j2_ = 40;
                    int i2b = 0, i1b = 0, i0b = 0, i1_b = 0, i2_b = 0;
                    int j2b = 0, j1b = 0, j0b = 0, j1_b = 0, j2_b = 0;

                    for (int k = j; k >= 0; k--) {
                        if (i >= 2) {
                            if (board[i - 2][k] == 0)
                                score[i - 2][k] += i2;
                            else
                                i2b++;
                            if (i2b == 2)
                                i2 -= 20;
                        }
                        if (i) {
                            if (board[i - 1][k] == 0)
                                score[i - 1][k] += i1;
                            else
                                i1b++;
                            if (i1b == 2)
                                i1 -= 20;
                        }
                        if (board[i][k] == 0)
                            score[i][k] += i0;
                        else if (k != j)
                            i0b++;
                        if (i0b == 2)
                            i0 -= 20;
                        if (i != 6) {
                            if (board[i + 1][k] == 0)
                                score[i + 1][k] += i1_;
                            else
                                i1_b++;
                            if (i1_b == 2)
                                i1_ -= 20;
                        }
                        if (i <= 4) {
                            if (board[i + 2][k] == 0)
                                score[i + 2][k] += i2_;
                            else
                                i2_b++;
                            if (i2_b == 2)
                                i2_ -= 20;
                        }
                    }
                    i2 = 40, i1 = 30, i0 = 30, i1_ = 30, i2_ = 40;
                    i2b = 0, i1b = 0, i0b = 0, i1_b = 0, i2_b = 0;
                    // fprintf(stderr, "%d %d %d %d %d\n", i2b, i1b, i0b, i1_b,
                    // i2_b);
                    for (int k = j; k < 7; k++) {
                        if (i >= 2) {
                            if (board[i - 2][k] == 0)
                                score[i - 2][k] += i2;
                            else
                                i2b++;
                            if (i2b == 2)
                                i2 -= 20;
                        }
                        if (i) {
                            if (board[i - 1][k] == 0)
                                score[i - 1][k] += i1;
                            else
                                i1b++;
                            if (i1b == 2)
                                i1 -= 20;
                        }
                        if (board[i][k] == 0)
                            score[i][k] += i0;
                        else if (k != j)
                            i0b++;
                        if (i0b == 2)
                            i0 -= 20;
                        if (i != 6) {
                            if (board[i + 1][k] == 0)
                                score[i + 1][k] += i1_;
                            else
                                i1_b++;
                            if (i1_b == 2)
                                i1_ -= 20;
                        }
                        if (i <= 4) {
                            if (board[i + 2][k] == 0)
                                score[i + 2][k] += i2_;
                            else
                                i2_b++;
                            if (i2_b == 2)
                                i2_ -= 20;
                        }
                    }

                    for (int k = i; k >= 0; k--) {
                        if (j >= 2) {
                            if (board[k][j - 2] == 0)
                                score[k][j - 2] += j2;
                            else
                                j2b++;
                            if (j2b == 2)
                                j2 -= 20;
                        }
                        if (j) {
                            if (board[k][j - 1] == 0)
                                score[k][j - 1] += j1;
                            else
                                j1b++;
                            if (j1b == 2)
                                j1 -= 20;
                        }
                        if (board[k][j] == 0)
                            score[k][j] += j0;
                        else if (k != i)
                            j0b++;
                        if (j0b == 2)
                            j0 -= 2;
                        if (j != 6) {
                            if (board[k][j + 1] == 0)
                                score[k][j + 1] += j1_;
                            else
                                j1_b++;
                            if (j1_b == 2)
                                j1_ -= 20;
                        }
                        if (j <= 4) {
                            if (board[k][j + 2] == 0)
                                score[k][j + 2] += j2_;
                            else
                                j2_b++;
                            if (j2_b == 2)
                                j2_ -= 20;
                        }
                    }

                    j2 = 40, j1 = 30, j0 = 30, j1_ = 30, j2_ = 40;
                    j2b = 0, j1b = 0, j0b = 0, j1_b = 0, j2_b = 0;

                    for (int k = i; k < 7; k++) {
                        if (j >= 2) {
                            if (board[k][j - 2] == 0)
                                score[k][j - 2] += j2;
                            else
                                j2b++;
                            if (j2b == 2)
                                j2 -= 20;
                        }
                        if (j) {
                            if (board[k][j - 1] == 0)
                                score[k][j - 1] += j1;
                            else
                                j1b++;
                            if (j1b == 2)
                                j1 -= 20;
                        }
                        if (board[k][j] == 0)
                            score[k][j] += j0;
                        else if (k != i)
                            j0b++;
                        if (j0b == 2)
                            j0 -= 20;
                        if (j != 6) {
                            if (board[k][j + 1] == 0)
                                score[k][j + 1] += j1_;
                            else
                                j1_b++;
                            if (j1_b == 2)
                                j1_ -= 20;
                        }
                        if (j <= 4) {
                            if (board[k][j + 2] == 0)
                                score[k][j + 2] += j2_;
                            else
                                j2_b++;
                            if (j2_b == 2)
                                j2_ -= 20;
                        }
                    }
                }
            } else {
                // empty，計算可能得分加到score(減少優先權)
                B.Add(i, j, color);
                score[i][j] += B.Score() * 10;
                // fprintf(stderr, "2. %ld%ld = %d\n", i, j, score[i][j]);
                B.Delete(i, j);
                // 擁擠度和優先權成正比
                if (i <= 2 and j <= 2)
                    score[i][j] -= up_left;
                else if (i <= 2 and j >= 4)
                    score[i][j] -= up_right;
                else if (i >= 4 and j <= 2)
                    score[i][j] -= down_left;
                else if (i >= 4 and j >= 4)
                    score[i][j] -= down_right;
            }
        }
    }

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            // find min
            if (min > score[i][j]) {
                // fprintf(stderr, "min %d\n", score[i][j] );
                min = score[i][j];
                min_c = i;
                min_r = j;
            }
        }
    }

    Detail* result = new Detail(CHAOS, color, min_c, min_r, TRIVIAL, TRIVIAL);
    return result;
}

// 1
///////////////////!!!////////////////////////////////////////////////////////////
int order_samecolor_find_move_tmp(int tmp[4], int move_tmp[4], char* result) {
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";

    // 找到想移動到的棋子了(move_tmp)
    result[0] = column[move_tmp[0]];
    result[1] = row[move_tmp[1]];
    int flag = 0;
    if (abs(move_tmp[0] - 3) >= abs(move_tmp[1] - 3)) {
        if (tmp[0] >= move_tmp[0]) {
            for (int i = move_tmp[0]; i <= tmp[0]; i++) {
                if (board[i][move_tmp[1]] != 0) {
                    result[2] = column[i - 1];
                    result[3] = row[move_tmp[1]];
                    flag = 1;
                    break;
                } else if (i == tmp[0]) {
                    result[2] = column[i];
                    result[3] = row[move_tmp[1]];
                    return 1;
                }
            }
        } else {
            for (int i = move_tmp[0]; i >= tmp[0]; i--) {
                if (board[i][move_tmp[1]] != 0) {
                    result[2] = column[i + 1];
                    result[3] = row[move_tmp[1]];
                    flag = 1;
                    break;
                } else if (i == tmp[0]) {
                    result[2] = column[i];
                    result[3] = row[move_tmp[1]];
                    return 1;
                }
            }
        }
    }
    // if flag=0/1
    if (tmp[1] >= move_tmp[1]) {
        for (int i = move_tmp[1]; i <= tmp[1]; i++) {
            if (board[move_tmp[0]][i] != 0) {
                result[0] = column[move_tmp[1]];
                result[1] = row[i - 1];
                flag = 2;
                break;
            } else if (i == tmp[1]) {
                result[2] = column[move_tmp[1]];
                result[3] = row[i];
                return 1;
            }
        }
    } else {
        for (int i = move_tmp[1]; i >= tmp[1]; i--) {
            if (board[i][move_tmp[1]] != 0) {
                result[2] = column[i - 1];
                result[3] = row[move_tmp[1]];
                flag++;
                break;
            } else if (i == tmp[1]) {
                result[0] = column[move_tmp[0]];
                result[1] = row[move_tmp[1]];
                result[2] = column[move_tmp[0]];
                result[3] = row[i];
                return 1;
            }
        }
    }

    if (flag == 0) {
        return 0;
    } else if (flag == 1) {
        if (result[2] != -1) {
            if (abs(result[0] - 3) < abs(result[3] - 3)) {
                result[2] = result[0];
                result[3] = result[1];
                result[0] = column[move_tmp[0]];
                result[1] = row[move_tmp[1]];
                return 1;
            } else {
                result[0] = column[move_tmp[0]];
                result[1] = row[move_tmp[1]];
                return 1;
            }
        } else {
            result[0] = column[move_tmp[0]];
            result[1] = row[move_tmp[1]];
            return 1;
        }
    }
    return 0;
}

// 2
bool order_samecolor_find_tmp(int numc, int color, int tmp[4], char* result) {
    // 找到想移動到的位置了(tmp)
    if (numc > 1 && colors[color] > numc && (tmp[1] != -1 || tmp[3] != -1)) {
        int move_tmp[4] = {-1, -1, -1, -1};
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < 7; j++) {
                if (board[i][j] == color) {
                    if (move_tmp[0] != -1 &&
                        (abs(i - 3) + abs(j - 3)) <
                            (abs(move_tmp[0] - 3) + abs(move_tmp[1] - 3))) {
                        move_tmp[0] = i;
                        move_tmp[1] = j;
                    } else if (move_tmp[0] == -1) {
                        move_tmp[0] = i;
                        move_tmp[1] = j;
                    }
                }
            }
        }
        if (tmp[0] != -1) {
            if (order_samecolor_find_move_tmp(tmp, move_tmp, result))
                return 1;
            else if (tmp[3] != -1) {
                tmp[0] = tmp[2];
                tmp[1] = tmp[3];
                if (order_samecolor_find_move_tmp(tmp, move_tmp, result))
                    return 1;
                else
                    return 0;
            }
        } else if (tmp[3] != -1) {
            tmp[0] = tmp[2];
            tmp[1] = tmp[3];
            if (order_samecolor_find_move_tmp(tmp, move_tmp, result))
                return 1;
            else
                return 0;
        } else
            return 0;
    } else {
        return 0;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

bool order_samecolor(char* result) {
    for (int i = 1; i <= 7; i++) {
        if (colors[i] > 1)
            break;
        else if (i == 7)
            return 0;
    }

    int counting_order[7] = {3, 4, 2, 5, 1, 6, 0};
    int counting_order_c = 0, counting_order_r = 0;
    int tmp[4] = {-1, -1, -1, -1};
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";

    // 旁邊往中間找
    while (counting_order_c >= 0) {
        counting_order_r = 6;
        while (counting_order_r >= 0) {
            int color = board[counting_order[counting_order_c]]
                             [counting_order[counting_order_r]];
            int c = counting_order[counting_order_c],
                r = counting_order[counting_order_r];
            int head[2] = {c, r}, tail[2] = {c, r};
            if (color == 0 || colors[color] <= 1)
                counting_order_r--;
            else {
                int numc = 1, numr = 1;
                c--;
                while (c >= 0) {  // 往左找重複
                    if (board[c][r] == color)
                        numc++;
                    else if (board[c][r] == 0) {
                        tmp[0] = c;
                        tmp[1] = r;
                    } else {
                        head[0] = c + 1;
                        head[1] = r;
                        break;
                    }
                    c--;
                }
                c = counting_order[counting_order_c] + 1;
                while (c < 7) {  // 往右找重複
                    if (board[c][r] == color)
                        numc++;
                    else if (board[c][r] == 0) {
                        tmp[2] = c;
                        tmp[3] = r;
                    } else {
                        tail[0] = c - 1;
                        tail[1] = r;
                        break;
                    }
                    c++;
                }
                if (order_samecolor_find_tmp(numc, color, tmp, result))
                    return 1;

                c = counting_order[counting_order_c];
                numc = 0;
                numr = 0;
                r = counting_order[counting_order_r] - 1;
                while (r >= 0) {  // 往上找重複
                    if (board[c][r] == color)
                        numr++;
                    else if (board[c][r] == 0) {
                        tmp[0] = c;
                        tmp[1] = r;
                    } else {
                        head[0] = c;
                        head[1] = r + 1;
                        break;
                    }
                    r--;
                }
                r = counting_order[counting_order_r] + 1;
                while (r < 7) {  // 往下找重複
                    if (board[c][r] == color)
                        numr++;
                    else if (board[c][r] == 0) {
                        tmp[2] = c;
                        tmp[3] = r;
                    } else {
                        tail[0] = c - 1;
                        tail[1] = r;
                        break;
                    }
                    r++;
                }
                if (order_samecolor_find_tmp(numc, color, tmp, result))
                    return 1;

                counting_order_r--;
            }
        }
        counting_order_c--;
    }
    // order_middlechess(result);
    return 0;
}

void c_r_continue(int i, int j, int* tmp_len, int c, int r) {
    tmp_len[0] = 0;
    tmp_len[1] = 0;
    // 檢查橫豎的連續顏色長度
    for (size_t b = 1; b < 6 - (j); b++) {
        if (i == c && (j + b) == r)
            break;

        if (board[i][j + b] == board[c][r])
            tmp_len[0]++;
        else
            break;
    }
    for (size_t b = 1; b <= j; b++) {
        if (i == c && (j - b) == r)
            break;
        if (board[i][j - b] == board[c][r])
            tmp_len[0]++;
        else
            break;
    }
    for (size_t b = 1; b < 6 - i; b++) {
        if ((i + b) == c && j == r)
            break;
        if (board[i + b][j] == board[c][r])
            tmp_len[1]++;
        else
            break;
    }
    for (size_t b = 1; b <= i; b++) {
        if ((i - b) == c && j == r)
            break;
        if (board[i - b][j] == board[c][r])
            tmp_len[1]++;
        else
            break;
    }
    return;
}

bool order_samecolor_2(char* result) {
    int tmp[6] = {0};
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";

    for (size_t i = 0; i < 7; i++) {
        for (size_t j = 0; j < 7; j++) {
            if (board[i][j] != 0) {
                // move_order( i, j, tmp);

                int tt[2] = {0};
                c_r_continue(i, j, tt, i, j);

                // right
                for (size_t a = 1; a < 6 - j; a++) {
                    if (board[i][j + a] == 0) {
                        int tmp_len[2] = {0};
                        c_r_continue(i, j + a, tmp_len, i, j);
                        if ((tmp_len[0] >= tt[0] && tmp_len[0] >= tt[1] &&
                             tmp_len[0] > tmp[4] && tmp_len[0] > tmp[5]) ||
                            (tmp_len[1] >= tt[0] && tmp_len[1] >= tt[1] &&
                             tmp_len[1] > tmp[4] && tmp_len[1] > tmp[5])) {
                            tmp[0] = i;
                            tmp[1] = j;
                            tmp[2] = i;
                            tmp[3] = j + a;
                            tmp[4] = tmp_len[0];
                            tmp[5] = tmp_len[1];
                        }
                    } else
                        break;
                }

                // left
                for (size_t a = 1; a <= j; a++) {
                    if (board[i][j - a] == 0) {
                        int tmp_len[2] = {0};
                        c_r_continue(i, j - a, tmp_len, i, j);
                        if (a == 1)
                            tmp_len[0] -= 1;
                        if ((tmp_len[0] >= tt[0] && tmp_len[0] >= tt[1] &&
                             tmp_len[0] > tmp[4] && tmp_len[0] > tmp[5]) ||
                            (tmp_len[1] >= tt[0] && tmp_len[1] >= tt[1] &&
                             tmp_len[1] > tmp[4] && tmp_len[1] > tmp[5])) {
                            tmp[0] = i;
                            tmp[1] = j;
                            tmp[2] = i;
                            tmp[3] = j - a;
                            tmp[4] = tmp_len[0];
                            tmp[5] = tmp_len[1];
                        }
                    } else
                        break;
                }

                // down
                for (size_t a = 1; a < 6 - i; a++) {
                    if (board[i + a][j] == 0) {
                        int tmp_len[2] = {0};
                        c_r_continue(i + a, j, tmp_len, i, j);
                        if (a == 1)
                            tmp_len[1] -= 1;
                        if ((tmp_len[0] >= tt[0] && tmp_len[0] >= tt[1] &&
                             tmp_len[0] > tmp[4] && tmp_len[0] > tmp[5]) ||
                            (tmp_len[1] >= tt[0] && tmp_len[1] >= tt[1] &&
                             tmp_len[1] > tmp[4] && tmp_len[1] > tmp[5])) {
                            tmp[0] = i;
                            tmp[1] = j;
                            tmp[2] = i + a;
                            tmp[3] = j;
                            tmp[4] = tmp_len[0];
                            tmp[5] = tmp_len[1];
                        }
                    } else
                        break;
                }

                // up
                for (size_t a = 1; a <= i; a++) {
                    if (board[i - a][j] == 0) {
                        int tmp_len[2] = {0};
                        c_r_continue(i - a, j, tmp_len, i, j);
                        if (a == 1)
                            tmp_len[1] -= 1;
                        if ((tmp_len[0] >= tt[0] && tmp_len[0] >= tt[1] &&
                             tmp_len[0] > tmp[4] && tmp_len[0] > tmp[5]) ||
                            (tmp_len[1] >= tt[0] && tmp_len[1] >= tt[1] &&
                             tmp_len[1] > tmp[4] && tmp_len[1] > tmp[5])) {
                            tmp[0] = i;
                            tmp[1] = j;
                            tmp[2] = i - a;
                            tmp[3] = j;
                            tmp[4] = tmp_len[0];
                            tmp[5] = tmp_len[1];
                        }
                    } else
                        break;
                }
            }
        }
    }
    result[0] = column[tmp[0]];
    result[1] = row[tmp[1]];
    result[2] = column[tmp[2]];
    result[3] = row[tmp[3]];

    if (board[result[2]][result[3]] != 0)
        return 0;
    else if (tmp[0] == tmp[2] && tmp[1] == tmp[3])
        return 0;
    else if (tmp[4] > 0 || tmp[5] > 0)
        return 1;
    else
        return 0;
}

// 4
bool order_middlechess(char* result) {
    int counting_order[7] = {3, 4, 2, 5, 1, 6, 0};
    int counting_order_c = 0, counting_order_r = 0;
    char column[8] = "ABCDEFG";
    char row[8] = "abcdefg";

    while (counting_order_c < 7) {
        counting_order_r = 0;
        while (counting_order_r < 7) {
            int c = counting_order[counting_order_c],
                r = counting_order[counting_order_r];

            if ((c - 1 >= 0 && board[c - 1][r] == board[c][r]) ||
                (c + 1 < 7 && board[c + 1][r] == board[c][r]) ||
                (r - 1 >= 0 && board[c][r - 1] == board[c][r]) ||
                (r + 1 < 7 && board[c][r + 1] == board[c][r])) {
                counting_order_r++;
                continue;
            }

            if (board[c][r] != 0) {
                int tmp[9];
                for (int i = 0; i < 9; i++)
                    tmp[i] = -1;

                for (int i = c - 1; i >= 0; i--) {
                    if (board[i][r] == 0) {
                        tmp[0] = i;
                        tmp[1] = r;
                    } else
                        break;
                }

                for (int i = c + 1; i < 7; i++) {
                    if (board[i][r] == 0) {
                        tmp[2] = i;
                        tmp[3] = r;
                    } else
                        break;
                }

                for (int i = r - 1; i >= 0; i--) {
                    if (board[c][i] == 0) {
                        tmp[4] = c;
                        tmp[5] = i;
                    } else
                        break;
                }

                for (int i = r + 1; i < 7; i++) {
                    if (board[c][i] == 0) {
                        tmp[6] = c;
                        tmp[7] = i;
                    } else
                        break;
                }

                int dis[4] = {-1}, max = 0;
                for (int i = 0; i < 4; i++) {
                    if (tmp[2 * i] == -1 || tmp[2 * i + 1] == -1)
                        continue;
                    else {
                        dis[i] = abs(tmp[2 * i] - 3) + abs(tmp[2 * i + 1] - 3);
                        if (dis[i] > dis[max])
                            max = i;
                    }
                }
                if (dis[max] == -1)
                    return 0;

                result[0] = column[c];
                result[1] = row[r];
                result[2] = column[tmp[2 * max]];
                result[3] = row[tmp[2 * max + 1]];
                // fprintf(stderr, "%d:%d%d\n", max, tmp[2 * max],
                //         tmp[2 * max + 1]);
                // fprintf(stderr, "%c%c%c%c\n", result[0], result[1],
                // result[2],
                //         result[3]);
                return 1;
            }
            counting_order_r++;
        }
        counting_order_c++;
    }

    return 0;
}

// 6
Detail* winner_order() {
    char* result = (char*)malloc(10);
    int steps = STEPS, nums = NUMS;  //, nums2=40;
    Detail* res = new Detail();
    if (chess_num % steps == 0 && chess_num < nums) {
        if (!order_samecolor_2(result))
            if (!order_samecolor(result))
                if (!order_middlechess(result))
                    res = greedy_order();
                else {
                    res = new Detail(ORDER, TRIVIAL, result[0] - 'A',
                                     result[1] - 'a', result[2] - 'A',
                                     result[3] - 'a');
                }
            else {
                res =
                    new Detail(ORDER, TRIVIAL, result[0] - 'A', result[1] - 'a',
                               result[2] - 'A', result[3] - 'a');
            }
        else {
            res = new Detail(ORDER, TRIVIAL, result[0] - 'A', result[1] - 'a',
                             result[2] - 'A', result[3] - 'a');
        }
    } else {
        res = greedy_order();
    }

    if (board[res->mc][res->mr] != 0) {
        res = greedy_order();
    }
    return res;
}

void Tree::Search_chaos(Node* current,
                        int i,
                        int floor,
                        int color,
                        int player) {
    if (i == floor) {
        return;
    }

    if (i == 0) {
        int sum = 0, size = 0, average = 0;
        for (int c = 0; c < 7; c++) {
            for (int r = 0; r < 7; r++) {
                if (board[c][r] == 0) {
                    B.Add(c, r, color);
                    sum += B.Score();
                    size++;
                    B.Delete(c, r);
                }
            }
        }

        average = sum / size;
        for (int c = 0; c < 7; c++) {
            for (int r = 0; r < 7; r++) {
                if (board[c][r] == 0 and B.Score() <= average) {
                    B.Add(c, r, color);
                    Insert(new Detail(CHAOS, color, c, r, 0, 0), current);
                    Search_order(
                        current->children[current->children.size() - 1], i + 1,
                        floor, player);
                    B.Delete(c, r);
                }
            }
        }
    } else {
        for (int k = 1; k < 7; k++) {
            // Algorithm
            if (colors[k - 1] != 7) {
                Detail* res = tree_chao(k);
                B.Add(res->c, res->r, k);
                Insert(res, current);
                Search_order(current->children[current->children.size() - 1],
                             i + 1, floor, player);
                B.Delete(res->c, res->r);
                if (i == floor - 1) {
                    benefit =
                        Compare(current->children[current->children.size() - 1],
                                benefit, player);
                }
            }
        }
    }
    floor++;
}

void Tree::Search_order(Node* current, int i, int floor, int player) {
    if (i == floor) {
        return;
    }

    if (i == 0) {
        for (int c = 0; c < 7; c++) {
            for (int r = 0; r < 7; r++) {
                int sum = 0, size = 0, average = 0;
                if (board[c][r] != 0) {
                    // right
                    // for (int k = 1; k < 7 - r; k++) {
                    //     if (board[c][r + k] == 0) {
                    //         B.Change(c, r, c, r + k);
                    //         sum += B.Score();
                    //         size++;
                    //         B.Change(c, r + k, c, r);
                    //     } else {
                    //         break;
                    //     }
                    // }
                    // // left
                    // for (int k = 1; k < r; k++) {
                    //     if (board[c][r - k] == 0) {
                    //         B.Change(c, r, c, r - k);
                    //         sum += B.Score();
                    //         size++;
                    //         B.Change(c, r - k, c, r);
                    //     } else {
                    //         break;
                    //     }
                    // }
                    // // up
                    // for (int k = 1; k < c; k++) {
                    //     if (board[c - k][r] == 0) {
                    //         B.Change(c, r, c - k, r);
                    //         sum += B.Score();
                    //         size++;
                    //         B.Change(c - k, r, c, r);
                    //     } else {
                    //         break;
                    //     }
                    // }
                    // // down
                    // for (int k = 1; k < 7 - c; k++) {
                    //     if (board[c + k][r] == 0) {
                    //         B.Change(c, r, c + k, r);
                    //         sum += B.Score();
                    //         size++;
                    //         B.Change(c + k, r, c, r);
                    //     } else {
                    //         break;
                    //     }
                    // }
                    // average = sum / size;
                    // right
                    for (int k = 1; k < 7 - r; k++) {
                        if (board[c][r + k] == 0) {
                            if (B.Score() >= average) {
                                B.Change(c, r, c, r + k);
                                Insert(
                                    new Detail(ORDER, TRIVIAL, c, r, c, r + k),
                                    current);
                                Search_chaos(
                                    current->children[current->children.size() -
                                                      1],
                                    i + 1, floor, -1, player);
                                B.Change(c, r + k, c, r);
                            }
                        } else {
                            break;
                        }
                    }
                    // left
                    for (int k = 1; k < r; k++) {
                        if (board[c][r - k] == 0) {
                            if (B.Score() >= average) {
                                B.Change(c, r, c, r - k);
                                Insert(
                                    new Detail(ORDER, TRIVIAL, c, r, c, r - k),
                                    current);
                                Search_chaos(
                                    current->children[current->children.size() -
                                                      1],
                                    i + 1, floor, -1, player);
                                B.Change(c, r - k, c, r);
                            }
                        } else {
                            break;
                        }
                    }
                    // up
                    for (int k = 1; k < c; k++) {
                        if (board[c - k][r] == 0) {
                            if (B.Score() >= average) {
                                B.Change(c, r, c - k, r);
                                Insert(
                                    new Detail(ORDER, TRIVIAL, c, r, c - k, r),
                                    current);
                                Search_chaos(
                                    current->children[current->children.size() -
                                                      1],
                                    i + 1, floor, -1, player);
                                B.Change(c - k, r, c, r);
                            }
                        } else {
                            break;
                        }
                    }
                    // down
                    for (int k = 1; k < 7 - c; k++) {
                        if (board[c + k][r] == 0) {
                            if (B.Score() >= average) {
                                B.Change(c, r, c + k, r);
                                Insert(
                                    new Detail(ORDER, TRIVIAL, c, r, c + k, r),
                                    current);
                                Search_chaos(
                                    current->children[current->children.size() -
                                                      1],
                                    i + 1, floor, -1, player);
                                B.Change(c + k, r, c, r);
                            }
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    } else {
        // Algorithm
        Detail* res = tree_order();
        B.Change(res->c, res->r, res->mc, res->mr);
        Insert(res, current);
        Search_chaos(current->children[current->children.size() - 1], i + 1,
                     floor, -1, player);
        B.Change(res->mc, res->mr, res->c, res->r);
    }

    floor++;
    return;
}

Node* Tree::Search(Node* current, int floor, int color, int player) {
    int i = 0;
    benefit = new Node();
    benefit->score = player == CHAOS ? 1000 : -1000;
    if (player == CHAOS) {
        Search_chaos(current, i, floor, color, player);
    } else if (player == ORDER) {
        Search_order(current, i, floor, player);
    } else {
        fprintf(stderr, "ERROR\n");
        exit(0);
    }

    return benefit->parent;
}

void chaos() {
    // fprintf(stderr, "I'm Chaos\n");
    char input_chaos[10] = {0};
    char output_chaos[4] = {0};

    int color = 0;
    double start, end;
    double cost = 0;
    while (1) {
        fprintf(stderr, "chess num : %d\n", chess_num);
        start = clock();
        scanf(" %s", input_chaos);
        // fprintf(stderr, "I get %s\n", input_chaos);
        color = input_chaos[0] - '0';

        int floor = chess_num + (CHAOS_FLOOR + 2) / 2 >= 48
                        ? (48 - chess_num) * 2 - 1
                        : CHAOS_FLOOR;
        bool changeway = floor <= 1 ? true : false;

        if (chess_num >= CHAOS_START and chess_num <= CHAOS_END and
            !changeway) {
            T.Init();
            Node* cur = new Node();
            Node* pick = new Node();

            pick = T.Search(cur, floor, color, CHAOS);
            fprintf(stderr, "floor: %d\n", floor);
            printf("%c%c\n", (char)(pick->info->c + 'A'),
                   (char)(pick->info->r + 'a'));
            // T.Show(pick);
            fflush(stdout);
            end = clock();
            cost += (double)(end - start) / CLOCKS_PER_SEC;
            fprintf(stderr, "chaos if time : %lf secs\n", cost);
            B.Add(pick->info->c, pick->info->r, pick->info->color);
        } else {
            Detail* pick = new Detail();
            pick = algorithm_chao(color);
            printf("%c%c\n", (char)(pick->c + 'A'), (char)(pick->r + 'a'));
            // T.Show(pick);
            fflush(stdout);
            end = clock();
            cost += (double)(end - start) / CLOCKS_PER_SEC;
            fprintf(stderr, "chaos else time : %lf secs\n", cost);
            B.Add(pick->c, pick->r, color);
        }
        chess_num++;

        scanf(" %s", input_chaos);
        // fprintf(stderr, "the order print %s\n", input_chaos);
        if (strncmp(input_chaos, "Quit", 4) == 0)
            break;
        else
            B.Change(input_chaos[0] - 'A', input_chaos[1] - 'a',
                     input_chaos[2] - 'A', input_chaos[3] - 'a');
        // end = clock();
        // fprintf(stderr, "time : %lf ms\n", end - start);
    }
}

void order(char input[10]) {
    // fprintf(stderr, "I'm Order\n");
    char input_order[10] = {0};
    char output_order[5] = {0};
    double start, end;
    double cost = 0;
    for (int i = 0; i < 3; i++) {
        input_order[i] = input[i];
    }

    while (1) {
        fprintf(stderr, "chess num : %d\n", chess_num);
        start = clock();
        // fprintf(stderr, "I get %c%c%c\n", input_order[0], input_order[1],
        //                 input_order[2]);
        B.Add(input_order[1] - 'A', input_order[2] - 'a', input_order[0] - '0');
        chess_num++;

        int floor = chess_num + (ORDER_FLOOR + 2) / 2 >= 48
                        ? (48 - chess_num) * 2 - 2
                        : ORDER_FLOOR;
        bool changeway = floor <= 1 ? true : false;

        if (chess_num < ORDER_FIRST) {
            Detail* pick = new Detail();
            pick = algorithm_order();
            printf("%c%c%c%c\n", pick->c + 'A', pick->r + 'a', pick->mc + 'A',
                   pick->mr + 'a');
            // T.Show(pick);
            fflush(stdout);
            end = clock();
            cost += (double)(end - start) / CLOCKS_PER_SEC;
            fprintf(stderr, "order if time : %lf secs\n", cost);
            B.Change(pick->c, pick->r, pick->mc, pick->mr);
        } else if (chess_num <= ORDER_SECOND) {
            Detail* pick = new Detail();
            pick = tree_order();
            printf("%c%c%c%c\n", pick->c + 'A', pick->r + 'a', pick->mc + 'A',
                   pick->mr + 'a');
            // T.Show(pick);
            fflush(stdout);
            end = clock();
            cost += (double)(end - start) / CLOCKS_PER_SEC;
            fprintf(stderr, "order else if 1 time : %lf secs\n", cost);
            B.Change(pick->c, pick->r, pick->mc, pick->mr);
        } else if (chess_num <= ORDER_THIRD and !changeway) {
            T.Init();
            Node* cur = new Node();
            Node* pick = new Node();

            pick = T.Search(cur, floor, TRIVIAL, ORDER);
            fprintf(stderr, "floor: %d\n", floor);
            printf("%c%c%c%c\n", pick->info->c + 'A', pick->info->r + 'a',
                   pick->info->mc + 'A', pick->info->mr + 'a');
            // T.Show(pick);
            fflush(stdout);
            end = clock();
            cost += (double)(end - start) / CLOCKS_PER_SEC;
            fprintf(stderr, "order else if 2 time : %lf secs\n", cost);
            B.Change(pick->info->c, pick->info->r, pick->info->mc,
                     pick->info->mr);
        } else {
            Detail* pick = new Detail();
            pick = tree_order();
            printf("%c%c%c%c\n", pick->c + 'A', pick->r + 'a', pick->mc + 'A',
                   pick->mr + 'a');
            // T.Show(pick);
            fflush(stdout);
            end = clock();
            cost += (double)(end - start) / CLOCKS_PER_SEC;
            fprintf(stderr, "order else time : %lf secs\n", cost);
            B.Change(pick->c, pick->r, pick->mc, pick->mr);
        }
        scanf(" %s", input_order);
        if (strncmp(input_order, "Quit", 4) == 0) {
            break;
        }
        // end = clock();
        // fprintf(stderr, "time : %lf ms\n", end - start);
    }
}

void debug() {
    Tree one;

    one.Init();
    one.Insert(new Detail(TRIVIAL, 2, TRIVIAL, TRIVIAL, TRIVIAL, TRIVIAL),
               one.root);
    one.Insert(new Detail(TRIVIAL, -1, TRIVIAL, TRIVIAL, TRIVIAL, TRIVIAL),
               one.root->children[0]);
    // one.Show(one.root->children[0]);
    printf("%d\n", one.root->children[0]->children[0]->parent->info->color);
}

int main() {
    // debug();
    // algorithm
    algorithm_chao = winner_chao;
    algorithm_order = winner_order;
    tree_chao = greedy_chao;
    tree_order = greedy_order;
    fprintf(stderr, "constant CLOCKS_PER_SEC: %ld\n", CLOCKS_PER_SEC);
    char input[10] = {0};
    scanf(" %s", input);

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            board[i][j] = 0;
        }
    }
    if (strncmp(input, "Start", 5) == 0) {
        chaos();
    } else {
        order(input);
    }
    return 0;
}