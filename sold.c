/**************************************************/
/*             BINARY CONWAY'S SOLDIERS           */
/*                                                */
/* User inputs (x, y) on start and program gives  */
/* directions (move sequence) if goal reachable.  */
/**************************************************/
/* This version stores boards as a binary sequence*/
/* to decrease memory demands and increase speed. */
/* For the normal version, please see basic_sold  */
/* files that have also been uploaded.            */
/**************************************************/

#include "sold.h"

int main(int argc, char **argv)
{
   static Board_list bl; 
   int i;
   Move_list ml; 
   int goal_bit;

   check_argc(argc);
   goal_bit = translate_x_y(atoi(argv[1]),atoi(argv[2]));
   check_goal(goal_bit);

   test_all(&bl);

   init_bl(&bl);

   do{
      empty_ml(&ml);
      gen_moves(&bl, bl.start, &ml);
      for (i = 0; i < ml.total; i++){
         clone_board(&bl, bl.start);
         bl.board[bl.end] = make_move(bl.board[bl.end], ml.move[i]);
         if ( get_bit(bl.board[bl.end].b, goal_bit) ){
            printf("\nGoal reached!\n");  
            print_moves(&bl);
            return 0;
         }
      }
      bl.start++;
   } while (bl.start != bl.end);

   printf("\nNo sequence found to reach goal\n");
   return 0;
}

void init_bl(Board_list *bl)
{
   /*init start board*/
   bl->board[0].b = init_board(bl->board[0].b);
   bl->start = 0;
   bl->end = 0; 
   bl->board->parent = ORIGINAL; 
}

bin set_empty()
{
   int i; 
   bin board = 0; 
   for (i = 0; i < ROWS * COLS; i++){
      board <<= 0; 
   }
   return board;
}

bin init_board(bin board)
{
   int i, mid = (ROWS * COLS)/2;
   /*check that size of board does not over-run holder*/
   if (ROWS*COLS > sizeof(uint64_t)*BYTE){
      fprintf(stderr, "\nERROR: input board size overruns uint64_t");
      exit(EXIT_FAILURE);
   }

   board = set_empty();

   /*does mid fall evenly on end of row?*/
   /*determine highest full row below mid */
   while (mid % COLS != COLS - 1 && mid % COLS != 0){
      mid--;
   }

   /*set lower half to 1s*/
   for (i = 0; i < mid; i++){
      board = set_bit(board, i); 
   }
   return board; 
}

void gen_moves(Board_list *bl, int curr_b, Move_list *ml)
{
   /*generates moves and stores them to ml*/
   int dir;
   Move m; 
   index move_index = 0; 
   ml->total = 0;
   for (m.bit = 0; m.bit < (ROWS * COLS); m.bit++){   
      for (dir = up; dir < DIRECTIONS; dir++){  
         if (check_move(bl, curr_b, m.bit, dir)){
            ml->total++;
            add_move(ml, move_index, m, dir);
            move_index++; 
         }
      }
   } 
}

bool check_move(Board_list *bl, index curr_b, int bit_index, int direction)
{
   int i;
   square space[SPACES] = {FULL, FULL, EMPTY};
   int o[DIRECTIONS] = OFFSET; 
   index s[SPACES];
   Move test_m; 
   test_m.bit = bit_index; 

   /*determine if direction possible from current space*/
   if (valid_dir(bit_index, direction) == OFFBOARD){
      return false; 
   }
   /*determine which three spaces are changed for move*/
   switch(direction){
      case up:      
         s[start] = current; s[mid] = down; s[end] = up;
         break;
      case left:
         s[start] = right; s[mid] = current; s[end] = left;
         break;
      case right:
         s[start] = left; s[mid] = current; s[end] = right;
         break;
      default:
         fprintf(stderr, "\nERROR: check_move encountered \
                 invalid direction\n");
         exit(1);
   }
   /*loop over 3 spaces and check if spaces are: FULL-FULL-EMPTY */
   for (i = 0; i < SPACES; i++){ 
      if (get_bit(bl->board[curr_b].b, test_m.bit + o[s[i]]) != space[i]){
         return false;
      }
   }
   return true;
}

