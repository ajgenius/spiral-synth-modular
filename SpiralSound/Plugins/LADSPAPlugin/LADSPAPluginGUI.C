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
#include <cstring>
#include <dlfcn.h>
#include <vector>
#include <algorithm>

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
	m_PluginList = PVec;

	int Width=20;
	int Height=100;

// Get maximum input port count
	m_GUICH->GetData("GetMaxInputPortCount",&(m_MaxInputPortCount));

// Set up buffers for data transfer via ChannelHandler
	m_InputPortNames = (char *)malloc(256 * m_MaxInputPortCount);
	m_InputPortSettings = (PortSettings *)malloc(sizeof(PortSettings) * m_MaxInputPortCount);
	m_InputPortValues = (PortValues *)calloc(m_MaxInputPortCount, sizeof(PortValues));
	m_InputPortDefaults = (float *)calloc(m_MaxInputPortCount, sizeof(float));

	if (!(m_InputPortNames && m_InputPortSettings &&
	      m_InputPortValues && m_InputPortDefaults)) {
		cerr<<"Memory allocation error\n"<<endl;
	}

// Set up widgets
	m_NameLabel = new Fl_Box(10,20,480,15,"None");
	m_NameLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_NameLabel->labelcolor(GUI_COLOUR);
	m_NameLabel->labelsize(12);
	add(m_NameLabel);

	m_MakerLabel = new Fl_Box(10,40,480,15,"None");
	m_MakerLabel->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	m_MakerLabel->labelcolor(GUI_COLOUR);
	m_MakerLabel->labelsize(12);
	add(m_MakerLabel);

	m_Tab = new Fl_Tabs(5,60,490,255,"");
	m_Tab->callback((Fl_Callback *)cb_TabChange);
	add(m_Tab);

	m_ControlGroup = new Fl_Group(0,80,490,255,"Control");
	m_ControlGroup->labelsize(12);

	m_ControlScroll = new Fl_Scroll(10,85,480,210,"");
	m_ControlScroll->align(FL_ALIGN_TOP_LEFT);
	m_ControlScroll->type(Fl_Scroll::VERTICAL);
	m_ControlScroll->box(FL_DOWN_BOX);
	m_ControlGroup->add(m_ControlScroll);

	m_ControlPack = new Fl_Pack(5,90,460,50,"");
	m_ControlScroll->add(m_ControlPack);

	m_SetupGroup = new Fl_Group(0,80,490,255,"Setup");
	m_SetupGroup->labelsize(12);

	m_Browser = new Fl_Choice(50,85,440,22,"Plugin:");
	m_Browser->labelsize(12);
	m_Browser->textsize(12);
	m_Browser->callback((Fl_Callback *)cb_Select);

	m_Browser->add("(None)");
	for (vector<LADSPAInfo::PluginEntry>::iterator i=m_PluginList.begin();
	     i!=m_PluginList.end(); i++)
	{
		m_Browser->add(i->Name.c_str());
	}
	m_Browser->value(0);

	m_SetupGroup->add(m_Browser);

	m_InputScroll = new Fl_Scroll(10,130,480,145,"   Value         Default        Min               Max               Clamp?    Port Name");
	m_InputScroll->labelsize(12);
	m_InputScroll->align(FL_ALIGN_TOP_LEFT);
	m_InputScroll->type(Fl_Scroll::VERTICAL);
	m_InputScroll->box(FL_DOWN_BOX);

	m_InputPack = new Fl_Pack(5,135,460,26,"");
	m_InputScroll->add(m_InputPack);

	m_SetupGroup->add(m_InputScroll);

	m_UpdateInputs = new Fl_Check_Button(10,282,120,25,"Update input values?");
	m_UpdateInputs->labelsize(12);
	m_UpdateInputs->value(true);
	m_UpdateInputs->callback((Fl_Callback *)cb_UpdateInputs);

	m_SetupGroup->add(m_UpdateInputs);

	m_Tab->add(m_ControlGroup);
	m_Tab->add(m_SetupGroup);
	m_Tab->value(m_SetupGroup);

	m_TabIndex = 1;
	m_PortIndex = 0;

	end();
}

