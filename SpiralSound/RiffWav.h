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

#include <string>

#ifndef WAVFILE
#define WAVFILE

#include <stdio.h>
#include "Sample.h"

#if __APPLE__
// this is the traditional way of setting 2 bytes alignment
// else the apple compiler might use 4, or even 8
#pragma options align=mac68k
#endif

struct CanonicalWavHeader
{
	char  RiffName[4];
	int   RiffFileLength;
	char  RiffTypeName[4];
	
	char  FmtName[4];
	int   FmtLength;
	short FmtTag;
	short FmtChannels;
	int   FmtSamplerate;
	int   FmtBytesPerSec;
	short FmtBlockAlign;
	short FmtBitsPerSample;
};

struct DataHeader
{
	char  DataName[4];
	int   DataLengthBytes;
};

#if __APPLE__
#pragma options align=reset
#endif

class WavFile
{
public:
	WavFile() : m_Stream(NULL), m_Samplerate(44100), m_BitsPerSample(16), m_DataStart(0) {}
	~WavFile() {Close();}	
	
	enum Mode{READ,WRITE};
	enum Channels{MONO,STEREO};
	
	int  Open(std::string FileName, Mode mode, Channels channels=MONO);
	int  Close();
	int  Save(Sample &data);
	int  Load(Sample &data);
	int  Save(short *data, int Bytes);
	int  Load(short *data);
	int  SeekToChunk(int Pos);
	int  LoadChunk(int NumSamples, Sample &ldata, Sample &rdata);
	int  GetSize(); // in samples
	bool Recording() {return (m_Stream!=NULL);}
	void SetSamplerate(int s) { m_Samplerate=s; }
	int  GetSamplerate() { return m_Header.FmtSamplerate; }
	void SetBitsPerSample(int s) { m_BitsPerSample=s; }
	int  GetBitsPerSample() { return m_Header.FmtBitsPerSample; }
	bool IsStereo() { return (m_Header.FmtChannels==2); }
	bool IsOpen() { return m_Stream!=NULL; }
		
private:

	FILE *m_Stream;
	int m_Samplerate;
	int m_BitsPerSample;

	long m_DataStart;
	long m_CurSeekPos;
	
	CanonicalWavHeader m_Header;
	DataHeader m_DataHeader;
};

#endif
