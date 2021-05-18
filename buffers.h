#include "stdio.h"
#include "string.h"

const int BUFFER_SIZE = 4;
char DATA_BUFFER[4];
char LOOKAHEAD_BUFFER[4];

int DATA_PTR = 0;
int LOOKAHEAD_PTR = 0;
fpos_t LOOKAHEAD_INITIAL_POSITION;
int LOOKAHEAD_USED = 0;
int DATA_SIZE = 0;
int LOOKAHEAD_SIZE = 0;
int STREAM_CLOSED = 0;
FILE *INPUT_FILE = NULL;

void error(const char *msg)
{
    printf("Error: ");
    printf(msg);
    printf('!\n');
}

void raw_input_open(const char *filename)
{
    INPUT_FILE = fopen(filename,"r");
    if(INPUT_FILE == NULL)
    {
        STREAM_CLOSED = 1;
        error(strcat("Unable to open the file",filename));
    }
    STREAM_CLOSED = 0;
}

void raw_input_close()
{
    STREAM_CLOSED = 1;
    fclose(INPUT_FILE);
}

int raw_is_readable()
{
    if(STREAM_CLOSED || feof(INPUT_FILE)) return 0;
    return 1;
}

char raw_readchar()
{
    if(!raw_is_readable()) return 0;
    return fgetc(INPUT_FILE);
}

int raw_read(char *dst, int max_len){
    if(!raw_is_readable()) return 0;
    return fread(dst,sizeof(char),max_len,INPUT_FILE);
}

void load_lookahead_buffer()
{
    LOOKAHEAD_PTR = 0;
    if(!LOOKAHEAD_USED){
        LOOKAHEAD_USED = 1;
        if(!STREAM_CLOSED)
            fgetpos(INPUT_FILE,&LOOKAHEAD_INITIAL_POSITION);
    }
    LOOKAHEAD_SIZE = raw_read(LOOKAHEAD_BUFFER,BUFFER_SIZE);
}

void reset_lookahead_buffer(){
    if(STREAM_CLOSED) return;
    fsetpos(INPUT_FILE,&LOOKAHEAD_INITIAL_POSITION);
    load_lookahead_buffer();
}

void load_data_buffer()
{
    if(!LOOKAHEAD_USED) load_lookahead_buffer();
    else reset_lookahead_buffer();
    memcpy(DATA_BUFFER,LOOKAHEAD_BUFFER,LOOKAHEAD_SIZE*sizeof(char));
    DATA_PTR = 0;
    DATA_SIZE = LOOKAHEAD_SIZE;
    LOOKAHEAD_USED = 0;
    load_lookahead_buffer();
}
