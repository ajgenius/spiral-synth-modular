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

#include "OutputPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_file_chooser.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

OutputPluginGUI::OutputPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
    Volume = new Fl_Knob(30, 22, 40, 40, "Volume");
    Volume->color(GUI_COLOUR);
	Volume->type(Fl_Knob::DOTLIN);
    Volume->labelsize(10);
    Volume->maximum(1);
    Volume->step(0.001);
    Volume->value(0.5);   
	Volume->callback((Fl_Callback*)cb_Volume);

	Record = new Fl_Button(30, 80, 40, 25, "Record");
    Record->type(1);
    Record->down_box(FL_DOWN_BOX);
    Record->labelsize(10);
    Record->callback((Fl_Callback*)cb_Record);   
	  	
	OpenRead = new Fl_Button(2, 110, 30, 15, "Read");
    OpenRead->type(1);
    OpenRead->down_box(FL_DOWN_BOX);
    OpenRead->labelsize(10);
    OpenRead->callback((Fl_Callback*)cb_OpenRead);   

	OpenDuplex = new Fl_Button(34, 110, 31, 15, "Dplx");
    OpenDuplex->type(1);
    OpenDuplex->down_box(FL_DOWN_BOX);
    OpenDuplex->labelsize(10);
    OpenDuplex->callback((Fl_Callback*)cb_OpenDuplex);   
	
	OpenWrite = new Fl_Button(68, 110, 30, 15, "Write");
    OpenWrite->type(1);
    OpenWrite->down_box(FL_DOWN_BOX);
    OpenWrite->labelsize(10);
    OpenWrite->callback((Fl_Callback*)cb_OpenWrite);   
		  	       
	end();
}

void OutputPluginGUI::UpdateValues(SpiralPlugin *o)
{
	Volume->value(OSSOutput::Get()->GetVolume());
}
	
//// Callbacks ////

inline void OutputPluginGUI::cb_Volume_i(Fl_Knob* o, void* v)
{ OSSOutput::Get()->SetVolume(o->value()); }
void OutputPluginGUI::cb_Volume(Fl_Knob* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_Volume_i(o,v); }

inline void OutputPluginGUI::cb_Record_i(Fl_Button* o, void* v)
{ 
	if (o->value())
	{
		char *fn=fl_file_chooser("Pick a Wav file to save to", "*.wav", NULL);
		
		if (fn && fn!="")
		{
			OSSOutput::Get()->WavOpen(fn);
		}
		else
		{
			OSSOutput::Get()->WavClose();
			o->value(false);
		}
	}
	else
	{
		OSSOutput::Get()->WavClose();
	}
}
void OutputPluginGUI::cb_Record(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_Record_i(o,v); }

inline void OutputPluginGUI::cb_OpenRead_i(Fl_Button* o, void* v)
{ 
	if (o->value())
	{
		OpenWrite->value(0);
		OSSOutput::Get()->Close();
		OSSOutput::Get()->OpenRead();
		m_GUICH->Set("Mode",(char)OutputPlugin::INPUT);
	}
	else
	{
		OpenWrite->value(0);
		OSSOutput::Get()->Close();
	}
}
void OutputPluginGUI::cb_OpenRead(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_OpenRead_i(o,v); }

inline void OutputPluginGUI::cb_OpenDuplex_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		OpenRead->value(0);
		OSSOutput::Get()->Close();
		OSSOutput::Get()->OpenReadWrite();
		m_GUICH->Set("Mode",(char)OutputPlugin::DUPLEX);
	}
	else
	{
		OpenRead->value(0);
		OSSOutput::Get()->Close();
	} 
}
void OutputPluginGUI::cb_OpenDuplex(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_OpenDuplex_i(o,v); }

inline void OutputPluginGUI::cb_OpenWrite_i(Fl_Button* o, void* v)
{ 
	if (o->value())
	{
		OpenRead->value(0);
		OSSOutput::Get()->Close();
		OSSOutput::Get()->OpenWrite();
		m_GUICH->Set("Mode",(char)OutputPlugin::OUTPUT);
	}
	else
	{
		OpenRead->value(0);
		OSSOutput::Get()->Close();
	}
}
void OutputPluginGUI::cb_OpenWrite(Fl_Button* o, void* v)
{ ((OutputPluginGUI*)(o->parent()))->cb_OpenWrite_i(o,v); }
