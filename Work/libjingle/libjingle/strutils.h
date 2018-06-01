
#ifndef __STRUTILS_H
#define __STRUTILS_H

typedef char s8;
typedef short int s16;
typedef int s32;
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;

typedef u16 UINT16;
typedef u8  BYTE;
typedef s32 INT32;

#define rv_ntohs(n) (n)
#define rv_htons(n) (n)

/* multi thread safe strtok version */
s8 * strtok_mts(s8 *s, const s8 *delim, s8 **lasts);

/* ascii->bmp string - returns bytes written to target buffer */
s32 chr2bmp(s8 *str, u8 *bmpStr);

/* same as chr2bmp, with maximum length */
s32 chrn2bmp(s8 *str, s32 maxStrLen, u8 *bmpStr);

/* bmp->ascii string - returns RVERROR if conversion was unsuccessful */
s32 bmp2chr(s8 *str, u8 *bmpStr, s32 bmpLen);

/* case insensitive strcmp */
s32 strcmp_ci(const s8 *dst, const s8 *src);

/* case insensitive strncmp */
s32 strncmp_ci(const s8 *dst, const s8 *src, u32 count);

s8* IpToString(u32 ipAddr, s8* buf);

u32 StringToIp(s8* ipAddr);



//GB2312字符转换成Unicode字符，返回该GB2312字符长度
//如果字符表中不存在，Unicode值为0
s32 Gb2Unicode(s8 achGbChar[], u16* pwUnicodeChar);

//Unicode字符转换成GB2312，返回GB2312的长度
//字符表中不存在，字符为0
s32 Unicode2Gb(u16 wUnicodeChar, s8 achGbcodeChar[]);

//把一个GB2312的字符串转换成Unicode的字符串，返回转换后字符串的长度
s32 StrGb2Unicode(char abyGbStr[], s32 nGbStrLen, u16 pwUnicodeBuff[], s32 nBuffLen);

//把一个Unicode的字符串转换成的GB2312字符串，返回转换后字符串的长度
s32 StrUnicode2Gb(u16 awUnicodeStr[], s32 nUniCharNum, char abyGbBuff[], s32 nBuffLen);


int UTF2Uni(const char* src, UINT16 *des);
int Uni2UTF(UINT16 wchar, char *utf8);

/**************************************/

#endif  /* __STRUTILS_H */




