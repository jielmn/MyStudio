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
/*                      内部(静态)函数                                     */
/***************************************************************************/



    

/****************************************************************************
 * 函数名：  _IsBlankChar                                                   *
 * 功  能：  判断字符是否空白字符( SPACE, TAB, \r, \n )                     *
 * 返回值：  TRUE       是空白字符                                          *
 *           FALSE      非空白字符                                          *
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
 * 函数名：  _GetNumberInfo                                                 *
 * 功  能：  获得最高位上的数字，以及位数                                   *
 * 参数  ：  dwNum      数字                                                *
 *           dwMode     进制                                                *
 *           pdwFirstDigital  最高位数字                                    *
 *           pdwDigitalCount  全部数字个数                                  *
 * 说明  ：  例如,    123      最大位数字为 1，  有3位数字                  *
 *                    0xa23    最大位数字为 10， 有3位数字                  *
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
 * 函数名：  _Ch2Num                                                        *
 * 功  能：  把字符转换成数字(已经确认字符都为小写)                         *
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
 * 函数名：  _GetNumValue                                                   *
 * 功  能：  根据一个数字所在的位置获取表示的值                             *
 * 参  数：  dwDigital,  数字                                               *
 *           dwPos    ,  位置。如个位，十位，百位等。序号从0开始            *
 *           dwMode   ,  进制( 10 / 16 )                                    *
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
 * 函数名：  _GetBase64Index                                                *
 * 功  能：  根据base4字符获取字母表中的索引                                *
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
/*                      接口函数                                           */
/***************************************************************************/




/****************************************************************************
 * 函数名：  strtrim                                                        *
 * 功  能：  除去字符串前后的空白字符                                       *
 * 返回值：  0          失败                                                *
 *           非0        裁剪后的字符串                                      *
 ****************************************************************************/
COMMONLMN_API char * StrTrim( INOUT char * szStr )
{
    // 如果参数为0
    if ( 0 == szStr )
	{
		return 0;
	}

    // 求得字符串长度
	DWORD dwLen = strlen( szStr );

    // 如果字符串长度为0
    if ( 0 == dwLen )
    {
        return szStr;
    }

	DWORD dwFirstNbPos = 0;             // 第一个不为空白字符的位置
    DWORD dwLastNbPos  = 0;             // 最后一个不为空白字符的位置

    // 计算出第一个不为空白字符的位置
	for ( dwFirstNbPos = 0; dwFirstNbPos < dwLen; dwFirstNbPos++ )
	{
		if ( !_IsBlankChar( szStr[dwFirstNbPos] ) )
		{
			break;
		}
	}

    // 计算出最后一个不为空白字符的位置
	for ( dwLastNbPos = dwLen - 1; dwLastNbPos >= dwFirstNbPos; dwLastNbPos-- )
	{
		if ( !_IsBlankChar( szStr[dwLastNbPos] ) )
		{
			break;
		}
	}

    // 计算前后剪裁掉空白字符后的应有的长度
	DWORD dwNewLen = dwLastNbPos - dwFirstNbPos + 1;

    // 如果长度无变化(无需剪裁)
	if ( dwNewLen == dwLen )
	{
		return szStr;
	}

    // 如果新字符串的长度为0
	if ( 0 == dwNewLen )
	{
		szStr[0] = '\0';
	}
    // 如果新字符串的长度不为0
	else
	{
        // 移动字符串
		memmove( szStr, szStr + dwFirstNbPos, dwNewLen );
		szStr[dwNewLen] = '\0';
	}

	return szStr;
}


