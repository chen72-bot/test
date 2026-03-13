//v1.0低配版赛博国际象棋对弈系统介绍
// 棋子：
// 'K'：白王   'k'：黑王
// 'Q'：白后   'q'：黑后
// 'R'：白车   'r'：黑车
// 'B'：白象   'b'：黑象
// 'N'：白马   'n'：黑马
// 'P'：白兵   'p'：黑兵
//
//
// 棋子行走规则：
// 王(K/k)：每次可向任意方向走一格
// 后(Q/q)：可沿八向任意移动
// 车(R/r)：可沿横竖直线任意移动
// 象(B/b)：可沿对角线任意移动
// 马(N/n)：走日
// 兵(P/p)：只能向前走一格，首步可走两格，吃子时斜走一格
//兵在后续版本中可以升变。
//后续版本添加了简易版的王车易位规则
//
//
// 变量&函数说明：
// SIZE: 棋盘的大小，国际象棋为8
// Piece: 单链表结构体，存储棋子的类型和位置
// create_piece: 创建一个新的棋子节点
// init_board: 初始化棋盘，返回所有棋子的链表头指针
// find_piece: 查找指定位置的棋子，返回指针
// remove_piece: 删除指定位置的棋子，释放内存，返回新链表头
// print_board: 打印棋盘，升变棋子带星号
// is_white: 判断白方；is_black: 判断黑方
// valid_pos: 检查坐标是否合法
// has_own: 检查目标是否合法
// move_valid: 检查走法是否合法
// main: 主函数，实现人机交互
// v1.1：新增变量from、to: 存储用户输入的起点和终点坐标
// v1.2：新增升变规则：
// 兵(P/p)到达对方底线时，玩家可选择升变为王后(Q/q)、车(R/r)、象(B/b)、马(N/n)
// 升变的棋子在棋盘上带星号显示（如 Q*、r* 等）
// v1.2.5：新增功能：
//   - 玩家只输入起点时，打印该棋子所有可落点
//   - 升变时提示玩家选择升变棋子
//   - 升变棋子用高位标记并带星号显示
// v1.3：新增王车易位规则：
//   王和车都未移动过，王与车之间无其他棋子，且王不会经过或落在被攻击的格子上时允许王车易位；
//   玩家输入如“e1 g1”或“e8 c8”即可尝试王车易位
// v1.3.5：删除了王不会经过或落在被攻击的格子上时才允许王车易位这一限制，这使得运行效率大幅提升
// v2.0：新增保存和读取棋局功能
//   玩家输入“save”保存棋局，输入“load”读取棋局。仅展示功能，默认只能保存一局。
//   读取棋局时，程序会自动释放原有棋盘内存.
//
//v2.1：v2.0版本的一些bug修复，现在可以侥幸地说，程序从低配版进化为中低配版了。
//v2.2：521特别版！！！添加了521彩蛋,扣1告诉你怎么触发（
//just kidding！：），键入521即可触发

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 8  //国际象棋棋盘大小

// 单链表存储棋子
typedef struct Piece {
    char type; // 棋子种类
    int row, col; // 位置
    int moved;    // 是否移动过（0未动，1已动）
    struct Piece *next;
} Piece;

// 创建新棋子节点
Piece* create_piece(char type, int row, int col) {
    Piece* p = (Piece*)malloc(sizeof(Piece));
    if (!p) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    p->type = type;
    p->row = row;
    p->col = col;
    p->moved = 0;
    p->next = NULL;
    return p;
}

// 初始化棋盘，返回链表头指针
Piece* init_board() {
    char init[SIZE][SIZE] = {
        {'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}
    };
    Piece *head = NULL, *tail = NULL;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (init[i][j] != ' ') {
                Piece *p = create_piece(init[i][j], i, j);
                if (!head) head = tail = p;
                else { tail->next = p; tail = p; }
            }
        }
    }
    return head;
}