LADSPAPluginGUI::~LADSPAPluginGUI(void)
{
	if (m_InputPortNames)  free(m_InputPortNames);
	if (m_InputPortSettings) free(m_InputPortSettings);
	if (m_InputPortValues) free(m_InputPortValues);
	if (m_InputPortDefaults) free(m_InputPortDefaults);
	Fl::check();
}

// Rearrange knobs depending on connections
// Knobs corresponding to connected ports are hidden,
// the rest are shown
void LADSPAPluginGUI::UpdateDefaultAdjustControls(void)
{
	int column = 0;

// First, clear out all groups in Pack
// We need to remove all the knobs first, or they'll go
// with the group. Which would be bad.
	while (m_ControlPack->children() > 0) {
		Fl_Group *Group = (Fl_Group *)m_ControlPack->child(0);
		while (Group->children() > 0) {
			Fl_Knob *Knob = (Fl_Knob *)Group->child(0);
			Group->remove(Knob);
		}
		m_ControlPack->remove(Group);
	}

	Fl_Group *NewGroup = new Fl_Group(0,0,460,65,"");
	NewGroup->box(FL_FLAT_BOX);
	m_ControlPack->add(NewGroup);

	for (unsigned long p = 0; p < m_InputPortCount; p++)
	{
		if (!m_InputPortValues[p].Connected) {
			m_PortDefaultAdjust[p]->position(50 + column * 105, 0);
			m_PortDefaultAdjust[p]->show();
			NewGroup->add(m_PortDefaultAdjust[p]);

			column++;
			if ((column > 3) && (p < m_InputPortCount - 1)) {
				NewGroup = new Fl_Group(0,0,460,65,"");
				NewGroup->box(FL_FLAT_BOX);
				m_ControlPack->add(NewGroup);

				column = 0;
			}
		} else {
			m_PortDefaultAdjust[p]->hide();
		}
	}

	m_ControlScroll->redraw();
}

// This lot is only done on patch load
void LADSPAPluginGUI::UpdateValues(SpiralPlugin *o)
{
	LADSPAPlugin* Plugin = (LADSPAPlugin*)o;
	SetPluginIndex(Plugin->GetPluginIndex());
	SetName(Plugin->GetName());
	SetMaker(Plugin->GetMaker());
	SetTabIndex(Plugin->GetTabIndex());
	SetUpdateInputs(Plugin->GetUpdateInputs());

	m_InputPortCount = Plugin->GetInputPortCount();
	const char *name;
	PortSettings settings;
	float defolt;

	for (unsigned long p = 0; p < m_InputPortCount; p++) {
		name = Plugin->GetInputPortName(p);
		settings = Plugin->GetInputPortSettings(p);
		defolt = Plugin->GetInputPortDefault(p);
		AddPortInfo(name);
		SetPortSettings(p, settings.Min, settings.Max, settings.Clamp, defolt);
		SetDefaultAdjust(p);
	}

	UpdateDefaultAdjustControls();

	m_PortIndex = m_InputPortCount;
}

// ****************************************************************************
// **                    Protected Member Functions                          **
// ****************************************************************************

