#include "buffers.h"

/*
    KW: Keyword
    ID: Identifier
    CT: Constant
    ST: String
    OP: Operator
    SP: Special Symbol
    DE: Default
*/
enum token_type {KW,ID,CT,ST,OP,SP,DE};

long cpos = 0;
char ch = 0;
enum token_type type = DE;
int str = 0;
int backslash = 0;
char token_val[32];
int ptr = 0;

int contained_in(char c, const char *clist){
    int i=0;
    char k = clist[i];
    while(k){
        if(c == k) return 1;
        k = clist[++i];
    }
    return 0;
}

void push_token(){

}

void lexical_analyzer(){

    while(has_more_data()){
        ch = pop_chr();
        if(backslash){
            token_val[ptr++] = ch;
            backslash = 0;
        }
        else if(str){
            if(ch == '\\') backslash = 1;
            else if(ch == '"'){
                push_token();
                str = 0;
            } 
        }
        if(type==DE){
            if(contained_in(ch,"()[]{}*=,;"))
            {
                type = SP;
                token_val[ptr++] = ch;
                push_token();
            }
            else if(ch == '"'){
                str = 1;
                type = ST;
            }
        }
            else if(contained_in())
        }

    }
}

void help(){
    printf("Usage: lab1 <input file name>\n");
}

int main( int argc, char **argv)
{
    if(argc < 2 || !strcmp(argv[1],"-h") || !strcmp(argv[1],"-H")){
        printf("Incorrect number of arguments: Required 2\n");
        help();
        return 0;
    }
    raw_input_open(argv[0]);
    raw_input_close();
    return 0;
}