// 查找指定位置的棋子，返回指针
Piece* find_piece(Piece* head, int row, int col) {
    while (head) {
        if (head->row == row && head->col == col) return head;
        head = head->next;
    }
    return NULL;
}

// 删除指定位置的棋子，返回新链表头
Piece* remove_piece(Piece* head, int row, int col) {
    Piece *prev = NULL, *cur = head;
    while (cur) {
        if (cur->row == row && cur->col == col) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur);
            return head;
        }
        prev = cur;
        cur = cur->next;
    }
    return head;
}

// 打印棋盘，升变棋子带星号
void print_board(Piece* head) {
    printf("    a   b   c   d   e   f   g   h\n");
    printf("  +---+---+---+---+---+---+---+---+\n");
    for (int i = 0; i < SIZE; i++) {
        printf("%d |", 8 - i);
        for (int j = 0; j < SIZE; j++) {
            Piece* p = find_piece(head, i, j);
            if (p) {
                if ((p->type & 0x80) && 
                    ((p->type & 0x7F) == 'Q' || (p->type & 0x7F) == 'R' || (p->type & 0x7F) == 'B' || (p->type & 0x7F) == 'N' ||
                     (p->type & 0x7F) == 'q' || (p->type & 0x7F) == 'r' || (p->type & 0x7F) == 'b' || (p->type & 0x7F) == 'n')) {
                    printf("%c*|", p->type & 0x7F);
                } else {
                    printf(" %c |", p->type & 0x7F);
                }
            } else printf("   |");
        }
        printf(" %d\n", 8 - i);
        printf("  +---+---+---+---+---+---+---+---+\n");
    }
    printf("    a   b   c   d   e   f   g   h\n");
}

// 判断白方棋子
int is_white(char c) { return c >= 'A' && c <= 'Z'; }
// 判断黑方
int is_black(char c) { return c >= 'a' && c <= 'z'; }

// 检查输入合法性（棋盘大小）
int valid_pos(int x, int y) { return x >= 0 && x < SIZE && y >= 0 && y < SIZE; }

// 检查输入合法性（目标位置是否已经有己方棋子）
int has_own(Piece* head, int row, int col, int turn) {
    Piece* p = find_piece(head, row, col);
    if (!p) return 0;
    if (turn == 0) return is_white(p->type);
    else return is_black(p->type);
}

// 检查输入合法性（走法）
int move_valid(char type, int fx, int fy, int tx, int ty, Piece* head) {
    int dx = tx - fx, dy = ty - fy;
    int adx = dx > 0 ? dx : -dx, ady = dy > 0 ? dy : -dy;
    if (type == 'P') { // 白兵
        if (fy == ty && fx - tx == 1 && !find_piece(head, tx, ty)) return 1;
        if (fy == ty && fx == 6 && fx - tx == 2 && !find_piece(head, fx-1, fy) && !find_piece(head, tx, ty)) return 1;
        if (adx == 1 && ady == 1 && fx - tx == 1 && find_piece(head, tx, ty) && is_black(find_piece(head, tx, ty)->type)) return 1;
        return 0;
    }
    if (type == 'p') { // 黑兵
        if (fy == ty && tx - fx == 1 && !find_piece(head, tx, ty)) return 1;
        if (fy == ty && fx == 1 && tx - fx == 2 && !find_piece(head, fx+1, fy) && !find_piece(head, tx, ty)) return 1;
        if (adx == 1 && ady == 1 && tx - fx == 1 && find_piece(head, tx, ty) && is_white(find_piece(head, tx, ty)->type)) return 1;
        return 0;
    }
    if (type == 'R' || type == 'r') {
        if (fx == tx) {
            int step = fy < ty ? 1 : -1;
            for (int y = fy + step; y != ty; y += step) if (find_piece(head, fx, y)) return 0;
            return 1;
        }
        if (fy == ty) {
            int step = fx < tx ? 1 : -1;
            for (int x = fx + step; x != tx; x += step) if (find_piece(head, x, fy)) return 0;
            return 1;
        }
        return 0;
    }
    if (type == 'N' || type == 'n') {
        return (adx == 2 && ady == 1) || (adx == 1 && ady == 2);
    }
    if (type == 'B' || type == 'b') {
        if (adx == ady && adx > 0) {
            int stepx = dx > 0 ? 1 : -1;
            int stepy = dy > 0 ? 1 : -1;
            for (int i = 1; i < adx; i++) if (find_piece(head, fx + i*stepx, fy + i*stepy)) return 0;
            return 1;
        }
        return 0;
    }
    if (type == 'Q' || type == 'q') {
        if (fx == tx) {
            int step = fy < ty ? 1 : -1;
            for (int y = fy + step; y != ty; y += step) if (find_piece(head, fx, y)) return 0;
            return 1;
        }
        if (fy == ty) {
            int step = fx < tx ? 1 : -1;
            for (int x = fx + step; x != tx; x += step) if (find_piece(head, x, fy)) return 0;
            return 1;
        }
        if (adx == ady && adx > 0) {
            int stepx = dx > 0 ? 1 : -1;
            int stepy = dy > 0 ? 1 : -1;
            for (int i = 1; i < adx; i++) if (find_piece(head, fx + i*stepx, fy + i*stepy)) return 0;
            return 1;
        }
        return 0;
    }
    if (type == 'K' || type == 'k') {
        return (adx <= 1 && ady <= 1 && (adx + ady) > 0);
    }
    return 0;
}

