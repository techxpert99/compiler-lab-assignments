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
const int MAX_COLUMNS = 3;

char *START_SYMBOL = "S";
char *INPUT_TERMINATOR_SYMBOL = "$";

struct stack_struct *heads, *bodies, *cmap, *symbol_set, *canonical_lr0_state_set ,*lr0_state_transition_map, *first_set, *follow_set;
int num_prods=0, num_symbols=0, start_symbol, input_terminator_symbol, start_grammar_symbol, num_states=0;

struct stack_struct *parsing_table;

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

struct item{
    int __pos;
    int __rule;
};

struct state{
    struct stack_struct *__items;
};

struct item *item_alloc(int rule){
    int tmp;
    struct item *it = (struct item *) malloc (sizeof(struct item));
    it->__pos = 0;
    it->__rule = rule;
}

void item_dealloc(struct item *it){
    free(it);
}

int item_shiftable(struct item *it){
    int body_size = stack_size( (struct stack_struct *) stack_get_ptr( it->__rule, bodies));
    return it->__pos < body_size;
}

int item_shift(struct item *it){
    if(!item_shiftable(it)) return 0;
    it->__pos++;
    return 1;
}

int item_next_symbol(struct item *it){
    if(!item_shiftable(it)) return -1;
    return stack_get(it->__pos, (struct stack_struct *) stack_get_ptr( it->__rule, bodies));
}

struct item *item_copy(struct item *item){
    struct item *copy = (struct item *) malloc ( sizeof( struct item));
    copy->__pos = item->__pos;
    copy->__rule = item->__rule;
    return copy;
}

int item_eq(struct item *i1, struct item *i2){
    return (i1->__rule == i2->__rule) && (i1->__pos == i2->__pos);
}

int item_rule(struct item *i){ return i->__rule;}

int item_pos(struct item *i){ return i->__pos;}

struct state *state_alloc(){
    struct state *state = (struct state*) malloc (sizeof( struct state));
    state->__items = stack_alloc();
    return state;
}

struct state *state_dealloc(struct state *state){
    struct item *item;
    while(!stack_empty(state->__items)){
        item = (struct item *) stack_pop_ptr(state->__items);
        item_dealloc( item);
    }
    stack_dealloc(state->__items);
    free(state);
}

struct item *state_get_item(int index, struct state *state){
    return (struct item *) stack_get_ptr(index, state->__items);
}

struct item *state_pop_item(struct state *state){
    return (struct item *)stack_pop_ptr(state->__items);
}

int state_empty(struct state *state){ return stack_empty(state->__items);}

int state_size(struct state *state){ return stack_size_ptr(state->__items);}

int item_in_state(struct item *i, struct state *s){
    for(int n=0;n<state_size(s);n++){
        if(item_eq(i, state_get_item(n,s)))
            return 1;
    }
    return 0;
}

int state_eq(struct state *s1, struct state *s2){
    int tmp1 = state_size(s1),tmp2 = state_size(s2);
    if(tmp1 != tmp2) return 0;
    for(tmp1=0;tmp1<tmp2;tmp1++) if(!item_in_state(state_get_item(tmp1,s1),s2)) return 0;
    return 1;
}

void state_push_item(struct item *item, struct state *state){
    if(item_in_state(item,state)) return;
    stack_push_ptr(item_copy(item), state->__items);
}

struct state *state_copy(struct state *state){
    struct state *copy = state_alloc();
    int n;
    for(n=0;n<state_size(state);n++)
        stack_push_ptr(item_copy((struct item *)stack_get_ptr(n,state->__items)), copy->__items);
    return copy;
}

struct state* lr0_closure(struct state *state){
    struct item *item;
    struct state *closed_state = state_alloc();
    struct state *stack = state_copy(state);
    int i,c;
    while(!state_empty(stack)){
        item = state_pop_item(stack);
        state_push_item(item,closed_state);
        c = item_next_symbol(item);
        item_dealloc(item);
        if(c<0) continue;
        if(is_non_terminal(c)){
            for(i=0;i<num_prods;i++){
                if(stack_get(i,heads) == c){
                    item = item_alloc(i);
                    if(!item_in_state(item, closed_state))
                        state_push_item(item, stack);
                    item_dealloc(item);
                }
            }
        }
    }
    state_dealloc(stack);
    return closed_state;
}

