#include <stdio.h>

#include "redis_client.h"

void  PrintNode( PTRedisXmlNode pNode, int nDepth )
{
    for ( int i = 0; i < nDepth; i++ )
    {
        printf("    ");
    }

    printf("name=[%s] ", pNode->m_szName );

    PTRedisXmlAttr pAttr = pNode->m_pFirstAttr;
    while( pAttr )
    {
        printf("%s=[%s] ", pAttr->m_szAttrName, pAttr->m_szAttrValue );
        pAttr = pAttr->m_pNextSibling;
    }

    if ( pNode->m_szBody )
    {
        printf("body=[%s] ", pNode->m_szBody );
    }

    printf("\n");

    PTRedisXmlNode  pChild = pNode->m_pFirstChild;
    while( pChild )
    {
        PrintNode( pChild, nDepth + 1 );
        pChild = pChild->m_pNextSibling;
    }
}

void test()
{
    PTRedisXmlNode pRoot = CreateRXmlNode("abc");
    PTRedisXmlNode p1    = CreateRXmlNode("xyz");
    PTRedisXmlNode p2    = CreateRXmlNode("ghc");
    PTRedisXmlNode p11   = CreateRXmlNode("x00");

    // SetRXmlBody( p1,  "xyz body!");
    SetRXmlAttr( p1, "a", "1" );
    SetRXmlAttr( p1, "b", "2" );
    SetRXmlAttr( p1, "c", "3" );

    // SetRXmlBody( p2,  "123 body!");
    SetRXmlAttr( p2, "a", "10" );
    SetRXmlAttr( p2, "b", "20" );
    SetRXmlAttr( p2, "c", "30" );

    //SetRXmlBody( p11, "x00 body!");
    //SetRXmlAttr( p11, "x", "300" );


    BOOL bRet = FALSE;

    bRet = AppendRXmlNode( pRoot, p1 );
    if ( !bRet )
    {
        printf("failed to AppendRXmlNode( pRoot, p1 ) \n");
    }

    bRet = AppendRXmlNode( pRoot, p2 );
    if ( !bRet )
    {
        printf("failed to AppendRXmlNode( pRoot, p2 ) \n");
    }


    bRet = AppendRXmlNode( p1, p11 );
    if ( !bRet )
    {
        printf("failed to AppendRXmlNode( p1, p11 ) \n");
    }

    PrintNode( pRoot, 0 );


    char * pXml = ConvertRXmlNode2Buf( pRoot );
    if ( pXml )
    {
        printf("xml=[%s]\n", pXml);

        FreeRXmlBuf( pXml );
    }

    bRet = DestroyRXmlNode( pRoot );
    if ( !bRet )
    {
        printf("failed to DestroyRXmlNode! \n");
    }
}

void test1()
{
    const char * p = "<abc a='123' b='456'>  <xyz x='100' z = '2000' >  <t1 t100='oop' t200='xde' >234<t300 /></t1></xyz><xyz y='200' d='test' ><t2>456</t2></xyz></abc >";
    DWORD dwLen = strlen(p);

    PTRedisXmlNode  pNode =  ParseRXmlBuf( p, dwLen );

    if ( 0 == pNode )
    {
        printf("failed to ParseRXmlBuf()! \n");
    }
    else
    {
        PrintNode( pNode, 0 );
        DestroyRXmlNode( pNode );
    }
}

void print_resp( PTRedisResp pResp )
{
    if ( REDIS_RESP_TYPE_SINGLE_LINE == pResp->m_eType )
    {
        printf("%s, %s \n", (pResp->m_bError ? "-" : "+"), pResp->m_szErrorDescription );
    }
    else if ( REDIS_RESP_TYPE_BULK == pResp->m_eType )
    {
        printf("data=[%s] \n", pResp->data.m_pFirstParam->m_pParam );
    }
}

