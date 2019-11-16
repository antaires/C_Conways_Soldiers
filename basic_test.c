/********************/
/*                  */
/*  test functions  */
/*                  */
/********************/

#include "basic_sold.h"

/*test functions*/
void fill_board(Board *b, Move *mv_list, int size);
bool is_startset(Board_list *bl);
bool test_fill(void);
void test_gen_move(Board_list *bl);
void test_make_mv(Board_list *bl);
bool comp_board_arr(Board *b, int arr[ROWS][COLS], Coord c);
void test_has_goal(Board_list *bl);
bool comp_board2board(Board *b1, Board *b2, Coord c);
bool is_legal_move(Board *b1, Board *b2, Move mv);
int peg_count(Board *b); 
void test_main_prog_loop(Board_list *bl);
void test_num_moves(Board_list *bl, Move_list *ml, Coord c, int move_count);

void test_all(Board_list *bl)
{
   Move_list ml; 
   Coord c = {ROWS, COLS};
   int move_count;
   Move m;

   init_start_board(bl, c);

   /*TESTS*/                          
   assert (is_startset(bl) == true);

   /*test legal moves for start*/
   gen_moves(bl,0, &ml);
   assert (ml.total == COLS);

   /*test read move for start board*/
   m.x = 0; m.y = 0;
   assert (read(bl, 0, m) == EMPTY);
   m.x = COLS-1; m.y = ROWS-1;
   assert (read(bl, 0, m) == FULL);
   m.x = 0; m.y = -1;
   assert (read(bl, 0, m) == OFFBOARD);
   m.x = -1; m.y = 0;
   assert (read(bl, 0, m) == OFFBOARD);
   m.x = 0; m.y = -100;
   assert (read(bl, 0, m) == OFFBOARD);

   /*test fill*/
   assert (test_fill() == true);
 
   /*test generating moves*/
   test_gen_move(bl); 

   /*test number of moves generated*/
   c.rows = c.cols = move_count = 3; 
   test_num_moves(bl, &ml, c, move_count); 

   c.rows = c.cols = 2; move_count = 0;
   test_num_moves(bl, &ml, c, move_count);

   c.rows = c.cols = move_count = 6; 
   test_num_moves(bl, &ml, c, move_count);

   /*test make moves*/
   test_make_mv(bl);

   /*test has moves*/
   test_has_goal(bl);

   /*test clone_parent*/
   c.rows = ROWS; c.cols = COLS;
   set_empty(&bl->boards[1]);
   clone_parent(bl, 0);     
   assert(comp_board2board(&bl->boards[0], &bl->boards[1], c) == true);
}

void test_main_prog_loop(Board_list *bl)
{
   /*testing full loop - considers peg count between moves*/
   Move_list ml; 
   Move goal_mv;
   Coord c = {ROWS, COLS};
   int i;
   int total_squares = c.rows * c.cols;
   int pegs; 

   goal_mv.x = 2;
   goal_mv.y = 2;
   goal_mv.type = current;
   check_goal(goal_mv);
   init_start_board(bl, c);
   /*check count of pegs on board*/
   pegs = peg_count(&bl->boards[START]);
   assert (pegs == total_squares/2); 
   do{
      empty_ml(&ml);
      gen_moves(bl, bl->start, &ml);
      for (i = 0; i < ml.total; i++){
         clone_parent(bl, bl->start);
         make_mv(&bl->boards[bl->end], ml.list[i]);
         /*check peg count (every move is -1)*/
         pegs--;
         assert (peg_count(&bl->boards[bl->end]) == pegs);
         if (has_goal(&bl->boards[bl->end], goal_mv)){
            bl->start = bl->end - 1;
            i = ml.total;  
            return;
         }
      }
      bl->start++;
   } while (bl->start != bl->end);
}

void fill_board(Board *b, Move *mv_list, int size)
{
   /*for testing: default fills board as EMPTY    */
   /*             sets squares on mv_list as FULL */
   int i; 
   /*set all EMPTY*/
   set_empty(b); 
   /*set squares on mv list to FULL*/
   for (i = 0; i < size; i++){
      b->config[mv_list[i].y][mv_list[i].x] = FULL;
   }
}

