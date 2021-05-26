/*
 * Author: Ritik Jain, 18114068
 * Since: May 24, 2021
 * Brief: Compiler Lab Assignment- Program 4: Demonstrates LL(1) Parsing
 * 
 * For the Context Free Grammar below:
 *  S' -> E $
 *  E  -> T E'
 *  E' -> + T E' |
 *  T  -> F T'
 *  T' -> * F T' | 
 *  F  -> ( E ) | id
 * 
 * -------------------------------------------------------------------------------------------
 * 
 * The LL(1) Parsing Table:
 * 
 * +-----------+------------------------------------------------------------------+
 * | Terminals | Non-Terminals                                                    |
 * |           +--------+--------------+------------+--------+-----------+--------+
 * |           | +      | *            | (          | )      | id        | $      |
 * +-----------+--------+--------------+------------+--------+-----------+--------+
 * |     E     |        |              |  E -> T E' |        | E -> T E' |        |
 * +-----------+--------+--------------+------------+--------+-----------+--------+
 * |     E'    | + T E' |              |            | E' ->  |           | E' ->  |
 * +-----------+--------+--------------+------------+--------+-----------+--------+
 * |     T     |        |              |  T -> F T' |        | T -> F T' |        |
 * +-----------+--------+--------------+------------+--------+-----------+--------+
 * |     T'    | T' ->  | T' -> * F T' |            | T' ->  |           | T' ->  |
 * +-----------+--------+--------------+------------+--------+-----------+--------+
 * |     F     |        |              | F -> ( E ) |        |  F -> id  |        |
 * +-----------+--------+--------------+------------+--------+-----------+--------+
 * 
 * Total Symbols: 12
 * Terminal Symbols: 6
 * Non-Terminal Symbols: 5
 * 
 * --------------------------------------------------------------------------------------------
 * 
 * Encoded Symbols:
 * 
 * E  : 0
 * E' : 1
 * T  : 2
 * T' : 3
 * F  : 4
 * +  : 5
 * *  : 6
 * (  : 7
 * )  : 8
 * id : 9
 * $  : 10
 * 
 * -1 is reserved for terminator/ empty string
 * 
 * Encoded Parsing Table (M[5 x 6]):
 * 
 * M[0][0]: {}
 * M[0][1]: {}
 * M[0][2] : {2,1,-1}
 * M[0][3]: {}
 * M[0][4]: {2,1,-1}
 * M[0][5]: {}
 * 
 * M[1][0]: {5,2,1,-1}
 * M[1][1]: {}
 * M[1][2]: {}
 * M[1][3]: {-1}
 * M[1][4]: {}
 * M[1][5]: {-1}
 * 
 * M[2][0]: {}
 * M[2][1]: {}
 * M[2][2]: {4,3,-1}
 * M[2][3]: {}
 * M[2][4]: {4,3,-1}
 * M[2][5]: {}
 * 
 * M[3][0]: {-1}
 * M[3][1]: {6,4,3,-1}
 * M[3][2]: {}
 * M[3][3]: {-1}
 * M[3][4]: {}
 * M[3][5]: {-1}
 * 
 * M[4][0]: {}
 * M[4][1]: {}
 * M[4][2]: {7,0,8,-1}
 * M[4][3]: {}
 * M[4][4]: {9,-1}
 * M[4][5]: {}
 *
 */

#include "dynamic_stack.h"
#include "string.h"
#include "stdio.h"

char *SYMBOL_SET[] = {"E","E'","T","T'","F","+","*","(",")","id","$"};
const int NUM_NON_TERMINALS = 5;
const int NUM_SYMBOLS = 11;
int ***PARSE_TABLE;

unsigned int PADDING_CONSTANT = 10;