int valid_dir (index bit_index, int direction)
{
   if (bit_index <= 0 || bit_index > ROWS * COLS){
      return OFFBOARD;
   }
   /*checks if making move in given direction is on board*/
   /*and handles wrapping the binary sequence onto a grid*/
   switch(direction){
      case right: 
         if (bit_index % COLS <= 0){
            return OFFBOARD; 
         }
         break;
      case left:
         if (bit_index % COLS >= COLS-1 ){
            return OFFBOARD;
         }
         break;
      case up:
         if (bit_index + COLS > ROWS * COLS){
            return OFFBOARD;
         }
         break;
      case down:
         if (bit_index - COLS < 0){
            return OFFBOARD;
         }
         break;
      case current: 
         break;
      default:
         fprintf(stderr, "\nERROR: move has no direction");
         exit(1);
   }
   /*index after move is a valid move on the board*/
   return ONBOARD;
}

Board make_move(Board b, Move m)
{   
  /*takes a board and LEGAL Move struct and updates board*/
   int i; 
   index s[SPACES];
   int o[DIRECTIONS] = OFFSET;
   b.move = m;
   switch(m.dir){
      case up:
         s[start] = up; s[mid] = current; s[end] = down; 
         break;
      case left:
         s[start] = left; s[mid] = current; s[end] = right; 
         break;
      case right:
         s[start] = right; s[mid] = current; s[end] = left;
         break;
      default:
         fprintf(stderr, "\nERROR: move type empty - no move made");
         exit(1);
   }   
   /*loop over 3 squares altered by move & toggle to FULL-EMPTY-EMPTY*/
   for (i = 0; i < SPACES; i++){
         b.b = toggle_bit(b.b, m.bit + o[s[i]]);
   }
   return b; 
}

void clone_board(Board_list *bl, index parent_index)
{
   /*takes a parent board to clone, and a board list  */
   /*clones the parent to the next empty board in list*/

   /*test board to clone is not last in list*/
   if (parent_index >= NUMBOARDS){
      fprintf(stderr, "ERROR: Insufficient memory to reach goal");
      exit(EXIT_FAILURE); 
   }
   bl->end++; 
   /*clone*/
   bl->board[bl->end] = bl->board[parent_index];
   /*set parent index of cloned board*/
   bl->board[bl->end].parent = parent_index;
   /*set rows/cols of board from start board*/
   bl->board[bl->end].row = bl->board[START].row;
   bl->board[bl->end].col = bl->board[START].col;
}

/****************************/
/****BITWISE FUNCTIONS*******/
/****************************/
int get_bit (bin board, int n)
{
   /*gets n bit, counts from end ...3210*/
   bin mask = set_empty();
   mask = mask | 1; 
   return (board >> n) & mask;  
}

bin set_bit (bin board, int n)
{
   bin mask = set_empty();
   mask = mask | 1;
   return (board | mask<<n); 
}

bin toggle_bit(bin board, int n)
{
   bin mask = set_empty();
   mask = mask | 1; 
   return (board ^ mask<<n); 
}

bin unset_bit (bin board, int n)
{
   bin mask = set_empty();
   /*get mask of all 1s*/
   mask = mask | !mask; 

   /*toggle the n bit to 0 on mask*/
   mask = toggle_bit(mask, n); 
   return (board & mask); 
}

/****************************/
/*****HELPER FUNCTIONS*******/
/****************************/

void add_move(Move_list *ml, index move_index, Move m, int direction)
{
   /*adds a legal move onto the move list*/
   ml->move[move_index].bit = m.bit;
   ml->move[move_index].dir = direction; 
}

void empty_ml(Move_list *ml)
{
   /*clears move list for next cycle*/
   int i;
   for (i = 0; i < MAXMOVES; i++){
      ml->move[i].bit = OFFBOARD;
      ml->move[i].dir = current;
      ml->total = EMPTY; 
   }
}

void print_bl(Board_list *bl)
{
   /*prints all filled boards in the board list*/
   int i;
   for (i = 0; i < bl->end; i++){
      print_board(bl->board[i]); 
   }
}

void print_board(Board b)
{
   /*get each individual bit and print it to form board*/
   int i, total = ROWS * COLS;
   char e = '-';
   char f = '*';

   /*print reverse to get proper orientation*/
   printf("\n");
   for (i = total-1; i >= 0; i--){
      if (get_bit(b.b, i) == 0){
         printf("%c", e);
      } else {
         printf("%c", f);
      }
      if (i % COLS == 0){
         printf("\n");
      } 
   }
   printf("\n");
}