const string LADSPAPluginGUI::GetHelpText(const string &loc)
{
//	if (loc == "DE") {
//	} else if (loc == "FR") {
//	} else {
	// Default to English?
		return string("LADSPA Plugin\n")
		            + "\n"
		            + "This plugin allows you to use any LADSPA plugin in SSM.\n"
		            + "\n"
		            + "It grows or shrinks the device GUI to allow you to connect\n"
		            + "up the ports as any other native SSM plugin, so you can\n"
		            + "seamlessly use the plugins as part of your layouts.\n"
		            + "\n"
		            + "The GUI window has two tabbed sections, Control and Setup.\n"
		            + "\n"
		            + "Setup is used to choose which LADSPA plugin to use, and\n"
		            + "allows you to configure port information.\n"
		            + "\n"
		            + "Once you have chosen a plugin, a row will appear for each\n"
		            + "input port:\n"
		            + "\n"
		            + "Value\n"
		            + "    The value being input to the port from a connection.\n"
		            + "Default\n"
		            + "    The value used as input if there is no connection. If"
		            + "    the port is connected, the default will use the value.\n"
		            + "    Upon disconnection, it will retain the last value\n"
		            + "    received.\n"
		            + "Min, Max\n"
		            + "    The range of values to scale a connected signal to,\n"
		            + "    assuming the signal is in the range -1.0 to +1.0.\n"
		            + "Clamp\n"
		            + "    Whether to scale inputs - if unchecked, the input is\n"
		            + "    not scaled."
		            + "Port Name"
		            + "    The name of the port, as supplied by the plugin.\n"
		            + "\n"
		            + "The Control tab will display a control knob for each port\n"
		            + "that is not connected. This allows adjustment of input\n"
		            + "directly.";
//	}
}

// ****************************************************************************
// **                      Private Member Functions                          **
// ****************************************************************************

void LADSPAPluginGUI::SetTabIndex(int index)
{
	m_TabIndex = index;
	if (m_TabIndex == 0) {
		m_Tab->value(m_ControlGroup);
	} else {
		m_Tab->value(m_SetupGroup);
	}
}

void LADSPAPluginGUI::SetUpdateInputs(bool state)
{
	m_UpdateInputState = state;
	m_UpdateInputs->value(m_UpdateInputState);
}

void LADSPAPluginGUI::SetPluginIndex(unsigned long n)
{
	m_PluginIndex = n;
	m_Browser->value(m_PluginIndex);
}

void LADSPAPluginGUI::SetName(const char *s)
{
	m_NameLabel->label(s);
}

