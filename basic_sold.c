/**************************************************/
/*        BASIC CONWAY'S SOLDIERS                 */
/* User inputs (x, y) on start and program gives  */
/* directions (move sequence) if it is reachable  */
/* This version has been uploaded along with a    */
/* binary version                                 */
/**************************************************/
#include "basic_sold.h"

int main(int argc, char **argv)
{
   static Board_list bl;
   int i; 
   Move_list ml;
   Move goal_mv;
   Coord c = {ROWS, COLS};

   check_argc(argc);
   goal_mv.x = atoi(argv[1]); 
   goal_mv.y = atoi(argv[2]);
   goal_mv.type = current; 
   check_goal(goal_mv);

   test_all(&bl);

   init_start_board(&bl, c);

   do{
      empty_ml(&ml);
      gen_moves(&bl, bl.start, &ml);
      for (i = 0; i < ml.total; i++){
         clone_parent(&bl, bl.start);
         make_mv(&bl.boards[bl.end], ml.list[i]);
         if (has_goal(&bl.boards[bl.end], goal_mv)){
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

void init_start_board(Board_list *bl, Coord c)
{
   int mid = c.rows/2; 
   int j, k, i = mid; 

   bl->start = START;
   bl->end = START; 
   bl->boards[START].parent = ORIGINAL; 
   bl->boards[START].row = c.rows;
   bl->boards[START].col = c.cols;
  
   /*fill upper half with empty spaces*/
   for (i = 0; i < mid; i++){
      for (j = 0; j < c.cols; j++){
         bl->boards[START].config[i][j] = EMPTY; 
      }
   }
   /*fill lower half of board with pegs*/
   for (i = mid; i < c.rows; i++){
      for (j = 0; j < c.cols; j++){
         bl->boards[START].config[i][j] = FULL; 
      }
   }
   /*set all other boards to EMPTY*/
   for (k = 1; k < NUMBOARDS; k++){
      set_empty(&bl->boards[k]);
   }
}

void gen_moves(Board_list *bl, int curr_b, Move_list *ml)
{
   /*generates moves and stores them to ml*/
   int dir;
   Move m; 
   index move_index = 0; 
   ml->total = 0;
   for (m.y = 0; m.y < bl->boards[START].row; m.y++){   
      for (m.x = 0; m.x < bl->boards[START].col; m.x++){
         for (dir = up; dir < DIRECTIONS; dir++){  
            if (check_move(bl, curr_b, m, dir)){
               ml->total++;
               add_move(ml, move_index, m, dir);
               move_index++; 
            }
         }
      }
   } 
}

bool check_move(Board_list *bl, index curr_b, Move m, int direction)
{
   int i;
   Offset o[DIRECTIONS] = OFFSET;
   square space[SPACES] = {FULL, FULL, EMPTY};
   index s[SPACES];
   Move test_m; 
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
   /*loop over 3 spaces and check if FULL-FULL-EMPTY */
   for (i = 0; i < SPACES; i++){ 
      /*returns false for OFFBOARD moves or otherwise illegal moves*/
      test_m.x = m.x + o[s[i]].x;
      test_m.y = m.y + o[s[i]].y;
      if (read(bl, curr_b, test_m) != space[i]){
         return false;
      }
   }
   return true;
}

void add_move(Move_list *ml, int move_index, Move m, int move_type)
{
   ml->list[move_index].x = m.x;
   ml->list[move_index].y = m.y;  
   ml->list[move_index].type = move_type; 
}

void check_goal(Move goal_mv)
{
   /*check goal is within bounds*/
   if (goal_mv.x < 0 || goal_mv.x >= COLS || 
       goal_mv.y < 0 || goal_mv.y >= ROWS){
      fprintf(stderr, "\nERROR: Input goal is out of bounds\n");
      exit(1);
   }
   /*check if goal reached at starting position*/
   if (goal_mv.y >= (ROWS/2)){
      printf("\nGoal reached on start - no move needed\n");
      exit(0);
   }
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

   /*add moves to goal_list following parent indices*/
   while (curr_board != 0){
      goal_moves.list[i++] = bl->boards[curr_board].move;
      print_bl(bl, curr_board, curr_board);
      curr_board = bl->boards[curr_board].parent; 
   }

   /*decrement i until first valid move on list*/
   i = MAXMOVES -1; 
   while (goal_moves.list[i].type == current){
      i--;
   }

   /*print each move in reverse for proper print order*/
   printf("\nMoves in (x, y) format:\n");
   for (; i >= 0 ; i--){
      print_direction(&goal_moves.list[i]);
   }
   printf("\n");
}

void print_direction(Move *m)
{
   Move sqr[STEPS];
   static int m_cnt = 0;
   Offset o[DIRECTIONS] = OFFSET;
   char dir[DIRECTIONS][WORDLEN] = {"CURR\0", "DOWN\0", "UP\0", 
                                    "LEFT\0", "RIGHT\0"};
   switch(m->type){
      case up:
         sqr[START].x = m->x + o[down].x; sqr[START].y = m->y + o[down].y;
         sqr[END].x = m->x + o[up].x; sqr[END].y = m->y + o[up].y;
         break;
      case left:
         sqr[START].x = m->x + o[right].x; sqr[START].y = m->y + o[right].y;
         sqr[END].x = m->x + o[left].x; sqr[END].y = m->y + o[left].y;
         break;
      case right:
         sqr[START].x = m->x + o[left].x; sqr[START].y = m->y + o[left].y;
         sqr[END].x = m->x + o[right].x; sqr[END].y = m->y + o[right].y;
         break;
      default:
         fprintf(stderr,"\nERROR: move has no type to print");
         exit(1);
   }
   printf("\n%d. Move peg %5s from (%d, %d) to (%d, %d)", ++m_cnt, 
          dir[m->type], sqr[0].x, sqr[0].y, sqr[1].x, sqr[1].y);
}

void empty_ml(Move_list *ml)
{
   int i; 
   for (i = 0; i < MAXMOVES; i++){
      ml->list[i].x = OFFBOARD;
      ml->list[i].y = OFFBOARD;
      ml->list[i].type = current;
      ml->total = EMPTY; 
   }
}

void clone_parent(Board_list *bl, index parent_index)
{
   /*takes a parent board to clone, and a board list  */
   /*clones the parent to the next empty board in list*/
   int i, j; 

   /*test board to clone is not last in list*/
   if (parent_index >= NUMBOARDS){
      fprintf(stderr, "ERROR: Insufficient memory to reach goal");
      exit(EXIT_FAILURE); 
   }

   /*increment the end count to access next empty board*/  
   bl->end++; 

   /*clone*/
   for (i = 0; i < ROWS; i++){
      for (j = 0; j < COLS; j++){
         bl->boards[bl->end].config[i][j] = 
         bl->boards[parent_index].config[i][j];
      }
   }
   /*set parent index of cloned board*/
   bl->boards[bl->end].parent = parent_index;
   /*set rows/cols of board from start board*/
   bl->boards[bl->end].row = bl->boards[START].row;
   bl->boards[bl->end].col = bl->boards[START].col;
}

int read(Board_list *bl, index curr_b, Move m)
{
   /*return -1 if current mv off board*/
   if (m.x < 0 || m.x >= bl->boards[START].col){ 
      return OFFBOARD; 
   } 
   if (m.y < 0 || m.y >= bl->boards[START].row){ 
      return OFFBOARD; 
   }  
   /*otherwise return value*/
   return bl-> boards[curr_b].config[m.y][m.x];
}

void set_empty(Board *b)
{
   int i, j;
   /*set all EMPTY*/
   for (i = 0; i<ROWS; i++){
      for (j = 0; j<COLS; j++){
         b->config[i][j] = EMPTY;
         b->parent = ORIGINAL;
      }
   }
}

void make_mv(Board *b, Move mv)
{   
  /*takes a board and LEGAL Move struct and updates board*/
   int i; 
   index s[SPACES];
   Offset o[DIRECTIONS] = OFFSET;
   index peg[SPACES] = {FULL, EMPTY, EMPTY};
   b-> move = mv;
   switch(mv.type){
      case up:
         s[0] = up; s[1] = current; s[2] = down; 
         break;
      case left:
         s[0] = left; s[1] = current; s[2] = right; 
         break;
      case right:
         s[0] = right; s[1] = current; s[2] = left;
         break;
      default:
         fprintf(stderr, "\nERROR: move type empty - no move made");
         exit(1);
   }    
   /*loop over three squares changed by move and set to FULL or EMPTY*/
   for (i = 0; i < SPACES; i++){
         b->config[mv.y + o[s[i]].y][mv.x + o[s[i]].x] = peg[i];
   }
}

bool has_goal(Board *b, Move mv)
{
   if (b->config[mv.y][mv.x] == FULL){
      return true;
   }
   return false; 
}

void print_bl(Board_list *bl, int start_b, int end_b)
{
   /*prints all boards in board list between given indices*/
   /*prints only one board if start_b = end_b             */
   int i, j, k, curr;
   char e = '-';
   char f = '*';
  
   for (k = start_b; k <= end_b; k++){
      printf("\n\n   ");
      /*add col count for x*/
      for (j = 0; j < COLS; j++){
         printf("%d", j);
      } 
      printf("\n  _");
      /*print line on top */
      for (j = 0; j < COLS; j++){
         printf("_");
      } 
      printf("\n0| ");
      for (i = 0; i < ROWS; i++){
         for (j = 0; j < COLS; j++){
            curr = bl->boards[k].config[i][j];
            if (curr == FULL){
               printf("%c", f);
            }  
            if (curr == EMPTY){
               printf("%c", e);
            }
            if (j == COLS - 1 && i < ROWS-1){
               printf("\n%d| ", i+1); /*add row count for y*/
            }
         }
      }
      printf("\n");
   }
}

void print_ml(Move_list *ml)
{
   int i;
   for (i = 0; i < ml->total; i++){
      printf("\n%d: y:%d x:%d type:%d", i+1, ml->list[i].y, ml->list[i].x, ml->list[i].type);
   }
}

void check_argc(int argc)
{
   if (argc != SPACES){
      fprintf(stderr, "\nERROR: type numbers x y on program start\n");
      exit(1);
   }
}

int peg_count(Board *b)
{
   int i, j, cnt = 0;
   for (i = 0; i < b->row; i++){
      for (j = 0; j < b->col; j++){
         if (b->config[i][j] == FULL){
            cnt++;
         }
      }
   }
   return cnt; 
}








