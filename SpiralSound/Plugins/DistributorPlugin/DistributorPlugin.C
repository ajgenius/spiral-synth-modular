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
#include "DistributorPlugin.h"
#include "DistributorPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"

extern "C" {
SpiralPlugin* SpiralPlugin_CreateInstance()
{
	return new DistributorPlugin;
}

char** SpiralPlugin_GetIcon()
{
	return SpiralIcon_xpm;
}

int SpiralPlugin_GetID()
{
	return 0x0056;
}

string SpiralPlugin_GetGroupName()
{
	return "Control";
}
}

///////////////////////////////////////////////////////

DistributorPlugin::DistributorPlugin()
{
	m_PluginInfo.Name="Distributor";
	m_PluginInfo.Width=220;
	m_PluginInfo.Height=250;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=8;
	m_PluginInfo.PortTips.push_back("Stream (A)");	
	m_PluginInfo.PortTips.push_back("Switcher (B)");	
	m_PluginInfo.PortTips.push_back("Out A1");
	m_PluginInfo.PortTips.push_back("Out B1");
	m_PluginInfo.PortTips.push_back("Out A2");
	m_PluginInfo.PortTips.push_back("Out B2");
	m_PluginInfo.PortTips.push_back("Out A3");
	m_PluginInfo.PortTips.push_back("Out B3");
	m_PluginInfo.PortTips.push_back("Out A4");
	m_PluginInfo.PortTips.push_back("Out B4");
}

DistributorPlugin::~DistributorPlugin()
{
}

PluginInfo &DistributorPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *DistributorPlugin::CreateGUI()
{
	return NULL;
}

void DistributorPlugin::Execute()
{
	if (! m_Defined)
	{
		m_Triggeronce=false;
		m_Previous=0;
		m_Now=0;
		m_Defined=true;
		m_ChannelSelect=0;
		m_trigdelay=0;
	}
	for (int n=0; n<m_HostInfo->BUFSIZE; n++)
	{
		// detect the trigger
		if ((GetInput(1,n))!=0)
		{
			if (GetInput(0,n)!=m_Previous)
			{	
				GetOutputBuf(m_ChannelSelect+1)->Zero();
				m_ChannelSelect=m_ChannelSelect+2;
				if (m_ChannelSelect>6)
				{
					m_ChannelSelect=0;
				}
				//zero the outputs first...
				GetOutputBuf(m_ChannelSelect)->Zero();
				GetOutputBuf(m_ChannelSelect+1)->Zero();
				//and copy the inputs as well
				if (InputExists(0)) //unless you want segmentation faults...
				GetOutputBuf(m_ChannelSelect)->Mix(*GetInput(0),0);
				GetOutputBuf(m_ChannelSelect+1)->Mix(*GetInput(1),0);
				//now save the present pitch value, because we'll need it next time around.
				m_Previous=GetInput(0,n);
				m_Triggeronce=true;
			}	
			else if (m_Triggeronce==false)
			{
				//just copy the trigger in this case.
				GetOutputBuf(m_ChannelSelect+1)->Zero();
				GetOutputBuf(m_ChannelSelect+1)->Mix(*GetInput(1),0);
				m_Triggeronce=true;
			}
		}
		else 
		{
			m_Triggeronce=false;
			m_trigdelay++;
			if (m_trigdelay>=10)
			{
				GetOutputBuf(m_ChannelSelect+1)->Zero();
				m_trigdelay=0;
			}
		}
		
	}

}