// 判断是否可以王车易位
// 参数说明：
// turn：当前回合（0白方，1黑方）
// fx, fy：王的起始位置
// tx, ty：王的目标位置
int can_castle(Piece* head, int turn, int fx, int fy, int tx, int ty) {
    // 白方王e1(7,4)，黑方王e8(0,4)
    if (turn == 0 && fx == 7 && fy == 4 && tx == 7 && (ty == 6 || ty == 2)) {
        Piece* king = find_piece(head, 7, 4);
        if (!king || king->type != 'K' || king->moved) return 0; // 王未动过
        if (ty == 6) { // 王侧易位
            Piece* rook = find_piece(head, 7, 7);
            if (!rook || rook->type != 'R' || rook->moved) return 0;
            if (find_piece(head, 7, 5) || find_piece(head, 7, 6)) return 0;
            return 1;
        } else if (ty == 2) { // 后侧易位
            Piece* rook = find_piece(head, 7, 0);
            if (!rook || rook->type != 'R' || rook->moved) return 0;
            if (find_piece(head, 7, 1) || find_piece(head, 7, 2) || find_piece(head, 7, 3)) return 0;
            return 1;
        }
    }
    if (turn == 1 && fx == 0 && fy == 4 && tx == 0 && (ty == 6 || ty == 2)) {
        Piece* king = find_piece(head, 0, 4);
        if (!king || king->type != 'k' || king->moved) return 0;
        if (ty == 6) { // 王侧易位
            Piece* rook = find_piece(head, 0, 7);
            if (!rook || rook->type != 'r' || rook->moved) return 0;
            if (find_piece(head, 0, 5) || find_piece(head, 0, 6)) return 0;
            return 1;
        } else if (ty == 2) { // 后侧易位
            Piece* rook = find_piece(head, 0, 0);
            if (!rook || rook->type != 'r' || rook->moved) return 0;
            if (find_piece(head, 0, 1) || find_piece(head, 0, 2) || find_piece(head, 0, 3)) return 0;
            return 1;
        }
    }
    return 0;
}

// 保存棋局
void save_game(Piece* head, int turn, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Failed to open file for saving!\n");
        return;
    }
    fprintf(fp, "%d\n", turn);
    for (Piece* p = head; p; p = p->next) {
        fprintf(fp, "%d %d %d %d\n", (int)p->type, p->row, p->col, p->moved);
    }
    fclose(fp);
    printf("Game saved to %s\n", filename);
}