struct state* lr0_goto(struct state* state, int symbol){
    struct state* new_state = state_alloc();
    struct state* closed_new_state;
    struct item* item;
    int n;
    for(n=0;n<state_size(state);n++){
        item = item_copy(state_get_item(n, state));
        if(item_next_symbol(item) == symbol){
            item_shift(item);
            state_push_item(item,new_state);
            item_dealloc(item);
        }
    }
    closed_new_state = lr0_closure(new_state);
    state_dealloc(new_state);
    return closed_new_state;
}

int state_in_stack(struct state *state, struct stack_struct *stack){
    int tmp;
    for(tmp=0;tmp<stack_size_ptr(stack);tmp++)
        if(state_eq(state, (struct state *) stack_get_ptr(tmp, stack)))
            return 1;
    return 0;
}

int state_search(struct state *state){
    int tmp;
    for(tmp=0;tmp<num_states;tmp++)
        if(state_eq(state,(struct state *) stack_get_ptr(tmp, canonical_lr0_state_set)))
            return tmp;
    return -1;
}

struct state *state_get( int index){
    return (struct state *) stack_get_ptr( index, canonical_lr0_state_set);
}

int lr0_transition_get(int state, int symbol){
    return stack_get(state*num_symbols+symbol, lr0_state_transition_map);
}

void lr0_transition_set(int from, int symbol, int to){
    stack_set(from*num_symbols+symbol,to,lr0_state_transition_map);
}

void lr0_items(){
    struct item *tmp = item_alloc(num_prods-1);
    struct state *tmp2 = state_alloc(), *tmp3;
    struct stack_struct *stack = stack_alloc();
    int tmp4,tmp5;
    canonical_lr0_state_set = stack_alloc();
    lr0_state_transition_map = stack_alloc();
    lr0_state_transition_map = stack_alloc();
    state_push_item(tmp,tmp2);
    tmp3 = lr0_closure(tmp2);
    state_dealloc(tmp2);
    item_dealloc(tmp);
    stack_push_ptr(tmp3,stack);
    while(!stack_empty(stack)){
        tmp2 = (struct state *) stack_pop_ptr( stack);
        stack_push_ptr( tmp2, canonical_lr0_state_set);
        num_states++;
        for(tmp4=0;tmp4<num_symbols;tmp4++){
            tmp3 = lr0_goto(tmp2, tmp4);
            if(!state_empty(tmp3) && !state_in_stack(tmp3,stack) &&  !state_in_stack(tmp3,canonical_lr0_state_set))
                stack_push_ptr(tmp3,stack);
            else state_dealloc(tmp3);
        }
    }
    stack_dealloc(stack);
    for(tmp4=0;tmp4<num_states*num_symbols;tmp4++) stack_push(-1,lr0_state_transition_map);
    for(tmp4=0;tmp4<num_states;tmp4++){
        tmp2 = (struct state*) stack_get_ptr(tmp4, canonical_lr0_state_set);
        for(tmp5=0;tmp5<num_symbols;tmp5++){
            tmp3 = lr0_goto(tmp2,tmp5);
            if(!state_empty(tmp3))
                lr0_transition_set(tmp4,tmp5,state_search(tmp3));
        }
    }
}