bool is_startset(Board_list *bl)
{
   int i, j, k, mid = ROWS/2; 
   /*check start, end, parent values*/
   if (bl->start != 0 || bl->end != 0 || bl->boards[0].parent != ORIGINAL){
      return false;
   }
   /*check top half is EMPTY*/
   for (i = 0; i < mid; i++){
      for (j = 0; j < COLS; j++){
         if(bl->boards[0].config[i][j] != EMPTY){
            return false; 
         }
      }
   }
   /*check bottom half is FULL*/
   for (i = mid; i < ROWS; i++){
      for (j = 0; j < COLS; j++){
         if(bl->boards[0].config[i][j] != FULL){
            return false; 
         }
      }
   }

   /*check rest of boards are initialized to EMPTY*/
   for (k = 1; k < NUMBOARDS; k++){
      for (i = mid; i < ROWS; i++){
         for (j = 0; j < COLS; j++){
            if(bl->boards[k].config[i][j] != EMPTY){
               return false; 
            }
         }
      }
   }   
   return true;
}

bool test_fill(void)
{
   /*set up arr to desired outcome and compare*/
   /*with fill_board to same configuration    */
   Board b1;
   Coord c = {4, 3};
   int arr[ROWS][COLS] = {{0,0,0},
                          {0,0,0},
                          {0,1,0},
                          {0,1,0}};
   Move mv_list[2];
   mv_list[0].y = 2; mv_list[0].x = 1;
   mv_list[1].y = 3; mv_list[1].x = 1;
   fill_board(&b1, mv_list, 2);
   if (comp_board_arr(&b1, arr, c)){
      return true;
   } 
   return false;
}

bool comp_board_arr(Board *b, int arr[ROWS][COLS], Coord c)
{
   int i, j;
   for (i = 0; i < c.rows; i++){
      for (j = 0; j < c.cols; j++){
         if (b->config[i][j] != arr[i][j]){
            return false;
         } 
      }
   }
   return true; 
}

bool comp_board2board(Board *b1, Board *b2, Coord c)
{
   int i, j;
   for (i = 0; i < c.rows; i++){
      for (j = 0; j < c.cols; j++){
         if (b1->config[i][j] != b2->config[i][j]){
            return false;
         } 
      }
   }
   return true; 
}

void test_gen_move(Board_list *bl)
{
   /*test gen_move with known outcomes*/
   Move_list ml;
   Move mv_list[2];
   Coord c = {4, 3};
   
   /*test gen_moves on start board*/
   init_start_board(bl, c);
   gen_moves(bl, 0, &ml);
   assert(ml.list[0].y == 2);
   assert(ml.list[0].x == 0);
   assert(ml.list[0].type == up);
   assert(ml.list[1].y == 2);
   assert(ml.list[1].x == 1);
   assert(ml.list[1].type == up);
   assert(ml.list[2].y == 2);
   assert(ml.list[2].x == 2);
   assert(ml.list[2].type == up);
   assert(ml.total == 3);
   
   /*check empty board generates 0 moves*/
   set_empty(&bl->boards[0]);
   gen_moves(bl, 0, &ml);
   assert(ml.total == 0);

   /*test on one move*/
   mv_list[0].y = 2; mv_list[0].x = 0; 
   mv_list[1].y = 2; mv_list[1].x = 1;
   fill_board(&bl->boards[0], mv_list, 2);
   gen_moves(bl, 0, &ml);
   assert(ml.list[0].type == right);
   assert(ml.total == 1);

   /*test left*/
   mv_list[0].y = 2; mv_list[0].x = 1; 
   mv_list[1].y = 2; mv_list[1].x = 2;
   fill_board(&bl->boards[0], mv_list, 2);
   gen_moves(bl, 0, &ml);
   assert(ml.list[0].y == 2);
   assert(ml.list[0].x == 1);
   assert(ml.list[0].type == left);
   assert(ml.total == 1);
}