void check_x_y(int x, int y)
{
   if (x >= COLS || y >= ROWS || x < 0 || y < 0){
      fprintf(stderr, "\nERROR: Input goal is out of bounds\n");
      exit(EXIT_FAILURE);
   }
}

void check_goal(int goal)
{
   int mid = (ROWS * COLS)/2;
   /*check goal is within bounds*/
   if (goal < 0 || goal >= ROWS * COLS){
      fprintf(stderr, "\nERROR: Input goal is out of bounds\n");
      exit(EXIT_FAILURE);
   }

   /*check if goal reached at starting position*/
   while (mid % COLS != COLS - 1 && mid % COLS != 0){
      mid--;
   }
   if (goal <= mid){
      printf("\nGoal reached on start - no move needed\n");
      exit(0);
   }
}

void check_argc(int argc)
{
   if (argc != SPACES){
      fprintf(stderr, "\nERROR: type numbers x y on program start\n");
      exit(EXIT_FAILURE);
   }
}

Coord translate_to_coord(Move m)
{
   int arr[ROWS][COLS] = {0};
   int total = ROWS * COLS - 1; 
   int value = total, i, j; 
   Coord c; 
   /*set values in arr to count down from total*/
   for ( i = 0; i < ROWS; i++){
      for ( j = 0; j < COLS; j++){
         arr[i][j] = value--;
         if (arr[i][j] == m.bit){
            c.x = j; c.y = i;
         }
      }
   }
   /*get in value of square*/
   return c; 
}

int translate_x_y(int x, int y)
{
   int arr[ROWS][COLS] = {0};
   int total = (ROWS * COLS) - 1; 
   int value = total, i, j; 
   /*check that x, y on board*/
   check_x_y(x, y);

   /*set values in arr to count down from total*/
   for ( i = 0; i < ROWS; i++){
      for ( j = 0; j < COLS; j++){
         arr[i][j] = value--;
      }
   }
   /*get number value of square*/
   return arr[y][x]; 
}

void print_moves(Board_list *bl)
{
   /*goes through boards in reverse to create move list to goal*/
   /*prints list in reverse order and gives written instructions*/
   /*NOTE bl.end  = goal move*/
   int i = 0; 
   index curr_board; 
   Move_list goal_moves; 
   empty_ml(&goal_moves); 
   goal_moves.total = MAXMOVES; 
   
   /*start at end and add board.move to mv_list*/
   curr_board = bl->end;

   printf("\nBoard sets (0,0) at upper left");

   /*add moves to goal_list following parent indices*/
   while (curr_board != 0){
      goal_moves.move[i++] = bl->board[curr_board].move;
      print_board(bl->board[curr_board]);
      curr_board = bl->board[curr_board].parent; 
   }

   /*decrement i until first valid move on list*/
   i = MAXMOVES -1; 
   while (goal_moves.move[i].dir == current){
      i--;
   }

   /*print each move in reverse for proper print order*/
   printf("\nMoves in (x, y) format:\n");
   for (; i >= 0 ; i--){
      print_direction(goal_moves.move[i]);
   }
   printf("\n");
}

void print_direction(Move move)
{
   Coord c = translate_to_coord(move);
   Coord sqr[STEPS];
   static int m_cnt = 0;
   Coord o[DIRECTIONS] = OFFSET_COORD;
   char dir[DIRECTIONS][WORDLEN] = {"CURR\0", "DOWN\0", "UP\0", 
                                    "LEFT\0", "RIGHT\0"};
   switch(move.dir){
      case up:
         sqr[START].x = c.x + o[down].x; sqr[START].y = c.y + o[down].y;
         sqr[END].x = c.x + o[up].x; sqr[END].y = c.y + o[up].y;
         break;
      case left:
         sqr[START].x = c.x + o[right].x; sqr[START].y = c.y + o[right].y;
         sqr[END].x = c.x + o[left].x; sqr[END].y = c.y + o[left].y;
         break;
      case right:
         sqr[START].x = c.x + o[left].x; sqr[START].y = c.y + o[left].y;
         sqr[END].x = c.x + o[right].x; sqr[END].y = c.y + o[right].y;
         break;
      default:
         fprintf(stderr,"\nERROR: move has no type to print");
         exit(EXIT_FAILURE);
   }
   printf("\n%d. Move peg %5s from (%d, %d) to (%d, %d)", ++m_cnt, 
          dir[move.dir], sqr[0].x, sqr[0].y, sqr[1].x, sqr[1].y);
}








