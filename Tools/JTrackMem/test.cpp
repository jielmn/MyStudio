#include <stdio.h>
#include <malloc.h>

// ����ļ�û�����ڴ����

void * test()
{
    return malloc( sizeof(int) );
}

void * test1()
{
    return new int;
}