void BUILD_PARSE_TABLE(){
    PARSE_TABLE = (int***) malloc(sizeof(int**)*5);
    PARSE_TABLE[0] = (int**) malloc(sizeof(int*)*6);
    PARSE_TABLE[1] = (int**) malloc(sizeof(int*)*6);
    PARSE_TABLE[2] = (int**) malloc(sizeof(int*)*6);
    PARSE_TABLE[3] = (int**) malloc(sizeof(int*)*6);
    PARSE_TABLE[4] = (int**) malloc(sizeof(int*)*6);
    PARSE_TABLE[5] = (int**) malloc(sizeof(int*)*6);
    PARSE_TABLE[0][0] = 0;
    PARSE_TABLE[0][1] = 0;
    PARSE_TABLE[0][2] = (int*) malloc(3*sizeof(int));
        PARSE_TABLE[0][2][0] = 2;
        PARSE_TABLE[0][2][1] = 1;
        PARSE_TABLE[0][2][2] = -1;
    PARSE_TABLE[0][3] = 0;
    PARSE_TABLE[0][4] = (int*) malloc(3*sizeof(int));
        PARSE_TABLE[0][4][0] = 2;
        PARSE_TABLE[0][4][1] = 1;
        PARSE_TABLE[0][4][2] = -1;
    PARSE_TABLE[0][5] = 0;
    PARSE_TABLE[1][0] = (int*) malloc(4*sizeof(int));
        PARSE_TABLE[1][0][0] = 5;
        PARSE_TABLE[1][0][1] = 2;
        PARSE_TABLE[1][0][2] = 1;
        PARSE_TABLE[1][0][3] = -1;
    PARSE_TABLE[1][1] = 0;
    PARSE_TABLE[1][2] = 0;
    PARSE_TABLE[1][3] = (int*) malloc(sizeof(int));
        PARSE_TABLE[1][3][0] = -1;
    PARSE_TABLE[1][4] = 0;
    PARSE_TABLE[1][5] = (int*) malloc(sizeof(int));
        PARSE_TABLE[1][5][0] = -1;
    PARSE_TABLE[2][0] = 0;
    PARSE_TABLE[2][1] = 0;
    PARSE_TABLE[2][2] = (int*) malloc(3*sizeof(int));
        PARSE_TABLE[2][2][0] = 4;
        PARSE_TABLE[2][2][1] = 3;
        PARSE_TABLE[2][2][2] = -1;
    PARSE_TABLE[2][3] = 0;
    PARSE_TABLE[2][4] = (int*) malloc(3*sizeof(int));
        PARSE_TABLE[2][4][0] = 4;
        PARSE_TABLE[2][4][1] = 3;
        PARSE_TABLE[2][4][2] = -1;
    PARSE_TABLE[2][5] = 0;
    PARSE_TABLE[3][0] = (int*) malloc(sizeof(int));
        PARSE_TABLE[3][0][0] = -1;
    PARSE_TABLE[3][1] = (int*) malloc(4*sizeof(int));
        PARSE_TABLE[3][1][0] = 6;
        PARSE_TABLE[3][1][1] = 4;
        PARSE_TABLE[3][1][2] = 3;
        PARSE_TABLE[3][1][3] = -1;
    PARSE_TABLE[3][2] = 0;
    PARSE_TABLE[3][3] = (int*) malloc(sizeof(int));
        PARSE_TABLE[3][3][0] = -1;
    PARSE_TABLE[3][4] = 0;
    PARSE_TABLE[3][5] = (int*) malloc(sizeof(int));
        PARSE_TABLE[3][5][0] = -1;
    PARSE_TABLE[4][0] = 0;
    PARSE_TABLE[4][1] = 0;
    PARSE_TABLE[4][2] = (int*) malloc(4*sizeof(int));
        PARSE_TABLE[4][2][0] = 7;
        PARSE_TABLE[4][2][1] = 0;
        PARSE_TABLE[4][2][2] = 8;
        PARSE_TABLE[4][2][3] = -1;
    PARSE_TABLE[4][3] = 0;
    PARSE_TABLE[4][4] = (int*) malloc(2*sizeof(int));
        PARSE_TABLE[4][4][0] = 9;
        PARSE_TABLE[4][4][1] = -1;
    PARSE_TABLE[4][5] = 0;
}

void FREE_PARSE_TABLE(){
    free(PARSE_TABLE[0][2]);
    free(PARSE_TABLE[0][4]);
    free(PARSE_TABLE[1][0]);
    free(PARSE_TABLE[1][3]);
    free(PARSE_TABLE[1][5]);
    free(PARSE_TABLE[2][2]);
    free(PARSE_TABLE[2][4]);
    free(PARSE_TABLE[3][0]);
    free(PARSE_TABLE[3][1]);
    free(PARSE_TABLE[3][3]);
    free(PARSE_TABLE[3][5]);
    free(PARSE_TABLE[4][2]);
    free(PARSE_TABLE[4][4]);
    free(PARSE_TABLE[0]);
    free(PARSE_TABLE[1]);
    free(PARSE_TABLE[2]);
    free(PARSE_TABLE[3]);
    free(PARSE_TABLE[4]);
    free(PARSE_TABLE);
}

int symbol_search(char *symbol, char **symbol_set, int num_symbols, int num_non_terminals)
{
    for(int i=0; i<num_symbols; i++){
        if(!strcmp(symbol,symbol_set[i])) return i;
    }
    return -1;
}

void message(char *msg, int collect){
    static struct stack_struct *cache = 0;
    static struct stack_struct *max_lengths = 0;
    static int col_index = 0, col_length = 0;
    int tmp,tmp2;
    char c;
    if(!cache){
        cache = stack_alloc();
        max_lengths = stack_alloc();
    }
    while(*msg){
        if(*msg == '\t'){
            col_index++;
            col_length=0;
        }
        else if(*msg == '\n'){
            col_index=0;
            col_length=0;
        }
        else{
            col_length++;
            if(col_index >= stack_size(max_lengths))
                stack_push(col_length,max_lengths);
            else if(stack_get(col_index,max_lengths) < col_length)
                stack_set(col_index,col_length,max_lengths);
        }
        stack_push(*msg,cache);
        msg++;
    }
    if(!collect){
        col_index=0;
        col_length=0;
        for(tmp=0;tmp<stack_size(cache);tmp++){
            c = (char) stack_get(tmp,cache);
            if(c == '\t'){
                for(tmp2=0;tmp2<(stack_get(col_index,max_lengths)-col_length)+PADDING_CONSTANT;tmp2++)
                    putchar(' ');
                col_index++;
                col_length=0;
            }
            else{
                col_length++;
                putchar(c);
                if(c == '\n'){
                    col_index=0;
                    col_length=0;
                }
            }
        }
        col_index=0;
        col_length=0;
        stack_dealloc(cache);
        stack_dealloc(max_lengths);
        cache = 0;
        max_lengths = 0;
    }
}