/****************************************************************************
 * 函数名：  Str2Lower                                                      *
 * 功  能：  转化为小写字符串                                               *
 * 返回值：  0          失败                                                *
 *           非0        转化后的字符串                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Lower( INOUT char * szStr )
{
    // 判断参数
    if ( 0 == szStr )
    {
        return 0;
    }

    // 获取字符串长度
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
 * 函数名：  Str2Upper                                                      *
 * 功  能：  转化为大写字符串                                               *
 * 返回值：  0          失败                                                *
 *           非0        转化后的字符串                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Upper( INOUT char * szStr )
{
    // 判断参数
    if ( 0 == szStr )
    {
        return 0;
    }

    // 获取字符串长度
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
 * 函数名：  StrReplace                                                     *
 * 功  能：  用字符串szReplaceWith替换字符串szSource中的所有                *
 *           为szToReplace的地方                                            *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
COMMONLMN_API int StrReplace(  OUT char * szDest, IN DWORD dwSize, 
				               IN const char * szSource,  
                               IN const char * szToReplace, 
				               IN const char * szReplaceWith  )
{
    // 检查参数
    if (    0 == szDest        || 0 == szSource || 0 == szToReplace
		 || 0 == szReplaceWith || 0 == dwSize )
	{
		return LMN_ERR_INVALID_ARGUMENT;
	}

    // 定义分别指向szDest和szSource的指针，便于书写
    char *       pchDest       = szDest;
    const char * pchSrc        = szSource;

    // 定义szDest剩余的长度(留一个字符用于结束符)
	DWORD  dwLeftSize          = dwSize - 1;

    // 求出被替代和替代字符串长度
	DWORD  dwToReplaceLen   = strlen( szToReplace );
    DWORD  dwRelpaceWithLen = strlen( szReplaceWith );

    // 查找第一个被替换的位置
	const char * pchFind = strstr( pchSrc, szToReplace );
	while( pchFind )
	{
        // 从pchSrc到pchFind之间的需要保留的字符串的长度
		DWORD dwLen = pchFind - pchSrc;
        
        // 如果缓冲区不够
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

            // 没有足够缓冲区来复制szReplaceWith
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

    // 没有找到szToReplace剩下的最后一段
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
 * 函数名：  Str2Int                                                        *
 * 功  能：  转换字符串(10进制或16进制)为数字(有符号的)                     *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 * 说明：    转换范围 -2147483648 ~ 2147483647                              *
 *           或者16进制 -0x80000000~0x7FFFFFFF                              *
 ****************************************************************************/
