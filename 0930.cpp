#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

// #define PRINT_PATH
// #define PRINT_ANCESTER

long long count = 0;

#define CHAOS 1
#define ORDERS 2
#define TRIVIAL -1

int board[7][7] = {0};
int colors[7] = {0};

class Node;

class Board {
public:
    int Score();
    void Add(int c, int r, int color);
    void Delete(int c, int r);
    void Change(int c1, int r1, int c2, int r2);
    void Move(int c, int r, int *result);
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

void Board::Move(int c, int r, int *result) {
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
    fprintf(stderr, "   ");
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
    Detail(int a, int b, int c, int d, int e, int f) : player(a), color(b), c(c), r(d), mc(e), mr(f){};
    friend class Node;
};

class Node {
public:
    Detail *info;
    int height;
    int score;
    Node *parent;
    vector<Node *> childs;
    Node() : info(0), height(0), score(0), parent(0), childs(0){};
    Node(Detail *a) : info(a), height(0), score(0), parent(0), childs(0){};
    friend class Tree;
};

Board B;

void pick_chaos(char *result, int color) {
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
    result[0] = column[min_c];
    result[1] = row[min_r];
}

void pick_order(char *result) {
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
    result[0] = column[max_c];
    result[1] = row[max_r];
    result[2] = column[max_move_c];
    result[3] = row[max_move_r];
}

void chaos() {
    char input_chaos[10] = {0};
    char output_chaos[3] = {0};

    int count = 0;
    int color = 0;
    while (1) { 
        scanf(" %s", input_chaos);
        color = input_chaos[0] - '0';

        pick_chaos(output_chaos, color);
        printf("%c%c\n", output_chaos[0], output_chaos[1]);
        fflush(stdout);
        B.Add(output_chaos[0] - 'A', output_chaos[1] - 'a',
                    input_chaos[0] - '0');
        // B.Show();
        count++;

        scanf(" %s", input_chaos);
        fprintf(stderr, "the order print %s\n", input_chaos);
        if (strncmp(input_chaos, "Quit", 4) == 0)
        break;
        else
        B.Change(input_chaos[0] - 'A', input_chaos[1] - 'a',
                    input_chaos[2] - 'A', input_chaos[3] - 'a');
    }
}

void order(char input[10]) {
    char input_order[10] = {0};
    char output_order[5] = {0};
    for (int i = 0; i < 3; i++) {
        input_order[i] = input[i];
    }

    int count = 0;
    while (1) {
        B.Add(input_order[1] - 'A', input_order[2] - 'a', input_order[0] - '0');
        count++;

        pick_order(output_order);
        printf("%c%c%c%c\n", output_order[0], output_order[1], output_order[2],
                output_order[3]);
        fflush(stdout);
        B.Change(output_order[0] - 'A', output_order[1] - 'a',
                    output_order[2] - 'A', output_order[3] - 'a');
        // B.Show();
        scanf(" %s", input_order);
        if (strncmp(input_order, "Quit", 4) == 0) {
            break;
        }
    }
}

int main()
{
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