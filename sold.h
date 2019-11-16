#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include <stdint.h>

#define NUMBOARDS 100000000
#define MAXMOVES 100
#define WORDLEN 7
#define ROWS 8
#define COLS 7

#define OFFSET {0, -COLS, COLS, 1, -1}
#define OFFSET_COORD {{0,0},{0,1},{0,-1},{-1,0},{1,0}}
#define DIRECTIONS 5   /*counts current location as 1 direction*/
#define SPACES 3

#define FULL 1
#define EMPTY 0

#define START 0
#define END 1
#define ORIGINAL -1
#define OFFBOARD -1
#define ONBOARD 1
#define STEPS 2
#define BYTE 8

enum bool {false, true};
typedef enum bool bool; 

typedef uint64_t bin;
typedef int index;
typedef int square; 

enum _directions {current, down, up, left, right};
typedef enum _directions directions;

enum _spaces {start, mid, end};
typedef enum _spaces spaces; 

struct _coord {
   int x;
   int y;
};
typedef struct _coord Coord; 

struct _move {
   int bit;
   int dir; 
};
typedef struct _move Move;

struct move_list {
  Move move[MAXMOVES];
  index total; 
};
typedef struct move_list Move_list;

struct _board{
   bin b; 
   int parent;
   Move move; /*move that led to board*/
   int row;
   int col;
   int size;
};
typedef struct _board Board; 

struct board_list{
   Board board[NUMBOARDS];
   int start; 
   int end;  
};
typedef struct board_list Board_list; 

/*primary*/
void gen_moves(Board_list *bl, int curr_b, Move_list *ml);
int valid_dir(index bit_index, int direction);
bool check_move(Board_list *bl, index curr_b, index bit, int direction);
Board make_move(Board b, Move m);

/*bitwise*/
int get_bit (bin b, int bit_num);
bin set_bit (bin b, int bit_num);
bin unset_bit (bin b, int bit_num);
bin toggle_bit(bin board, int n);

/*helper*/
void init_bl(Board_list *bl);
bin init_board(bin b);
void check_goal(int goal_m);
void check_x_y(int x, int y);
bin set_empty(void);
void clone_board(Board_list *bl, index parent);
void add_move(Move_list *ml, index move, Move m, int direction);
void empty_ml(Move_list *ml);
void print_board(Board b);
void print_bl(Board_list *b);
void print_moves(Board_list *bl);
void print_direction(Move move);
int translate_x_y(int x, int y);
Coord translate_to_coord(Move m);
void check_argc(int argc);

/*tests*/
void test_all(Board_list *bl);
int peg_count (Board b);

#endif
