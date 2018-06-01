#include "StdAfx.h"
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "Inner.h"
#include "LmnCommon.h"



#ifdef __cplusplus
extern "C" {
#endif


static const char  s_base64_alphabet[] = 
{ 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P', 
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f', 
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v', 
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/', '=' };




/***************************************************************************/
/*                      �ڲ�(��̬)����                                     */
/***************************************************************************/



    

/****************************************************************************
 * ��������  _IsBlankChar                                                   *
 * ��  �ܣ�  �ж��ַ��Ƿ�հ��ַ�( SPACE, TAB, \r, \n )                     *
 * ����ֵ��  TRUE       �ǿհ��ַ�                                          *
 *           FALSE      �ǿհ��ַ�                                          *
 ****************************************************************************/
static BOOL _IsBlankChar( IN char ch )
{
    if ( ' ' == ch || '\t' == ch || '\r' == ch || '\n' == ch )
    {
	    return TRUE;
    }
    else
    {
	    return FALSE;
    }
}

/****************************************************************************
 * ��������  _GetNumberInfo                                                 *
 * ��  �ܣ�  ������λ�ϵ����֣��Լ�λ��                                   *
 * ����  ��  dwNum      ����                                                *
 *           dwMode     ����                                                *
 *           pdwFirstDigital  ���λ����                                    *
 *           pdwDigitalCount  ȫ�����ָ���                                  *
 * ˵��  ��  ����,    123      ���λ����Ϊ 1��  ��3λ����                  *
 *                    0xa23    ���λ����Ϊ 10�� ��3λ����                  *
 ****************************************************************************/
static void _GetNumberInfo( IN  DWORD dwNum, IN DWORD dwMode, 
                            OUT DWORD * pdwFirstDigital,
                            OUT DWORD * pdwDigitalCount )
{
    *pdwFirstDigital = 0;
    *pdwDigitalCount = 0;

    while( dwNum > 0 )
    {
        (*pdwDigitalCount)++;
        *pdwFirstDigital = dwNum;
        dwNum /= dwMode;
    }
}

/****************************************************************************
 * ��������  _Ch2Num                                                        *
 * ��  �ܣ�  ���ַ�ת��������(�Ѿ�ȷ���ַ���ΪСд)                         *
 ****************************************************************************/
static BOOL _Ch2Digital( IN char ch, IN BOOL bDigital, OUT DWORD * pdwDigital )
{
	if ( bDigital )
	{
		if ( ch >= '0' && ch <= '9' )
		{
			*pdwDigital = ch - '0';
            return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
        if ( ch >= '0' && ch <= '9' )
		{
			*pdwDigital = ch - '0';
            return TRUE;
		}
		else if ( ch >= 'a' && ch <= 'f' )
		{
			*pdwDigital = ch - 'a' + 10;
            return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

/****************************************************************************
 * ��������  _GetNumValue                                                   *
 * ��  �ܣ�  ����һ���������ڵ�λ�û�ȡ��ʾ��ֵ                             *
 * ��  ����  dwDigital,  ����                                               *
 *           dwPos    ,  λ�á����λ��ʮλ����λ�ȡ���Ŵ�0��ʼ            *
 *           dwMode   ,  ����( 10 / 16 )                                    *
 ****************************************************************************/
static DWORD _GetNumValue( IN DWORD dwDigital, IN DWORD dwPos, IN DWORD dwMode )
{
	DWORD i = 0;
	for ( i = 0; i < dwPos; i++ )
	{
		dwDigital *= dwMode;
	}
	return dwDigital;
}

 
/****************************************************************************
 * ��������  _GetBase64Index                                                *
 * ��  �ܣ�  ����base4�ַ���ȡ��ĸ���е�����                                *
 ****************************************************************************/
static BOOL _GetBase64Index( IN char chBase64, BYTE * pbyIndex )
{
    if ( chBase64 >= 'A' && chBase64 <= 'Z' )
    {
        *pbyIndex = chBase64 - 'A';
        return TRUE;
    }
    else if ( chBase64 >= 'a' && chBase64 <= 'z' )
    {
        *pbyIndex = chBase64 - 'a' + 26;
        return TRUE;
    }
    else if ( chBase64 >= '0' && chBase64 <= '9' )
    {
        *pbyIndex = chBase64 - '0' + 52;
        return TRUE;
    }
    else if ( chBase64 == '+' )
    {
        *pbyIndex = 62;
        return TRUE;
    }
    else if ( chBase64 == '/' )
    {
        *pbyIndex = 63;
        return TRUE;
    }
    else if ( chBase64 == '=' )
    {
        *pbyIndex = 64;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

















/***************************************************************************/
/*                      �ӿں���                                           */
/***************************************************************************/




/****************************************************************************
 * ��������  strtrim                                                        *
 * ��  �ܣ�  ��ȥ�ַ���ǰ��Ŀհ��ַ�                                       *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        �ü�����ַ���                                      *
 ****************************************************************************/
COMMONLMN_API char * StrTrim( INOUT char * szStr )
{
    // �������Ϊ0
    if ( 0 == szStr )
	{
		return 0;
	}

    // ����ַ�������
	DWORD dwLen = strlen( szStr );

    // ����ַ�������Ϊ0
    if ( 0 == dwLen )
    {
        return szStr;
    }

	DWORD dwFirstNbPos = 0;             // ��һ����Ϊ�հ��ַ���λ��
    DWORD dwLastNbPos  = 0;             // ���һ����Ϊ�հ��ַ���λ��

    // �������һ����Ϊ�հ��ַ���λ��
	for ( dwFirstNbPos = 0; dwFirstNbPos < dwLen; dwFirstNbPos++ )
	{
		if ( !_IsBlankChar( szStr[dwFirstNbPos] ) )
		{
			break;
		}
	}

    // ��������һ����Ϊ�հ��ַ���λ��
	for ( dwLastNbPos = dwLen - 1; dwLastNbPos >= dwFirstNbPos; dwLastNbPos-- )
	{
		if ( !_IsBlankChar( szStr[dwLastNbPos] ) )
		{
			break;
		}
	}

    // ����ǰ����õ��հ��ַ����Ӧ�еĳ���
	DWORD dwNewLen = dwLastNbPos - dwFirstNbPos + 1;

    // ��������ޱ仯(�������)
	if ( dwNewLen == dwLen )
	{
		return szStr;
	}

    // ������ַ����ĳ���Ϊ0
	if ( 0 == dwNewLen )
	{
		szStr[0] = '\0';
	}
    // ������ַ����ĳ��Ȳ�Ϊ0
	else
	{
        // �ƶ��ַ���
		memmove( szStr, szStr + dwFirstNbPos, dwNewLen );
		szStr[dwNewLen] = '\0';
	}

	return szStr;
}


/****************************************************************************
 * ��������  Str2Lower                                                      *
 * ��  �ܣ�  ת��ΪСд�ַ���                                               *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        ת������ַ���                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Lower( INOUT char * szStr )
{
    // �жϲ���
    if ( 0 == szStr )
    {
        return 0;
    }

    // ��ȡ�ַ�������
    DWORD dwLen = strlen( szStr );

    DWORD i;
    for ( i = 0; i < dwLen; i++ )
    {
        if ( szStr[i] >= 'A' && szStr[i] <= 'Z' )
        {
            szStr[i] = szStr[i] - 'A' + 'a';
        }
    }

    return szStr;
}

/****************************************************************************
 * ��������  Str2Upper                                                      *
 * ��  �ܣ�  ת��Ϊ��д�ַ���                                               *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        ת������ַ���                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Upper( INOUT char * szStr )
{
    // �жϲ���
    if ( 0 == szStr )
    {
        return 0;
    }

    // ��ȡ�ַ�������
    DWORD dwLen = strlen( szStr );

    DWORD i;
    for ( i = 0; i < dwLen; i++ )
    {
        if ( szStr[i] >= 'a' && szStr[i] <= 'z' )
        {
            szStr[i] = szStr[i] - 'a' + 'A';
        }
    }

    return szStr;
}

/****************************************************************************
 * ��������  StrReplace                                                     *
 * ��  �ܣ�  ���ַ���szReplaceWith�滻�ַ���szSource�е�����                *
 *           ΪszToReplace�ĵط�                                            *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API int StrReplace(  OUT char * szDest, IN DWORD dwSize, 
				               IN const char * szSource,  
                               IN const char * szToReplace, 
				               IN const char * szReplaceWith  )
{
    // ������
    if (    0 == szDest        || 0 == szSource || 0 == szToReplace
		 || 0 == szReplaceWith || 0 == dwSize )
	{
		return LMN_ERR_INVALID_ARGUMENT;
	}

    // ����ֱ�ָ��szDest��szSource��ָ�룬������д
    char *       pchDest       = szDest;
    const char * pchSrc        = szSource;

    // ����szDestʣ��ĳ���(��һ���ַ����ڽ�����)
	DWORD  dwLeftSize          = dwSize - 1;

    // ��������������ַ�������
	DWORD  dwToReplaceLen   = strlen( szToReplace );
    DWORD  dwRelpaceWithLen = strlen( szReplaceWith );

    // ���ҵ�һ�����滻��λ��
	const char * pchFind = strstr( pchSrc, szToReplace );
	while( pchFind )
	{
        // ��pchSrc��pchFind֮�����Ҫ�������ַ����ĳ���
		DWORD dwLen = pchFind - pchSrc;
        
        // �������������
		if ( dwLen > dwLeftSize )
		{
			memcpy( pchDest, pchSrc, dwLeftSize );
			pchDest[dwLeftSize] = '\0';
			return LMN_ERR_NOT_ENOUGH_BUFF;
		}
		else
		{
			memcpy( pchDest, pchSrc, dwLen );
			pchDest    += dwLen;
			dwLeftSize -= dwLen;

            // û���㹻������������szReplaceWith
			if ( dwRelpaceWithLen > dwLeftSize )
			{
				memcpy( pchDest, szReplaceWith, dwLeftSize );
				pchDest[dwLeftSize] = '\0';
				return LMN_ERR_NOT_ENOUGH_BUFF;
			}
			else
			{
				memcpy( pchDest, szReplaceWith, dwRelpaceWithLen );
				pchDest    += dwRelpaceWithLen;
				dwLeftSize -= dwRelpaceWithLen;

				pchSrc  = pchFind + dwToReplaceLen;
				pchFind = strstr( pchSrc, szToReplace );
			}
		}
	}

    // û���ҵ�szToReplaceʣ�µ����һ��
    DWORD  dwLastLen  =  strlen( pchSrc );
	if ( dwLastLen > dwLeftSize )
	{
		memcpy( pchDest, pchSrc, dwLeftSize );
		pchDest[dwLeftSize] = '\0';
		return LMN_ERR_NOT_ENOUGH_BUFF;
	}

	memcpy( pchDest, pchSrc, dwLastLen );
    pchDest[dwLastLen] = '\0';
	dwLeftSize -= dwLastLen;
    
    return LMN_ERR_OK;
}


/****************************************************************************
 * ��������  Str2Int                                                        *
 * ��  �ܣ�  ת���ַ���(10���ƻ�16����)Ϊ����(�з��ŵ�)                     *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 * ˵����    ת����Χ -2147483648 ~ 2147483647                              *
 *           ����16���� -0x80000000~0x7FFFFFFF                              *
 ****************************************************************************/
COMMONLMN_API int Str2Int( const char * szNum, int * pnNum )
{
    // ������
    if ( 0 == szNum || 0 == pnNum )
	{
		return LMN_ERR_INVALID_ARGUMENT;
	}

    // ����trim��Сдת����������Ҫ����һ���ڴ� 
	DWORD  dwLen   =  strlen( szNum );
    // char * pchBuf  =  new char[ dwLen + 1 ];
    char * pchBuf  =  (char *)g_pfnAllocMem( sizeof(char)*(dwLen + 1) );

    // ��������ڴ�ʧ��
    if ( 0 == pchBuf )
    {
        return LMN_ERR_NO_MEMORY;
    }

	strcpy( pchBuf, szNum );
	StrTrim( pchBuf );

	// תΪСд
	Str2Lower( pchBuf );

    // ���д����ĳ���
	dwLen = strlen( pchBuf );

    // �����0�����ַ���
	if ( 0 == dwLen )
	{
		SAFE_FREE( pchBuf );
		return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
	}


    // ����ָ�������ַ�����ָ��
	char * pchNum  = pchBuf;
    // �����Ƿ�����
	BOOL bPositive = TRUE;
    // �����Ƿ�10������
	BOOL bDigital  = TRUE;


    // �����һ����Ϊ'-'
	if ( '-' == pchNum[0] )
	{
		bPositive = FALSE;
		pchNum++;
        // ������������ָ�������
		dwLen--;
	}
    // �����һ����Ϊ'+'
	else if ( '+' == pchNum[0] )
	{
		bPositive = TRUE;
		pchNum++;
        dwLen--;
	}
    else
    {
        bPositive = TRUE;
    }

    // �ٴμ����Ч��
	if ( 0 == dwLen )
	{
		SAFE_FREE( pchBuf );
		return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
	}

	
	// ����Ƿ�16��������
	if ( dwLen >= 2 && '0' == pchNum[0] && 'x' == pchNum[1] )
	{
		bDigital = FALSE;
		pchNum   += 2;
		dwLen    -= 2;
	}
	else
	{
		bDigital = TRUE;
	}


    // ȷ�����ƺ��ٴμ�鳤��
    if ( 0 == dwLen )
    {
        SAFE_FREE( pchBuf );
		return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
    }

   
    // �����ȥ�����⣬���������ַ������ܴﵽ�����������
    // �����͸����������ֵ����ͬ������Ҫ��1
    DWORD dwMaxNum = (DWORD)(-1) / 2;
    if ( !bPositive )
    {
        dwMaxNum++;
    }

    // �������
    DWORD dwMode = bDigital ? 10 : 16;

    // ���������ֵ����λ����( 10���Ʒ�Χ��0 ~ 9, 16���Ʒ�Χ��0 ~ 15 )
    DWORD dwMaxNumFirstDigital = 0;
    DWORD dwMaxNumDigitalCnt   = 0;

    // ȡ�������ֵ���ֵ����λ���֣��Լ�λ��
    _GetNumberInfo( dwMaxNum, dwMode, &dwMaxNumFirstDigital, &dwMaxNumDigitalCnt );

    // ����ַ����ַ���������������ֵ����ָ���
    if ( dwLen > dwMaxNumDigitalCnt )
    {
        SAFE_FREE( pchBuf );
	    return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
    }


	DWORD   i = 0;
    // �����������ֵ�ֵ
	DWORD   dwNum = 0;

    // ���ÿ���ַ�
    // ����߼������ȣ�Ҫ���һ���ַ����ֲ��ܳ���������ֵ����λ���������
    //           ��ȥ���λ���ִ����ֵ�Ľ�����������������ÿ�����ִ����ֵ
    //           ���ܳ���ǰ�汣��Ľ��
    DWORD   dwLeftValue = dwMaxNum;

	for ( i = 0; i < dwLen; i++ )
	{
        DWORD  dwDigital = 0;

        // ���ת��ʧ�ܣ�������Ч����
		if ( !_Ch2Digital( pchNum[i], bDigital, &dwDigital ) )
        {
            SAFE_FREE( pchBuf );
			return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
        }

        // ����ǵ�һλ������һλ����
        if ( 0 == i )
        {
            if ( dwDigital > dwMaxNumFirstDigital )
            {
                SAFE_FREE( pchBuf );
		    	return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
            }
            dwNum       += _GetNumValue( dwDigital, dwLen - 1, dwMode );
            dwLeftValue -= dwNum;
        }
        else
        {
            DWORD dwValue = _GetNumValue( dwDigital, dwLen - i - 1, dwMode );
            if ( dwValue > dwLeftValue )
            {
                SAFE_FREE( pchBuf );
		    	return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
            }
            dwNum       += dwValue;
            dwLeftValue -= dwValue;
        }
	}

	if ( bPositive )
	{
		*pnNum = dwNum;
	}
	else
	{
		*pnNum = -(int)dwNum;
	}

    SAFE_FREE( pchBuf );
    return LMN_ERR_OK;
}


/****************************************************************************
 * ��������  EncodeBase64                                                   *
 * ��  �ܣ�  ���ֽ���ת����base64��ʽ                                       *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API int EncodeBase64( OUT   void  *  pBase64, 
                                INOUT DWORD *  pdwSize, 
                                IN    void  *  pSrc, 
                                IN    DWORD    dwSrcLen )
{
    // ������
    if ( 0 == pBase64 || 0 == pdwSize || 0 == pSrc )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    BYTE * pbyBase64    = (BYTE *)pBase64;
    const BYTE * pbySrc = (BYTE*)pSrc;

    // ���㱻3����ı����Լ�����
    DWORD dwMultiple  = dwSrcLen / 3;
    DWORD dwResidue   = dwSrcLen % 3;

    // ������С��Ҫ�Ļ����С
    DWORD dwMinSize = 4 * dwMultiple;
    if ( 0 != dwResidue )
    {
        dwMinSize += 4;
    }

    // ������岻����
    if ( *pdwSize < dwMinSize )
    {
        return LMN_ERR_NOT_ENOUGH_BUFF;
    }

    // Base64����
    // �߼�����3���ֽ�Ϊһ�Σ�������ÿ�Ρ��������1��2���ֽڣ��Ǿ����ر���
    DWORD i;
    for ( i = 0; i < dwMultiple; i++ )
    {
        const BYTE * pbySegment = pbySrc + 3 * i;
    
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 =   ( GET_BACK_BITS( pbySegment[0], 2 ) << 4 ) 
                  | GET_FRONT_BITS( pbySegment[1], 4 );
        BYTE b3 = ( GET_BACK_BITS(pbySegment[1],4)<<2 ) 
                  | GET_FRONT_BITS(pbySegment[2], 2);
        BYTE b4 = GET_BACK_BITS( pbySegment[2], 6 );

        BYTE * pbyDest = pbyBase64 + 4 * i;

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[b3];
        pbyDest[3] = s_base64_alphabet[b4];
    }

    const BYTE * pbySegment = pbySrc + 3 * i;
    BYTE *       pbyDest    = pbyBase64 + 4 * i;

    // �����1���ֽ�
    if ( 1 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( pbySegment[0], 2 )<<4 );

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[64];
        pbyDest[3] = s_base64_alphabet[64];
    }
    // �����2���ֽ�
    else if ( 2 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( pbySegment[0], 2 )<<4) | GET_FRONT_BITS(pbySegment[1], 4);
        BYTE b3 = GET_BACK_BITS(pbySegment[1],4)<<2;

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[b3];
        pbyDest[3] = s_base64_alphabet[64];
    }

    *pdwSize = dwMinSize;
    return LMN_ERR_OK;
}


/****************************************************************************
 * ��������  DecodeBase64                                                   *
 * ��  �ܣ�  ��base64��ʽת�����ֽ���                                       *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API int DecodeBase64( OUT   void  * pDest, 
                                INOUT DWORD * pdwSize, 
                                IN    void  * pBase64, 
                                IN    DWORD   dwBase64Len  )
{
    // ������
    if ( 0 == pDest || 0 == pdwSize || 0 == pBase64 )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    BYTE * pbyDest = (BYTE *)pDest;
    const BYTE * pbyBase64 = (BYTE *)pBase64;

    // base64���ݸ�ʽ����
    if ( 0 != dwBase64Len % 4 )
    {
        return LMN_ERR_BASE64_LENGTH_INVALID;
    }

    // ��4���ֽ�Ϊһ�ν��зֶ�
    DWORD dwMultiple =  dwBase64Len / 4;

    // ��黺�����Ƿ��㹻
    DWORD dwMinSize = 0;
    const BYTE * pbyLastSegment = 0;

    if ( dwMultiple > 0 )
    {
        dwMinSize = (dwMultiple - 1) * 3;
        pbyLastSegment = pbyBase64 + 4 * (dwMultiple - 1);
        // ������һ�ε�����2���ֽ�Ϊ'='������Ҫ1���ֽڻ�����
        if ( s_base64_alphabet[64] == pbyLastSegment[2] )
        {
            dwMinSize += 1;
        }
        // ������һ�ε�����12���ֽ�Ϊ'='������Ҫ2���ֽڻ�����
        else if ( s_base64_alphabet[64] == pbyLastSegment[3] )
        {
            dwMinSize += 2;
        }
        else
        {
            dwMinSize += 3;
        }
    }

    if ( *pdwSize < dwMinSize )
    {
        return LMN_ERR_NOT_ENOUGH_BUFF;
    }
    
    
    DWORD i;
    // ����ÿһ��
    for ( i = 0; i < dwMultiple; i++ )
    {
        BYTE * pbySegment   = pbyDest   + 3 * i;
        const BYTE * pbySrc = pbyBase64 + 4 * i;

        BYTE  abyIndex[4];
        DWORD j;
        for ( j = 0; j < 4; j++ )
        {
            // ��ȡ����ʧ��
            if ( !_GetBase64Index( pbySrc[j], &abyIndex[j] ) )
            {
                return LMN_ERR_WRONG_BASE64_FORMAT;
            }
        }

        if ( 64 == abyIndex[0] || 64 == abyIndex[1] )
        {
            return LMN_ERR_WRONG_BASE64_FORMAT;
        }

        // ����������һ��
        if ( i < dwMultiple - 1 )
        {
            if ( 64 == abyIndex[2] || 64 == abyIndex[3] )
            {
                return LMN_ERR_WRONG_BASE64_FORMAT;
            }

            pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
            pbySegment[1] = (GET_BACK_BITS(abyIndex[1],4)<<4) | GET_FRONT_BITS(abyIndex[2],6);
            pbySegment[2] = (GET_BACK_BITS(abyIndex[2],2)<<6) | abyIndex[3];
        }
        else
        {
            if ( 64 == abyIndex[2] )
            {
                if ( 64 != abyIndex[3] )
                {
                    return LMN_ERR_WRONG_BASE64_FORMAT;
                }
                
                pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
            }
            else if ( 64 == abyIndex[3] )
            {   
                pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
                pbySegment[1] = (GET_BACK_BITS(abyIndex[1],4)<<4) | GET_FRONT_BITS(abyIndex[2],6);
            }
            else
            {
                pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
                pbySegment[1] = (GET_BACK_BITS(abyIndex[1],4)<<4) | GET_FRONT_BITS(abyIndex[2],6);
                pbySegment[2] = (GET_BACK_BITS(abyIndex[2],2)<<6) | abyIndex[3];
            }
        }
    }

    *pdwSize = dwMinSize;
    return LMN_ERR_OK;
}






COMMONLMN_API char  Char2Lower( IN   char ch )
{
    if ( ch >= 'A' && ch <= 'Z')
    {
        return 'a' + (ch - 'A');
    }
    else
    {
        return ch;
    }
}

COMMONLMN_API char  Char2Upper( IN   char ch )
{
    if ( ch >= 'a' && ch <= 'z')
    {
        return 'A' + (ch - 'a');
    }
    else
    {
        return ch;
    }
}

COMMONLMN_API  int  StrICmp( IN const char * s1, IN const char * s2 )
{
    if ( 0 == s1 )
    {
        return -1;
    }
    else if ( 0 == s2 )
    {
        return 1;
    }

    while( Char2Lower(*s1) == Char2Lower(*s2) && '\0' != *s1 )
    {
        s1++;
        s2++;
    }

    if ( Char2Lower(*s1) < Char2Lower(*s2) )
    {
        return -1;
    }
    else if ( Char2Lower(*s1) > Char2Lower(*s2) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


COMMONLMN_API  int  StrNiCmp( IN const char * s1, IN const char * s2, IN DWORD dwCnt )
{
    unsigned long  dwCmpCnt = 0;

    if ( 0 == s1 )
    {
        return -1;
    }
    else if ( 0 == s2 )
    {
        return 1;
    }

    while( dwCmpCnt < dwCnt && Char2Lower(*s1) == Char2Lower(*s2) && '\0' != *s1 )
    {
        s1++;
        s2++;
        dwCmpCnt++;
    }

    if ( dwCmpCnt == dwCnt )
    {
        return 0;
    }

    if ( Char2Lower(*s1) < Char2Lower(*s2) )
    {
        return -1;
    }
    else if ( Char2Lower(*s1) > Char2Lower(*s2) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



#ifdef __cplusplus
} // extern "C" { 
#endif








