/*  SpiralPlugin
 *  Copyleft (C) 2000 David Griffiths <dave@pawfal.org>
 *  LADSPA Plugin by Nicolas Noble <nicolas@nobis-crew.org>
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

#include "LADSPAPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include "ladspa.h"
#include <cstdio>
#include <cmath>
#include <dlfcn.h>
#include <vector>
#include "utils.h"

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

LADSPAPluginGUI::LADSPAPluginGUI(int w, int h,LADSPAPlugin *o, ChannelHandler *ch,const HostInfo *Info, const vector<LPluginInfo> &PVec) :
SpiralPluginGUI(w,h,o,ch)
{
	PluginList=PVec;

	m_Filename="None";

	int Width=20;
	int Height=100;

	m_Browser= new Fl_Hold_Browser(5,20,290,260,"LADSPA Plugins");
	m_Browser->callback((Fl_Callback*)cb_Select);

	for (vector<LPluginInfo>::iterator i=PluginList.begin();
		 i!=PluginList.end(); i++)
	{
		m_Browser->add((*i).Name.c_str());
	}

// Get maximum input port count
	m_GUICH->GetData("GetMaxInputPortCount",&(m_InData.MaxInputPortCount));

// Set up buffers for data transfer via ChannelHandler
	m_InData.InputPortNames = (char *)malloc(256 * m_InData.MaxInputPortCount);
	m_InData.InputPortRanges = (PortRange *)malloc(sizeof(PortRange) * m_InData.MaxInputPortCount);
	m_InData.InputPortValues = (float *)malloc(sizeof(float) * m_InData.MaxInputPortCount);

	if (!(m_InData.InputPortNames && m_InData.InputPortRanges)) {
		cerr<<"Memory allocation error\n"<<endl;
	}

	m_InputScroll = new Fl_Scroll(300,80,290,150,"  Value   Min    Max   Clamp?");
	m_InputScroll->align(FL_ALIGN_TOP_LEFT);
	m_InputScroll->type(Fl_Scroll::VERTICAL);
	m_InputScroll->box(FL_DOWN_BOX);
	add(m_InputScroll);
	m_InputPack = new Fl_Pack(300,85,300,100,"");
	m_InputScroll->add(m_InputPack);

	m_Name = new Fl_Box(290,20,10,20,"None");
	m_Name->align(FL_ALIGN_RIGHT);
	m_Name->labelcolor(GUI_COLOUR);
	m_Name->labelsize(10);
	add(m_Name);

	m_Maker = new Fl_Box(290,40,10,20,"None");
	m_Maker->align(FL_ALIGN_RIGHT);
	m_Maker->labelcolor(GUI_COLOUR);
	m_Maker->labelsize(10);
	add(m_Maker);

	m_OutputGain = new Fl_Knob(545,240,40,40,"Output Gain");
	m_OutputGain->color(GUI_COLOUR);
	m_OutputGain->type(Fl_Knob::DOTLIN);
	m_OutputGain->maximum(2);
	m_OutputGain->step(0.001);
	m_OutputGain->value(1.0);
	m_OutputGain->labelsize(10);
	m_OutputGain->callback((Fl_Callback*)cb_Gain);
	add(m_OutputGain);

	m_UpdateInputs = new Fl_Button(480,252,50,25,"Refresh");
	m_UpdateInputs->labelsize(10);
	m_UpdateInputs->value(1);
	m_UpdateInputs->type(1);
	add(m_UpdateInputs);

	m_UpdateMinMax = new Fl_Button(400,252,85,25,"Update Min/Max");
	m_UpdateMinMax->labelsize(10);
	m_UpdateMinMax->callback((Fl_Callback*)cb_MinMax);
	add(m_UpdateMinMax);

	m_PowerAmp = new Fl_Button(320,252,80,25,"PowerAmp");
	m_PowerAmp->labelsize(10);
	m_PowerAmp->value(0);
	m_PowerAmp->type(1);
	m_PowerAmp->callback((Fl_Callback*)cb_PowerAmp);
	add(m_PowerAmp);

	end();
}

LADSPAPluginGUI::~LADSPAPluginGUI(void)
{
	if (m_InData.InputPortNames)  free(m_InData.InputPortNames);
	if (m_InData.InputPortRanges) free(m_InData.InputPortRanges);
	Fl::check();
}

void LADSPAPluginGUI::UpdatePortDisplay(int n, float v)
{
	if (!m_UpdateInputs->value()) return;

	char temp[256];
	sprintf(temp,"%.4f",v);
	m_PortOutput[n]->value(temp);
}

void LADSPAPluginGUI::SetMinMax(int n, float min, float max, bool clamp)
{
	char temp[256];
	sprintf(temp,"%.4f",min);
	m_PortMin[n]->value(temp);

	sprintf(temp,"%.4f",max);
	m_PortMax[n]->value(temp);

	sprintf(temp, "%d",clamp);
	m_PortClamp[n]->value(atoi(temp));
}

void LADSPAPluginGUI::GetMinMax(int n, float &min, float &max, bool &clamp)
{
	min = atof(m_PortMin[n]->value());
	max = atof(m_PortMax[n]->value());
	clamp = m_PortClamp[n]->value();
}

void LADSPAPluginGUI::SetName(const char *s)
{
	m_Name->label(s);
}

void LADSPAPluginGUI::SetMaker(const char *s)
{
	m_Maker->label(s);
}

void LADSPAPluginGUI::ClearPortInfo()
{
	for (vector<Fl_Group*>::iterator i=m_PackVec.begin();
		 i!=m_PackVec.end(); i++)
	{
		m_InputPack->remove((*i));
	}

	m_InputScroll->remove(m_InputPack);
	delete m_InputPack;
	m_InputPack = new Fl_Pack(x()+300,y()+85,300,100,"");
	m_InputScroll->add(m_InputPack);

	m_PortOutput.clear();
	m_PackVec.clear();
	m_PortMin.clear();
	m_PortMax.clear();
	m_PortClamp.clear();
}

void LADSPAPluginGUI::AddPortInfo(const char *Info)
{
	Fl_Group* NewGroup = new Fl_Group(0,85,150,15,"");
	NewGroup->box(FL_FLAT_BOX);
	m_InputPack->add(NewGroup);
	m_PackVec.push_back(NewGroup);

	Fl_Box* NewText = new Fl_Box(100,85,80,15,"");
	NewText->label(Info);
	NewText->labelsize(8);
	NewText->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	NewGroup->add(NewText);

	Fl_Output* NewOutput = new Fl_Output(5,85,20,15,"");
	NewOutput->value(0);
	NewOutput->textsize(10);
	NewGroup->add(NewOutput);
	m_PortOutput.push_back(NewOutput);

	Fl_Input* NewInput = new Fl_Input(26,85,20,15,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewGroup->add(NewInput);
	m_PortMin.push_back(NewInput);

	NewInput = new Fl_Input(47,85,20,15,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewGroup->add(NewInput);
	m_PortMax.push_back(NewInput);

	Fl_Check_Button* NewCheckButton = new Fl_Check_Button(80,85,15,15,"");
	NewCheckButton->value(0);
	NewGroup->add(NewCheckButton);
	m_PortClamp.push_back(NewCheckButton);

	NewGroup->redraw();
	m_InputPack->redraw();
	m_InputScroll->redraw();

	redraw();
}

void LADSPAPluginGUI::UpdateValues(SpiralPlugin *o)
{
	LADSPAPlugin* Plugin = (LADSPAPlugin*)o;
	m_OutputGain->value(Plugin->GetGain());
	m_PowerAmp->value(Plugin->GetAmped());
	SetName(Plugin->GetName());
	SetMaker(Plugin->GetMaker());

	unsigned long n = Plugin->GetInputPortCount();
	const char *name;
	PortRange range;

	for (unsigned long p = 0; p < n; p++) {
		name = Plugin->GetPortName(p);
		range = Plugin->GetPortRange(p);
		AddPortInfo(name);
		SetMinMax(p, range.Min, range.Max, range.Clamp);
	}
}

void LADSPAPluginGUI::Update(void)
{
	m_GUICH->GetData("GetInputPortCount", &(m_InData.InputPortCount));
	m_GUICH->GetData("GetInputPortValues", m_InData.InputPortValues);
	
	for (unsigned long n=0; n < m_InData.InputPortCount; n++) {
		UpdatePortDisplay(n, m_InData.InputPortValues[n]);
	}
}

inline void LADSPAPluginGUI::cb_Gain_i(Fl_Knob* o, void* v)
{
	m_GUICH->Set("SetGain",(float)(o->value()));
}
void LADSPAPluginGUI::cb_Gain(Fl_Knob* o, void* v)
{
	((LADSPAPluginGUI*)(o->parent()))->cb_Gain_i(o,v);
}

inline void LADSPAPluginGUI::cb_Select_i(Fl_Hold_Browser* o)
{
	m_Filename=PluginList[o->value()-1].Filename;
	m_Label=PluginList[o->value()-1].Label;

	m_GUICH->Set("SetPluginIndex",o->value()-1);
	m_GUICH->SetCommand(LADSPAPlugin::SELECTPLUGIN);

// Wait until next update for plugin to be loaded etc.
	m_GUICH->Wait();

// Now get the new values to populate GUI controls
	m_GUICH->GetData("GetName", m_InData.Name);
	m_GUICH->GetData("GetMaker", m_InData.Maker);
	m_GUICH->GetData("GetInputPortCount", &(m_InData.InputPortCount));
	m_GUICH->GetData("GetInputPortNames", m_InData.InputPortNames);
	m_GUICH->GetData("GetInputPortRanges", m_InData.InputPortRanges);

	SetName((const char *)m_InData.Name);
	SetMaker((const char *)m_InData.Maker);

// Clear out port info, and refresh
	ClearPortInfo();

	for (unsigned long n = 0; n < m_InData.InputPortCount; n++) {
		AddPortInfo((const char *)(m_InData.InputPortNames + n * 256));
		SetMinMax(n, m_InData.InputPortRanges[n].Min,
		             m_InData.InputPortRanges[n].Max,
			     m_InData.InputPortRanges[n].Clamp);
	}
}
void LADSPAPluginGUI::cb_Select(Fl_Hold_Browser* o)
{
	((LADSPAPluginGUI*)(o->parent()))->cb_Select_i(o);
}

inline void LADSPAPluginGUI::cb_MinMax_i(Fl_Button* o, void* v)
{
	unsigned long n=0;
	for (vector<Fl_Input*>::iterator i=m_PortMin.begin();
		 i!=m_PortMin.end(); i++)
	{
		m_InData.InputPortRanges[n].Min = atof((*i)->value());
		n++;
	}
	n=0;
	for (vector<Fl_Input*>::iterator i=m_PortMax.begin();
		 i!=m_PortMax.end(); i++)
	{
		m_InData.InputPortRanges[n].Max = atof((*i)->value());
		n++;
	}
	n=0;
	for (vector<Fl_Check_Button*>::iterator i=m_PortClamp.begin();
		 i!=m_PortClamp.end(); i++)
	{
		m_InData.InputPortRanges[n].Clamp = (bool)((*i)->value());
		n++;
	}

	m_GUICH->SetData("SetInputPortRanges", m_InData.InputPortRanges);
	m_GUICH->SetCommand(LADSPAPlugin::SETRANGES);
}
void LADSPAPluginGUI::cb_MinMax(Fl_Button* o, void* v)
{
	((LADSPAPluginGUI*)(o->parent()))->cb_MinMax_i(o,v);
}

inline void LADSPAPluginGUI::cb_PowerAmp_i(Fl_Button* o, void* v)
{
	m_GUICH->Set("SetAmped",(bool)(o->value()));
}
void LADSPAPluginGUI::cb_PowerAmp(Fl_Button* o, void* v)
{
	((LADSPAPluginGUI*)(o->parent()))->cb_PowerAmp_i(o,v);
}
