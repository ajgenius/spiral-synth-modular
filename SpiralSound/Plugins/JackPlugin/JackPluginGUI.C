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

#include "JackPlugin.h"
#include "JackPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_file_chooser.H>
#include <FL/Fl_Hold_Browser.H>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

static int Numbers[255];

////////////////////////////////////////////////////////////////////////

int OptionsList(const vector<string> &List)
{
	Fl_Double_Window *Win    = new Fl_Double_Window(300,300);
	Fl_Button *Ok            = new Fl_Button(10,275,40,20,"Ok");
	Ok->labelsize(10);
	Fl_Button *Cancel        = new Fl_Button(50,275,40,20,"Cancel");
	Cancel->labelsize(10);	
	Fl_Hold_Browser* Browser = new Fl_Hold_Browser(5,5,290,265,"");
	
	for (vector<string>::const_iterator i = List.begin();
		 i!=List.end(); i++)
	{
		Browser->add(i->c_str());
	}
	
	Win->show();

	int Choice=-1;
	
	for (;;) 
	{
    	Fl::wait();
    	Fl_Widget* o = Fl::readqueue();
    	if (o==Ok || o==Browser) 
		{ 			
			Choice=Browser->value();
			Win->hide();
			delete Win;
			break; 
		}
		if (o==Cancel) 
		{ 
			Choice=-1; 
			Win->hide();
			delete Win;
			break; 
		}
		
		if (o==Win) break; 		
  	}
	
	return Choice;
}

////////////////////////////////////////////////////////////////////////

JackPluginGUI::JackPluginGUI(int w, int h,JackPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	for (int n=0; n<255; n++) Numbers[n]=n;

	m_Indicator	= new Fl_LED_Button(85,15,30,30,"");
	m_Indicator->value(0);
	m_Indicator->color(FL_RED);
	
	m_Attach = new Fl_Button(5,40,190,20,"Attach");
	m_Attach->type(0);
	m_Attach->labelsize(10);
	m_Attach->callback((Fl_Callback*)cb_Attach);
	
	m_Detach = new Fl_Button(5,60,190,20,"Detach");
	m_Detach->type(0);
	m_Detach->labelsize(10);
	m_Detach->callback((Fl_Callback*)cb_Detach);

	int yoff=80;
	
	for (int n=0; n<NUM_OUTPUTS; n++)
	{
		sprintf(m_OutputName[n],"Output %d",n);
		m_OutputLabel[n] = new Fl_Box(5,n*30+yoff,95,10,m_OutputName[n]);
		m_OutputLabel[n]->labelsize(8);
		//m_OutputLabel[n]->labeltype(FL_ENGRAVED_LABEL);
		m_OutputButton[n] = new Fl_Button(5,n*30+yoff+10,95,20,"None");
		m_OutputButton[n]->type(1);
		m_OutputButton[n]->labelsize(8);
		m_OutputButton[n]->callback((Fl_Callback*)cb_OutputConnect,&Numbers[n]);
	}
	
	for (int n=0; n<NUM_INPUTS; n++)
	{
		sprintf(m_InputName[n],"Input %d",n);
		m_InputLabel[n] = new Fl_Box(100,n*30+yoff,95,10,m_InputName[n]);
		m_InputLabel[n]->labelsize(8);
		//m_InputLabel[n]->labeltype(FL_ENGRAVED_LABEL);
		m_InputButton[n] = new Fl_Button(100,n*30+yoff+10,95,20,"None");
		m_InputButton[n]->type(1);
		m_InputButton[n]->labelsize(8);
		m_InputButton[n]->callback((Fl_Callback*)cb_InputConnect,&Numbers[n]);
	}

	end();
}

void JackPluginGUI::UpdateValues(SpiralPlugin *o)
{
	
}

void JackPluginGUI::Update()
{
	m_Indicator->value(m_GUICH->GetBool("Connected"));
	redraw();	
}
	
//// Callbacks ////
inline void  JackPluginGUI::cb_Attach_i(Fl_Button* o, void* v)
{ 
	//m_GUICH->SetCommand(JackPlugin::ATTACH);
	JackClient::Get()->Attach();
}
void  JackPluginGUI::cb_Attach(Fl_Button* o, void* v)
{ ((JackPluginGUI*)(o->parent()))->cb_Attach_i(o,v);}

inline void  JackPluginGUI::cb_Detach_i(Fl_Button* o, void* v)
{ 
	//m_GUICH->SetCommand(JackPlugin::DETACH);
	JackClient::Get()->Detach();
}
void  JackPluginGUI::cb_Detach(Fl_Button* o, void* v)
{ ((JackPluginGUI*)(o->parent()))->cb_Detach_i(o,v);}

inline void JackPluginGUI::cb_OutputConnect_i(Fl_Button* o, void* v)
{ 
	if (o->value())
	{
		m_GUICH->SetCommand(JackPlugin::UPDATE_NAMES);	
		m_GUICH->Wait();
		
		// bit of a hack for multithreaded safety
		int ninputs=m_GUICH->GetInt("NumOutputPortNames");
		char inputs[MAX_INPUTPORTS][256];
		m_GUICH->GetData("InputPortNames",inputs);
			
		vector<string> Inputs;
		for (int n=0; n<ninputs; n++) Inputs.push_back(inputs[n]);
		int choice=OptionsList(Inputs);
			
		// connect this plugin's output to a jack input
		if (choice>0)
		{
			//m_GUICH->Set("Num",(*(int*)v));
			//m_GUICH->SetData("Port",inputs[choice-1]);
			//m_GUICH->SetCommand(JackPlugin::CONNECTOUTPUT);
			
			JackClient::Get()->ConnectOutput((*(int*)v),inputs[choice-1]);
			
			o->label(inputs[choice-1]);
			o->redraw();
		}
	}
	else
	{
		JackClient::Get()->DisconnectOutput((*(int*)v));
		o->label("None");
		o->redraw();
	}
}
void JackPluginGUI::cb_OutputConnect(Fl_Button* o, void* v)
{ ((JackPluginGUI*)(o->parent()))->cb_OutputConnect_i(o,v);}

inline void JackPluginGUI::cb_InputConnect_i(Fl_Button* o, void* v)
{ 
	if (o->value())
	{
		m_GUICH->SetCommand(JackPlugin::UPDATE_NAMES);	
		m_GUICH->Wait();
		
		// bit of a hack for multithreaded safety
		int noutputs=m_GUICH->GetInt("NumOutputPortNames");
		char outputs[MAX_OUTPUTPORTS][256];
		m_GUICH->GetData("OutputPortNames",outputs);
			
		vector<string> Outputs;
		for (int n=0; n<noutputs; n++) Outputs.push_back(outputs[n]);
		int choice=OptionsList(Outputs);
	
		// connect this plugin's input to a jack output
		if (choice>0)
		{
			//m_GUICH->Set("Num",(*(int*)v));
			//m_GUICH->SetData("Port",outputs[choice-1]);
			//m_GUICH->SetCommand(JackPlugin::CONNECTINPUT);
			
			JackClient::Get()->ConnectInput((*(int*)v),outputs[choice-1]);
			
			o->label(outputs[choice-1]);
			o->redraw();
		}
	}
	else
	{
		JackClient::Get()->DisconnectInput((*(int*)v));
		o->label("None");
		o->redraw();
	}
}
void JackPluginGUI::cb_InputConnect(Fl_Button* o, void* v)
{ ((JackPluginGUI*)(o->parent()))->cb_InputConnect_i(o,v);}
