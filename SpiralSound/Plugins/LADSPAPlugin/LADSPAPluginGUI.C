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

#include <cstdio>
#include <cmath>
#include <dlfcn.h>
#include <vector>

#include <FL/fl_draw.h>
#include <FL/fl_draw.H>

#include "LADSPAPluginGUI.h"
#include "LADSPAInfo.h"

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

LADSPAPluginGUI::LADSPAPluginGUI(int w, int h,
                                 LADSPAPlugin *o,
                                 ChannelHandler *ch,
                                 const HostInfo *Info,
                                 const vector<LADSPAInfo::PluginEntry> &PVec) :
SpiralPluginGUI(w,h,o,ch)
{
	PluginList = PVec;

	int Width=20;
	int Height=100;

// Get maximum input port count
	m_GUICH->GetData("GetMaxInputPortCount",&(m_InData.MaxInputPortCount));

// Set up buffers for data transfer via ChannelHandler
	m_InData.InputPortNames = (char *)malloc(256 * m_InData.MaxInputPortCount);
	m_InData.InputPortSettings = (PortSettings *)malloc(sizeof(PortSettings) * m_InData.MaxInputPortCount);
	m_InData.InputPortValues = (float *)calloc(m_InData.MaxInputPortCount, sizeof(float));

	if (!(m_InData.InputPortNames && m_InData.InputPortSettings)) {
		cerr<<"Memory allocation error\n"<<endl;
	}

// Set up widgets
	m_Browser = new Fl_Choice(60,20,420,25,"Plugin:");
	m_Browser->callback((Fl_Callback *)cb_Select);

	m_Browser->add("(None)");
	for (vector<LADSPAInfo::PluginEntry>::iterator i=PluginList.begin();
		 i!=PluginList.end(); i++)
	{
		m_Browser->add(i->Name.c_str());
	}
	m_Browser->value(0);

	m_Name = new Fl_Box(10,50,480,20,"None");
	m_Name->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_Name->labelcolor(GUI_COLOUR);
	m_Name->labelsize(10);
	add(m_Name);

	m_Maker = new Fl_Box(10,70,480,20,"None");
	m_Maker->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_Maker->labelcolor(GUI_COLOUR);
	m_Maker->labelsize(10);
	add(m_Maker);

	m_InputScroll = new Fl_Scroll(10,110,480,150,"   Value            Default         Min               Max                 Clamp?      Port Name");
	m_InputScroll->labelsize(12);
	m_InputScroll->align(FL_ALIGN_TOP_LEFT);
	m_InputScroll->type(Fl_Scroll::VERTICAL);
	m_InputScroll->box(FL_DOWN_BOX);
	add(m_InputScroll);
	m_InputPack = new Fl_Pack(5,115,460,20,"");
	m_InputScroll->add(m_InputPack);

	m_PowerAmp = new Fl_Button(125,277,100,25,"PowerAmp");
	m_PowerAmp->labelsize(10);
	m_PowerAmp->value(0);
	m_PowerAmp->type(1);
	m_PowerAmp->callback((Fl_Callback*)cb_PowerAmp);
	add(m_PowerAmp);

	m_UpdateMinMax = new Fl_Button(230,277,100,25,"Update Settings");
	m_UpdateMinMax->labelsize(10);
	m_UpdateMinMax->callback((Fl_Callback*)cb_MinMax);
	add(m_UpdateMinMax);

	m_UpdateInputs = new Fl_Button(335,277,100,25,"Refresh");
	m_UpdateInputs->labelsize(10);
	m_UpdateInputs->value(1);
	m_UpdateInputs->type(1);
	add(m_UpdateInputs);

	m_OutputGain = new Fl_Knob(440,265,40,40,"Output Gain");
	m_OutputGain->color(GUI_COLOUR);
	m_OutputGain->type(Fl_Knob::DOTLIN);
	m_OutputGain->maximum(2);
	m_OutputGain->step(0.001);
	m_OutputGain->value(1.0);
	m_OutputGain->labelsize(10);
	m_OutputGain->callback((Fl_Callback*)cb_Gain);
	add(m_OutputGain);

	end();
}

LADSPAPluginGUI::~LADSPAPluginGUI(void)
{
	if (m_InData.InputPortNames)  free(m_InData.InputPortNames);
	if (m_InData.InputPortSettings) free(m_InData.InputPortSettings);
	Fl::check();
}

void LADSPAPluginGUI::UpdatePortDisplay(int n, float v)
{
	if (!m_UpdateInputs->value()) return;

	char temp[256];
	sprintf(temp,"%.4f",v);
	m_PortOutput[n]->value(temp);
}

void LADSPAPluginGUI::SetPortSettings(unsigned long n, float min, float max, bool clamp, float defolt)
{
	char temp[256];
	sprintf(temp,"%.4f",min);
	m_PortMin[n]->value(temp);

	sprintf(temp,"%.4f",max);
	m_PortMax[n]->value(temp);

	sprintf(temp, "%d",clamp);
	m_PortClamp[n]->value(atoi(temp));

	sprintf(temp, "%.4f",defolt);
	m_PortDefault[n]->value(temp);
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
	m_InputPack = new Fl_Pack(x()+5,y()+115,460,20,"");
	m_InputScroll->add(m_InputPack);

	m_PortOutput.clear();
	m_PackVec.clear();
	m_PortMin.clear();
	m_PortMax.clear();
	m_PortClamp.clear();
	m_PortDefault.clear();
}

