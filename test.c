/****************************/
/*****  TESTS ***************/
/****************************/

#include "sold.h"
#define TEST_SIZE 6 
#define STANDR 8
#define STANDC 7

bin add_pegs(bin b, int num);
void test_peg_count(Board b);
void test_init_board(Board b);
void test_clone_board(Board_list *bl);
void test_translate_functions(void);
void test_bitwise_functions(Board b); 
void test_valid_dir(void);
void test_make_move(Board b);
void test_check_move(Board_list *bl);
void test_gen_moves(Board_list *bl);

void test_all(Board_list *bl)
{
   Board b;

   init_bl(bl); 

   b.b = set_empty();
   b.size = TEST_SIZE;

   test_peg_count(b); 
   test_init_board(b);
   test_clone_board(bl);
   test_translate_functions();
   test_bitwise_functions(b); 
   test_valid_dir(); 
   test_make_move(b); 
   test_check_move(bl);
   test_gen_moves(bl);
}

void test_peg_count(Board b)
{
   /*test peg count*/
   b.b = add_pegs(b.b, 3);
   assert (peg_count(b) == 3); 
   b.b = add_pegs(b.b, 4);
   assert (peg_count(b) == 4); 
   b.b = add_pegs(b.b, 5);
   assert (peg_count(b) == 5);
   b.b = set_empty();
   assert (peg_count(b) == 0);
   b.b = add_pegs(b.b, 6);
   assert (peg_count(b) == 6);
}

void test_init_board(Board b)
{
   b.b = set_empty();
   b.size = TEST_SIZE;

   /*test init board*/
   b.b = init_board(b.b);
   b.size = ROWS * COLS;
   assert (peg_count(b) == (ROWS * COLS)/2);
   b.b = set_empty();
   assert (peg_count(b) == 0);
}

void test_clone_board(Board_list *bl)
{
   /*test clone board*/ 
   clone_board(bl, 0);
   /*confrim two boards are now equal*/
   assert ( (bl->board[0].b ^ bl->board[1].b) == 0);

   bl->board[0].b = set_empty();
   bl->board[1].b = set_empty();
   bl->end = 0;
   /*set board 0*/
   bl->board[0].b = set_bit(bl->board[0].b, 4);
   bl->board[0].b = set_bit(bl->board[0].b, 1);
   bl->board[0].b = set_bit(bl->board[0].b, 5);
   bl->board[0].b = set_bit(bl->board[0].b, 2);
   /*test board 0 and 1 are unequal*/
   assert ( (bl->board[0].b ^ bl->board[1].b) != 0);
   clone_board(bl, 0);
   /*test boards 0 and 1 are equal after cloning*/
   assert ( (bl->board[0].b ^ bl->board[1].b) == 0);
}

void test_translate_functions(void)
{
   Coord c;
   Move m; 

   /*test translate (x, y) to binary index
       example: |8|7|6|
                |5|4|3|
                |2|1|0|           */
   assert (translate_x_y(COLS-1, ROWS-1) == 0);
   assert (translate_x_y(0, 0) == ROWS * COLS - 1);
   assert (translate_x_y(COLS-1, 0) % COLS == 0);
   assert (translate_x_y(COLS-1, 1) % COLS == 0);
   assert (translate_x_y(COLS-1, 2) % COLS == 0);
   assert (translate_x_y(0, ROWS-1) == COLS - 1);

   /*test translate binary index to (x, y) */
   m.bit = 0;
   c = translate_to_coord(m);
   assert (c.x == COLS - 1);
   assert (c.y == ROWS - 1);
   m.bit = ROWS * COLS - 1;
   c = translate_to_coord(m);
   assert (c.x == 0);
   assert (c.y == 0);
}

void test_bitwise_functions(Board b)
{
   int i;
   /*test get bit*/
   b.b = set_empty();
   b.b = set_bit(b.b, 4);
   assert ( get_bit(b.b, 1) == 0); 
   assert ( get_bit(b.b, 4) == 1);
   assert ( get_bit(b.b, 6) == 0);
   b.b = set_empty();
   for (i = 0; i < TEST_SIZE; i++){
      b.b = toggle_bit(b.b, i);
   }
   assert ( get_bit(b.b, 3) == 1);
   assert ( get_bit(b.b, 4) == 1);
   assert ( get_bit(b.b, 5) == 1);
   b.b = set_empty();
   assert ( get_bit(b.b, 5) == 0);

   /*test unset bit*/
   b.b = set_bit(b.b, 0);
   assert ( unset_bit(b.b, 0) == 0);

   /*test set bit*/
   b.b = set_empty();
   assert ( set_bit(b.b, 3) == 8);
   b.b = set_empty();
   assert ( set_bit(b.b, 1) == 2); 
   b.b = set_empty();
   assert ( set_bit(b.b, 2) == 4);
   b.b = set_empty();
   assert ( set_bit(b.b, 5) == 32);

   /*test toggle bit*/
   b.b = set_bit(b.b, 5);
   assert (toggle_bit(b.b, 5) == 0);
   b.b = set_empty();
}

