
#include "StrResLoader.h"



int main()
{
    MyStrResLoader  res;
    res.LoadFile("新建文本文档.txt");
    printf( "%s \n", res[1] );
    printf( "%s \n", res[2] );
    printf( "%s \n", res[3] );
    printf( "%s \n", res[4] );
    return 0;
}

