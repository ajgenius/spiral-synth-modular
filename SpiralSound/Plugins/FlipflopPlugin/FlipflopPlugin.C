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
#include "FlipflopPlugin.h"
#include "FlipflopPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new FlipflopPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 46;
}

string GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

FlipflopPlugin::FlipflopPlugin() :
m_Count(4),
m_Current(0),
m_Triggered(false),
m_CurrentLevel(1.0f)
{
	m_PluginInfo.Name="Flipflop";
	m_PluginInfo.Width=90;
	m_PluginInfo.Height=80;
	m_PluginInfo.NumInputs=1;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input");	
	m_PluginInfo.PortTips.push_back("Output");

	m_AudioCH->Register("Count",&m_Count);
}

FlipflopPlugin::~FlipflopPlugin()
{
}

PluginInfo &FlipflopPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *FlipflopPlugin::CreateGUI()
{
	return NULL;
}

void FlipflopPlugin::Execute()
{	
	bool Triggered;
	
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		if (GetInput(0,n)>0)
		{
			if(!m_Triggered)
			{
				m_Triggered=true;
				m_CurrentLevel=-m_CurrentLevel;
			}
		}
		else
		{
			if (m_Triggered)
			{
				m_Triggered=false;
			}
		}
		
		SetOutput(0,n,m_CurrentLevel);
	}
}

void FlipflopPlugin::ExecuteCommands()
{
}
	
void FlipflopPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<endl;
}

void FlipflopPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
}
