
#include "StrResLoader.h"



int main()
{
    MyStrResLoader  res;
    res.LoadFile("�½��ı��ĵ�.txt");
    printf( "%s \n", res[1] );
    printf( "%s \n", res[2] );
    printf( "%s \n", res[3] );
    printf( "%s \n", res[4] );
    return 0;
}

