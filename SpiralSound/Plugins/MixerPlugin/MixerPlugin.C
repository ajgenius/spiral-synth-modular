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
#include "MixerPlugin.h"
#include "MixerPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new MixerPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0007;
}

string GetGroupName()
{
	return "SpiralSound";
}
}

///////////////////////////////////////////////////////

MixerPlugin::MixerPlugin()
{
	m_PluginInfo.Name="Mixer";
	m_PluginInfo.Width=100;
	m_PluginInfo.Height=125;
	m_PluginInfo.NumInputs=4;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input one");	
	m_PluginInfo.PortTips.push_back("Input two");	
	m_PluginInfo.PortTips.push_back("Input three");	
	m_PluginInfo.PortTips.push_back("Input four");	
	m_PluginInfo.PortTips.push_back("Output");
	
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		m_ChannelVal[n]=1.0f;		
	}
	
	m_AudioCH->Register("Value",&m_GUIArgs.Value);
	m_AudioCH->Register("Num",&m_GUIArgs.Num);
}

MixerPlugin::~MixerPlugin()
{
}

PluginInfo &MixerPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *MixerPlugin::CreateGUI()
{
	return new MixerPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_AudioCH,m_HostInfo);
}

void MixerPlugin::Execute()
{
	// Mix the inputs
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		SetOutput(0,n,(GetInput(0,n)*m_ChannelVal[0])+
					  (GetInput(1,n)*m_ChannelVal[1])+
					  (GetInput(2,n)*m_ChannelVal[2])+
					  (GetInput(3,n)*m_ChannelVal[3]));
	}
}

void MixerPlugin::ExecuteCommands()
{
	if (m_AudioCH->IsCommandWaiting())
	{
		switch (m_AudioCH->GetCommand())
		{
			case (SETCH) : SetChannel(m_GUIArgs.Num,m_GUIArgs.Value); break;
		}
	}
}

void MixerPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		s<<m_ChannelVal[n]<<" ";
	}
}

void MixerPlugin::StreamIn(istream &s)
{	
	int version;
	s>>version;
	for (int n=0; n<NUM_CHANNELS; n++)
	{
		s>>m_ChannelVal[n];
	}
}
