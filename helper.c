/*
 * Author: Ritik Jain, 18114068
 * Since: May 25, 2021
 * Brief: Compiler Lab Assignment- Program 5: Constructs SLR(1) Parsing Table and LR(0) Canonical Items
 * 
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dynamic_stack.h"

const int MAX_LENGTH = 1024;
const int CHARSET_SIZE = (1<<(sizeof(char)*8));
const int PADDING_CONSTANT = 10;
char *START_SYMBOL = "S";
char *INPUT_TERMINATOR_SYMBOL = "$";

struct stack_struct *heads, *bodies, *cmap, *symbol_set;
int num_prods=0, num_symbols=0, start_symbol, input_terminator_symbol, start_grammar_symbol;

int symbol_search(char *symbol){
    for(int tmp=0; tmp<num_symbols; tmp++)
        if(!strcmp(((char*)stack_get_ptr(tmp,symbol_set)),symbol)) return tmp;
    return -1;
}

int symbol_exists(char *symbol){ return symbol_search(symbol) >= 0;}

int symbol_add(char *symbol){
    int n = symbol_search(symbol);
    if(n>=0) return n;
    n=0;
    while(symbol[n]) n++;
    char *sym = (char*) malloc(sizeof(char)*(n+1));
    memcpy(sym,symbol,sizeof(char)*(n+1));
    stack_push_ptr(sym,symbol_set);
    num_symbols++;
    return num_symbols-1;
}

char *symbol_get(int i){
    int n=0;
    char *s = (char*) stack_get_ptr(i,symbol_set);
    while(s[n])n++;
    char *t = (char*) malloc(sizeof(char)*(n+1));
    memcpy(t,s,sizeof(char)*(n+1));
    return t;
} 

void free_symbols(){
    for(int i=0;i<num_symbols;i++)
        free(stack_get_ptr(i,symbol_set));
    stack_dealloc(symbol_set);
}

int special_char(char c){
    switch(c){
        case '|': case ' ': case '\t': case '-': case '>': case 0: return 1;
        default: return 0;
    }
}

void skip_spaces(char **buffer_ptr){
    char *buffer = *buffer_ptr;
    while(*buffer == ' ' || *buffer == '\t') buffer++;
    *buffer_ptr = buffer;
}

int parse_productions(char *buffer){
    struct stack_struct *body = stack_alloc();
    char *tmp, *tmp2;
    int head;
    int body_length = 0;
    skip_spaces(&buffer);
    if(special_char(*buffer)){
        stack_dealloc(body);
        return 1;
    }
    tmp = buffer;
    while(!special_char(*buffer)) buffer++;
    if(tmp == buffer){
        stack_dealloc(body);
        return 1;
    }
    tmp2 = (char*) malloc(sizeof(char)*(buffer-tmp+1));
    tmp2[buffer-tmp] = 0;
    memcpy(tmp2,tmp,sizeof(char)*(buffer-tmp));
    head = symbol_add(tmp2);
    free(tmp2);
    skip_spaces(&buffer);
    if(*buffer != '-'){
        return 1;
    }
    buffer++;
    skip_spaces(&buffer);
    if(*buffer != '>'){
        free(body);
        return 1;
    }
    buffer++;
    while(1){
        skip_spaces(&buffer);
        if(!(*buffer) || *buffer == '|'){
            if(stack_size(heads) == num_prods){
                stack_dealloc(body);
                return 2;
            }
            stack_push(head,heads);
            stack_push_ptr(body, bodies);
            if(!(*buffer)) return 0;
            body = stack_alloc();
            buffer++;
            continue;
        }
        tmp = buffer;
        while(*buffer && *buffer != '\t' && *buffer != ' ' && *buffer != '|') buffer++;
        tmp2 = (char*) malloc(sizeof(char)*(buffer-tmp+1));
        tmp2[buffer-tmp] = 0;
        memcpy(tmp2,tmp,sizeof(char)*(buffer-tmp));
        stack_push(symbol_add(tmp2), body);
        free(tmp2);
    }
}

int is_terminal(int c){
    return stack_get(c, cmap) == 2;
}

int is_non_terminal(int c){
    return stack_get(c, cmap) == 1;
}

void map_non_terminals(){
    int tmp;
    for(tmp=0;tmp<num_prods;tmp++)
        stack_set(stack_get(tmp,heads), 1, cmap);
}

void map_terminals(){
    int tmp,tmp3,c;
    struct stack_struct *tmp2;
    for(tmp=0;tmp<num_prods;tmp++)
    {
        tmp2 =  (struct stack_struct *) stack_get_ptr(tmp, bodies);
        for(tmp3=0;tmp3<stack_size(tmp2);tmp3++){
            c = stack_get(tmp3,tmp2);
            if(!is_non_terminal(c))
                stack_set(c, 2, cmap);
        }
    }
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

int count_non_terminals(){
    int i,n=0;
    for(i=0;i<num_symbols;i++)
        if(is_non_terminal(i)) n++;
    return n;
}

int read_input_grammar(){
    int tmp,tmp2,tmp3;
    char *buffer, *tmp4, *tmp5;
    struct stack_struct *tmp6;
    bodies = stack_alloc();
    heads = stack_alloc();
    cmap = stack_alloc();
    symbol_set = stack_alloc();

    printf("The symbols '%s' and '%s' are reserved and cannot be a part of the grammar\n",START_SYMBOL,INPUT_TERMINATOR_SYMBOL);
    printf("Enter the number of production rules in the Context Free Grammar: ");
    scanf("%d\n",&num_prods);
    if(num_prods < 1){
        printf("Error: The number of productions has to be more than 0\n");
        return 1;
    }
    buffer = (char*) malloc(sizeof(char)*MAX_LENGTH);
    while(stack_size(heads)<num_prods){
        scanf("%[^\r\n]",buffer);
        tmp3 = getchar();
        if(tmp3 == '\r') getchar();
        tmp2 = parse_productions(buffer);
        if(tmp2==1){
            printf("Error: Unable to  parse rule #%d: %s\n",stack_size(heads)+1,buffer);
            for(tmp2=0;tmp2<stack_size(heads);tmp2++)
                stack_dealloc((struct stack_struct *)stack_get_ptr(tmp2,bodies));
            stack_dealloc(bodies);
            stack_dealloc(heads);
            stack_dealloc(cmap);
            free_symbols();
            free(buffer);
            return 1;
        }
        else if(tmp2==2){
            printf("Warning: Production overflow detected. Considering only the first %d production(s)\n",num_prods);
            break;
        }
    }
    for(tmp=0;tmp<num_symbols;tmp++) stack_push(0,cmap);
    map_non_terminals();
    map_terminals();
    if(symbol_exists(START_SYMBOL) || symbol_exists(INPUT_TERMINATOR_SYMBOL)){
        printf("Error: The symbols '%s' and '%s' are reserved, and cannot be used in the grammar\n",START_SYMBOL,INPUT_TERMINATOR_SYMBOL);
        for(tmp2=0;tmp2<num_prods;tmp2++)
            stack_dealloc((struct stack_struct *)stack_get_ptr(tmp2,bodies));
        stack_dealloc(bodies);
        stack_dealloc(heads);
        stack_dealloc(cmap);
        free_symbols();
        free(buffer);
        return 1;
    }
    if(count_non_terminals() > 1)
    {
        printf("Enter the start symbol: ");
        scanf("%[^\r\n]",buffer);
        tmp3 = getchar();
        if(tmp3 == '\r') getchar();
        tmp4 = buffer;
        tmp5 = buffer;
        while(*tmp5 && *tmp5 != ' ' && *tmp5 != '\t') tmp5++;
        if(tmp4 == tmp5){
            printf("Error: No start symbol specified!\n");
            for(tmp2=0;tmp2<num_prods;tmp2++)
                stack_dealloc((struct stack_struct *)stack_get_ptr(tmp2,bodies));
            stack_dealloc(bodies);
            stack_dealloc(heads);
            stack_dealloc(cmap);
            free_symbols();
            free(buffer);
            return 1;
        }
        tmp = tmp5-tmp4;
        tmp5 = (char*) malloc(sizeof(char)*(tmp+1));
        tmp5[tmp] = 0;
        memcpy(tmp5,tmp4,sizeof(char)*tmp);
        tmp = symbol_search(tmp5);
        free(tmp5);
        if(tmp<0 || is_terminal(tmp)){
            printf("Error: The symbol specified cannot be the start symbol!\n");
            for(tmp2=0;tmp2<num_prods;tmp2++)
                stack_dealloc((struct stack_struct *)stack_get_ptr(tmp2,bodies));
            stack_dealloc(bodies);
            stack_dealloc(heads);
            stack_dealloc(cmap);
            free_symbols();
            free(buffer);
            return 1;
        }
        start_grammar_symbol = tmp;
    }
    else{
        start_grammar_symbol = stack_peek(heads);
        printf("Selecting '%s' as the start symbol\n",symbol_get(start_grammar_symbol));
    }
    free(buffer);
    input_terminator_symbol = symbol_add(INPUT_TERMINATOR_SYMBOL);
    start_symbol = symbol_add(START_SYMBOL);
    stack_set(input_terminator_symbol,2,cmap);
    stack_set(start_symbol,1,cmap);
    stack_push(start_symbol,heads);
    tmp6 = stack_alloc();
    stack_push(start_grammar_symbol,tmp6);
    stack_push_ptr(tmp6,bodies);
    num_prods++;
    return 0;
}

int main(){
    printf("%d",read_input_grammar());
}