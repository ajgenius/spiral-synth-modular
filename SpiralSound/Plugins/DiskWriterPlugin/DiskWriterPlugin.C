/*  SpiralSound
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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

// for lrintf()
#define	_ISOC9X_SOURCE	1
#define _ISOC99_SOURCE	1
#include  <math.h>

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <limits.h>

#include "DiskWriterPlugin.h"
#include "DiskWriterPluginGUI.h"
#include <FL/fl_file_chooser.H>
#include "SpiralIcon.xpm"

static const HostInfo* host;

#define CHECK_AND_REPORT_ERROR	if (result<0)         \
								{                     \
									perror("Sound device did not accept settings"); \
									m_OutputOk=false; \
									return false;     \
								}

extern "C"
{
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new DiskWriterPlugin;
}

char** SpiralPlugin_GetIcon()
{	
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 41;
}

string SpiralPlugin_GetGroupName()
{
	return "InputOutput";
}
}

///////////////////////////////////////////////////////

DiskWriterPlugin::DiskWriterPlugin() :
m_Recording(false)
{		
	m_PluginInfo.Name="DiskWriter";
	m_PluginInfo.Width=100;
	m_PluginInfo.Height=66;
	m_PluginInfo.NumInputs=3;
	m_PluginInfo.NumOutputs=0;
	m_PluginInfo.PortTips.push_back("Left Out");
	m_PluginInfo.PortTips.push_back("Right Out");
	m_PluginInfo.PortTips.push_back("Record Controller");

	m_AudioCH->RegisterData("Filename",ChannelHandler::INPUT,m_GUIArgs.Name,256);
}

DiskWriterPlugin::~DiskWriterPlugin()
{	
}

PluginInfo &DiskWriterPlugin::Initialise(const HostInfo *Host)
{	
	PluginInfo& Info= SpiralPlugin::Initialise(Host);
	host=Host;
	return Info;
}

SpiralGUIType *DiskWriterPlugin::CreateGUI()
{
	return new DiskWriterPluginGUI(m_PluginInfo.Width,
										  m_PluginInfo.Height,
										  this,
										  m_AudioCH,
										  m_HostInfo);
}

void DiskWriterPlugin::Execute()
{  
	if(m_Recording && m_Wav.IsOpen()) 
    {	
		int on=0;
		float t;
		
		short Buffer[host->BUFSIZE*2];
		
		for (int n=0; n<host->BUFSIZE; n++)
		{
			// stereo channels - interleave	
			t=GetInput(0,n);
			if (t>1) t=1;
			if (t<-1) t=-1;
			Buffer[on]=lrintf(t*SHRT_MAX);
			on++;
			
			t=GetInput(1,n);
			if (t>1) t=1;
			if (t<-1) t=-1;
			Buffer[on]=lrintf(t*SHRT_MAX);
			on++;
		}
		
		// stereo 16bit * bufsize
        m_Wav.Save(Buffer,host->BUFSIZE*2*2);
    }
}

void DiskWriterPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch(m_AudioCH->GetCommand())
		{
			case OPENWAV : 
				m_Wav.Open(m_GUIArgs.Name,WavFile::WRITE, WavFile::STEREO);
			break;
			case CLOSEWAV : m_Wav.Close(); break;
			case RECORD : m_Recording=true; break;
			case STOP : m_Recording=false;	break;	
			default : break;
		}
	}
}
