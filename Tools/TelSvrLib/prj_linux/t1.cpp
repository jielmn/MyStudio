#include <stdio.h>
#include "TelSvrLib.h"
#include <string.h>

#include <windows.h>


int add(int n, void * args[] )
{
    int a = (int)args[0];
    int b = (int)args[1];
    int c = a + b;
    JTelSvrPrint( "add result is %d", c );
    return 0;
}

int test(int n, void * args[] )
{
    char * a = (char *)args[0];
    char * b = (char *)args[1];
    char buff[128];
    strcpy( buff, a );
    strcat( buff, b );
    JTelSvrPrint( "test result is %s", buff );
    return 0;
}

int test1(int n, void * args[] )
{
    Sleep( 10000 );
    return 0;
}

int main(int argc, char* argv[])
{
    JTelSvrRegCommand( "add",  "add 2 number",    add,  JTEL_ARG_TYPE_INT,  JTEL_ARG_TYPE_INT, 0 );
    JTelSvrRegCommand( "test", "strcat 2 string", test, JTEL_ARG_TYPE_TEXT, JTEL_ARG_TYPE_TEXT, 0 );
    JTelSvrRegCommand( "test1", "strcat 2 string", test1, 0 );
    JTelSvrRegCommand( "test2dd", "strcat 2 string", test, JTEL_ARG_TYPE_TEXT, JTEL_ARG_TYPE_TEXT, 0 );
    JTelSvrRegCommand( "tesxest", "strcat 2 string", test, JTEL_ARG_TYPE_TEXT, JTEL_ARG_TYPE_TEXT, 0 );

	int nRet = JTelSvrStart( 3600 );
    if ( 0 != nRet )
    {
        printf("!!! failed to start telnet[%d]\n", nRet );
    }
    else
    {
        printf("Start server\n");
    }

    char s[80];
    gets( s );

    while( 0 != strcmp( s, "quit") )
    {
        if ( 0 == strcmp(s,"stop") )
        {
            JTelSvrStop();
        }
        else if ( 0 == strcmp(s,"start") )
        {
            int nRet = JTelSvrStart( 3600 );
            if ( 0 != nRet )
            {
                printf("!!! failed to start telnet[%d]\n", nRet );
            }
            else
            {
                printf("=== Start server\n");
            }
        }
        else if ( 0 == strcmp(s,"quit") )
        {
            break;
        }

        gets( s );
    }
    
    

    getchar();
	return 0;
}

