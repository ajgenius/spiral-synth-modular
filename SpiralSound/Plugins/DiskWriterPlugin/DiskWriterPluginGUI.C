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

#include "DiskWriterPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_file_chooser.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

DiskWriterPluginGUI::DiskWriterPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	Open = new Fl_Button(5, 15, 90, 20, "Open");
    Open->type(1);
    Open->down_box(FL_DOWN_BOX);
    Open->labelsize(10);
    Open->callback((Fl_Callback*)cb_Open);   

	Record = new Fl_Button(5, 35, 90, 20, "Record");
    Record->type(1);
    Record->down_box(FL_DOWN_BOX);
    Record->labelsize(10);
    Record->callback((Fl_Callback*)cb_Record);   
	       
	end();
}

void DiskWriterPluginGUI::UpdateValues(SpiralPlugin *o)
{
}
	
//// Callbacks ////

inline void DiskWriterPluginGUI::cb_Open_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		char *fn=fl_file_chooser("Pick a Wav file to save to", "*.wav", NULL);
		char t[256];
		sprintf(t,"%s",fn);
		
		if (fn && fn!="")
		{
			m_GUICH->SetData("Filename",(void*)t);
			m_GUICH->SetCommand(DiskWriterPlugin::OPENWAV);
		}
		else
		{
			m_GUICH->SetCommand(DiskWriterPlugin::CLOSEWAV);
			o->value(false);
		}
	}
	else
	{
		m_GUICH->SetCommand(DiskWriterPlugin::CLOSEWAV);
	}
}
void DiskWriterPluginGUI::cb_Open(Fl_Button* o, void* v)
{ ((DiskWriterPluginGUI*)(o->parent()))->cb_Open_i(o,v); }

inline void DiskWriterPluginGUI::cb_Record_i(Fl_Button* o, void* v)
{ 
	if (o->value()) m_GUICH->SetCommand(DiskWriterPlugin::RECORD);
	else m_GUICH->SetCommand(DiskWriterPlugin::STOP);
}
void DiskWriterPluginGUI::cb_Record(Fl_Button* o, void* v)
{ ((DiskWriterPluginGUI*)(o->parent()))->cb_Record_i(o,v); }

const string DiskWriterPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "One way of recording your creations to disk. First open a file\n"
    + "you wish to save to, then hit record to start recording.\n"
    + "You are able to stop and restart recording without closing the\n"
    + "file, which should make life a little easier if you are doing\n"
    + "things like recording lots of little samples.";
}