void print_state(struct stack_struct *stack, char  **input, char **symbol_set, int input_ptr, int input_size){
    for(int i=0;i<stack_size(stack); i++){
        if(i != stack_size(stack)-1){
            message(symbol_set[stack_lookback(i,stack)],1);
            message(" ",1);
        }
        else message(symbol_set[stack_lookback(i,stack)],1);
    }
    message("\t",1);
    for(int i=input_ptr; i<input_size; i++)
    {
        if(i<input_size-1){
            message(input[i],1);
            message(" ",1);
        }
        else message(input[i],1);
    }
}

void print_rule(int symbol1, int symbol2, char **symbol_set, int num_symbols, int num_non_terminals, int ***ll1_parsing_table){
    int *tmp;
    if(     
            (symbol2 < num_non_terminals)
        ||  (symbol1 >= num_non_terminals && symbol1 != symbol2)
        ||  (symbol1 < num_non_terminals && !ll1_parsing_table[symbol1][symbol2-num_non_terminals])
      )
      {
        message("No rule found",1);
        return;
      }
    if(symbol1 == symbol2)
    {
        message("POP ",1);
        message(symbol_set[symbol1],1);
        return;
    }
    message(symbol_set[symbol1],1);
    message(" -> ",1);
    tmp = ll1_parsing_table[symbol1][symbol2-num_non_terminals];
    while(*tmp>=0){
        message(symbol_set[*tmp],1);
        message(" ",1);
        tmp++;
    }
}

int exec_ll1_parser(char **symbol_set, int num_symbols, int num_non_terminals, int ***ll1_parsing_table, char **input, int input_size, int init_symbol, int input_end_symbol){
    int tmp,tmp2,tmp3,tmp4,input_ptr=0;
    struct stack_struct *stack = stack_alloc();
    if(init_symbol<0 || init_symbol>=num_non_terminals){
        message("\nInternal Error parsing the input: Unrecognized Start Symbol!",0);
        return 1;
    }
    if(input_end_symbol>=num_symbols || input_end_symbol<num_non_terminals){
        message("\nInternal Error parsing the input: Unrecognized Input Terminator Symbol!",0);
        return 1;
    }
    stack_push(input_end_symbol,stack);
    stack_push(init_symbol, stack);
    message("STACK\tINPUT\tRULE\n",1);
    print_state(stack,input,symbol_set,input_ptr,input_size);
    while(input_ptr < input_size){
        tmp = symbol_search(input[input_ptr],symbol_set,num_symbols,num_non_terminals);
        if(stack_empty(stack)){
            message("\n",0);
            message("\nError parsing the input: Stack empty!",0);
            return 1;
        }
        tmp2 = stack_pop(stack);
        message("\t",1);
        print_rule(tmp2,tmp,symbol_set,num_symbols,num_non_terminals,ll1_parsing_table);
        message("\n",1);
        if(tmp==tmp2) input_ptr++;
        else if(tmp<num_non_terminals || tmp >= num_symbols){
            message("",0);
            message("\nError parsing the input: Unrecognized input character!",0);
            return 1;
        }
        else if(tmp2 >= num_non_terminals){
            message("",0);
            message("\nError parsing the input: terminal mismatch!",0);
            return 1;
        }
        else if(!ll1_parsing_table[tmp2][tmp-num_non_terminals]){
            message("",0);
            message("\nError parsing the input: No entry in the parsing table!",0);
            return 1;
        }
        else{
            tmp3 = 0;
            tmp = tmp-num_non_terminals;
            while(ll1_parsing_table[tmp2][tmp][tmp3]>=0) tmp3++;
            while(tmp3>0){
                tmp4=ll1_parsing_table[tmp2][tmp][tmp3-1];
                if(tmp4<0 || tmp4>=num_symbols){
                    message("",0);
                    message("\nInternal Error parsing the input: Invalid Parse Table!",0);
                    return 1;
                }
                stack_push(tmp4,stack);
                tmp3--;
            }
        }
        print_state(stack,input,symbol_set,input_ptr,input_size);
    }
    message("",0);
    message("\nInput Successfully Parsed!",0);
    stack_dealloc(stack);
    return 0;
}

char *get_input(int *input_size){
}

int main(){
    BUILD_PARSE_TABLE();
    char *input[] = {"id","*","id","*","id","*","id","+","id", "id", "$"};
    int input_size = 10;
    exec_ll1_parser(SYMBOL_SET,NUM_SYMBOLS,NUM_NON_TERMINALS,(int***)PARSE_TABLE,input,input_size,0,10);
    FREE_PARSE_TABLE();
    return 0;
}