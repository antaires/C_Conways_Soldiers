#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define NUMBOARDS 2000000 
#define MAXMOVES 100      
#define ROWS 8           
#define COLS 7            
#define FULL 1
#define EMPTY 0
#define OFFSET {{0,0},{0,1},{0,-1},{-1,0},{1,0}}

#define CURRENT 0
#define UP -1
#define DOWN 1
#define LEFT -1
#define RIGHT 1

#define DIRECTIONS 5   /*counts current location as 1 direction*/
#define WORDLEN 7
#define SPACES 3       /*squares to check / alter for valid move*/

#define OFFBOARD -1
#define ORIGINAL -1 
#define LIMIT 4
#define STEPS 2

#define START 0
#define END 1

enum bool {false, true};
typedef enum bool bool; 

enum _direction {current, down, up, left, right};
typedef enum _direction direction;

enum _spaces {start, mid, end};
typedef enum _spaces spaces; 

typedef char square; 
typedef int index; 

struct offset {
   int x;
   int y;
};
typedef struct offset Offset;

struct move {
   int x; 
   int y;
   int type;  
};
typedef struct move Move; 

struct move_list {
  Move list[MAXMOVES];
  index total; 
};
typedef struct move_list Move_list;

struct board {
   square config[ROWS][COLS]; 
   index parent;
   Move move;    /*move made on board*/
   int row;
   int col; 
};
typedef struct board Board; 

struct board_list{
   Board boards[NUMBOARDS];
   index start; 
   index end;  
};
typedef struct board_list Board_list; 

struct coord {
   int rows;
   int cols;
}; 
typedef struct coord Coord; 

/*primary functions*/
void init_start_board(Board_list *bl, Coord c); 
void gen_moves(Board_list *bl, int curr_b, Move_list *ml);
bool check_move(Board_list *bl, index curr_b, Move m, int direction);
int read(Board_list *bl, index curr_b, Move m);
void make_mv(Board *b, Move mv); 
bool has_goal(Board *b, Move mv); 
void clone_parent(Board_list *bl, index parent_index);
void set_empty(Board *b);
void add_move(Move_list *ml, int move_index, Move m, int move_type);

/*helper functions*/
void check_goal(Move goal_mv);
void print_ml(Move_list *ml);
void empty_ml(Move_list *ml);
void print_moves(Board_list *bl);
void print_direction(Move *m);
void print_bl(Board_list *bl, int start_b, int end_b);
void check_argc(int argc);

/*test functions*/
void test_all(Board_list *bl);
#endif
