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

#include <stdio.h>
#include "ControllerPlugin.h"
#include "ControllerPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new ControllerPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 0x0003;
}
}

///////////////////////////////////////////////////////

ControllerPlugin::ControllerPlugin() :
m_Num(4)
{
	m_Version=3;
	
	m_PluginInfo.Name="CV Control";
	m_PluginInfo.Width=240;
	m_PluginInfo.Height=224;
	m_PluginInfo.NumInputs=0;
	m_PluginInfo.NumOutputs=4;
	m_PluginInfo.PortTips.push_back("CV 1");	
	m_PluginInfo.PortTips.push_back("CV 2");	
	m_PluginInfo.PortTips.push_back("CV 3");	
	m_PluginInfo.PortTips.push_back("CV 4");	
	
	for (int n=0; n<MAX_CHANNELS; n++)
	{
		m_ChannelVal[n]=0.0f;		
	}
}

ControllerPlugin::~ControllerPlugin()
{
}

PluginInfo &ControllerPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *ControllerPlugin::CreateGUI()
{
	m_GUI = new ControllerPluginGUI(m_PluginInfo.Width,
								  	    m_PluginInfo.Height,
										this,m_HostInfo);
	m_GUI->hide();
	return m_GUI;
}

void ControllerPlugin::Execute()
{
	for(int c=0; c<m_Num; c++)
	{
		for (int n=0; n<m_HostInfo->BUFSIZE; n++)
		{			
			SetOutput(c,n,m_ChannelVal[c]);
		}
	}
}

void ControllerPlugin::SetNum(int n)
{
	// once to clear the connections with the current info
	UpdatePluginInfoWithHost();
	
	if (m_Num<n)
	{
		char t[256];
		sprintf(t,"CV %d",n);
		m_PluginInfo.PortTips.push_back(t);
		AddOutput();
		m_PluginInfo.NumOutputs++;
	}
	else
	{
		vector<string>::iterator i=m_PluginInfo.PortTips.end();
		m_PluginInfo.PortTips.erase(i--);
		RemoveOutput();
		m_PluginInfo.NumOutputs--;
	}
	
	m_Num=n;
	m_PluginInfo.NumOutputs=n;
	
	// do the actual update
	UpdatePluginInfoWithHost();	
}

// use with caution
void ControllerPlugin::Clear()
{
	m_PluginInfo.PortTips.clear();
	RemoveAllOutputs();
	m_PluginInfo.NumOutputs=0;
}

void ControllerPlugin::StreamOut(ostream &s)
{
	s<<m_Version<<" ";
	
	switch (m_Version)
	{
		case 1:
		{
			for (int n=0; n<4; n++)
			{
				s<<m_ChannelVal[n]<<" ";
			}
		} break;
		
		case 2:
		{
			s<<m_Num<<" ";			
			for (int n=0; n<m_Num; n++)
			{
				s<<m_ChannelVal[n]<<" ";
			}
			//s<<*(ControllerPluginGUI*)m_GUI<<" ";
			((ControllerPluginGUI*)m_GUI)->StreamOut(s);
		} break;
		
		case 3:
		{
			s<<m_Num<<" ";			
			for (int n=0; n<m_Num; n++)
			{
				s<<m_ChannelVal[n]<<" ";
			}
			((ControllerPluginGUI*)m_GUI)->StreamOut(s);
		} break;
	}
}

void ControllerPlugin::StreamIn(istream &s)
{
	int version;
	s>>version;
	
	switch (version)
	{
		case 1:
		{
			for (int n=0; n<4; n++)
			{
				s>>m_ChannelVal[n];
			}
		} break;
		
		case 2:
		{
			Clear();
		
			s>>m_Num;
			
			for (int n=0; n<m_Num; n++)
			{
				s>>m_ChannelVal[n];
			}
			
			s>>*(ControllerPluginGUI*)m_GUI;								
			
			// add the channels one by one
			for (int n=0; n<m_Num; n++)
			{						
				char t[256];
				sprintf(t,"CV %d",n);
				m_PluginInfo.PortTips.push_back(t);
				AddOutput();
			}
			
			m_PluginInfo.NumOutputs=m_Num;
			UpdatePluginInfoWithHost();	
			
		} break;
		
		case 3:
		{
			Clear();
		
			s>>m_Num;
			
			for (int n=0; n<m_Num; n++)
			{
				s>>m_ChannelVal[n];
			}
			
			((ControllerPluginGUI*)m_GUI)->StreamIn(s);
			
			// add the channels one by one
			for (int n=0; n<m_Num; n++)
			{						
				char t[256];
				sprintf(t,"CV %d",n);
				m_PluginInfo.PortTips.push_back(t);
				AddOutput();
			}
			
			m_PluginInfo.NumOutputs=m_Num;
			UpdatePluginInfoWithHost();	
			
		} break;
	}
}
