#include "buffers.h"


void help(){
    printf("Usage: lab1 <input file name>\n");
}

int main( int argc, char **argv)
{
    if(argc != 1 || !strcmp(argv[0],"-h") || !strcmp(argv[0],"-H")){
        help();
        return 0;
    }
    raw_input_open(argv[0]);
    raw_input_close();
    return 0;
}