

#include <string>
#include <xtree>




#include "JTrackMem.h"

template <class T>
void help( T a, T b)
{
    char * p = new char[90];
    strcpy( p, "123" );
    printf( p );
    //delete[] p;
}

extern void * test();
extern void * test1();


class ABC
{
public:
    ABC()
    {
        printf( "ABC() \n" );
    }

    virtual void test()
    {
        printf("test! \n");
    }

    virtual ~ABC()
    {
        printf( "~ABC() \n" );
    }
};

class DDD : public ABC
{
public:
    DDD()
    {
        printf("DDD \n");
    }

    virtual ~DDD()
    {
        printf("~DDD \n");
    }
};

int main()
{
    help( 1, 2 );
    
    void * p = malloc( 10 );
    
    
    // free(p);


    p = realloc( p, 100 );

    ABC * q = new ABC;

    q->test();
    

    ABC * t = new DDD;

    delete t;
    
    PLeakItem pLeak = 0;
    unsigned int n = 0;
    PHackItem pHack = 0;
    unsigned int m = 0;

    JDebugGetReport( &pLeak, &n, &pHack, &m );

    if ( 0 == n )
    {
        printf("no leaked item\n");
    }

    unsigned int i;
    for ( i = 0; i < n; i++ )
    {
        PLeakItem q = pLeak + i;
        printf("%2u: %s, %u, %u\n", i + 1, q->szFileName, q->nFileLen, q->nMemSize );
    }

    printf("================\n");
    if ( 0 == m )
    {
        printf("no hacked item\n");
    }

    for ( i = 0; i < m; i++ )
    {
        PHackItem y = pHack + i;
        printf("%2u: %s, %u\n", i + 1, y->szFileName, y->nFileLen);
    }
    
    JDebugFree( pLeak );

    JDebugFree( pHack );
    
    JDebugClear();
    // 由于没有释放内存跟踪模块的s_pList，导致多泄露了12个字节
    getchar();
    return 0;
}
