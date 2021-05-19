#include "stdio.h"
#include "stdlib.h"
#include "string.h"

const int MAX_LENGTH = 1024;
const int CHARSET_SIZE = (1<<(sizeof(char)*8));

int special_char(char c){
    switch(c){
        case '|': case ' ': case '-': case '>': return 1;
        default: return 0;
    }
}

void skip_spaces(char **buffer_ptr){
    char *buffer = *buffer_ptr;
    while(*buffer && *buffer == ' ') buffer++;
    *buffer_ptr = buffer;
}

int parse_productions(char *buffer, char *heads, char **bodies, int *index, int num_prods){
    char *body = (char*) malloc(sizeof(char)*MAX_LENGTH);
    char head = 0;
    int body_length = 0;
    skip_spaces(&buffer);
    if(special_char(*buffer)){
        free(body);
        return 1;
    }
    head = *buffer;
    buffer++;
    skip_spaces(&buffer);
    if(*buffer != '-'){
        free(body);
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
            if(*index == num_prods){
                free(body);
                return 2;
            }
            body[body_length] = 0;
            heads[*index] = head;
            bodies[*index] = (char*) malloc(sizeof(char)*(body_length+1));
            memcpy(bodies[*index],body,sizeof(char)*(body_length+1));
            *index = *index+1;
            body_length = 0;
            if(!(*buffer)){
                free(body);
                return 0;
            }
            buffer++;
            continue;
        }
        body[body_length++] = *buffer;
        buffer++;
    }
}

int char_val(char c){
    if(c<0) return CHARSET_SIZE+c;
    return c;
}

void map_non_terminals(char *cmap, char *heads, int num_prods){
    char *map;
    int tmp;
    for(tmp=0;tmp<num_prods;tmp++)
        cmap[char_val(heads[tmp])] = 1;
}

char **calculate_first_set(char *heads, char **bodies, int num_prods, char *cmap){
    char **first_set, head, *body;
    int change,tmp,tmp2,tmp3,tmp4,nullable;
    first_set = (char**)malloc(CHARSET_SIZE*sizeof(char*));
    for(tmp=0;tmp<CHARSET_SIZE;tmp++)
    {
        if(cmap[tmp]){
            first_set[tmp] = (char*)malloc(CHARSET_SIZE*sizeof(char));
            for(tmp2=0;tmp2<CHARSET_SIZE;tmp2++) first_set[tmp][tmp2] = 0;
        }
    }
    change = 1;
    while(change){
        change = 0;
        for(tmp=0;tmp<num_prods;tmp++){
            head = heads[tmp];
            body = bodies[tmp];
            nullable = 1;
            tmp4 = char_val(head);
            while(*body){
                tmp2 = char_val(*body);
                if(cmap[tmp2]){
                    for(tmp3=1;tmp3<CHARSET_SIZE;tmp3++){
                        if(first_set[tmp2][tmp3] && !first_set[tmp4][tmp3]){
                            first_set[tmp4][tmp3] = 1;
                            change = 1;
                        }
                    }
                    if(!first_set[tmp2][0]){
                        nullable = 0;
                        break;
                    } 
                }
                else{
                    if(!first_set[tmp4][tmp2]){
                        first_set[tmp4][tmp2] = 1;
                        change = 1;
                    }
                    nullable = 0;
                    break;
                }
                body++;
            }
            if(nullable && !first_set[tmp4][0]){
                first_set[tmp4][0] = 1;
                change = 1;
            }
        }
    }
    return first_set;
}

int main(int argc, char **argv){
    int num_prods,tmp,tmp2,tmp3;
    char *heads, *buffer, **bodies, **first_set, *cmap;
    printf("Enter the number of production rules: ");
    scanf("%d\n",&num_prods);
    heads = (char*) malloc(sizeof(char)*num_prods);
    bodies = (char**) malloc(sizeof(char*)*num_prods);
    buffer = (char*) malloc(sizeof(char)*MAX_LENGTH);
    tmp=0;
    while(tmp<num_prods){
        scanf("%[^\r\n]",buffer);
        tmp3 = getchar();
        if(tmp3 == '\r') getchar();
        tmp2 = parse_productions(buffer,heads,bodies,&tmp,num_prods);
        if(tmp2==1){
            printf("Error: Unable to  parse rule #%d: %s",tmp+1,buffer);
            for(tmp2=0;tmp2<tmp;tmp++)
                free(bodies[tmp2]);
            free(bodies);
            free(heads);
            free(buffer);
            return 0;
        }
        else if(tmp2==2){
            printf("Warning: Production overflow detected. Considering only the first %d production(s)",num_prods);
            break;
        }
    }
    cmap = (char*)malloc(CHARSET_SIZE*sizeof(char));
    for(tmp=0;tmp<CHARSET_SIZE;tmp++) cmap[tmp] = 0;
    map_non_terminals(cmap,heads,num_prods);
    first_set = calculate_first_set(heads, bodies, num_prods, cmap);
    printf("\n");
    for(tmp=0;tmp<CHARSET_SIZE;tmp++){
        if(cmap[tmp]){
            printf("First(%c): [",(char)tmp);
            if(first_set[tmp][0]) tmp3=1;
            else tmp3=0;
            for(tmp2=1;tmp2<CHARSET_SIZE;tmp2++){
                if(first_set[tmp][tmp2])
                {
                    if(tmp3) printf(",");
                    printf("%c",(char)tmp2);
                    tmp3=1;
                }
            }
            printf("]\n");
        }
    }
    for(tmp=0;tmp<num_prods;tmp++)
        free(bodies[tmp]);
    free(bodies);
    free(heads);
    free(buffer);
    for(tmp=0;tmp<CHARSET_SIZE;tmp++){
        if(cmap[tmp]) free(first_set[tmp]);
    }
    free(first_set);
    free(cmap);
    return 0;
}