/*  SpiralSynth
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "RiffWav.h"
//#include "SpiralInfo.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#define TRACE_OUT

using namespace std;

const int HEADERLEN = (4+24+8);

#if __BYTE_ORDER == BIG_ENDIAN
#define SWAPSHORT(a) (a)=(((a)<<8)|(((a)>>8)&0xff))
#define SWAPINT(a) (a)=(((a)&0x000000ff)<<24)|(((a)&0x0000ff00)<<8)|(((a)&0x00ff0000)>>8)|(((a)&0xff000000)>>24)
#else
#define SWAPSHORT(a)
#define SWAPINT(a)
#endif

int WavFile::Open(string FileName, Mode mode, Channels channels)
{
	if (m_Stream!=NULL)
	{
		cerr<<"WavFile: File already open ["<<FileName<<"]"<<endl;
		return 0;
	}

	if (mode==WRITE) m_Stream = fopen (FileName.c_str(), "wb");
	else m_Stream = fopen (FileName.c_str(), "rb");

	if (m_Stream == (FILE*)0)
	{
		cerr<<"WavFile: File ["<<FileName<<"] does not exist"<<endl;
		return 0;
	}

	//Write the header

	if (mode==WRITE)
	{
		m_Header.RiffName[0]='R';
		m_Header.RiffName[1]='I';
		m_Header.RiffName[2]='F';
		m_Header.RiffName[3]='F';
	
		m_Header.RiffFileLength=HEADERLEN; // bzzt - wrong
	
		m_Header.RiffTypeName[0]='W';
		m_Header.RiffTypeName[1]='A';
		m_Header.RiffTypeName[2]='V';
		m_Header.RiffTypeName[3]='E';
			
		m_Header.FmtName[0]='f';
		m_Header.FmtName[1]='m';
		m_Header.FmtName[2]='t';
		m_Header.FmtName[3]=' ';
		
		m_Header.FmtLength=0x00000010; // length of fmt data (16 bytes)
		m_Header.FmtTag=0x0001;        // Format tag: 1 = PCM
		if (channels==STEREO) m_Header.FmtChannels=2;     
		else m_Header.FmtChannels=1;     
		m_Header.FmtSamplerate=WavFile::m_Samplerate;
	
		m_Header.FmtBitsPerSample=16;
		m_Header.FmtBytesPerSec=m_Header.FmtSamplerate*m_Header.FmtBlockAlign;
		m_Header.FmtBlockAlign=m_Header.FmtChannels*m_Header.FmtBitsPerSample/8;	

		m_DataHeader.DataName[0]='d';	
		m_DataHeader.DataName[1]='a';
		m_DataHeader.DataName[2]='t';
		m_DataHeader.DataName[3]='a';
		
		m_DataHeader.DataLengthBytes=0;
			
		SWAPINT(m_Header.RiffFileLength);
		SWAPINT(m_Header.FmtLength);
		SWAPSHORT(m_Header.FmtTag);
		SWAPSHORT(m_Header.FmtChannels);
		SWAPINT(m_Header.FmtSamplerate);
		SWAPINT(m_Header.FmtBytesPerSec);
		SWAPSHORT(m_Header.FmtBlockAlign);
		SWAPSHORT(m_Header.FmtBitsPerSample);
		SWAPINT(m_DataHeader.DataLengthBytes);
			
		fwrite(&m_Header,1,sizeof(CanonicalWavHeader),m_Stream);		
		fwrite(&m_DataHeader,1,sizeof(DataHeader),m_Stream);		
	
		return 1;
	}
	else
	if (mode==READ)
	{
		fread(&m_Header,sizeof(CanonicalWavHeader),1,m_Stream);

		SWAPINT(m_Header.RiffFileLength);
		SWAPINT(m_Header.FmtLength);
		SWAPSHORT(m_Header.FmtTag);
		SWAPSHORT(m_Header.FmtChannels);
		SWAPINT(m_Header.FmtSamplerate);
		SWAPINT(m_Header.FmtBytesPerSec);
		SWAPSHORT(m_Header.FmtBlockAlign);
		SWAPSHORT(m_Header.FmtBitsPerSample);

		#ifdef TRACE_OUT		
		cerr<<FileName<<endl;
		cerr<<"RiffFileLength "<<m_Header.RiffFileLength<<endl;
		cerr<<"FmtLength "<<m_Header.FmtLength<<endl;
		cerr<<"FmtTag "<<m_Header.FmtTag<<endl;
		cerr<<"FmtChannels "<<m_Header.FmtChannels<<endl;
		cerr<<"FmtSamplerate "<<m_Header.FmtSamplerate<<endl;
		cerr<<"FmtBytesPerSec "<<m_Header.FmtBytesPerSec<<endl;
		cerr<<"FmtBlockAlign "<<m_Header.FmtBlockAlign<<endl;
		cerr<<"FmtBitsPerSample "<<m_Header.FmtBitsPerSample<<endl;
		#endif
		
		// skip the rest of the fmt header if necissary
		if (m_Header.FmtLength>16)
		{
			fseek(m_Stream,m_Header.FmtLength-16,SEEK_CUR);
		}
		
		fread(&m_DataHeader,sizeof(DataHeader),1,m_Stream);

		SWAPINT(m_DataHeader.DataLengthBytes);

		while (m_DataHeader.DataName[0]!='d' || 
			   m_DataHeader.DataName[1]!='a' || 
			   m_DataHeader.DataName[2]!='t' || 
			   m_DataHeader.DataName[3]!='a')
		{
			// crawl through the rest of the propriatory headers
			// if we need to to try and get to the data header
			if (feof(m_Stream) || fseek(m_Stream,-(sizeof(DataHeader)-1),SEEK_CUR)==-1)
			{	
				cerr<<"WavFile: File open error, wrong format ["<<FileName<<"]"<<endl;		
				return 0;
			}
			
			fread(&m_DataHeader,sizeof(DataHeader),1,m_Stream);
		}
		
		fgetpos(m_Stream,(fpos_t*)&m_DataStart);
		m_CurSeekPos=m_DataStart;
		
		#ifdef TRACE_OUT		
		cerr<<m_DataHeader.DataName[0]<<m_DataHeader.DataName[1]<<
		      m_DataHeader.DataName[2]<<m_DataHeader.DataName[3]<<endl;
		cerr<<"DataLengthBytes "<<m_DataHeader.DataLengthBytes<<endl;
		#endif	
			
		// check we have a wav file here
		if (m_Header.RiffName[0]=='R' &&
			m_Header.RiffName[1]=='I' &&
			m_Header.RiffName[2]=='F' &&
			m_Header.RiffName[3]=='F')
		{
			return 1;
		}
		
		fclose(m_Stream);
		cerr<<"WavFile: File open error, wrong format ["<<FileName<<"]"<<endl;
		return 0;
	}
	return 0;
}

int WavFile::Close()
{
	if (m_Stream==NULL) 
	{
		return 0;
	}

	// write the total length in	
	fseek(m_Stream, 40, SEEK_SET);
	fwrite(&m_DataHeader.DataLengthBytes,4,1,m_Stream);
	fclose(m_Stream); 

	m_Stream=NULL;
	return 1;	
}

int WavFile::Save(Sample &data)
{
	if (m_Stream==NULL || data.GetLength()==0)
	{
		return 0;
	}
	
  	// convert to integer for saving
	short *temp=new short[data.GetLength()];
	for (int n=0; n<data.GetLength(); n++)
	{
		//clip
		float v=data[n];
		if (v<-1) v=-1; if (v>1) v=1;
		temp[n]=(short)(v*SHRT_MAX);
		SWAPSHORT(temp[n]);
	}
	
	m_DataHeader.DataLengthBytes+=data.GetLength()*2;
	fwrite(temp,sizeof(&temp),data.GetLength()/2,m_Stream);
	
	// leak!
	delete[] temp;
	
	return 1;
}

int WavFile::Save(short *data, int Bytes)
{
	if (m_Stream==NULL || data==NULL)
	{
		return 0;
	}
	
	m_DataHeader.DataLengthBytes+=Bytes;
	fwrite(data,sizeof(data),Bytes/4,m_Stream);
	return 1;
}

int WavFile::GetSize()
{	
	if (m_Header.FmtBitsPerSample!=8 && m_Header.FmtBitsPerSample!=16)
	{
		cerr<<"WavFile Warning: FmtBitsPerSample="<<m_Header.FmtBitsPerSample<<" (can't cope, treating as 16)"<<endl;
		m_Header.FmtBitsPerSample=16;
	}
	
	int ret=0;
	
	if (m_Header.FmtBitsPerSample==8) 
	{
		if (IsStereo()) ret=m_DataHeader.DataLengthBytes/2;
		else ret=m_DataHeader.DataLengthBytes;
	}
	else 
	{
		if (IsStereo()) ret=m_DataHeader.DataLengthBytes/4;
		else ret=m_DataHeader.DataLengthBytes/2;	
	}
	
	return ret;
}

int WavFile::Load(Sample &data)
{
	if (m_Header.FmtChannels>1) // mix the channels into a mono buffer 
	{
		#ifdef TRACE_OUT		
		cerr<<"WavFile::Load - Channels = "<<m_Header.FmtChannels<<
		" Mixing down to mono..."<<endl;
		#endif
		
		short *TempBuf = new short[m_DataHeader.DataLengthBytes];
		if (m_DataHeader.DataLengthBytes!=(int)fread(TempBuf,1,m_DataHeader.DataLengthBytes,m_Stream))
    	{
    	    cerr<<"WavFile: Read error"<<endl;
    		return 0; 
    	}
		
		for (int n=0; n<GetSize(); n++)
		{
			long value=0;
			for (int i=0; i<m_Header.FmtChannels; i++)
			{
				short s = TempBuf[(n*m_Header.FmtChannels)+i];
				SWAPSHORT(s);
				value+=s;
			}
			value/=m_Header.FmtChannels;
			
			data.Set(n,value/(float)SHRT_MAX);
		}
		
		m_DataHeader.DataLengthBytes /= m_Header.FmtChannels;
		m_Header.FmtChannels=1;
		 
		delete[] TempBuf;
	}
	else // it's mono.
	{
    	short *TempBuf = new short[GetSize()];
		int t=(int)fread(TempBuf,1,m_DataHeader.DataLengthBytes,m_Stream);
		if (m_DataHeader.DataLengthBytes!=t)		
    	{
    	    cerr<<"WavFile: Read error read "<<t<<" expected "<<m_DataHeader.DataLengthBytes<<endl;
    		//return 0; 
    	}
		
		for (int n=0; n<GetSize(); n++)
		{
			short s = TempBuf[n];
			SWAPSHORT(s);
			data.Set(n,s/(float)SHRT_MAX);
		}
		
		delete[] TempBuf;
	}
	
	return 1;
}

int WavFile::Load(short *data)
{
	if (m_Header.FmtChannels>1) // mix the channels into a mono buffer 
	{
		#ifdef TRACE_OUT		
		cerr<<"WavFile::Load - Channels = "<<m_Header.FmtChannels<<
		" Mixing down to mono..."<<endl;
		#endif
		
		short *TempBuf = new short[m_DataHeader.DataLengthBytes];
		if (m_DataHeader.DataLengthBytes!=(int)fread(TempBuf,1,m_DataHeader.DataLengthBytes,m_Stream))
    	{
    	    cerr<<"WavFile: Read error"<<endl;
    		return 0; 
    	}
		
		for (int n=0; n<GetSize(); n++)
		{
			long value=0;
			for (int i=0; i<m_Header.FmtChannels; i++)
			{
				short s = TempBuf[(n*m_Header.FmtChannels)+i];
				SWAPSHORT(s);
				value+=s;
			}
			value/=m_Header.FmtChannels;
			
			data[n]=value;
		}
		
		m_DataHeader.DataLengthBytes /= m_Header.FmtChannels;
		m_Header.FmtChannels=1;
		
		delete[] TempBuf;
	}
	else // we can read the data directly in, it's mono.
	{
    	if (m_DataHeader.DataLengthBytes==
		(int)fread(data,1,m_DataHeader.DataLengthBytes,m_Stream))
    	{
#if __BYTE_ORDER == BIG_ENDIAN
			short *TempBuf = (short*)data;
			for (int n=0; n < m_DataHeader.DataLengthBytes / 2; n++)
				SWAPSHORT(TempBuf[n]);
#endif
    	    return 1;
    	}
	
    	cerr<<"WavFile: Read error"<<endl;
    	return 0;
	}
	
	return 0;
}

int WavFile::SeekToChunk(int Pos)
{
	Pos*=4;
	if (m_CurSeekPos==m_DataStart+Pos) return 1;
	
	m_CurSeekPos=m_DataStart+Pos;
	
	#ifdef TRACE_OUT		
	cerr<<"Seeking to "<<m_DataStart+Pos<<" "<<m_CurSeekPos<<endl;
	#endif
	if (fseek(m_Stream, m_DataStart+Pos, SEEK_SET)!=0)
	{
		cerr<<"WavFile::SeekToChunk: Seek error"<<endl;
	}
	
	return 1;
}

int WavFile::LoadChunk(int NumSamples, Sample &ldata, Sample &rdata)
{
	if (m_Header.FmtChannels>1) // untangle the interleaved data
	{
		int SizeBytes=NumSamples*4; // stereo,16bit
		short *TempBuf = new short[NumSamples*2];
		if (SizeBytes!=(int)fread(TempBuf,1,SizeBytes,m_Stream))
    	{
    	    cerr<<"WavFile: Read chunk error"<<endl;
    		return 0; 
    	}
		
		m_CurSeekPos+=SizeBytes;
		
		for (int n=0; n<NumSamples; n++)
		{
			ldata.Set(n,TempBuf[n*2]/(float)SHRT_MAX);
			rdata.Set(n,TempBuf[(n*2)+1]/(float)SHRT_MAX);
		}
		
		delete[] TempBuf;
	}
	else // we can read the data directly in, it's mono.
	{
    	if (m_DataHeader.DataLengthBytes==
		(int)fread(ldata.GetNonConstBuffer(),1,NumSamples*2,m_Stream))
    	{
    	    return 1;
    	}
	
    	cerr<<"WavFile: Read error"<<endl;
    	return 0;
	}
	
	return 0;
}