void LADSPAPluginGUI::AddPortInfo(const char *Info)
{
	Fl_Group* NewGroup = new Fl_Group(0,0,430,18,"");
	NewGroup->box(FL_FLAT_BOX);
	m_InputPack->add(NewGroup);
	m_PackVec.push_back(NewGroup);

// Value
	Fl_Output* NewOutput = new Fl_Output(10,0,60,18,"");
	NewOutput->value(0);
	NewOutput->textsize(10);
	NewGroup->add(NewOutput);
	m_PortOutput.push_back(NewOutput);

// Fixed Value/Default
	Fl_Input* NewInput = new Fl_Input(72,0,60,18,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewGroup->add(NewInput);
	m_PortDefault.push_back(NewInput);

// Min
	NewInput = new Fl_Input(134,0,60,18,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewGroup->add(NewInput);
	m_PortMin.push_back(NewInput);

// Max
	NewInput = new Fl_Input(196,0,60,18,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewGroup->add(NewInput);
	m_PortMax.push_back(NewInput);

// Clamp
	Fl_Check_Button* NewCheckButton = new Fl_Check_Button(270,0,10,18,"");
	NewCheckButton->value(0);
	NewGroup->add(NewCheckButton);
	m_PortClamp.push_back(NewCheckButton);

// Port Name
	Fl_Box* NewText = new Fl_Box(322,0,10,18,"");
	NewText->label(Info);
	NewText->labelsize(10);
	NewText->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	NewGroup->add(NewText);

	NewGroup->redraw();
	m_InputPack->redraw();
	m_InputScroll->redraw();
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
	PortSettings settings;

	for (unsigned long p = 0; p < n; p++) {
		name = Plugin->GetPortName(p);
		settings = Plugin->GetPortSettings(p);
		AddPortInfo(name);
		SetPortSettings(p, settings.Min, settings.Max, settings.Clamp, settings.Default);
	}
}

void LADSPAPluginGUI::Update(void)
{
	m_GUICH->GetData("GetPluginIndex", &(m_InData.PluginIndex));
	m_GUICH->GetData("GetInputPortCount", &(m_InData.InputPortCount));
	m_GUICH->GetData("GetInputPortValues", m_InData.InputPortValues);

	m_Browser->value(m_InData.PluginIndex);

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

inline void LADSPAPluginGUI::cb_Select_i(Fl_Choice* o)
{
	if (o->value() == 0) {
	// "(None)" selected
		m_GUICH->SetCommand(LADSPAPlugin::CLEARPLUGIN);
	} else {
	// Plugin selected
		m_GUICH->Set("SetPluginIndex",o->value());
		m_GUICH->SetCommand(LADSPAPlugin::SELECTPLUGIN);
	}

// Wait until next update for plugin to be loaded etc.
	m_GUICH->Wait();

// Now get the new values to populate GUI controls
	m_GUICH->GetData("GetName", m_InData.Name);
	m_GUICH->GetData("GetMaker", m_InData.Maker);
	m_GUICH->GetData("GetInputPortCount", &(m_InData.InputPortCount));
	m_GUICH->GetData("GetInputPortNames", m_InData.InputPortNames);
	m_GUICH->GetData("GetInputPortSettings", m_InData.InputPortSettings);

	SetName((const char *)m_InData.Name);
	SetMaker((const char *)m_InData.Maker);

// Clear out port info, and refresh
	ClearPortInfo();

	for (unsigned long n = 0; n < m_InData.InputPortCount; n++) {
		AddPortInfo((const char *)(m_InData.InputPortNames + n * 256));
		SetPortSettings(n, m_InData.InputPortSettings[n].Min,
				m_InData.InputPortSettings[n].Max,
				m_InData.InputPortSettings[n].Clamp,
				m_InData.InputPortSettings[n].Default);
	}

	redraw();
}
void LADSPAPluginGUI::cb_Select(Fl_Choice* o)
{
	((LADSPAPluginGUI*)(o->parent()))->cb_Select_i(o);
}

inline void LADSPAPluginGUI::cb_MinMax_i(Fl_Button* o, void* v)
{
	unsigned long n=0;
	for (vector<Fl_Input*>::iterator i=m_PortMin.begin();
		 i!=m_PortMin.end(); i++)
	{
		m_InData.InputPortSettings[n].Min = atof((*i)->value());
		n++;
	}
	n=0;
	for (vector<Fl_Input*>::iterator i=m_PortMax.begin();
		 i!=m_PortMax.end(); i++)
	{
		m_InData.InputPortSettings[n].Max = atof((*i)->value());
		n++;
	}
	n=0;
	for (vector<Fl_Check_Button*>::iterator i=m_PortClamp.begin();
		 i!=m_PortClamp.end(); i++)
	{
		m_InData.InputPortSettings[n].Clamp = (bool)((*i)->value());
		n++;
	}
	n=0;
	for (vector<Fl_Input*>::iterator i=m_PortDefault.begin();
		 i!=m_PortDefault.end(); i++)
	{
		m_InData.InputPortSettings[n].Default = atof((*i)->value());
		n++;
	}

	m_GUICH->SetData("SetInputPortSettings", m_InData.InputPortSettings);
	m_GUICH->SetCommand(LADSPAPlugin::SETPORTSETTINGS);
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