void test_send( const char * p )
{
    HRedisClient  h = 0;
    int ret = ConnectRedis( &h, "192.168.1.20", 0, 5 );
    printf("connect redis server result = %d \n", ret );
    if ( 0 != ret )
    {
        return;
    }

    const char * pCommand = p;
    char buf[1024] = { 0 };
    int len = 1024;
    ret = SendRedisData( h, pCommand, strlen(pCommand), buf, (DWORD *)&len );
    printf("send result = %d\n", ret );

    if ( 0 == ret )
    {
        printf("recv buff = [%s], len = %d \n", buf, len);
    }
    else
    {
        return;
    }

    PTRedisResp pResp = 0;
    ret = ParseRedisResp( buf, len, &pResp );
    if ( 0 != ret )
    {
        printf("failed to ParseRedisResp()! \n");
    }
    else
    {
        print_resp( pResp );

        FreeRedisResp( pResp );
    }
    

    ret = CloseRedis(h);
    printf("close redis handle = %d\n", ret );
}

void test2()
{
    HRedisRequest h = CreateRedisRequest();
    if ( 0 == h )
    {
        printf("failed to CreateRedisRequest()!\n");
        return;
    }

    int ret = AddRedisRequestParam( h, "get1", "world1", 0 );
    if ( 0 != ret )
    {
        printf("failed to AddRedisRequestParam()!\n");
        return;
    }

    char * pBuf = 0;
    DWORD dwBufLen = 0;
    ret = ConvertRedisRequest2Buf( h, &pBuf, &dwBufLen );
    if ( 0 != ret )
    {
        printf("failed to ConvertRedisRequest2Buf()! \n");
        return;
    }

    test_send( pBuf );

    ret = FreeRedisRequestBuf( pBuf );
    if ( 0 != ret )
    {
        printf("failed to FreeRedisRequestBuf()!\n");
        return;
    }


    ret = ClearRedisRequest( h );
    if ( 0 != ret )
    {
        printf("failed to ClearRedisRequest()! \n");
        return;
    }





    ret = AddRedisRequestParam( h, "get", "hello", 0 );
    if ( 0 != ret )
    {
        printf("failed to AddRedisRequestParam()!\n");
        return;
    }

    ret = ConvertRedisRequest2Buf( h, &pBuf, &dwBufLen );
    if ( 0 != ret )
    {
        printf("failed to ConvertRedisRequest2Buf()! \n");
        return;
    }

    test_send( pBuf );

    ret = FreeRedisRequestBuf( pBuf );
    if ( 0 != ret )
    {
        printf("failed to FreeRedisRequestBuf()!\n");
        return;
    }

    ret = ClearRedisRequest( h );
    if ( 0 != ret )
    {
        printf("failed to ClearRedisRequest()! \n");
        return;
    }





    TRedisRequestParam  params[2];
    params[0].m_pParam = "get";
    params[0].m_dwParamLen = strlen(params[0].m_pParam);

    params[1].m_pParam = "hello";
    params[1].m_dwParamLen = strlen(params[1].m_pParam);

    ret = AddRedisRequestParamEx( h, params, 2 );
    if ( 0 != ret )
    {
        printf("failed to AddRedisRequestParamEx()!\n");
        return;
    }

    ret = ConvertRedisRequest2Buf( h, &pBuf, &dwBufLen );
    if ( 0 != ret )
    {
        printf("failed to ConvertRedisRequest2Buf()! \n");
        return;
    }

    test_send( pBuf );

    ret = FreeRedisRequestBuf( pBuf );
    if ( 0 != ret )
    {
        printf("failed to FreeRedisRequestBuf()!\n");
        return;
    }

    ret = ClearRedisRequest( h );
    if ( 0 != ret )
    {
        printf("failed to ClearRedisRequest()! \n");
        return;
    }






    ret = DestroyRedisRequest( h );
    if ( 0 != ret )
    {
        printf("failed to DestroyRedisRequest()! \n");
        return;
    }
}

int main()
{
#if 0
    HRedisClient  h = 0;
    int ret = ConnectRedis( &h, "192.168.1.20", 0, 5 );
    printf("connect redis server result = %d \n", ret );

    const char * pCommand = "*3\r\n$3\r\nset\r\n$5\r\nhello\r\n$3\r\n\x01\x02\x03\r\n";
    char buf[1024] = { 0 };
    int len = 1024;
    ret = SendRedisData( h, pCommand, strlen(pCommand), buf, (DWORD *)&len );
    printf("send result = %d\n", ret );

    if ( 0 == ret )
    {
        printf("recv buff = [%s], len = %d \n", buf, len);
    }

    ret = CloseRedis(h);
    printf("close redis handle = %d\n", ret );
#else
    // test();
    // test1();
    test2();
#endif

    getchar();
    return 0;
}
