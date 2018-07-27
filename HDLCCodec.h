/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
+                           HDLCCodec.h                            +
+     --------------------------------------------------------     +
+                                                                  +
+摘要: HDLC(High level Data Link Control的声明                     +
+作者: Sharper Huang                                               +
+创建: 2011-7-14 0:31:51                                           +
+  (C) Hangzhou Synway software., ltd                              +
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef __HDCLCODEC_H__
#define __HDCLCODEC_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  MIN_FRAME_BUFFER 16
#define  DEF_FRAME_BUFFER 256
#define  MAX_FRAME_BUFFER 4096

//#define  HDLC_FRAME_END    0 //文件结束
#define  HDLC_FRAME_DATA   1 //数据帧
#define  HDLC_FRAME_ABORT  2 //异常帧
#define  HDLC_FRAME_DONE   3 

#define  HDLC_FRAME_MAXBYTES 2048

extern unsigned char C_BYTEINVERSE_MAP[256];      //BIG-ENDIAN <==>LITTLE-ENDIAN

unsigned short int ComputeFCS (const unsigned char *szData, int nLen);
bool   CheckFCS (const unsigned char *szData, int nLen);
//-------------------------------------------------------------
//解码类
//-------------------------------------------------------------
//回调函数，当一帧数据解码完成后，调用此函数
typedef void (__stdcall *ONFRAMEDECODED)(int nType,const char* szFrame,int nLen,void* pUser);
class CHDLCDecode  
{
public:
	CHDLCDecode();
	virtual ~CHDLCDecode();
public:
	void StartDecode(ONFRAMEDECODED pfCB,unsigned short wBufferSize,void* pUserData);
 	int  DecodeBuffer(const char* pData,int nSize);
private:
	inline void OnFrameFlagByte(int nOffset);
	inline void OnFrameAbort();
    inline void AddBit(unsigned char ucBit);
    inline int  OnByteDecoded(unsigned char ucByte);
	void        ResetDecode(unsigned int dwBufSize=HDLC_FRAME_MAXBYTES);
private:
    unsigned int   m_nDecodedBytes;
    unsigned int   m_nBlockSize;
    unsigned char* m_pszDecodedBuf;
    unsigned char  m_ucDecodingByte;
    unsigned char  m_nHighLevelBits;
    bool           m_bFlagByte;
    unsigned int   m_nOffsetInStream;
    unsigned char  m_nDecodedBits;
    ONFRAMEDECODED m_pfCB; 
    void*  m_pUserData;

};

//-------------------------------------------------------------
//编码类
//-------------------------------------------------------------
typedef void (__stdcall *ONFRAMEENCODED)(int nType,const char* szBuffer,int nLen,void* pUser);
class CHDLCEncode  
{
public:
	CHDLCEncode();
	virtual ~CHDLCEncode();
//public:
//    void __stdcall CBBlockEncoded(int nType,const char* szBuffer,int nLen,void* pUser);
public:
	unsigned int EncodeFrame( const char *szRaw,
                                       unsigned int dwRawSize,
                                       char* szEncodedBuffer,
                                       unsigned int dwBufferSize);

	unsigned char GetFlagByte() const { return m_ucFlagByte;}
	unsigned char GetFlagByteInverse() const ;
	int EncodeFrameStream(int nType,const char* szBuffer,unsigned int dwLen);
    void SetPlaybackBaudRate(unsigned int nBaudRate);
    void StartEncode(ONFRAMEENCODED   pfCB, 
                                unsigned short wMaxFrameSize, 
                                void*          pUserData);

private:
	inline  void  InsertFlagByte(bool bFrameDone=false);
    __forceinline  void  EncodeByte(unsigned char ucTemp);
	inline  void  AddBit(unsigned char ucBit);
	inline  void  ResetEncode(unsigned int dwBufSize);
	inline  void  OnByteEncoded(unsigned char ucByte);
    inline  void  OnFrameEncoded();
private:
    unsigned int     m_nBytesPerSec;
    bool             m_bFlagByteInserted;
    ONFRAMEENCODED   m_pfCB;

	unsigned char    m_ucFlagByte;    //标志位
    void*            m_pUserData;

    unsigned int     m_nBlockSize;      
    unsigned int     m_nEncodedBytes;   //已编码的字节数
    unsigned char*   m_pszStreamEncodedBuf;   //已编码的字节缓存

	unsigned  char   m_nHighLevelBits; //连续高电平（BIT 1)位数
    unsigned  char   m_ucEncodingByte; //正在编码的字节
    unsigned  char   m_nEncodedBits;   //已编码的位（BIT)数
	//unsigned char*   
     unsigned char*   m_psResultBuf;       //已编码的字节缓存
     unsigned int     m_nResultBufSize;    //已编码的字节缓存
     unsigned int     m_nResultBufPtr;     //已编码的字节缓存
};

#endif //__HDCLCODEC_H__