// 读取棋局
Piece* load_game(int* turn, const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open file for loading!\n");
        return NULL;
    }
    Piece* head = NULL, *tail = NULL;
    int t;
    if (fscanf(fp, "%d", &t) != 1) {
        fclose(fp);
        printf("File format error!\n");
        return NULL;
    }
    *turn = t;
    int type, row, col, moved;
    while (fscanf(fp, "%d %d %d %d", &type, &row, &col, &moved) == 4) {
        Piece* p = create_piece((char)type, row, col);
        p->moved = moved;
        if (!head) head = tail = p;
        else { tail->next = p; tail = p; }
    }
    fclose(fp);
    printf("Game loaded from %s\n", filename);
    return head;
}

//主函数
int main() {
    Piece* head = init_board();
    char input[16], from[3] = "", to[3] = "";
    int turn = 0;
    char filename[64] = "chess_save.txt";
    while (1) {
        print_board(head);
        printf("%s move (e.g. e2 e4 or e2, save, load): ", turn == 0 ? "White" : "Black");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Input error!\n");
            continue;
        }
        if (strncmp(input, "save", 4) == 0) {
            save_game(head, turn, filename);
            continue;
        }
        if (strncmp(input, "load", 4) == 0) {
            int newturn = 0;
            Piece* newhead = load_game(&newturn, filename);
            if (newhead) {
                // 释放原有棋盘
                while (head) {
                    Piece* tmp = head;
                    head = head->next;
                    free(tmp);
                }
                head = newhead;
                turn = newturn;
            }
            continue;
        }
        if (strncmp(input, "521", 3) == 0) {
            printf("\n—— 521 ——\n");
            printf("521,521,521,521,521,521,521,521,521,521\n");
            printf("Love & Chess, Happy 521!\n\n");
            continue;
        }
        int n = sscanf(input, "%2s %2s", from, to);
        if (n < 1 || strlen(from) != 2 || (n == 2 && strlen(to) != 2)) {
            printf("Input format error!\n");
            continue;
        }
        int fx = 8 - (from[1] - '0');
        int fy = from[0] - 'a';
        int tx = -1, ty = -1;
        if (n == 2) {
            tx = 8 - (to[1] - '0');
            ty = to[0] - 'a';
        }
        if (!valid_pos(fx, fy) || (n == 2 && !valid_pos(tx, ty))) {
            printf("Invalid position!\n");
            continue;
        }
        Piece* p = find_piece(head, fx, fy);
        if (!p) {
            printf("No piece at the starting position!\n");
            continue;
        }
        if ((turn == 0 && !is_white(p->type)) || (turn == 1 && !is_black(p->type))) {
            printf("You can't move the opponent's piece!\n");
            continue;
        }
        // 如果只输入了己方棋子位置，打印所有预落点
        if (n == 1) {
            printf("Possible moves for %c at %c%d: ", p->type, fy+'a', 8-fx);
            int found = 0;
            for (int tx2 = 0; tx2 < SIZE; tx2++) {
                for (int ty2 = 0; ty2 < SIZE; ty2++) {
                    if ((tx2 != fx || ty2 != fy) && valid_pos(tx2, ty2) && !has_own(head, tx2, ty2, turn) && move_valid(p->type, fx, fy, tx2, ty2, head)) {
                        printf("%c%d ", ty2+'a', 8-tx2);
                        found = 1;
                    }
                }
            }
            if (!found) printf("No valid moves.");
            printf("\n");
            continue;
        }
        if (has_own(head, tx, ty, turn)) {
            printf("You can't capture your own piece!\n");
            continue;
        }
        if (!move_valid(p->type, fx, fy, tx, ty, head)) {
            printf("Invalid move!\n");
            continue;
        }
        // 释放被吃掉棋子的内存
        Piece* captured = find_piece(head, tx, ty);
        if (captured) {
            head = remove_piece(head, tx, ty); 
        }
        // 在链表中存储棋子的移动
        p->row = tx; p->col = ty;
        // 升变
        if ((p->type == 'P' && p->row == 0) || (p->type == 'p' && p->row == 7)) {
            char choice;
            printf("Congratulations,Pawn promotion! Please choose new piece (Q/R/B/N): ");
            while (1) {
                if (scanf(" %c", &choice) != 1) {
                    printf("Input error!\n");
                    while (getchar() != '\n');
                    continue;
                }
                if (p->type == 'P' && (choice == 'Q' || choice == 'R' || choice == 'B' || choice == 'N')) {
                    p->type = (char)(choice + 128); // 用最高位的1和0区分升变，十六进制保证不发生溢出
                    break;
                } else if (p->type == 'p' && (choice == 'q' || choice == 'r' || choice == 'b' || choice == 'n')) {
                    p->type = (char)(choice + 128);
                    break;
                } else {
                    printf("Invalid choice! You should choose Q/R/B/N: ");
                }
            }
        }
        // 普通走子和吃子时，标记王和车均已动
        if (p->type == 'K' || p->type == 'k' || p->type == 'R' || p->type == 'r') p->moved = 1;
        // 在主循环中处理王车易位
        if (can_castle(head, turn, fx, fy, tx, ty)) {
            // 白方king侧易位
            if (turn == 0 && fx == 7 && fy == 4 && tx == 7 && ty == 6) {
                Piece* king = find_piece(head, 7, 4);
                Piece* rook = find_piece(head, 7, 7);
                king->col = 6; king->moved = 1;
                rook->col = 5; rook->moved = 1;
                printf("White castles kingside!\n");
                turn = 1 - turn;
                continue;
            }
            // 白方Queen侧易位
            if (turn == 0 && fx == 7 && fy == 4 && tx == 7 && ty == 2) {
                Piece* king = find_piece(head, 7, 4);
                Piece* rook = find_piece(head, 7, 0);
                king->col = 2; king->moved = 1;
                rook->col = 3; rook->moved = 1;
                printf("White castles queenside!\n");
                turn = 1 - turn;
                continue;
            }
            // 黑方King侧易位
            if (turn == 1 && fx == 0 && fy == 4 && tx == 0 && ty == 6) {
                Piece* king = find_piece(head, 0, 4);
                Piece* rook = find_piece(head, 0, 7);
                king->col = 6; king->moved = 1;
                rook->col = 5; rook->moved = 1;
                printf("Black castles kingside!\n");
                turn = 1 - turn;
                continue;
            }
            // 黑方Queen侧易位
            if (turn == 1 && fx == 0 && fy == 4 && tx == 0 && ty == 2) {
                Piece* king = find_piece(head, 0, 4);
                Piece* rook = find_piece(head, 0, 0);
                king->col = 2; king->moved = 1;
                rook->col = 3; rook->moved = 1;
                printf("Black castles queenside!\n");
                turn = 1 - turn;
                continue;
            }
        }
        // 胜利结算：判断对方王是否还在
        if ((turn == 0 && !find_piece(head, 0, 4) && !find_piece(head, 7, 4)) ||
            (turn == 1 && !find_piece(head, 0, 4) && !find_piece(head, 7, 4))) {
            int king_found = 0;
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    Piece* pk = find_piece(head, i, j);
                    if ((turn == 0 && pk && (pk->type == 'k')) || (turn == 1 && pk && (pk->type == 'K'))) {
                        king_found = 1;
                        break;
                    }
                }
                if (king_found) break;
            }
            if (!king_found) {
                printf("Checkmate！%s wins!\n", turn == 0 ? "White" : "Black");
                break;
            }
        }
        turn = 1 - turn;
    }
    return 0;
}
//赛博国际象棋对弈系统v2.2.0
// word文档中上传的代码是v2.0版本的
//虽然程序实现的内容相当无趣且微不足道
//但笔者也是呕心沥血了哈（
//希望大家能够喜欢
//祝所有读到这里的评者高分飘过@0_0
//
//
//
//
//
//
//
//
//此处在Visual Studio 中恰为彩蛋位置
//但到了互评日...也许要是儿童节快乐的部分了