void test_valid_dir(void)
{
   assert ( valid_dir(-2, up) == OFFBOARD);
   assert ( valid_dir(0, down) == OFFBOARD);
   assert ( valid_dir(COLS-1, left) == OFFBOARD);
   assert ( valid_dir(COLS, right) == OFFBOARD);
   assert ( valid_dir(ROWS*COLS, up) == OFFBOARD);
   assert ( valid_dir(ROWS/2, up) == ONBOARD);
}

void test_make_move(Board b)
{
   /*peg count decreases 1 with a move*/
   Move m; 
   if (valid_dir (8, up)){ 
      b.b = set_empty(); 
      b.b = set_bit(b.b, 8);
      b.b = set_bit(b.b, 1);
      assert ( peg_count(b) == 2);
      m.bit = 8;  
      m.dir = up;
      b = make_move(b, m);
      assert ( peg_count(b) == 1);
   }
}

void test_check_move(Board_list *bl)
{
   init_bl(bl);
   
   if (bl->board[START].row == STANDR && bl->board[START].col == STANDC){
   assert (check_move(bl, START, (ROWS*COLS/2)-(5%COLS), up) == true);
   assert (check_move(bl, START, (ROWS*COLS/2)-(2%COLS), up) == true);
   assert (check_move(bl, START, 0, up) == false);
   assert (check_move(bl, START, COLS-1, left) == false); 
   }
   
   bl->board[START].b = set_empty();
   assert (check_move(bl, START, 0, down) == false);
   assert (check_move(bl, START, 10, right) == false);
   assert (check_move(bl, START, -1100, up) == false);
   assert (check_move(bl, START, 2, left) == false);
   assert (check_move(bl, START, 1000, down) == false);
   assert (check_move(bl, START, 30, right) == false);
   assert (check_move(bl, START, 0, up) == false);
   assert (check_move(bl, START, 4, left) == false);

}

void test_gen_moves(Board_list *bl)
{
   Move_list ml;

   empty_ml(&ml); 
   init_bl(bl);
   gen_moves(bl, START, &ml);
   assert( ml.total == COLS);

   if (bl->board[START].row == STANDR && bl->board[START].col == STANDC){
   bl->board[START].b = set_empty();
   bl->board[START].b = set_bit(bl->board[START].b, 2);
   bl->board[START].b = set_bit(bl->board[START].b, 3);
   bl->board[START].b = set_bit(bl->board[START].b, 9);
   bl->board[START].b = set_bit(bl->board[START].b, 10);
   empty_ml(&ml);
   gen_moves(bl, START, &ml);
   assert (ml.total == 6);

   bl->board[START].b = set_empty();
   bl->board[START].b = set_bit(bl->board[START].b, 17);
   bl->board[START].b = set_bit(bl->board[START].b, 24);
   empty_ml(&ml);
   gen_moves(bl, START, &ml);
   assert (ml.total == 2);

   bl->board[START].b = set_empty();
   bl->board[START].b = set_bit(bl->board[START].b, 4);
   bl->board[START].b = set_bit(bl->board[START].b, 11);
   bl->board[START].b = set_bit(bl->board[START].b, 18);
   bl->board[START].b = set_bit(bl->board[START].b, 25);
   bl->board[START].b = set_bit(bl->board[START].b, 32);
   bl->board[START].b = set_bit(bl->board[START].b, 39);
   bl->board[START].b = set_bit(bl->board[START].b, 46);
   bl->board[START].b = set_bit(bl->board[START].b, 53);
   empty_ml(&ml);
   gen_moves(bl, START, &ml);
   assert (ml.total == 16);
   }

   bl->board[START].b = set_empty();
   empty_ml(&ml);
   gen_moves(bl, START, &ml);
   assert (ml.total == 0);
}

bin add_pegs(bin b, int num)
{
   int i;
   for (i = 0; i < num; i++){
     b = set_bit(b, i); 
   }
   return b;
}

int peg_count (Board b)
{
   unsigned int cnt = 0;

   while (b.b){
      cnt += b.b & 1;
      b.b >>= 1; 
   } 
   return (int) cnt; 
}