void calculate_first_set(){
    int tmp,tmp2,tmp3,tmp5,change,head,nullable;
    struct stack_struct *tmp4, *body, *first;
    first_set = stack_alloc();
    for(tmp=0;tmp<num_symbols;tmp++){
        tmp4 = stack_alloc();
        for(tmp2=0;tmp2<=num_symbols;tmp2++)
            stack_push(0,tmp4);
        if(is_terminal(tmp))
            stack_set(tmp,1,tmp4);
        stack_push_ptr(tmp4, first_set);
    }
    change = 1;
    while(change){
        change = 0;
        for(tmp=0;tmp<num_prods;tmp++){
            head = stack_get(tmp,heads);
            body = (struct stack_struct *)stack_get_ptr(tmp,bodies);
            nullable = 1;
            first = (struct stack_struct*) stack_get_ptr(head,first_set);
            for(tmp2=0;tmp2<stack_size(body);tmp2++){
                tmp3 = stack_get(tmp2,body);
                tmp4 = (struct stack_struct*) stack_get_ptr(tmp3,first_set);
                if(is_non_terminal(tmp3)){
                    for(tmp5=0;tmp5<num_symbols;tmp5++){
                        if(stack_get(tmp5,tmp4) && !stack_get(tmp5,first)){
                            stack_set(tmp5,1,first);
                            change = 1;
                        }
                    }
                    if(!stack_get(num_symbols,tmp4)){
                        nullable = 0;
                        break;
                    }
                }
                else{
                    if(!stack_get(tmp3,first)){
                        stack_set(tmp3,1,first);
                        change = 1;
                    }
                    nullable = 0;
                    break;
                }
            }
            if(nullable && !stack_get(num_symbols,first)){
                stack_set(num_symbols,1,first);
                change = 1;
            }
        }
    }
}

struct stack_struct *first_set_of_string(struct stack_struct *body, int index){
    int tmp,sym,tmp3,nullable=1;
    struct stack_struct *first = stack_alloc(), *tmp2;
    for(tmp=0;tmp<=num_symbols;tmp++)
        stack_push(0,first);
    for(tmp=index;tmp<stack_size(body);tmp++){
        sym = stack_get(tmp,body);
        tmp2 = (struct stack_struct*) stack_get_ptr(sym, first_set);
        for(tmp3=0;tmp3<num_symbols;tmp3++)
            if(stack_get(tmp3,tmp2))
                stack_set(tmp3,1,first);
        if(!stack_get(num_symbols,tmp2)){
            nullable = 0;
            break;
        }
    }
    if(nullable) stack_set(num_symbols,1,first);
    return first;
}

void calculate_follow_set(){
    int tmp,tmp3,tmp4,change=1,head,symbol;
    struct stack_struct *tmp2, *body, *first, *follow, *hfollow;
    follow_set = stack_alloc();
    for(tmp=0;tmp<num_symbols;tmp++){
        tmp2 = stack_alloc();
        for(tmp3=0;tmp3<num_symbols;tmp3++){
            stack_push(0,tmp2);
        }
        if(tmp == start_symbol)
            stack_set(input_terminator_symbol,1,tmp2);
        stack_push_ptr(tmp2,follow_set);
    }
    while(change){
        change = 0;
        for(tmp=0;tmp<num_prods;tmp++){
            head = stack_get(tmp,heads);
            body = (struct stack_struct*) stack_get_ptr(tmp,bodies);
            hfollow =  (struct stack_struct*) stack_get_ptr(head,follow_set);
            for(tmp3=0;tmp3<stack_size(body);tmp3++)
            {
                symbol = stack_get(tmp3,body);
                follow = (struct stack_struct*) stack_get_ptr(symbol,follow_set);
                first = first_set_of_string(body,tmp3+1);
                for(tmp4=0;tmp4<num_symbols;tmp4++){
                    if(stack_get(tmp4,first) && !stack_get(tmp4,follow)){
                        stack_set(tmp4,1,follow);
                        change = 1;
                    }
                }
                if(stack_get(num_symbols,first)){
                    for(tmp4=0;tmp4<num_symbols;tmp4++){
                        if(stack_get(tmp4,hfollow) && !stack_get(tmp4,follow)){
                            stack_set(tmp4,1,follow);
                            change = 1;
                        }
                    }
                }
                stack_dealloc(first);
            }
        }
    }
}