void LADSPAPluginGUI::SetMaker(const char *s)
{
	m_MakerLabel->label(s);
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

void LADSPAPluginGUI::SetDefaultAdjust(unsigned long n)
{
// Set default adjust knob
	float min = atof(m_PortMin[n]->value());
	float max = atof(m_PortMax[n]->value());
	float def = atof(m_PortDefault[n]->value());
	float value = ((max - min) > 0.0f) ? (def - min) / (max - min) : 0.5f;

	m_PortDefaultAdjust[n]->value(value);
}

void LADSPAPluginGUI::AddPortInfo(const char *Info)
{
	Fl_Group* NewGroup = new Fl_Group(0,0,460,24,"");
	NewGroup->box(FL_FLAT_BOX);
	m_InputPack->add(NewGroup);

// Value
	Fl_Output* NewOutput = new Fl_Output(10,0,60,18,"");
	NewOutput->value(0);
	NewOutput->textsize(10);
	NewOutput->color(FL_BACKGROUND_COLOR);
	NewOutput->readonly(1);
	NewGroup->add(NewOutput);
	m_PortValue.push_back(NewOutput);

// Fixed Value/Default
	Fl_Input* NewInput = new Fl_Input(72,0,60,18,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_Default);
	NewGroup->add(NewInput);
	m_PortDefault.push_back(NewInput);

// Min
	NewInput = new Fl_Input(134,0,60,18,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_Min);
	NewGroup->add(NewInput);
	m_PortMin.push_back(NewInput);

// Max
	NewInput = new Fl_Input(196,0,60,18,"");
	NewInput->value(0);
	NewInput->textsize(10);
	NewInput->callback((Fl_Callback *)cb_Max);
	NewGroup->add(NewInput);
	m_PortMax.push_back(NewInput);

// Clamp
	Fl_Check_Button* NewCheckButton = new Fl_Check_Button(265,0,10,18,"");
	NewCheckButton->value(0);
	NewCheckButton->callback((Fl_Callback *)cb_Clamp);
	NewGroup->add(NewCheckButton);
	m_PortClamp.push_back(NewCheckButton);

// Port Name
	Fl_Box* NewText = new Fl_Box(320,0,10,18,"");
	NewText->label(Info);
	NewText->labelsize(10);
	NewText->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
	NewGroup->add(NewText);

	NewGroup->redraw();
	m_InputPack->redraw();
	m_InputScroll->redraw();

// Control knobs - these aren't displayed yet, as their display will depend
// on what is connected. All that is decided in UpdateControlKnobs()
	int len = strlen(Info);
	len -= 5;                  // Get rid of (CV), (AU) bit
	len = len > 20 ? 20 : len; // Truncate to fit
	char *label = (char *)malloc(len + 1);
	if (label)
	{
		strncpy(label, Info, len);
		label[len] = '\0';
	}
	m_PortDefaultAdjustLabels.push_back(label);

	Fl_Knob* NewKnob = new Fl_Knob(0,0,40,40,"");
	NewKnob->label(m_PortDefaultAdjustLabels[m_PortDefaultAdjustLabels.size() - 1]);
	NewKnob->labelsize(10);
	NewKnob->color(GUI_COLOUR);
	NewKnob->maximum(1.0f);
	NewKnob->step(0.001f);
	NewKnob->callback((Fl_Callback *)cb_DefaultAdjust);
	NewKnob->hide();
	m_PortDefaultAdjust.push_back(NewKnob);
}

// This is done all the time
void LADSPAPluginGUI::Update(void)
{
	char temp[256];
	bool state_changed = false;

	m_GUICH->GetData("GetInputPortCount", &(m_InputPortCount));
	m_GUICH->GetData("GetInputPortValues", m_InputPortValues);
	m_GUICH->GetData("GetInputPortDefaults", m_InputPortDefaults);

// Need to show that a connection is present
// regardless of Refresh being set
	for (unsigned long p = 0; p < m_InputPortCount; p++) {
	// Check if plugin connect state is different to GUI state
	// A readonly default implies connection
		if ((m_InputPortValues[p].Connected &&
		     !(m_PortDefault[p]->readonly())) ||
		    (m_PortDefault[p]->readonly() &&
		     !(m_InputPortValues[p].Connected))) {
			if (m_InputPortValues[p].Connected) {
			// Disable
				m_PortDefault[p]->readonly(1);
				m_PortDefault[p]->color(FL_BACKGROUND_COLOR);
			} else {
			// Enable
				m_PortDefault[p]->readonly(0);
				m_PortDefault[p]->color(FL_BACKGROUND2_COLOR);
			}

			sprintf(temp,"%.4f", m_InputPortDefaults[p]);
			m_PortDefault[p]->value(temp);

			SetDefaultAdjust(p);
			m_PortDefault[p]->redraw();

			state_changed = true;
		}

	// Only update values if Refresh is set
		if (m_UpdateInputs->value()) {
			sprintf(temp,"%.4f", m_InputPortValues[p].Value);
			m_PortValue[p]->value(temp);

			if (m_InputPortValues[p].Connected) {
				sprintf(temp,"%.4f", m_InputPortDefaults[p]);
				m_PortDefault[p]->value(temp);
			}
		}
	}

// If a connection has been added/removed, we need to
// rearrange the knobs
	if (state_changed) UpdateDefaultAdjustControls();
}

void LADSPAPluginGUI::ClearPlugin(void)
{
	m_PluginIndex = 0;
	m_InputPortCount = 0;
	m_PortIndex = 0;

	m_GUICH->SetCommand(LADSPAPlugin::CLEARPLUGIN);
	m_GUICH->Wait();

// Clear out port info, and refresh
	m_InputScroll->remove(m_InputPack);
	delete m_InputPack;
	m_InputPack = new Fl_Pack(x()+5,y()+135,460,26,"");
	m_InputScroll->add(m_InputPack);

	m_PortValue.clear();
	m_PortMin.clear();
	m_PortMax.clear();
	m_PortClamp.clear();
	m_PortDefault.clear();
	m_PortDefaultAdjust.clear();

	for (vector<char *>::iterator i = m_PortDefaultAdjustLabels.begin();
	     i != m_PortDefaultAdjustLabels.end(); i++)
	{
		if (*i) free (*i);
	}
	m_PortDefaultAdjustLabels.clear();
}

void LADSPAPluginGUI::SelectPlugin(void)
{
// Now get the new values to populate GUI controls
	m_GUICH->GetData("GetName", m_Name);
	m_GUICH->GetData("GetMaker", m_Maker);
	m_GUICH->GetData("GetInputPortCount", &(m_InputPortCount));
	m_GUICH->GetData("GetInputPortNames", m_InputPortNames);
	m_GUICH->GetData("GetInputPortSettings", m_InputPortSettings);
	m_GUICH->GetData("GetInputPortDefaults", m_InputPortDefaults);

	SetName((const char *)m_Name);
	SetMaker((const char *)m_Maker);

	for (unsigned long p = 0; p < m_InputPortCount; p++) {
		AddPortInfo((const char *)(m_InputPortNames + p * 256));
		SetPortSettings(p, m_InputPortSettings[p].Min,
				   m_InputPortSettings[p].Max,
				   m_InputPortSettings[p].Clamp,
				   m_InputPortDefaults[p]);

		SetDefaultAdjust(p);
	}

	UpdateDefaultAdjustControls();
	m_PortIndex = m_InputPortCount;
}

// ****************************************************************************
// **                     Widget Callback Functions                          **
// ****************************************************************************

inline void LADSPAPluginGUI::cb_TabChange_i(Fl_Tabs *o)
{
	m_TabIndex = o->find(o->value());

	m_GUICH->SetData("SetTabIndex", &m_TabIndex);
	m_GUICH->SetCommand(LADSPAPlugin::SETTABINDEX);
}
void LADSPAPluginGUI::cb_TabChange(Fl_Tabs *o)
{       //                     GUI
	((LADSPAPluginGUI*)(o->parent()))->cb_TabChange_i(o);
}

inline void LADSPAPluginGUI::cb_Select_i(Fl_Choice* o)
{
	ClearPlugin();

	m_PluginIndex = o->value();

	if (o->value() != 0) {
	// Plugin selected
		m_GUICH->SetData("SetPluginIndex",&m_PluginIndex);
		m_GUICH->SetCommand(LADSPAPlugin::SELECTPLUGIN);
		m_GUICH->Wait();

		SelectPlugin();
	}

//	redraw();
}
void LADSPAPluginGUI::cb_Select(Fl_Choice* o)
{       //                     Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()))->cb_Select_i(o);
}

inline void LADSPAPluginGUI::cb_UpdateInputs_i(Fl_Check_Button* o)
{
	m_UpdateInputState = (bool)(o->value());

	m_GUICH->SetData("SetUpdateInputs", &m_UpdateInputState);
	m_GUICH->SetCommand(LADSPAPlugin::SETUPDATEINPUTS);
}
void LADSPAPluginGUI::cb_UpdateInputs(Fl_Check_Button* o)
{       //                     Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()))->cb_UpdateInputs_i(o);
}

inline void LADSPAPluginGUI::cb_Default_i(Fl_Input* o)
{
// Which Default was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortDefault.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortDefault[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_PortDefault.begin(),
		                                           m_PortDefault.end(),
		                                           o);
		m_PortIndex = distance(m_PortDefault.begin(), i);
	}

	m_Default = atof(o->value());
	m_Min = atof(m_PortMin[m_PortIndex]->value());
	m_Max = atof(m_PortMax[m_PortIndex]->value());

