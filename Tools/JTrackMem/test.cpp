#include <stdio.h>
#include <malloc.h>

// 这个文件没有用内存跟踪

void * test()
{
    return malloc( sizeof(int) );
}

void * test1()
{
    return new int;
}