void build_slr1_parse_table(){
    int tmp1,tmp2,tmp3,tmp7,tmp9;
    struct stack_struct *tmp4, *tmp8;
    struct state *tmp5;
    struct item *tmp6;
    parsing_table = stack_alloc();
    for(tmp1=0;tmp1<num_states;tmp1++){
        for(tmp2=0;tmp2<num_symbols;tmp2++){
            tmp4 = (struct stack_struct *) stack_alloc();
            stack_push_ptr(tmp4, parsing_table);
            tmp3 = lr0_transition_get(tmp1, tmp2);
            if(tmp3<0) continue;
            if(is_terminal(tmp2))
                stack_push(tmp3,tmp4);
            else
                stack_push(2*(num_states+num_prods)+tmp3,tmp4);
        }
        tmp5 = state_get( tmp1);
        for(tmp2=0;tmp2<state_size(tmp5);tmp2++){
            tmp6 = state_get_item( tmp2, tmp5);
            if(!item_shiftable(tmp6)){
                tmp7 = item_rule(tmp6);
                tmp4 = (struct stack_struct *) stack_get_ptr( stack_get( tmp7, heads), follow_set);
                tmp7 += num_states+num_prods;
                for(tmp3=0; tmp3 < num_symbols; tmp3++){
                    if(!stack_get(tmp3,tmp4)) continue;
                    tmp8 = (struct stack_struct *) stack_get_ptr( tmp1*num_symbols+tmp3, parsing_table);
                    for(tmp9=0;tmp9<stack_size(tmp8);tmp9++){
                        if(stack_get(tmp9, tmp8) == tmp7)
                            break;
                    }
                    if(tmp9 == stack_size(tmp8))
                        stack_push(tmp7, tmp8);
                }
            }
        }
    }
}

void free_bodies(){
    int tmp;
    for(tmp=0;tmp<num_prods;tmp++)
        stack_dealloc((struct stack_struct *)stack_get_ptr(tmp,bodies));
}

void free_items(){
    int tmp;
    struct state *tmp2;
    while(!stack_empty(canonical_lr0_state_set)){
        tmp2 = (struct state *) stack_pop_ptr(canonical_lr0_state_set);
        state_dealloc(tmp2);
    }
    stack_dealloc(canonical_lr0_state_set);
}

void free_first_set(){
    int tmp;
    for(tmp=0;tmp<num_symbols;tmp++)
        stack_dealloc( (struct stack_struct*) stack_get_ptr(tmp, first_set));
    stack_dealloc(first_set);
}

void free_follow_set(){
    int tmp;
    for(tmp=0;tmp<num_symbols;tmp++)
        stack_dealloc( (struct stack_struct*) stack_get_ptr(tmp, follow_set));
    stack_dealloc(follow_set);
}

void free_parsing_table(){
    int tmp;
    for(tmp=0;tmp<num_symbols*num_states;tmp++)
        stack_dealloc( (struct stack_struct *) stack_get_ptr( tmp, parsing_table));
    stack_dealloc( parsing_table);
}

void release(){
    stack_dealloc(bodies);
    stack_dealloc(heads);
    stack_dealloc(cmap);
    stack_dealloc(lr0_state_transition_map);
    free_symbols();
    free_items();
    free_first_set();
    free_follow_set();
    free_parsing_table();
}

void print_item(int state, int item){
    int rule,pos,tmp;
    struct item* it;
    struct stack_struct *body;
    it = state_get_item(item, state_get( state));
    rule = item_rule(it);
    pos = item_pos(it);
    body = (struct stack_struct*) stack_get_ptr( rule, bodies);
    message(symbol_get(stack_get(rule, heads)),1);
    message(" -> ",1);
    for(tmp=0;tmp<pos;tmp++){
        message(symbol_get(stack_get(tmp, body)),1);
        message(" ",1);
    }
    message(".",1);
    for(;tmp<stack_size(body);tmp++)
    {
        message(" ",1);
        message(symbol_get(stack_get(tmp, body)),1);
    }
}

void print_production(int rule){
    struct stack_struct *body;
    int tmp;
    body = (struct stack_struct*) stack_get_ptr( rule, bodies);
    message(symbol_get(stack_get(rule, heads)),1);
    message(" -> ",1);
    for(tmp=0;tmp<stack_size(body);tmp++){
        message(symbol_get(stack_get(tmp, body)),1);
        if(tmp != stack_size(body)-1)
            message(" ",1);
    }
}

