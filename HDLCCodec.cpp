// HDCLCodec.cpp: implementation of the CHDCLCodec class.
//
//////////////////////////////////////////////////////////////////////
//HDLC面向比特的同步协议
#include <malloc.h>
#include "HDLCCodec.h"

const unsigned char BYTE_HDLC_FRAME_FLAG =  0x7E;
unsigned char C_BYTEINVERSE_MAP[256] = { 
    0x00,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,
    0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
    0x08,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,
    0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
    0x04,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,
    0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
    0x0c,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,
    0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
    0x02,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,
    0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
    0x0a,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,
    0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
    0x06,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,
    0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
    0x0e,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,
    0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
    0x01,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,
    0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
    0x09,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,
    0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
    0x05,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,
    0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
    0x0d,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,
    0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
    0x03,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,
    0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
    0x0b,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,
    0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
    0x07,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,
    0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
    0x0f,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,
    0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff,
  }; 


 static unsigned short int C_HDLC_FCS_TABLE[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                            ComputeFCS 
--------------------------------------------------------
*函数描述: 用于计算FCS
*函数参数:                             
*          szData  : unsigned char* => 指向字节数组的指针
*          nLen    : int            => 数字长度，必须大于等于1
*返回类型: unsigned short
*返 回 值: CCITT FCS VALUE
*
*
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
unsigned short int ComputeFCS (const unsigned char *szData, int nLen)
{  

    unsigned short int wFCS  =   0xffff;
    while (nLen--)
    {   
        wFCS = (wFCS >> 8) ^ C_HDLC_FCS_TABLE[ ( wFCS ^ ((unsigned short int)*szData) ) & 0xff];
        szData++;
    }
    return (wFCS ^ 0xFFFF);
}

bool  CheckFCS (const unsigned char *szData, int nLen)
{  

    unsigned short  wFCS      = 0xffff;
    unsigned short wStreamFCS = 0;
    if (nLen > 2)
    {
         wFCS = ComputeFCS( szData, nLen - 2);

       wStreamFCS = ((unsigned short)szData[nLen - 2]) & 0xFF;
       wStreamFCS = (   ( (unsigned short)szData[nLen - 1]) << 8) |  wStreamFCS ;

    }

    return (wFCS == wStreamFCS);
}
//静态函数
static unsigned short CheckBufferSize(unsigned short wBufferSize)
{
			if (0 == wBufferSize )
			{
				wBufferSize = DEF_FRAME_BUFFER;
			}
			else
			{
				if(wBufferSize < MIN_FRAME_BUFFER)
					wBufferSize = MIN_FRAME_BUFFER;
				else if (wBufferSize > MAX_FRAME_BUFFER )
				{
					wBufferSize = MAX_FRAME_BUFFER;
				}
			} 
			return wBufferSize;
}

//////////////////////////////////////////////////////////////////////
//构造/析构函数
//////////////////////////////////////////////////////////////////////

CHDLCDecode::CHDLCDecode()
{

    m_pfCB            = 0;
	m_pUserData       = 0;
	m_pszDecodedBuf   = 0;
    m_nOffsetInStream = 0;

}

CHDLCDecode::~CHDLCDecode()
{
    if (m_pszDecodedBuf)
    {
      free(m_pszDecodedBuf);
      m_pszDecodedBuf = 0;
    }
   
}
//////////////////////////////////////////////////////////////////////
//私有函数
//////////////////////////////////////////////////////////////////////


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCDecode::ResetDecode
--------------------------------------------------------
*函数描述: 用于重置解码过程
*函数参数:                      
*          dwBufSize : unsigned int => 缓冲区字节数
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void CHDLCDecode::ResetDecode(unsigned int dwBufSize)
{
    m_bFlagByte        = false; //检测出标志位标志，ｔｒｕｅ已检测到标志位
    m_nHighLevelBits   = 0; //连续高电平（BIT 1)位数
    m_ucDecodingByte   = 0; //正在解码的字节
    m_nDecodedBits     = 0; //已解码的位（BIT)数
    m_nDecodedBytes    = 0; //已解码的字节数
	//分配解码缓存区
    if (!m_pszDecodedBuf || dwBufSize > m_nBlockSize )
    {
        m_pszDecodedBuf =(unsigned char*) realloc(m_pszDecodedBuf, dwBufSize);
    }
     m_nBlockSize  = dwBufSize;
 

}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                        CHDLCDecode::AddBit
--------------------------------------------------------
*函数描述: 用于将已解码的BIT加到正在解码的字节中
*函数参数:                         
*          ucBit : unsigned char => 0/1，解码后的位(BIT)
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline void CHDLCDecode::AddBit(unsigned char ucBit)
{
 
       m_ucDecodingByte <<= 1;
       m_ucDecodingByte |= ucBit;
       //满一个字节（8 bits）
       if (8 ==(++m_nDecodedBits) )
       {
          if (m_bFlagByte) //如果标志位已检测到
          {
             OnByteDecoded(m_ucDecodingByte); //写入缓存区
             m_nDecodedBits = 0;
          }
          else
          {
             m_nDecodedBits = ucBit?0:1;
          }
          m_ucDecodingByte = 0;
       }
 
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCDecode::OnByteDecoded
--------------------------------------------------------
*函数描述: 用于将一个已解码的字节写入缓存区
*函数参数:                      
*          ucByte : unsigned char =>已解码的字节
*返回类型: inline int 
*返 回 值: 1 -- 已回调，0 - 未回调
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline int CHDLCDecode::OnByteDecoded(unsigned char ucByte)
{
   if (m_bFlagByte) //只有在已检测到标准未时才加入缓冲区
   {
     m_pszDecodedBuf[m_nDecodedBytes] = ucByte;
	 //缓冲区满
     if((++m_nDecodedBytes) == m_nBlockSize)
     {
        if (m_pfCB)
        {
          m_pfCB(HDLC_FRAME_DATA,(const char*)m_pszDecodedBuf,m_nDecodedBytes,m_pUserData);
        }

        m_nDecodedBytes = 0;
		return 1;
     }  

   }
   return 0;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCDecode::OnFrameAbort
--------------------------------------------------------
*函数描述: 用于检测到“取消”标志时的处理
*函数参数: 无
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline void CHDLCDecode::OnFrameAbort()
{
      if( m_pfCB )
      {
           m_pfCB(HDLC_FRAME_ABORT,
			   (const char*)m_pszDecodedBuf,
               m_nDecodedBytes,
               m_pUserData);
      }

      m_ucDecodingByte    = 0x0;
      m_nDecodedBits      = 0;
	  m_nDecodedBytes     = 0;
      m_bFlagByte         = false;

}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCDecode::OnFrameFlagByte
--------------------------------------------------------
*函数描述: 用于检测到标志字节（连续6个1和1个0）时的处理
*函数参数: 无
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline void CHDLCDecode::OnFrameFlagByte(int nOffset)
{
   if (m_bFlagByte) //已检测到标志位，
   {
      if ( m_nDecodedBytes > 0)//上一个标志位后有数据
      {
          if (m_pfCB )
          {
               m_pfCB(HDLC_FRAME_DONE,
                       (const char*)m_pszDecodedBuf,
                       m_nDecodedBytes,
                       m_pUserData);
          }
          m_nDecodedBytes    = 0;
          m_bFlagByte        = false;
      }
	  //连续的标志位，不作处理
   }
   else
   {
      m_nOffsetInStream =  nOffset;
      m_bFlagByte       =  true;
   }
   //检测到标志位后清理正在解码的控制参数
   m_ucDecodingByte    = 0;
   m_nDecodedBits      = 0;
}


//////////////////////////////////////////////////////////////////////
//公有函数
//////////////////////////////////////////////////////////////////////

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCEncode::StartDecode
--------------------------------------------------------
*函数描述: 用于启动一个解码过程
*函数参数:                      
*          pfCB          : ONFRAMEENCODED => 回调函数地址
*          wBufferSize   : unsigned short => 解码缓冲区字节数
*          pUserData     : void* =>用户数据，作为参数透传给回调函数
*返回类型: 无返回值
* 说   明:
*      解码过程启动后，等待调用者写入编码后的数据，
*      当已解码字节数达到wBufferSize指定阀值
*      或一帧数据解码完成时调用参数pfCB所指向的函数
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void CHDLCDecode::StartDecode(ONFRAMEDECODED   pfCB, 
                                unsigned short wBufferSize, 
                                void*          pUserData)
{
        m_pfCB      = pfCB;
        m_pUserData = pUserData;
        ResetDecode( CheckBufferSize(wBufferSize) );
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCDecode::DecodeBuffer
--------------------------------------------------------
*函数描述: 用于接收流数据并解码
*函数参数:                      
*          pData  : const char* => 数据缓冲区首地址
*          nSize  : int         => 数据字节数
*返回类型: int 
*返 回 值: 本次数据回调次数
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int CHDLCDecode::DecodeBuffer(const char *pData, int nSize)
{
  const unsigned char* p    = (const unsigned char*)pData;
  const unsigned char* pEnd = p + nSize;
  register unsigned char  ucTemp;
  int nFrame = 0;
  for (; p < pEnd;p++)
  {
      ucTemp =   *p; 
      for (int i = 0 ;i < 8; i++)
      {
           if(0x80 == (ucTemp & 0x80) )
           {
              m_nHighLevelBits++;
              AddBit(1);
              if( m_nHighLevelBits > 6)    //连续7个1 Abort
              {
                OnFrameAbort();
              }
           }
           else
           {
                 if(6 == m_nHighLevelBits)    //连续6个1
                 {
                      OnFrameFlagByte(pEnd  - p);
                 }
                 else if(5 != m_nHighLevelBits) //连续5个'1' 删除 '0'
                 {
                      AddBit(0);
                 }  
                 m_nHighLevelBits = 0;
         }       
         ucTemp <<= 1;
      }//for(i = 0 
  }//(; p < pEnd;p++

   
  return  nFrame;
}
//----------------------------------------------------------------------------
// 编码类
//----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
//构造/析构函数
//////////////////////////////////////////////////////////////////////


CHDLCEncode::CHDLCEncode()
{
   m_pfCB           = 0;
   m_pUserData      = 0;
   m_pszStreamEncodedBuf = 0;
   m_psResultBuf    =  0;     
   m_nResultBufSize = 0;   
   m_nResultBufPtr  = 0; 
   SetPlaybackBaudRate( 19200);
   ResetEncode(DEF_FRAME_BUFFER);
}

CHDLCEncode::~CHDLCEncode()
{
	if (m_pszStreamEncodedBuf)
    {
      free(m_pszStreamEncodedBuf);
      m_pszStreamEncodedBuf = 0;
    }

}
//////////////////////////////////////////////////////////////////////
//私有函数
//////////////////////////////////////////////////////////////////////


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCEncode::ResetEncode
--------------------------------------------------------
*函数描述: 用于重置编码过程，所有控制变量设置为初始值
*函数参数:                      
*          dwBufSize : unsigned int => 编码缓冲区（回调最大）字节数
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline void CHDLCEncode::ResetEncode(unsigned int dwBufSize)
{

	m_bFlagByteInserted = false;

	m_nHighLevelBits = 0; //连续高电平（BIT 1)位数
	
	m_nEncodedBytes   = 0;

    m_ucEncodingByte = 0; //正在编码的字节
    m_nEncodedBits   = 0; //已编码的位（BIT)数
    m_nEncodedBytes  = 0; //已编码的字节数
	m_ucFlagByte     = 0x7E;
	//分配缓存区
    if (!m_pszStreamEncodedBuf || dwBufSize > m_nBlockSize )
    {
        m_pszStreamEncodedBuf =(unsigned char*) realloc(m_pszStreamEncodedBuf, dwBufSize);
    }
     m_nBlockSize  = dwBufSize;

}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                      CHDLCEncode::EncodeByte
--------------------------------------------------------
*函数描述: 用于对一个字节进行编码
*函数参数:                       
*          ucTemp : unsigned char =>  要编码
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline void CHDLCEncode::EncodeByte(unsigned char ucTemp)
{
      for (int i = 0 ;i < 8; i++)
      {
         if(0x80 == (ucTemp & 0x80) )
         {
              AddBit(1);
			  if(5 == (++m_nHighLevelBits) ) //5个'1' 插入 '0'
			  {
				 AddBit(0);
                 m_nHighLevelBits =0;
			  }
         }
         else
         {
             AddBit(0);
             m_nHighLevelBits = 0;
         }       
         ucTemp <<= 1;
      }
}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCEncode::InsertFlagByte
--------------------------------------------------------
*函数描述: 用于插入标志位
*函数参数:                      
*          bFrameStart : bool => 是否帧完成
*返回类型: 无返回值
*
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline void CHDLCEncode::InsertFlagByte(bool bFrameDone)
{
	  const    unsigned long dw3FlagByts = 0x7E7E7E;

      unsigned char ucOutput = BYTE_HDLC_FRAME_FLAG;

		//Step1：构成一个32整数
       //m_nEncodedBits 左移位数               dwTemp
      //-------------------------------３------２--------１-------０-----------
		  //  1   <<      (7)       |0000000x|01111110|01111110|01111110|;  
		  //  2   <<      (6)       |000000xx|01111110|01111110|01111110|;  
		  //  3   <<      (5)       |00000xxx|01111110|01111110|01111110|;  
		  //  4   <<      (4)       |0000xxxx|01111110|01111110|01111110|;  
		  //  5   <<      (3)       |000xxxxx|01111110|01111110|01111110|;  
		  //  6   <<      (2)       |00xxxxxx|01111110|01111110|01111110|;  
		  //  7   <<      (1)       |0xxxxxxx|01111110|01111110|01111110|;  
		  //                          |
		  //                          m_ucEncodingByte

	   //Step2
       // m_nEncodedBits 左移位数          dwTemp                       m_ucFlagByte    
       //-----------------------------３--------２------１-------０----------
		  //  1   <<      (7)     |x0111111|00111111|00111111|????????|;  0X3F
		  //  2   <<      (6)     |xx011111|10011111|10011111|????????|;  0X9F
		  //  3   <<      (5)     |xxx01111|11001111|11001111|????????|;  0XCF
		  //  4   <<      (4)     |xxxx0111|11100111|11100111|????????|;  0XE7
		  //  5   <<      (3)     |xxxxx011|11110011|11110011|????????|;  0XF3
		  //  6   <<      (2)     |xxxxxx01|11111001|11111001|????????|;  0XF9
		  //  7   <<      (1)     |xxxxxxx0|11111100|11111100|????????|;  0XFC
		  //                           |      |    
  		  //                     ucOutput     |
		  //                                  m_ucFlagByte

	  //Step3: dwTemp右移24位取低字节=》ucOutput(最后一个数据字节）
      // 右移24                        dwTemp                     
      //-------------------3--------2--------1--------0----------
	  //  1   << (7)    |00000000|00000000|00000000|x0111111|
	  //  2   << (6)    |00000000|00000000|00000000|xx011111|
	  //  3   << (5)    |00000000|00000000|00000000|xxx01111|
	  //  4   << (4)    |00000000|00000000|00000000|xxxx0111|
	  //  5   << (3)    |00000000|00000000|00000000|xxxxx011|
	  //  6   << (2)    |00000000|00000000|00000000|xxxxxx01|
      //  7   << (1)    |00000000|00000000|00000000|xxxxxxx0|
	  //                                             |    
	  //                                            ucOutput

	  //Step4 : dwTemp 右移 16位取低字节 => m_ucFlagByte（结束标志位）
      // 右移16                        dwTemp                     
      //--------------------3--------2--------1--------0----------
	  // 1  << (7)    |00000000|00000000|x0111111|00111111|
	  // 2  << (6)    |00000000|00000000|xx011111|10011111|
	  // 3  << (5)    |00000000|00000000|xxx01111|11001111|
	  // 4  << (4)    |00000000|00000000|xxxx0111|11100111|
	  // 5  << (3)    |00000000|00000000|xxxxx011|11110011|
	  // 6  << (2)    |00000000|00000000|xxxxxx01|11111001|
	  // 7  << (1)    |00000000|00000000|xxxxxxx0|11111100|
	  //                                              |
	  //                                              m_ucFlagByte

	  //Step5 m_ucFlagByte 右移nShift 位　＝》　m_ucEncodingByte
      //              m_ucFlagByte            m_ucEncodingByte         
      //--------------------------------------------------------------------
	  // 1  >> (7)    |00111111|　　　　　　｜0 0 0 0 0 0 0 ０ ｜　
	  // 2  >> (6)    |10011111|　　　　　　｜0 0 0 0 0 0 １０ ｜　　
	  // 3  >> (5)    |11001111|　　　　　　｜0 0 0 0 0 １１０ ｜　　　
	  // 4  >> (4)    |11100111|　　　　　　｜0 0 0 0 １１１０ ｜　　
	  // 5  >> (3)    |11110011|　　　　　　｜0 0 0 １１１１０ ｜　　
	  // 6  >> (2)    |11111001|　　　　　　｜0 0 １１１１１０ ｜　　　
	  // 7  >> (1)    |11111100|　　　　　　｜0 １１１１１１０ ｜　

	  if(0 != m_nEncodedBits)
	  {
		   int nShift    = 8 - m_nEncodedBits;
		   unsigned long dwTemp = m_ucEncodingByte;
		   dwTemp = (dwTemp << 24) | dw3FlagByts; //step 1

		   dwTemp <<= nShift; //step 2

		   ucOutput            = (unsigned char) (dwTemp >> 24);//step 3
		   m_ucFlagByte        = (unsigned char) ( dwTemp >> 16); //step 4
		   m_ucEncodingByte    =  ( m_ucFlagByte >> nShift); //step 5

	  }
	  else
	  {
		   m_ucFlagByte = BYTE_HDLC_FRAME_FLAG;
	  }



	  if(bFrameDone ) //End Flag Byte
      {
//   bFrameStart = true, FreeBytes为缓冲区的空闲字节数
//     m_nEncodedBits  FreeBytes   追加字节  回调次数
//        0                           1         1
//        !=0             >1          2         1 
//        !=0              1          2         2
          
           OnByteEncoded(ucOutput);
		   if(0 != m_nEncodedBits )
		   {
 			   OnByteEncoded(m_ucFlagByte);
           }

           if(m_pfCB)
		   {
			  m_pfCB(HDLC_FRAME_DONE,
					  (const char*)m_pszStreamEncodedBuf,
					  m_nEncodedBytes,   //有可能为0
					  m_pUserData);
		   }
		   m_nEncodedBytes = 0;
	  }
	  else //Start Flag Byte
	  {
 	        OnByteEncoded(ucOutput);
      } 

 }

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCEncode::EncodeFrameStream
--------------------------------------------------------
*函数描述: 用于对帧数据进行连续编码
*函数参数:                      
*          nType     : int          => 类型
*                      HDCL_FRAME_DATA -- 数据帧的一个包
*                      HDCL_FRAME_DONE -- 数据帧的最后一个包
*          szBuffer  : const char*  => 数据帧内容
*          dwLen     : unsigned int => 数据帧字节数
*返回类型: int 
*返 回 值: 0 -- 失败， 1 -- 成功
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int CHDLCEncode::EncodeFrameStream(int nType, 
							 const char *szBuffer, 
							 unsigned int dwLen)
{

    if (!m_pszStreamEncodedBuf || !szBuffer)
    {
          return 0;
    }


  //--插入起始标志位
  if (!m_bFlagByteInserted)
  {
	    InsertFlagByte(false);
	    m_bFlagByteInserted = true;
  }

  const char* p    = szBuffer;
  const char* pEnd = szBuffer + dwLen;
  register unsigned char  ucTemp;
  for (; p < pEnd; p++)
  {
	  ucTemp = *p;
      EncodeByte(ucTemp);
  }
   
  
   if (HDLC_FRAME_DONE == nType)
   {
         OnFrameEncoded();
   }

  return 1;
}

unsigned int CHDLCEncode::EncodeFrame( const char  *szRaw,
                                       unsigned int dwRawSize,
                                       char* szEncodedBuffer,
                                       unsigned int dwBufferSize)
{
    if (m_pfCB || !szRaw || !szEncodedBuffer || 0 ==  dwBufferSize)
    {
          return 0;
    }
    
    m_psResultBuf    = (unsigned char*)szEncodedBuffer;     
    m_nResultBufSize = dwBufferSize;   
    m_nResultBufPtr  = 0; 
       
 //--插入起始标志位
     InsertFlagByte(false);
     const unsigned char* p    = (const unsigned char* )szRaw;
     const unsigned char* pEnd = p + dwRawSize;
     for (; p < pEnd; p++)
      {
          EncodeByte(*p);
      }
      OnFrameEncoded();
      return m_nResultBufPtr;
}

unsigned char CHDLCEncode::GetFlagByteInverse() const 
{ 
  return C_BYTEINVERSE_MAP[m_ucFlagByte];
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                        CHDLCEncode::AddBit
--------------------------------------------------------
*函数描述: 用于正在编码的字节增加1位（BIT)
*函数参数:                         
*          ucBit : unsigned char => 位值 （0/1)
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
__forceinline void CHDLCEncode::AddBit(unsigned char ucBit)
{
       m_ucEncodingByte <<= 1;
       m_ucEncodingByte |= ucBit;
	   //满一个字节，加入缓冲区
       if (8 == (++m_nEncodedBits) )
       {
          OnByteEncoded( m_ucEncodingByte);
          m_nEncodedBits   = 0;
          m_ucEncodingByte = 0;
       }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                    CHDLCEncode::OnByteEncoded
--------------------------------------------------------
*函数描述: 用于将一个已解码的字节加入缓冲区，缓冲区满时调用回调函数
*函数参数:                     
*          ucByte : unsigned char =>已解码的字节
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline  void CHDLCEncode::OnByteEncoded(unsigned char ucByte)
{
   //字节流和存储是倒序关系      
 
    if(m_pfCB)
    {
       m_pszStreamEncodedBuf[m_nEncodedBytes++] = ucByte;
       if( m_nEncodedBytes ==  m_nBlockSize)
       {
              m_pfCB(HDLC_FRAME_DATA,
			      (const char*)m_pszStreamEncodedBuf,
			      m_nEncodedBytes,
			      m_pUserData);
		      m_nEncodedBytes = 0;
       }
    }
    else
    {
       if (m_nResultBufPtr <  m_nResultBufSize)
       {
           m_psResultBuf[m_nResultBufPtr] = ucByte;
        }
        m_nResultBufPtr++;
    }
   return ;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                    CHDLCEncode::OnFrameEncoded
--------------------------------------------------------
*函数描述: 用于
*函数参数: 无
*返回类型: 无返回值
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
inline  void CHDLCEncode::OnFrameEncoded()
{
   //--插入结束标志位
    InsertFlagByte(true);
    m_bFlagByteInserted = false;

    if(m_pfCB)
	{
          m_pfCB(HDLC_FRAME_DONE,
			  (const char*)m_pszStreamEncodedBuf,
			  m_nEncodedBytes,
			  m_pUserData);
	}
	m_nEncodedBytes = 0;

}
//////////////////////////////////////////////////////////////////////
//公有函数
//////////////////////////////////////////////////////////////////////
void CHDLCEncode::SetPlaybackBaudRate(unsigned int nBaudRate)
{
  m_nBytesPerSec = nBaudRate/8;
}




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*
                     CHDLCEncode::StartEncode
--------------------------------------------------------
*函数描述: 用于启动一个编码过程
*函数参数:                      
*          pfCB          : ONFRAMEENCODED => 回调函数地址，
*          wMaxFrameSize : unsigned short => 编码缓冲区字节数
*          pUserData     : void* =>用户数据，作为参数透传给回调函数
*返回类型: 无返回值
* 说   明:
*      编码过程，等待调用者写入一解码的数据帧，
*      当已编码字节数达到wMaxFrameSize指定阀值
*      或一帧数据编码完成时调用参数pfCB所指向的函数
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void CHDLCEncode::StartEncode(ONFRAMEENCODED   pfCB, 
                                unsigned short wMaxFrameSize, 
                                void*          pUserData)
{
        if (pfCB)
        {
            m_pfCB      = pfCB;
            m_pUserData = pUserData;
        }
        else
        {

        }
        ResetEncode(CheckBufferSize(wMaxFrameSize));
}





