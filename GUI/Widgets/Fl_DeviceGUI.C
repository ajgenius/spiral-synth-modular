/*  DeviceGUI composite Widget
 *  Copyleft (C) 2002 David Griffiths <dave@pawfal.org>
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

#include "Fl_DeviceGUI.h"
#include "Fl_Canvas.h"
#include "../../SpiralSynthModularInfo.h"

int Fl_DeviceGUI::Numbers[512];

Fl_DeviceGUI::Fl_DeviceGUI(const DeviceGUIInfo& Info, Fl_Group *PW, Fl_Pixmap *Icon) :
Fl_Group(Info.XPos, Info.YPos, Info.Width+(PortGroupWidth*2), Info.Height+TitleBarHeight, ""),
m_PluginWindow(NULL),
m_Icon(NULL),
m_Name(Info.Name),
m_ID(-1),
m_DelMe(false)
{	
	for (int n=0; n<512; n++) Numbers[n]=n;
	
	type(1);
	box(FL_PLASTIC_UP_BOX);
    labeltype(FL_ENGRAVED_LABEL);
    align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	color(SpiralSynthModularInfo::GUICOL_Device);
		
	m_Icon=Icon;
		
	m_DragBar = new Fl_DragBar(Info.XPos, Info.YPos, Info.Width+PortGroupWidth*2, TitleBarHeight, m_Name.c_str());
	m_DragBar->labelsize(10);	
	m_DragBar->type(Fl_DragBar::FLDRAG);
	m_DragBar->color(SpiralSynthModularInfo::GUICOL_Device);
	
	m_PluginWindow = PW;
	
	//Add the input/output ports
	Setup(Info, true);
}

void Fl_DeviceGUI::Clear()
{
	end();
}

int  Fl_DeviceGUI::handle(int event)
{	
	int t=Fl_Group::handle(event);
	
	if (t==0)
	{	
		if (event==FL_PUSH && Fl::event_button()==1) 
		{
			if (m_PluginWindow && !m_DelMe)
			{					
				if(!m_PluginWindow->visible())
				{
					m_PluginWindow->show();
				}
				/*else 
				{
					m_PluginWindow->hide();
				}*/			
			}
		}	
		if (event==FL_RELEASE && Fl::event_button()==2) 
		{		
			m_DelMe = true;
		}
	}
	
	return t;
}

void  Fl_DeviceGUI::draw()
{	
	Fl_Group::draw();
	if (m_Icon)
	{
		int Centx=x()+w()/2;
		int Centy=y()+h()/2;
		m_Icon->draw(Centx-m_Icon->w()/2,Centy-m_Icon->h()/2);
	}
}

void Fl_DeviceGUI::Setup(const DeviceGUIInfo& Info, bool FirstTime)
{
	m_Info=Info;
	
	// Remove all current connections - it's the safest thing to do.
	if (parent() && !FirstTime) 
	{
		((Fl_Canvas*)(parent()))->ClearConnections(this);
	}

	// delete the current ports
	for(vector<Fl_Button*>::iterator i=m_PortVec.begin();
			i!=m_PortVec.end(); i++)
	{		
		remove(*i);
		delete(*i);
	}

	m_PortVec.clear();
	
	int InputX=x()+2;
	int OutputX=x()+PortGroupWidth+Info.Width+4;
	int StartY=y()+TitleBarHeight;
	int PortDist=10;
	int PortNum=0;
	
	h(Info.Height+TitleBarHeight);
	
	for (int n=0; n<Info.NumInputs; n++)
	{
		Fl_Button* NewInput = new Fl_Button(InputX,StartY+PortDist*n,PortSize,PortSize,"");
		NewInput->type(1);
		NewInput->value(false);
		NewInput->box(FL_ROUNDED_BOX);
		
        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (Info.PortTypes[n]) {
             case 0:     col = (Fl_Color) WIRE_COL0;
                         break;
             case 1:     col = (Fl_Color) WIRE_COL1;
                         break;
             case 2:     col = (Fl_Color) WIRE_COL2;
                         break;
             case 3:     col = (Fl_Color) WIRE_COL3;
                         break;
             case 4:     col = (Fl_Color) WIRE_COL4;
                         break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		NewInput->selection_color(col);
		 
		NewInput->down_box(FL_ROUNDED_BOX);
		NewInput->tooltip(Info.PortTips[n].c_str());
		NewInput->callback((Fl_Callback*)cb_Port,(void*)&Numbers[PortNum]);		
		m_PortVec.push_back(NewInput);
		add(NewInput);
		PortNum++;
	}
	
	for (int n=0; n<Info.NumOutputs; n++)
	{
		Fl_Button* NewOutput = new Fl_Button(OutputX,StartY+PortDist*n,PortSize,PortSize,"");
		NewOutput->type(1);
		NewOutput->value(false);
		NewOutput->box(FL_ROUNDED_BOX);

        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (Info.PortTypes[n+Info.NumInputs]) {
             case 0:     col = (Fl_Color) WIRE_COL0;
                         break;
             case 1:     col = (Fl_Color) WIRE_COL1;
                         break;
             case 2:     col = (Fl_Color) WIRE_COL2;
                         break;
             case 3:     col = (Fl_Color) WIRE_COL3;
                         break;
             case 4:     col = (Fl_Color) WIRE_COL4;
                         break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		NewOutput->selection_color(col);
  

		NewOutput->down_box(FL_ROUNDED_BOX);
		NewOutput->tooltip(Info.PortTips[n+Info.NumInputs].c_str());
		NewOutput->callback((Fl_Callback*)cb_Port,(void*)&Numbers[PortNum]);		
		m_PortVec.push_back(NewOutput);
		add(NewOutput);
		PortNum++;
	}
}

inline void Fl_DeviceGUI::cb_Port_i(Fl_Button* o, void* v)
{
	int Port=*(int*)(v);
	PortType Pt;
	
	if (m_DelMe) return;
	
	// Find out if this is an input or an output.
	if (Port<m_Info.NumInputs) 
	{
		Pt=INPUT;
	}
	else
	{
		Pt=OUTPUT;
		Port-=m_Info.NumInputs;
	}
			
	((Fl_Canvas*)(parent()))->PortClicked(this,Pt,Port,o->value());
		
		
}
void Fl_DeviceGUI::cb_Port(Fl_Button* o, void* v)
{((Fl_DeviceGUI*)(o->parent()))->cb_Port_i(o,v);}
