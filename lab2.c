#include "stdio.h"
#include "stdlib.h"

const int MAX_LENGTH = 1024;

int main( int argc, char **argv){
    char *input = malloc(MAX_LENGTH*sizeof(char));
    char *ptr = input;
    int accept = 1;
    int mode = 0;
    int rule_2_match = 0;
    printf("Enter string: ");
    scanf("%[^\n]",input);
    while(*ptr){
        if(*ptr == 'a' && mode == 0);
        else if(*ptr == 'b'){
            if(mode == 0) mode = 1;
            else if(mode == 1 && !rule_2_match) rule_2_match = 1;
            else if(rule_2_match>0) rule_2_match=-1;
        }
        else{
            accept = 0;
            break;
        }
        ptr++;
    }
    if(!accept)
        printf("%s is rejected. Failed to match any of the rules 'a*', 'a*b+', 'abb'",input);
    else if(!mode)
        printf("%s is accepted under rule 'a*'",input);
    else if(rule_2_match>0)
        printf("%s is accepted under rules 'a*b+' and 'abb'",input);
    else
        printf("%s is accepted under rule 'a*b+'",input);
    free(input);
    return 0;
}