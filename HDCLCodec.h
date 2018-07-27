/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
+                           HDCLCodec.h                            +
+     --------------------------------------------------------     +
+                                                                  +
+摘要: HDCL编解码类的声明                                          +
+作者: Sharper Huang                                               +
+创建: 2011-7-6 14:01:30                                           +
+  (C) Hangzhou Synway software., ltd                              +
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef __HDCLCODEC_H__
#define __HDCLCODEC_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  DEF_FRAME_BUFFER 256
#define  MAX_FRAME_BUFFER 4096

#define  HDCL_FRAME_DATA   0
#define  HDCL_FRAME_DONE   1
#define  HDCL_FRAME_ABORT  2

typedef void (__stdcall *ONFRAMEDECODED)(int nType,const char* szBuffer,int nLen,void* pUser);
typedef void (__stdcall *ONFRAMEENCODED)(int nType,const char* szBuffer,int nLen,void* pUser);

//-------------------------------------------------------------
//解码类
//-------------------------------------------------------------
class CHDCLDecode  
{
public:
	CHDCLDecode();
	virtual ~CHDCLDecode();
public:
	void Reset();
	void StartDecode(ONFRAMEDECODED pfCB,unsigned short wBufferSize,void* pUserData);
 	int  DecodeBuffer(const char* pData,int nSize);
private:
	inline void OnFrameFlag();
	inline void OnFrameAbort();
    inline void AddBit(unsigned char ucBit);
    inline int  OnByteReady(unsigned char ucByte);
private:
    unsigned int   m_nFrameBytes;
    unsigned int   m_nBlockSize;
    unsigned char* m_pszBlockBuf;
    unsigned char  m_ucLastByte;
    int            m_nLastByteBits;
    int            m_nHighLevelBits;
    bool           m_bFlagByte;
    ONFRAMEDECODED m_pfCB; 
    void*  m_pUserData;

};

//-------------------------------------------------------------
//编码类
//-------------------------------------------------------------
class CHDCLEncode  
{
public:
	CHDCLEncode(unsigned int nBaudRate=19200);
	virtual ~CHDCLEncode();
public:
	int EncodeBuffer(int nType,const char* szBuffer,unsigned int dwLen);
	int EncodeIdlePack(unsigned int dwMillsecs);
   void SetPlaybackBaudRate(unsigned int nBaudRate);
   void StartEncode(ONFRAMEENCODED   pfCB, 
                                unsigned short wBufferSize, 
                                void*          pUserData);
private:
    unsigned int     m_nBytesPerSec;
    ONFRAMEENCODED   m_pfCB;
    void*            m_pUserData;
    unsigned   char  m_ucFlagByte;
    bool             m_bFlagByteInserted;
    unsigned int     m_nBlockSize;
    unsigned char*   m_pszBlockBuf;
};

#endif //__HDCLCODEC_H__