void print_lr0_states(){
    int num,max_items,size,state,item,j,k,i;
    char *buffer = (char *)malloc(sizeof(char)*MAX_LENGTH);
    message("Canonical Set of LR(0) Items:\n\n",0);
    num = num_states%MAX_COLUMNS ? num_states/MAX_COLUMNS+1:num_states/MAX_COLUMNS;
    for(i=0; i<num; i++){
        max_items = 0;
        for(j=0; j<MAX_COLUMNS; j++){
            state = i*MAX_COLUMNS+j;
            if(state>=num_states) continue;
            message("State ",1);
            sprintf(buffer,"%d",state);
            message(buffer,1);
            message(":\t",1);
            size = state_size( state_get( state));
            if(size > max_items)
                max_items = size;
        }
        message("\n\n",1);
        for(item=0;item<max_items; item++){
            for(j=0; j<MAX_COLUMNS; j++){
                state = i*MAX_COLUMNS+j;
                if(state>=num_states) continue;
                size = state_size( state_get( state));
                if(item<size)
                    print_item(state, item);
                message("\t",1);
            }
            message("\n",1);
        }
        message("\n\n",1);
    }
    message("",0);
    message("\n\n",0);
    free(buffer);
}

void print_slr1_parse_table(){
    int num_ter,tab_before,i,j,k,act,max_acts,num_print;
    struct stack_struct *acts;
    char *buffer = (char *) malloc(sizeof(char)*MAX_LENGTH);
    message("SLR(1) Parsing Table:\n\n",0);
    message("STATE\t",1);
    num_ter = 0;
    for(i=0;i<num_symbols;i++)
        if(is_terminal(i))
            num_ter++;
    tab_before = (num_ter-1)/2;
    for(i=0;i<tab_before;i++)
        message("\t",1);
    message("ACTIONS",1);
    tab_before = (num_ter-tab_before) + (num_symbols-num_ter-1)/2;
    for(i=0;i<tab_before;i++)
        message("\t",1);
    message("GOTO\n\n",1);
    message("\t",1);
    for(i=0;i<num_symbols;i++){
        if(is_terminal(i)){
            message(symbol_get(i),1);
            message("\t",1);
        }
    }
    for(i=0;i<num_symbols;i++){
        if(is_non_terminal(i)){
            message(symbol_get(i),1);
            message("\t",1);
        }
    }
    message("\n\n",1);
    for(i=0;i<num_states;i++){
        sprintf(buffer,"%d",i);
        message(buffer,1);
        max_acts=0;
        for(j=0;j<num_symbols;j++){
            k = stack_size((struct stack_struct*) stack_get_ptr( num_symbols*i+j, parsing_table));
            if(k>max_acts)
                max_acts = k;
        }
        for(k=0;k<max_acts;k++){
            message("\t",1);
            for(j=0;j<num_symbols;j++){
                if(is_non_terminal(j)) continue;
                acts = (struct stack_struct *) stack_get_ptr( num_symbols*i+j, parsing_table);
                if(k<stack_size(acts)){
                    act = stack_get(k, acts);
                    if(!(act/(num_prods+num_states))){
                        message("shift to ",1);
                        sprintf(buffer,"%d",act);
                        message(buffer,1);
                    }
                    else{
                        message("reduce with ",1);
                        print_production(act%(num_prods+num_states));
                    }
                }
                message("\t",1);
            }
            num_print=0;
            for(j=0;j<num_symbols;j++){
                if(is_terminal(j)) continue;
                acts = (struct stack_struct *) stack_get_ptr( num_symbols*i+j, parsing_table);
                if(k<stack_size(acts)){
                    act = stack_get(k,acts);
                    message("go to ",1);
                    sprintf(buffer,"%d",act%(num_prods+num_states));
                    message(buffer,1);
                }
                num_print++;
                if(num_print + num_ter == num_symbols) break;
                message("\t",1);
            }
            message("\n",1);
        }
        message("\n\n",1);
    }
    message("",0);
    free(buffer);
}

int main(int argc, char **argv){
    if(read_input_grammar()) return 1;
    lr0_items();
    calculate_first_set();
    calculate_follow_set();
    build_slr1_parse_table();
    printf("\n\n");
    print_lr0_states();
    printf("\n\n");
    print_slr1_parse_table();
    printf("\n\n");
    release();
    return 0;
}