// If default is out of [Min, Max] range, stretch range
	if (m_Default < m_Min) {
		m_PortMin[m_PortIndex]->value(m_PortDefault[m_PortIndex]->value());
		m_PortMin[m_PortIndex]->redraw();
	} else if (m_Default > m_Max) {
		m_PortMax[m_PortIndex]->value(m_PortDefault[m_PortIndex]->value());
		m_PortMax[m_PortIndex]->redraw();
	}

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);
	m_GUICH->SetData("SetInputPortDefault", &m_Default);
	m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

// Set Default Adjust knob to corresponding position
	SetDefaultAdjust(m_PortIndex);
}
void LADSPAPluginGUI::cb_Default(Fl_Input* o)
{       //                  Group        Pack      Scroll    Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()))->cb_Default_i(o);
}

inline void LADSPAPluginGUI::cb_Min_i(Fl_Input* o)
{
// Which Min was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortMin.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortMin[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_PortMin.begin(),
		                                           m_PortMin.end(),
		                                           o);
		m_PortIndex = distance(m_PortMin.begin(), i);
	}

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);

// Check that min is really min and max is really max
	m_Min = atof(o->value());
	m_Max = atof(m_PortMax[m_PortIndex]->value());

	if (m_Min > m_Max) {
	// Swap min and max (need to set max as well)
		float min = m_Min;
		m_Min = m_Max;
		m_Max = min;

		m_GUICH->SetData("SetInputPortMax", &m_Max);
		m_GUICH->SetCommand(LADSPAPlugin::SETMAX);
		m_GUICH->Wait();

	// Swap displayed min and max
		char temp[256];
		strncpy(temp, m_PortMin[m_PortIndex]->value(), 256);
		m_PortMin[m_PortIndex]->value(m_PortMax[m_PortIndex]->value());
		m_PortMax[m_PortIndex]->value(temp);
		m_PortMin[m_PortIndex]->redraw();
		m_PortMax[m_PortIndex]->redraw();
	}
	m_GUICH->SetData("SetInputPortMin", &m_Min);
	m_GUICH->SetCommand(LADSPAPlugin::SETMIN);

