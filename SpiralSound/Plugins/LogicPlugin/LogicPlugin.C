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
#include "LogicPlugin.h"
#include "LogicPluginGUI.h"
#include <FL/Fl_Button.h>
#include "SpiralIcon.xpm"
#include "../../NoteTable.h"

extern "C" {
SpiralPlugin* CreateInstance()
{
	return new LogicPlugin;
}

char** GetIcon()
{
	return SpiralIcon_xpm;
}

int GetID()
{
	return 50;
}

string GetGroupName()
{
	return "Maths/Logic";
}
}

///////////////////////////////////////////////////////

LogicPlugin::LogicPlugin() :
m_Operator(AND),
m_Constant(0)
{
	m_PluginInfo.Name="Logic";
	m_PluginInfo.Width=75;
	m_PluginInfo.Height=100;
	m_PluginInfo.NumInputs=2;
	m_PluginInfo.NumOutputs=1;
	m_PluginInfo.PortTips.push_back("Input 1");	
	m_PluginInfo.PortTips.push_back("Input 2");	
	m_PluginInfo.PortTips.push_back("Output");
		
	m_AudioCH->Register("Operator",(int*)&m_Operator);
	m_AudioCH->Register("Constant",&m_Constant);
}

LogicPlugin::~LogicPlugin()
{
}

PluginInfo &LogicPlugin::Initialise(const HostInfo *Host)
{	
	return SpiralPlugin::Initialise(Host);
}

SpiralGUIType *LogicPlugin::CreateGUI()
{
	return new LogicPluginGUI(m_PluginInfo.Width,
						     m_PluginInfo.Height,
							 this,m_AudioCH,m_HostInfo);
}

void LogicPlugin::Execute()
{	
	float Freq=0, OldFreq=0;
	
	switch (m_Operator) 
	{
		case AND : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if (GetInput(0,n)>0 && GetInput(1,n)>0) SetOutput(0,n,1.0f);
				else SetOutput(0,n,-1.0f);
			} 
			break;
		case OR : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if (GetInput(0,n)>0 || GetInput(1,n)>0) SetOutput(0,n,1.0f);
				else SetOutput(0,n,-1.0f);
			} 
			break;
		case NOT : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if (GetInput(0,n)>0) SetOutput(0,n,-1.0f);
				else SetOutput(0,n,1.0f);
			} 
			break;
		case NAND : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if (!(GetInput(0,n)>0 && GetInput(1,n)>0)) SetOutput(0,n,1.0f);
				else SetOutput(0,n,-1.0f);
			} 
			break;
		case NOR : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if (!(GetInput(0,n)>0) && !(GetInput(1,n)>0)) SetOutput(0,n,1.0f);
				else SetOutput(0,n,-1.0f);
			} 
			break;
		case XOR : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if ((GetInput(0,n)>0 || GetInput(1,n)>0) &&
				   !(GetInput(0,n)>0 && GetInput(1,n)>0)) SetOutput(0,n,1.0f);
				else SetOutput(0,n,-1.0f);
			} 
			break;
		case XNOR : 
			for (int n=0; n<m_HostInfo->BUFSIZE; n++)
			{
				if ((GetInput(0,n)>0 && GetInput(1,n)>0) ||
				   (!(GetInput(0,n)>0) && !(GetInput(1,n)>0)))  SetOutput(0,n,1.0f);
				else SetOutput(0,n,-1.0f);
			} 
			break;
	}
}

void LogicPlugin::ExecuteCommands()
{
}
	
void LogicPlugin::StreamOut(ostream &s) 
{
	s<<m_Version<<endl;
	s<<m_Constant<<" ";
}

void LogicPlugin::StreamIn(istream &s) 
{
	int version;
	s>>version;
	s>>m_Constant;
}
