#include <stdio.h>
#include <string.h>

int main(int argc, char** argv[]){

    printf("argc: %d\n", argc);

    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n",i,argv[i]);
    }
    
    if (argc !=3){

        printf("invalid number of arguments. \n");
        printf("Usage:\n\t./prog -p <port> \n");
        return 1;
    }
    
    if (strcmp(argv[1], "-p") != 0){
        printf("unknow argument: %s\n", argv[1]);
        return 1;
    }

    int port = atoi(argv[2]);
    if(port == 0){
        printf("invalid port: %s\n", argv[2]);
        return 1;
    }
    
    if (port < 1000){
        printf("invalid port, it shoult be > 1000\n");
        return 1;
    }

    printf("Running at port %d\n",port);


    
    
    return 0;
}