// Clip default to range
	m_Default = atof(m_PortDefault[m_PortIndex]->value());
	if (m_Default < m_Min) {
		m_Default = m_Min;
		m_GUICH->SetData("SetInputPortDefault",&m_Default);

		m_GUICH->Wait();
		m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

	// Print to displayed default
		char temp[256];
		sprintf(temp, "%.4f", m_Default);
		m_PortDefault[m_PortIndex]->value(temp);
		m_PortDefault[m_PortIndex]->redraw();
	}

// Reposition Default Adjust knob to reflect new range
	SetDefaultAdjust(m_PortIndex);
}
void LADSPAPluginGUI::cb_Min(Fl_Input* o)
{       //                  Group        Pack      Scroll    Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()))->cb_Min_i(o);
}

inline void LADSPAPluginGUI::cb_Max_i(Fl_Input* o)
{
// Which Max was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortMax.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortMax[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Input *>::iterator i = std::find(m_PortMax.begin(),
		                                           m_PortMax.end(),
		                                           o);
		m_PortIndex = distance(m_PortMax.begin(), i);
	}

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);

// Check that min is really min and max is really max
	m_Max = atof(o->value());
	m_Min = atof(m_PortMin[m_PortIndex]->value());

	if (m_Min > m_Max) {
	// Swap min and max (need to set max as well)
		float max = m_Min;
		m_Min = m_Max;
		m_Max = max;

		m_GUICH->SetData("SetInputPortMin", &m_Min);
		m_GUICH->SetCommand(LADSPAPlugin::SETMIN);
		m_GUICH->Wait();

	// Swap displayed min and max
		char temp[256];
		strncpy(temp, m_PortMax[m_PortIndex]->value(), 256);
		m_PortMax[m_PortIndex]->value(m_PortMin[m_PortIndex]->value());
		m_PortMin[m_PortIndex]->value(temp);
		m_PortMax[m_PortIndex]->redraw();
		m_PortMin[m_PortIndex]->redraw();
	}

	m_GUICH->SetData("SetInputPortMax", &m_Max);
	m_GUICH->SetCommand(LADSPAPlugin::SETMAX);

