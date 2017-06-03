
#ifndef _ENCODING_MANAGER_H
#define _ENCODING_MANAGER_H

#include <fonts_manager.h>
#include <disp_manager.h>

typedef struct EncodingOpr {
	char *name;    /* 编码模块的名字 */
	int iHeadLen;  /* 文件头的长度: 一般在文件的开始用几个字节来表示它的编码方式 */
	PT_FontOpr ptFontOprSupportedHead;  /* 把能支持这种编码的"字体模块", 放在这个链表里 */
	int (*isSupport)(unsigned char *pucBufHead);  /* 用这个函数来判断是否支持某个文件 */
	int (*GetCodeFrmBuf)(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);  /* 取出一个字符的编码值 */
	struct EncodingOpr *ptNext;  /* 链表 */
}T_EncodingOpr, *PT_EncodingOpr;


/**********************************************************************
 * 函数名称： RegisterEncodingOpr
 * 功能描述： 注册"编码模块", 一行文字可能采用ASCII编码,也可能用UTF8等编码.
 *            怎么从一行文字中取出一个字,不同的编码取法不一样,
 *            每一种编码方式都调用RegisterEncodingOpr把相关结构体放入g_ptEncodingOprHead链表
 * 输入参数： ptEncodingOpr - 一个结构体,内含某种编码方式的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int RegisterEncodingOpr(PT_EncodingOpr ptEncodingOpr);

/**********************************************************************
 * 函数名称： ShowEncodingOpr
 * 功能描述： 显示本程序能支持的"编码方式"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ShowEncodingOpr(void);

/**********************************************************************
 * 函数名称： GetDispOpr
 * 功能描述： 根据名字取出指定的"显示模块"
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 显示模块的PT_DispOpr结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_DispOpr GetDispOpr(char *pcName);

/**********************************************************************
 * 函数名称： SelectEncodingOprForFile
 * 功能描述： 给文件选择一个编码模块
 * 输入参数： pucFileBufHead - 文件头部, 即调用mmap映射文件所得到内存指针
 * 输出参数： 无
 * 返 回 值： NULL   - 现有的编码方式都不支持此文件
 *            非NULL - 支持此文件的编码方式对应的结构体指针
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PT_EncodingOpr SelectEncodingOprForFile(unsigned char *pucFileBufHead);

/**********************************************************************
 * 函数名称： AddFontOprForEncoding
 * 功能描述： 要显示一个字符, 涉及三个概念:
 *            1. 字符的编码值, 即这个文件用什么数值来表示, 比如'A'用0x41来表示,
 *               '中'的编码方式有多种, GBK编码方式里它的数值为0xD0D6, 而在UNICODE
 *               编码里它的数值为0x4E2D
 *            2. 编码值的表示方式, 比如UTF8/UTF16LE/UTF16BE是UNICODE编码的三种表示方式,例如:
 *               'A'的ASCII表示为0x41;UTF16-LE表示为0x41,0x00;UTF16-BE表示为0x00,0x41;UTF8表示为0x41
 *               '中'的GBK表示为0xD6,0xD0;UTF16-LE表示为0x2D,0x4E;UTF16-BE表示为0x4E,0x2D;UTF8表示为0xE4,0xB8,0xAD
 *               注意:UTF8/UTF16LE/UTF16BE只是UNICODE编码的不同表示方式,对于同一个字符,它们表达的编码值是一样的,只是表示方法不同
 *            3. 字符的位图,即字符应该怎么描画
 *            举例: 使用freetype矢量字库来显示UTF8编码格式的文件内容时,
 *                  我们要先取出UTF8的值用来解析出UNICODE值,然后根据UNICODE值去字库文件中取出位图,
 *                  最后在LCD上显示出来
 *            一个文本文件的编码方式是确定的,
 *        比如它要么是ANSI(字母用ASCII,汉字用GBK),要么是UTF8等等; 文件里字符的位图来源可以有多种,
 *        比如ASCII字符可以从确定的数组中获得它的位图, GBK字符可以从"HZK16"这个文件中获得它的位图,
 *        也可以从某个freetype矢量字库中获得它的位图.
 *           AddFontOprForEncoding函数就是给某个编码方式(ptEncodingOpr)提供字符的处理方法(ptFontOpr),
 *        ptFontOpr将被放入"ptEncodingOpr->ptFontOprSupportedHead"链表中
 * 输入参数： ptEncodingOpr - 编码方式
 *            ptFontOpr     - 字符处理结构体,内含"GetFontBitmap"函数(即根据编码值获得位图)
 * 输出参数： 无
 * 返 回 值： 0   - 成功
 *            -1  - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);

/**********************************************************************
 * 函数名称： DelFontOprFrmEncoding
 * 功能描述： AddFontOprForEncoding的反函数,从PT_EncodingOpr->ptFontOprSupportedHead链表中把某个PT_FontOpr去掉
 * 输入参数： ptEncodingOpr - 编码方式
 *            ptFontOpr     - 字符处理结构体,内含"GetFontBitmap"函数(即根据编码值获得位图)
 * 输出参数： 无
 * 返 回 值： 0  - 成功
 *            -1 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int DelFontOprFrmEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);

/**********************************************************************
 * 函数名称： EncodingInit
 * 功能描述： 调用各种编码方式的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int EncodingInit(void);

/**********************************************************************
 * 函数名称： AsciiEncodingInit
 * 功能描述： 根据字符的ANSI编码值,有freetype/ascii/gbk这三种方法获得它的字符位图
 *            AsciiEncodingInit先给g_tAsciiEncodingOpr添加freetype/ascii/gbk这三种PT_FontOpr结构体;
 *            最后注册g_tAsciiEncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int AsciiEncodingInit(void);

/**********************************************************************
 * 函数名称： Utf16beEncodingInit
 * 功能描述： 根据字符的UNICODE编码值,有freetype/ascii这2种方法获得它的字符位图
 *            Utf16beEncodingInit先给g_tUtf16beEncodingOpr添加freetype/ascii这2种PT_FontOpr结构体;
 *            最后注册g_tUtf16beEncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  Utf16beEncodingInit(void);

/**********************************************************************
 * 函数名称： Utf16leEncodingInit
 * 功能描述： 根据字符的UNICODE编码值,有freetype/ascii这2种方法获得它的字符位图
 *            Utf16leEncodingInit先给g_tUtf16leEncodingOpr添加freetype/ascii这2种PT_FontOpr结构体;
 *            最后注册g_tUtf16leEncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  Utf16leEncodingInit(void);

/**********************************************************************
 * 函数名称： Utf8EncodingInit
 * 功能描述： 根据字符的UNICODE编码值,有freetype/ascii这2种方法获得它的字符位图
 *            Utf8EncodingInit先给g_tUtf8EncodingOpr添加freetype/ascii这2种PT_FontOpr结构体;
 *            最后注册g_tUtf8EncodingOpr
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0      - 成功
 *            其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int  Utf8EncodingInit(void);

/**********************************************************************
 * 函数名称： GetCodeFrmBuf
 * 功能描述： 从字符串中取出编码值,这是一个简单的版本, 只能处理ASCII/GBK字符串
 * 输入参数： pucBufStart - 字符串起始位置
 *            pucBufEnd   - 字符串结束位置(这个位置的字符不处理)
 * 输出参数： pdwCode     - 所取出的编码值存在这里
 * 返 回 值： 0 - 字符串处理完毕,没有获得编码值
 *            1 - 得到一个ASCII码
 *            2 - 得过一个GBK码
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
int GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

#endif /* _ENCODING_MANAGER_H */