void test_num_moves(Board_list *bl, Move_list *ml, Coord c, int move_count)
{
   empty_ml(ml);
   init_start_board(bl, c);
   gen_moves(bl, 0, ml);
   assert (ml->total == move_count);
}

void test_make_mv(Board_list *bl)
{
   Coord c = {4, 3};
   int arr_up[ROWS][COLS] =   {{0,0,0}, /*final board after up move*/
                               {0,0,1},
                               {0,0,0},
                               {0,0,0}};

   int arr_left[ROWS][COLS] = {{0,0,0}, /*final for both left and down*/
                               {0,0,0},
                               {1,0,0},
                               {0,0,0}};

   int arr_right[ROWS][COLS]= {{0,0,0}, /*final board after right move*/
                               {0,0,0},
                               {0,0,1},
                               {0,0,0}};
   Move_list ml;
   Move mv_list[2];
   Move mv;
   mv.type = current; 
   mv_list[0].y = 0; mv_list[0].x = 0;
   mv_list[1].y = 0; mv_list[1].x = 0;

   init_start_board(bl, c);

   /*test move up*/
   mv_list[0].y = 2; mv_list[0].x = 2; 
   mv_list[1].y = 3; mv_list[1].x = 2;
   fill_board(&bl->boards[0], mv_list, 2);
   mv.x = 2; mv.y = 2; mv.type = up; 
   make_mv(&bl->boards[0], mv);
   assert (comp_board_arr(&bl->boards[0], arr_up, c) == true);

   /*test move left */
   mv_list[0].y = 2; mv_list[0].x = 1; 
   mv_list[1].y = 2; mv_list[1].x = 2;
   fill_board(&bl->boards[0], mv_list, 2);
   gen_moves(bl, 0, &ml);
   assert(ml.list[0].type == left);
   assert (ml.total == 1); 
   mv.x = 1; mv.y = 2; mv.type = left;
   make_mv(&bl->boards[0], mv);
   assert (comp_board_arr(&bl->boards[0], arr_left, c) == true);

   /*test move right*/
   mv_list[0].y = 2; mv_list[0].x = 0; 
   mv_list[1].y = 2; mv_list[1].x = 1;
   fill_board(&bl->boards[0], mv_list, 2); 
   gen_moves(bl, 0, &ml);
   assert(ml.list[0].type == right);
   assert(ml.total == 1);
   mv.x = 1; mv.y = 2; mv.type = right;
   make_mv(&bl->boards[0], mv);
   assert (comp_board_arr(&bl->boards[0], arr_right, c) == true);
}

void test_has_goal(Board_list *bl)
{
   Move mv_list[1];
   Move mv; 
   Coord c = {4, 3};

   init_start_board(bl, c);

   /*test empty half of board always returns false*/
   mv.x = 0; mv.y = 0; 
   assert (has_goal(&bl->boards[0], mv) == false);
   mv.x = 1; mv.y = 1; 
   assert (has_goal(&bl->boards[0], mv) == false);
   mv.x = 0; mv.y = 1; 
   assert (has_goal(&bl->boards[0], mv) == false);

   /*test full half of  board always returns true*/
   mv.x = 0; mv.y = (bl->boards[START].row/2) + 1; 
   assert (has_goal(&bl->boards[0], mv) == true);
   mv.x = bl->boards[START].col-1; mv.y = bl->boards[START].row-1; 
   assert (has_goal(&bl->boards[0], mv) == true);
   mv.x = bl->boards[START].col/2; mv.y = (bl->boards[START].row/2)+1; 
   assert (has_goal(&bl->boards[0], mv) == true);

   /*test a few specific boards*/
   mv_list[0].y = 1; mv_list[0].x = 2;
   fill_board(&bl->boards[0], mv_list, 1);
   mv.y = 1; mv.x = 2; 
   assert (has_goal(&bl->boards[0], mv) == true);
   mv.x = 2; mv.y = 2; 
   assert (has_goal(&bl->boards[0], mv) == false);
   mv.x = 1; mv.y = 3; 
   assert (has_goal(&bl->boards[0], mv) == false);
}