// Clip default to range
	m_Default = atof(m_PortDefault[m_PortIndex]->value());
	if (m_Default > m_Max) {
		m_Default = m_Max;
		m_GUICH->SetData("SetInputPortDefault",&m_Default);

		m_GUICH->Wait();
		m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

	// Print to displayed default
		char temp[256];
		sprintf(temp, "%.4f", m_Default);
		m_PortDefault[m_PortIndex]->value(temp);
		m_PortDefault[m_PortIndex]->redraw();
	}

// Reposition Default Adjust knob to reflect new range
	SetDefaultAdjust(m_PortIndex);
}
void LADSPAPluginGUI::cb_Max(Fl_Input* o)
{       //                  Group        Pack      Scroll    Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()))->cb_Max_i(o);
}

inline void LADSPAPluginGUI::cb_Clamp_i(Fl_Check_Button* o)
{
// Which Clamp was changed?
	bool do_search = false;
	if (m_PortIndex == m_PortClamp.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortClamp[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query if it is different from last one changed
		vector<Fl_Check_Button *>::iterator i = std::find(m_PortClamp.begin(),
		                                                  m_PortClamp.end(),
		                                                  o);
		m_PortIndex = distance(m_PortClamp.begin(), i);
	}

	m_Clamp = (bool)(o->value());

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);
	m_GUICH->SetData("SetInputPortClamp", &m_Clamp);
	m_GUICH->SetCommand(LADSPAPlugin::SETCLAMP);
}
void LADSPAPluginGUI::cb_Clamp(Fl_Check_Button* o)
{       //                  Group        Pack      Scroll    Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()))->cb_Clamp_i(o);
}

inline void LADSPAPluginGUI::cb_DefaultAdjust_i(Fl_Knob *o)
{
// First, find which knob is being adjusted
	bool do_search = false;
	if (m_PortIndex == m_PortDefaultAdjust.size()) { do_search = true; }
	if (!do_search) { do_search = (o != (m_PortDefaultAdjust[m_PortIndex])) ? true : false; }

	if (do_search) {
	// Only bother to re-query knob if it is different from last one adjusted
		vector<Fl_Knob *>::iterator i = std::find(m_PortDefaultAdjust.begin(),
		                                          m_PortDefaultAdjust.end(),
		                                          o);
		m_PortIndex = distance(m_PortDefaultAdjust.begin(), i);
	}

	m_Default = o->value();

// Convert knob value [0.0, 1.0] to value in Min, Max range
	float min = atof(m_PortMin[m_PortIndex]->value());
	float max = atof(m_PortMax[m_PortIndex]->value());
	m_Default = ((max - min) > 0.0f) ? min + (max - min) * m_Default : min;

// Pass value to plugin
	m_GUICH->SetData("SetInputPortIndex", &m_PortIndex);
	m_GUICH->SetData("SetInputPortDefault", &m_Default);
	m_GUICH->SetCommand(LADSPAPlugin::SETDEFAULT);

// Copy to Default field in Port Setup list
	char temp[256];
	sprintf(temp, "%.4f", m_Default);
	m_PortDefault[m_PortIndex]->value(temp);
}

void LADSPAPluginGUI::cb_DefaultAdjust(Fl_Knob *o)
{       //                     Group     Pack      Scroll    Group     Tab       GUI
	((LADSPAPluginGUI*)(o->parent()->parent()->parent()->parent()->parent()->parent()))->cb_DefaultAdjust_i(o);
}