COMMONLMN_API int Str2Int( const char * szNum, int * pnNum )
{
    // 检查参数
    if ( 0 == szNum || 0 == pnNum )
	{
		return LMN_ERR_INVALID_ARGUMENT;
	}

    // 进行trim和小写转换，所以需要复制一份内存 
	DWORD  dwLen   =  strlen( szNum );
    // char * pchBuf  =  new char[ dwLen + 1 ];
    char * pchBuf  =  (char *)g_pfnAllocMem( sizeof(char)*(dwLen + 1) );

    // 如果分配内存失败
    if ( 0 == pchBuf )
    {
        return LMN_ERR_NO_MEMORY;
    }

	strcpy( pchBuf, szNum );
	StrTrim( pchBuf );

	// 转为小写
	Str2Lower( pchBuf );

    // 进行处理后的长度
	dwLen = strlen( pchBuf );

    // 如果是0长度字符串
	if ( 0 == dwLen )
	{
		SAFE_FREE( pchBuf );
		return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
	}


    // 定义指向数字字符串的指针
	char * pchNum  = pchBuf;
    // 定义是否正数
	BOOL bPositive = TRUE;
    // 定义是否10进制数
	BOOL bDigital  = TRUE;


    // 如果第一符号为'-'
	if ( '-' == pchNum[0] )
	{
		bPositive = FALSE;
		pchNum++;
        // 除符号外的数字个数减少
		dwLen--;
	}
    // 如果第一符号为'+'
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

    // 再次检查有效性
	if ( 0 == dwLen )
	{
		SAFE_FREE( pchBuf );
		return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
	}

	
	// 检查是否16进制数字
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


    // 确定进制后，再次检查长度
    if ( 0 == dwLen )
    {
        SAFE_FREE( pchBuf );
		return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
    }

   
    // 定义除去符号外，待解析的字符串所能达到的最大正整数
    // 正数和负数的最大面值数不同。负数要大1
    DWORD dwMaxNum = (DWORD)(-1) / 2;
    if ( !bPositive )
    {
        dwMaxNum++;
    }

    // 定义进制
    DWORD dwMode = bDigital ? 10 : 16;

    // 定义最大面值的首位数字( 10进制范围：0 ~ 9, 16进制范围：0 ~ 15 )
    DWORD dwMaxNumFirstDigital = 0;
    DWORD dwMaxNumDigitalCnt   = 0;

    // 取得最大面值数字的最高位数字，以及位数
    _GetNumberInfo( dwMaxNum, dwMode, &dwMaxNumFirstDigital, &dwMaxNumDigitalCnt );

    // 如果字符串字符个数超过最大数字的数字个数
    if ( dwLen > dwMaxNumDigitalCnt )
    {
        SAFE_FREE( pchBuf );
	    return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
    }


	DWORD   i = 0;
    // 定义正数部分的值
	DWORD   dwNum = 0;

    // 检查每个字符
    // 检查逻辑：首先，要求第一个字符数字不能超过最大数字的最高位。最大数字
    //           减去最高位数字代表的值的结果保存起来。后面的每个数字代表的值
    //           不能超过前面保存的结果
    DWORD   dwLeftValue = dwMaxNum;

	for ( i = 0; i < dwLen; i++ )
	{
        DWORD  dwDigital = 0;

        // 如果转换失败，不是有效数字
		if ( !_Ch2Digital( pchNum[i], bDigital, &dwDigital ) )
        {
            SAFE_FREE( pchBuf );
			return LMN_ERR_WRONG_FORMAT_FOR_NUMBER;
        }

        // 如果是第一位，检查第一位数字
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
 * 函数名：  EncodeBase64                                                   *
 * 功  能：  把字节流转换成base64格式                                       *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
COMMONLMN_API int EncodeBase64( OUT   void  *  pBase64, 
                                INOUT DWORD *  pdwSize, 
                                IN    void  *  pSrc, 
                                IN    DWORD    dwSrcLen )
{
    // 检查参数
    if ( 0 == pBase64 || 0 == pdwSize || 0 == pSrc )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    BYTE * pbyBase64    = (BYTE *)pBase64;
    const BYTE * pbySrc = (BYTE*)pSrc;

    // 计算被3除后的倍数以及余数
    DWORD dwMultiple  = dwSrcLen / 3;
    DWORD dwResidue   = dwSrcLen % 3;

    // 计算最小需要的缓冲大小
    DWORD dwMinSize = 4 * dwMultiple;
    if ( 0 != dwResidue )
    {
        dwMinSize += 4;
    }

    // 如果缓冲不够大
    if ( *pdwSize < dwMinSize )
    {
        return LMN_ERR_NOT_ENOUGH_BUFF;
    }

    // Base64编码
    // 逻辑：以3个字节为一段，逐个检查每段。如果余下1到2个字节，那就做特别处理
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

    // 如果余1个字节
    if ( 1 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( pbySegment[0], 2 )<<4 );

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[64];
        pbyDest[3] = s_base64_alphabet[64];
    }
    // 如果余2个字节
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
 * 函数名：  DecodeBase64                                                   *
 * 功  能：  把base64格式转换成字节流                                       *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
COMMONLMN_API int DecodeBase64( OUT   void  * pDest, 
                                INOUT DWORD * pdwSize, 
                                IN    void  * pBase64, 
                                IN    DWORD   dwBase64Len  )
{
    // 检查参数
    if ( 0 == pDest || 0 == pdwSize || 0 == pBase64 )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    BYTE * pbyDest = (BYTE *)pDest;
    const BYTE * pbyBase64 = (BYTE *)pBase64;

    // base64数据格式不对
    if ( 0 != dwBase64Len % 4 )
    {
        return LMN_ERR_BASE64_LENGTH_INVALID;
    }

    // 按4个字节为一段进行分段
    DWORD dwMultiple =  dwBase64Len / 4;

    // 检查缓冲区是否足够
    DWORD dwMinSize = 0;
    const BYTE * pbyLastSegment = 0;

    if ( dwMultiple > 0 )
    {
        dwMinSize = (dwMultiple - 1) * 3;
        pbyLastSegment = pbyBase64 + 4 * (dwMultiple - 1);
        // 如果最后一段倒数第2个字节为'='，还需要1个字节缓冲区
        if ( s_base64_alphabet[64] == pbyLastSegment[2] )
        {
            dwMinSize += 1;
        }
        // 如果最后一段倒数第12个字节为'='，还需要2个字节缓冲区
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
    // 遍历每一段
    for ( i = 0; i < dwMultiple; i++ )
    {
        BYTE * pbySegment   = pbyDest   + 3 * i;
        const BYTE * pbySrc = pbyBase64 + 4 * i;

        BYTE  abyIndex[4];
        DWORD j;
        for ( j = 0; j < 4; j++ )
        {
            // 获取索引失败
            if ( !_GetBase64Index( pbySrc[j], &abyIndex[j] ) )
            {
                return LMN_ERR_WRONG_BASE64_FORMAT;
            }
        }

        if ( 64 == abyIndex[0] || 64 == abyIndex[1] )
        {
            return LMN_ERR_WRONG_BASE64_FORMAT;
        }

        // 如果不是最后一段
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








