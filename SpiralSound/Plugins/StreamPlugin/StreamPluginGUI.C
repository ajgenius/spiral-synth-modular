/*  SpiralPlugin
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

#include "StreamPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;
char PitchLabel[256];

bool StreamPluginGUI::TimerSet=false;
vector<StreamPluginGUI*> StreamPluginGUI::PluginList;

////////////////////////////////////////////

StreamPluginGUI::StreamPluginGUI(int w, int h,StreamPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	

	StartTimer(this);
	int Width=20;
	int Height=100;
	//What are Width and Height for.

	m_PitchValue=1.0f;
		
	m_Load = new Fl_Button(2, 130, 30, 30, "Load");
    m_Load->labelsize(9);
	m_Load->callback((Fl_Callback*)cb_Load);
	add(m_Load);
	
	int sx=32;
	
	m_ToStart = new Fl_Button(sx, 130, 30, 30, "@|<");
    m_ToStart->labelsize(10);
	m_ToStart->labeltype(FL_SYMBOL_LABEL);
	m_ToStart->callback((Fl_Callback*)cb_ToStart);
	add(m_ToStart);
	
	m_Stop = new Fl_Button(sx+30, 130, 30, 30, "@||");
    m_Stop->labelsize(10);
	m_Stop->labeltype(FL_SYMBOL_LABEL);
	m_Stop->callback((Fl_Callback*)cb_Stop);
	add(m_Stop);
	
	m_Play = new Fl_Button(sx+60, 130, 30, 30, "@>");
    m_Play->labelsize(10);
	m_Play->labeltype(FL_SYMBOL_LABEL);
	m_Play->callback((Fl_Callback*)cb_Play);
	add(m_Play);
	
	m_Div = new Fl_Button(sx+90, 130, 30, 30, "/2");
    m_Div->labelsize(9);
	m_Div->callback((Fl_Callback*)cb_Div);
	add(m_Div);

	m_Reset = new Fl_Button(sx+120, 130, 30, 30, "Reset");
    m_Reset->labelsize(9);
	m_Reset->callback((Fl_Callback*)cb_Reset);
	add(m_Reset);
	
	m_Loop = new Fl_Button(sx+150, 130, 30, 30, "X2");
    m_Loop->labelsize(9);
	m_Loop->callback((Fl_Callback*)cb_Loop);
	add(m_Loop);
		
	m_Nudge = new Fl_Repeat_Button(sx+180, 130, 30, 30, "Nudge");
    m_Nudge->labelsize(9);
	m_Nudge->callback((Fl_Callback*)cb_Nudge);
	add(m_Nudge);
	
	// Andy Preston changed
        // m_Volume = new Fl_Knob(170, 15, 50, 50, "Volume");
        // to
	m_Volume = new Fl_Knob (180, 15, 50, 50, "Volume");
    m_Volume->color(GUI_COLOUR);
	m_Volume->type(Fl_Knob::LINELIN);
    m_Volume->labelsize(10);
    m_Volume->maximum(2);
    m_Volume->step(0.001);
    m_Volume->value(1);   
	m_Volume->callback((Fl_Callback*)cb_Volume);
	add(m_Volume);
	
	m_Pitch = new Fl_Slider(5,85,235,20, "");
    m_Pitch->type(FL_HORIZONTAL);
	m_Pitch->align(FL_ALIGN_INSIDE | FL_ALIGN_RIGHT);
    m_Pitch->labelsize(10);
	m_Pitch->labelcolor(GUI_COLOUR);
	m_Pitch->maximum(20);
    m_Pitch->step(0.001);
	sprintf(PitchLabel,"%1.3f   ",1.0);
	m_Pitch->label(PitchLabel);
	m_Pitch->value(11);
	m_Pitch->callback((Fl_Callback*)cb_Pitch);
	add(m_Pitch);

        // Andy Preston changed
        // int k=4;
        // m_Display[0] = new Fl_SevenSeg(10, 20, 30, 60);
	// m_Display[0]->bar_width(k);
	// m_Display[0]->color(FL_WHITE);
	// m_Display[0]->color2(GUI_COLOUR);
	// add(m_Display[0]);
	// m_Display[1] = new Fl_SevenSeg(30, 20, 30, 60);
	// m_Display[1]->bar_width(k);
	// m_Display[1]->color(FL_WHITE);
	// m_Display[1]->color2(GUI_COLOUR);
	// add(m_Display[1]);
        // m_Display[2] = new Fl_SevenSeg(60, 20, 30, 60);
	// m_Display[2]->bar_width(k);
	// m_Display[2]->color(FL_WHITE);
	// m_Display[2]->color2(GUI_COLOUR);
	// add(m_Display[2]);
	// m_Display[3] = new Fl_SevenSeg(80, 20, 30, 60);
	// m_Display[3]->bar_width(k);
	// m_Display[3]->color(FL_WHITE);
	// m_Display[3]->color2(GUI_COLOUR);
	// add(m_Display[3]);
        // m_Display[4] = new Fl_SevenSeg(110, 20, 30, 60);
	// m_Display[4]->bar_width(k);
	// m_Display[4]->color(FL_WHITE);
	// m_Display[4]->color2(GUI_COLOUR);
	// add(m_Display[4]);
	// m_Display[5] = new Fl_SevenSeg(130, 20, 30, 60);
	// m_Display[5]->bar_width(k);
	// m_Display[5]->color(FL_WHITE);
	// m_Display[5]->color2(GUI_COLOUR);
	// add(m_Display[5]);
        // to
        for (int dis=0; dis<6; dis++) {
            m_Display[dis] = new Fl_SevenSeg (5 + 28*dis, 20, 28, 60);
            m_Display[dis] -> bar_width (4);
            m_Display[dis] -> color (FL_WHITE);
	    m_Display[dis] -> color2 (GUI_COLOUR);
            if (dis > 0 && dis % 2 == 0) m_Display[dis] -> dp (colon);
            add (m_Display[dis]);
        }

	m_Pos = new Fl_Slider(5,108,235,20,"");
	m_Pos->type(FL_HORIZONTAL);
	m_Pos->maximum(1);
	m_Pos->callback((Fl_Callback*)cb_Pos);
	add(m_Pos);

	end();
}

StreamPluginGUI::~StreamPluginGUI()
{
	cerr << "~StreamPluginGUI" << endl;
	StopTimer(this);
}

void StreamPluginGUI::SetTime()
{
	cerr << "foo" << endl;
	float t=m_GUICH->GetFloat("TimeOut");

	m_Pos->value(t);

	m_Display[5]->value((int)(t*100)%10);
	m_Display[4]->value((int)(t*10)%10);

	m_Display[3]->value((int)t%10);
	m_Display[2]->value((int)(t/10)%6);

	m_Display[1]->value((int)(t/60)%10);
	m_Display[0]->value((int)(t/600)%10);
	redraw();
}


void StreamPluginGUI::UpdateValues(SpiralPlugin *o)
{
	StreamPlugin *Plugin = (StreamPlugin*)o;

	m_Volume->value(Plugin->GetVolume());
	m_Pitch->value(Plugin->GetPitch()+10);
	m_Loop->value(Plugin->GetLoop());
}
	
inline void StreamPluginGUI::cb_Load_i(Fl_Button* o, void* v)
{ 
	char *fn=fl_file_chooser("Load a sample", "{*.wav,*.WAV}", NULL);
		
	if (fn && fn!='\0')
	{
		strcpy(m_TextBuf,fn);
		m_GUICH->Set("FileName",m_TextBuf);
		m_GUICH->SetCommand(StreamPlugin::LOAD);
	}
}
void StreamPluginGUI::cb_Load(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Load_i(o,v);}

inline void StreamPluginGUI::cb_Volume_i(Fl_Knob* o, void* v)
{ m_GUICH->Set("Volume",(float)o->value()); }
void StreamPluginGUI::cb_Volume(Fl_Knob* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Volume_i(o,v);}

inline void StreamPluginGUI::cb_Pitch_i(Fl_Slider* o, void* v)
{ 
	m_GUICH->Set("Pitch",(float)o->value()-10); 
	sprintf(PitchLabel,"%1.3f   ",o->value()-10);
	m_Pitch->label(PitchLabel); 
	m_PitchValue=o->value()-10;
}
void StreamPluginGUI::cb_Pitch(Fl_Slider* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Pitch_i(o,v);}

inline void StreamPluginGUI::cb_Loop_i(Fl_Button* o, void* v) //Why is this function named so.
{ 
	m_PitchValue*=2.0f;
	m_GUICH->SetCommand(StreamPlugin::DOUBLE);
	sprintf(PitchLabel,"%1.3f   ",m_PitchValue);
	m_Pitch->label(PitchLabel); 
	m_Pitch->value(m_PitchValue+10); 
	redraw();
}
void StreamPluginGUI::cb_Loop(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Loop_i(o,v);}

inline void StreamPluginGUI::cb_Div_i(Fl_Button* o, void* v)
{ 
	m_PitchValue/=2.0f;
	m_GUICH->SetCommand(StreamPlugin::HALF);
	sprintf(PitchLabel,"%1.3f   ",m_PitchValue);
	m_Pitch->label(PitchLabel); 
	m_Pitch->value(m_PitchValue+10); 
	redraw();
}
void StreamPluginGUI::cb_Div(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Div_i(o,v);}

inline void StreamPluginGUI::cb_ToStart_i(Fl_Button* o, void* v)
{ m_GUICH->SetCommand(StreamPlugin::RESTART); }
void StreamPluginGUI::cb_ToStart(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_ToStart_i(o,v);}

inline void StreamPluginGUI::cb_Stop_i(Fl_Button* o, void* v)
{ m_GUICH->SetCommand(StreamPlugin::STOP); }
void StreamPluginGUI::cb_Stop(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Stop_i(o,v);}

inline void StreamPluginGUI::cb_Play_i(Fl_Button* o, void* v)
{ m_GUICH->SetCommand(StreamPlugin::PLAY); }
void StreamPluginGUI::cb_Play(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Play_i(o,v);}

inline void StreamPluginGUI::cb_Reset_i(Fl_Button* o, void* v)
{ 
	m_GUICH->SetCommand(StreamPlugin::RESET);
	sprintf(PitchLabel,"%1.3f   ",1.0);
	m_Pitch->label(PitchLabel); 
	m_Pitch->value(11);
	m_PitchValue=1.0f;
	redraw();
}
void StreamPluginGUI::cb_Reset(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Reset_i(o,v);}

inline void StreamPluginGUI::cb_Nudge_i(Fl_Button* o, void* v)
{ 
	m_GUICH->SetCommand(StreamPlugin::NUDGE); 
}
void StreamPluginGUI::cb_Nudge(Fl_Button* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Nudge_i(o,v);}

inline void StreamPluginGUI::cb_Pos_i(Fl_Slider* o, void* v)
{
	m_GUICH->Set("Time",(float)o->value());
	m_GUICH->SetCommand(StreamPlugin::SET_TIME);
}
void StreamPluginGUI::cb_Pos(Fl_Slider* o, void* v)
{ ((StreamPluginGUI*)(o->parent()))->cb_Pos_i(o,v);}

//The timer stuff

#define TIME_OUT_DURATION 0.01
//This is temparary, should use a variable, and let it be set from the command line ore something.

void StreamPluginGUI::StartTimer(StreamPluginGUI* p)
{
	if (!TimerSet)
	{
		Fl::add_timeout(TIME_OUT_DURATION,&StreamPluginGUI::TimerCallBack);
		TimerSet=True;
	}
	PluginList.push_back(p);
}

void StreamPluginGUI::StopTimer(StreamPluginGUI* p)
{
	cerr << "StopTimer" << endl;
	for (vector<StreamPluginGUI*>::iterator i=PluginList.begin();i!=PluginList.end();i++)
		if ((*i)==p)
			i=PluginList.erase(i);
}

//If Spiral synth modular unloaded the .so file this is in, the timeout
//would probably cause a problem
void StreamPluginGUI::TimerCallBack(void* x)
{
	/*for(vector<const StreamPluginGUI*>::iterator i=PluginList.begin();
		i!=PluginList.end();i++)
	{*/
	for (vector<StreamPluginGUI*>::iterator i=PluginList.begin();i!=PluginList.end();i++)
		(*i)->SetTime();
	Fl::add_timeout(TIME_OUT_DURATION,&StreamPluginGUI::TimerCallBack);
}
