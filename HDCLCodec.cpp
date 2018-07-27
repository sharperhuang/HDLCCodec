// HDCLCodec.cpp: implementation of the CHDCLCodec class.
//
//////////////////////////////////////////////////////////////////////

#include <malloc.h>
#include "HDCLCodec.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHDCLDecode::CHDCLDecode()
{

    m_pszBlockBuf    = 0;
    m_nBlockSize     = 0;
    Reset();

}

CHDCLDecode::~CHDCLDecode()
{
    if (m_pszBlockBuf)
    {
      free(m_pszBlockBuf);
      m_pszBlockBuf = 0;
    }
   
}

void CHDCLDecode::Reset()
{
    m_ucLastByte     = 0;
    m_bFlagByte      = false;
    m_nHighLevelBits = 0;
    m_nLastByteBits  = 0;
    m_nFrameBytes    = 0;
    m_pfCB           = 0;

}

inline void CHDCLDecode::AddBit(unsigned char ucBit)
{
 
       m_ucLastByte <<= 1;
       m_ucLastByte |= ucBit;
       m_nLastByteBits++;
       if (8 == m_nLastByteBits )
       {
          m_nLastByteBits = 0;
          if (m_bFlagByte)
          {
             OnByteReady(m_ucLastByte);
             m_nLastByteBits = 0;
          }
          else
          {
             m_nLastByteBits = ucBit?0:1;
          }
          m_ucLastByte = 0;
       }
 
}

inline int CHDCLDecode::OnByteReady(unsigned char ucByte)
{
   if (m_bFlagByte)
   {
     m_pszBlockBuf[m_nFrameBytes] = ucByte;
     m_nFrameBytes++;
     if(m_nFrameBytes == MAX_FRAME_BUFFER)
     {
        if (m_pfCB)
        {
          m_pfCB(HDCL_FRAME_DATA,(const char*)m_pszBlockBuf,MAX_FRAME_BUFFER,m_pUserData);
        }
        m_nFrameBytes = 0;
     }  
   }
   m_ucLastByte    = 0;
   m_nLastByteBits = 0;
   return 0;
}

inline void CHDCLDecode::OnFrameAbort()
{
      if (m_pfCB )
      {
           m_pfCB(HDCL_FRAME_ABORT,(const char*)0,0,m_pUserData);
      }
      m_ucLastByte    = 0;
      m_nLastByteBits = 0;
      m_bFlagByte     = false;
}

inline void CHDCLDecode::OnFrameFlag()
{
   if (m_bFlagByte)
   {
      if ( m_nFrameBytes > 0)
      {
          if (m_pfCB )
          {
               m_pfCB(HDCL_FRAME_DONE,(const char*)m_pszBlockBuf,m_nFrameBytes,m_pUserData);
          }
           m_nFrameBytes    = 0;
           m_bFlagByte      = false;
      }
   }
   else
   {
      m_bFlagByte     = true;
      m_ucLastByte    = 0;
      m_nLastByteBits = 0;
   }
   
}



int CHDCLDecode::DecodeBuffer(const char *pData, int nSize)
{
  const char* p    = pData;
  const char* pEnd = pData + nSize;
  register unsigned char  ucTemp;
  static unsigned char C_SHIFTBIT[] = {0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};
  for (; p < pEnd;p++)
  {
      ucTemp = *p;
      for (int i = 0 ;i < 8; i++)
      {
           ucTemp <<= i;
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
                  OnFrameFlag();
             }
             else if(5 != m_nHighLevelBits) //删除0
             {
                  AddBit(0);
             }  
             m_nHighLevelBits = 0;
         }       
      }
  }

   
  return  nSize;
}



void CHDCLDecode::StartDecode(ONFRAMEDECODED   pfCB, 
                                unsigned short wBufferSize, 
                                void*          pUserData)
{
        Reset();
        m_pfCB      = pfCB;
        m_pUserData = pUserData;
        if (0 == wBufferSize)
        {
          wBufferSize = DEF_FRAME_BUFFER;
        }
        if (wBufferSize > MAX_FRAME_BUFFER )
        {
            wBufferSize = DEF_FRAME_BUFFER;
        }
        if (!m_pszBlockBuf || wBufferSize > m_nBlockSize )
        {
           m_pszBlockBuf =(unsigned char*) realloc(m_pszBlockBuf, wBufferSize);
        }
        m_nBlockSize  = wBufferSize;
 
}

//----------------------------------------------------------------------------
// 编码类
//----------------------------------------------------------------------------



CHDCLEncode::CHDCLEncode(unsigned int nBaudRate)
{
   m_ucFlagByte         = 0x7E;
   m_bFlagByteInserted  = false;
   m_pszBlockBuf        = 0;
   SetPlaybackBaudRate( nBaudRate);
}
CHDCLEncode::~CHDCLEncode()
{
}

void CHDCLEncode::SetPlaybackBaudRate(unsigned int nBaudRate)
{
  
  m_nBytesPerSec = nBaudRate/8;
}


void CHDCLEncode::StartEncode(ONFRAMEENCODED   pfCB, 
                                unsigned short wBufferSize, 
                                void*          pUserData)
{
    //    Reset();
        m_pfCB      = pfCB;
        m_pUserData = pUserData;
/*        if (0 == wBufferSize)
        {
          wBufferSize = DEF_FRAME_BUFFER;
        }
        if (wBufferSize > MAX_FRAME_BUFFER )
        {
            wBufferSize = DEF_FRAME_BUFFER;
        }
        m_pszBlockBuf = new unsigned char[wBufferSize];
*/ 
}

int CHDCLEncode::EncodeIdlePack(unsigned int dwMillsecs)
{
    //m_nBytesPerSec
    return 0;
}

int CHDCLEncode::EncodeBuffer(int nType, const char *szBuffer, unsigned int dwLen)
{

       if (!m_pszBlockBuf)
       {
          return 0;
       }
      if (HDCL_FRAME_DATA == nType || HDCL_FRAME_DONE == nType)
      {
          if (!m_bFlagByteInserted)
          {
             m_bFlagByteInserted = true;
          }
          if (HDCL_FRAME_DONE == nType)
          {
            //AddFlagByte();
             m_bFlagByteInserted = false;
          }
      }
      return 1;
}
