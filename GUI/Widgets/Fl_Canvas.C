/*  Canvas Widget
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

#include "Fl/fl_draw.H"
#include "Fl_Canvas.h"
#include "Fl_DeviceGUI.h"
#include <iostream>
#include "../../SpiralSynthModularInfo.h"

//#define IMG_BACKGROUND

#ifdef IMG_BACKGROUND
#include <png.h>
#endif

////////////////////////////////////////////////////////////////////////

Fl_Canvas::Fl_Canvas(int x, int y, int w, int h, char *name) :
Fl_Group(x,y,w,h,name),
cb_Connection(NULL),
cb_Unconnect(NULL),
cb_AddDevice(NULL),
m_ToolMenu(false)
{
	m_IncompleteWire.OutputChild=-1;
	m_IncompleteWire.OutputPort=-1;
	m_IncompleteWire.InputChild=-1;
	m_IncompleteWire.InputPort=-1;
	
	m_BG=NULL;
	m_BGData=NULL;
	
#ifdef IMG_BACKGROUND
	if (SpiralSynthModularInfo::BGIMG=="None") return;
	FILE *fp=fopen(SpiralSynthModularInfo::BGIMG.c_str(),"rb");
	if (!fp) 
	{
		SpiralInfo::Alert("Couldn't open image ["+SpiralSynthModularInfo::BGIMG+"]");
	}
	else
	{
		png_structp png_ptr;
		png_infop info_ptr;
		
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   	 	info_ptr = png_create_info_struct(png_ptr);
   		png_init_io(png_ptr, fp);
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);		
		fclose(fp);
		
		m_BGData = new char[png_ptr->rowbytes*png_ptr->height];
		int p=0;
		for (unsigned int row = 0; row < png_ptr->height; row++)
		{
			for (unsigned int i=0; i<png_ptr->rowbytes; i++)
			{
				m_BGData[p]=(char)info_ptr->row_pointers[row][i];
				p++;
			}
		}
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
		m_BG = new Fl_Image((unsigned char*)m_BGData,
			(int)png_ptr->width,
			(int)png_ptr->height,(int)png_ptr->rowbytes/(int)png_ptr->width); 
#else
		m_BG = new Fl_RGB_Image((unsigned char*)m_BGData,(int)png_ptr->width,(int)png_ptr->height);
#endif
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);	
	}
#endif 
	
}

Fl_Canvas::~Fl_Canvas()
{
#ifdef IMG_BACKGROUND
	if (m_BGData) delete[] m_BGData;
#endif 	
}

void Fl_Canvas::draw()
{			
	Fl_Widget*const* a = array();
	
  	if (damage() & ~FL_DAMAGE_CHILD) // redraw the entire thing:
	{ 	    
		if (m_BG)
		{
			int X=0,Y=0;
			while (Y<w())
			{	
				while (X<h())
				{
					m_BG->draw(parent()->x()+X,parent()->y()+Y);
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
					X+=m_BG->w;
#else
					X+=m_BG->w();
#endif	
				}		
#if FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION == 0
 				Y+=m_BG->h;
#else
				Y+=m_BG->h();
#endif
				X=0;
			}
		}
		else
		{
			draw_box();
		}
				
   	 	for (int i=children(); i--;) 
		{
   			Fl_Widget& o = **a++;
   	 		draw_child(o);
   			draw_outside_label(o);
		}
		
		DrawWires();
	}
	else // only redraw the children that need it:
	{	
		for (int i=children(); i--;) update_child(**a++);
  	}
	
	if (m_ToolMenu)
	{
		int Pos=0,X=0,Y=0,textw,texth;
		int DegreesPerItem=30;
		float conv=3.151/180.0f;
		bool selected=false;
		m_Selected=-1;
		
		fl_font(fl_font(),10);
		for (vector< pair<string,int> >::iterator i=m_PluginNameList.begin();
			 i!=m_PluginNameList.end(); ++i)
		{								
			textw=0;
			fl_font(fl_font(),10);
			fl_measure(i->first.c_str(), textw, texth);
			
			X=m_x-(textw/2);
			Y=m_y-(m_PluginNameList.size()*5)+Pos*10;
						
			if (Fl::event_y()>Y-10 && Fl::event_y()<Y && 
				Fl::event_x()>X && Fl::event_x()<X+textw) 
			{
				fl_font(fl_font(),15);
				fl_measure(i->first.c_str(), textw, texth);
				X=m_x-(textw/2);
				m_Selected=i->second;
				selected=true;
			}
			else selected=false;
						
			fl_color(FL_GRAY);
						
			fl_color(FL_WHITE);
			fl_draw(i->first.c_str(),X-1,Y+1);

			if (selected) fl_color(FL_BLUE);	
			else fl_color(FL_BLACK);			

			fl_draw(i->first.c_str(),X,Y);			
			
			Pos+=1;	
		}
	}
}

void Fl_Canvas::Poll()
{
	// bit of a workaround...
	if(m_IncompleteWire.InputChild!=-1 || m_IncompleteWire.OutputChild!=-1)
	{
		redraw();
	}
}

void Fl_Canvas::DrawWires()
{
	for(vector<CanvasWire>::iterator i=m_WireVec.begin();
		i!=m_WireVec.end(); i++)
	{
		if (i->OutputChild>children() || i->InputChild>children())
		{
			cerr<<"wire output child = "<<i->OutputChild<<endl;
			cerr<<"wire input child = "<<i->InputChild<<endl;
			SpiralInfo::Alert("Wire drawing mismatch!");
			return;
		}
			   
		Fl_DeviceGUI* SourceDevice = (Fl_DeviceGUI*)(child(i->OutputChild));
		Fl_DeviceGUI* DestDevice   = (Fl_DeviceGUI*)(child(i->InputChild));
			 
		if (!SourceDevice || !DestDevice) 
		{
			SpiralInfo::Alert("Cant find source or dest device while drawing wires");
			return;
		}

         Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (SourceDevice->GetPortType(i->OutputPort+SourceDevice->GetInfo()->NumInputs)) {
             case 0:     col = (Fl_Color) WIRE_COL0; break;
             case 1:     col = (Fl_Color) WIRE_COL1; break;
             case 2:     col = (Fl_Color) WIRE_COL2; break;
             case 3:     col = (Fl_Color) WIRE_COL3; break;
             case 4:     col = (Fl_Color) WIRE_COL4; break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		fl_color(col);

		fl_line(SourceDevice->GetPortX(i->OutputPort+SourceDevice->GetInfo()->NumInputs),
				SourceDevice->GetPortY(i->OutputPort+SourceDevice->GetInfo()->NumInputs),
				DestDevice->GetPortX(i->InputPort),
				DestDevice->GetPortY(i->InputPort));	
	}
	
	// draw the wire we are currently connecting
	if(m_IncompleteWire.InputChild!=-1)
	{
		Fl_DeviceGUI* Device = (Fl_DeviceGUI*)(child(m_IncompleteWire.InputChild));
		
		if (!Device) 
		{
			SpiralInfo::Alert("Cant find source or dest device while drawing wires");
			return;
		}
		
		
        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (Device->GetPortType(m_IncompleteWire.InputPort)) {
             case 0:     col = (Fl_Color) WIRE_COL0; break;
             case 1:     col = (Fl_Color) WIRE_COL1; break;
             case 2:     col = (Fl_Color) WIRE_COL2; break;
             case 3:     col = (Fl_Color) WIRE_COL3; break;
             case 4:     col = (Fl_Color) WIRE_COL4; break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		fl_color(col);

		fl_line(Device->GetPortX(m_IncompleteWire.InputPort),
				Device->GetPortY(m_IncompleteWire.InputPort),
				Fl::event_x(),
				Fl::event_y());	
	}
	
	if(m_IncompleteWire.OutputChild!=-1)
	{
		Fl_DeviceGUI* Device = (Fl_DeviceGUI*)(child(m_IncompleteWire.OutputChild));
		
		if (!Device) 
		{
			SpiralInfo::Alert("Cant find source or dest device while drawing wires");
			return;
		}
		
		
        Fl_Color col = (Fl_Color) WIRE_COL0;
         switch (Device->GetPortType(m_IncompleteWire.OutputPort+Device->GetInfo()->NumInputs)) {
             case 0:     col = (Fl_Color) WIRE_COL0; break;
             case 1:     col = (Fl_Color) WIRE_COL1; break;
             case 2:     col = (Fl_Color) WIRE_COL2; break;
             case 3:     col = (Fl_Color) WIRE_COL3; break;
             case 4:     col = (Fl_Color) WIRE_COL4; break;
             default:    col = (Fl_Color) WIRE_COL0;
         }
 		fl_color(col);

		fl_line(Device->GetPortX(m_IncompleteWire.OutputPort+Device->GetInfo()->NumInputs),
				Device->GetPortY(m_IncompleteWire.OutputPort+Device->GetInfo()->NumInputs),
				Fl::event_x(),
				Fl::event_y());	
	}
}
void Fl_Canvas::ClearIncompleteWire()
{		
	// Turn off both ports
	if (m_IncompleteWire.OutputChild!=-1)
	{
		((Fl_DeviceGUI*)(child(m_IncompleteWire.OutputChild)))->RemoveConnection(m_IncompleteWire.OutputPort+
			((Fl_DeviceGUI*)(child(m_IncompleteWire.OutputChild)))->GetInfo()->NumInputs);
	}
	
	if (m_IncompleteWire.InputChild!=-1)
	{
		((Fl_DeviceGUI*)(child(m_IncompleteWire.InputChild)))->RemoveConnection(m_IncompleteWire.InputPort);
	}
	m_IncompleteWire.Clear();
}

int Fl_Canvas::handle(int event)
{
	if (Fl_Group::handle(event)) return 1;
	
	if (event==FL_PUSH) 
	{
		ClearIncompleteWire();
	}
	
	if (Fl::event_button()==3)
	{
		if (event==FL_PUSH) 
		{	
			m_ToolMenu=true;
			m_x=Fl::event_x();
			m_y=Fl::event_y();
			redraw();
		}
		
		if (event==FL_DRAG) redraw();
		
		if (event==FL_RELEASE && Fl::event_button()==3) 
		{
			m_ToolMenu=false;
			if (m_Selected!=-1 && cb_AddDevice) 
			{
				int args[3];
				args[0]=m_Selected;
				args[1]=m_x;
				args[2]=m_y;
				cb_AddDevice(this,args);
			}
			redraw();
		}	
	}

	return 1;
}
	
void Fl_Canvas::PortClicked(Fl_DeviceGUI* Device, int Type, int Port, bool Value)
{
	// find out which child this comes from.
	int ChildNum=-1;	
	for(int n=0; n<children(); n++)
	{
		if(child(n)==Device)
		{
			ChildNum=n;
		}
	}
	
	if (ChildNum==-1) 
	{
		SpiralInfo::Alert("Port clicked callback can't find source child.");
		return;
	}
	
	if(Value) // Turned on the port
	{
		if(m_IncompleteWire.InputChild==-1 || m_IncompleteWire.OutputChild==-1)
		{
			if (Type==Fl_DeviceGUI::OUTPUT)
			{
				// make sure we don't make a output->output connection
				if (m_IncompleteWire.OutputChild==-1)
				{	
					m_IncompleteWire.OutputChild=ChildNum;
					m_IncompleteWire.OutputPort=Port;
					m_IncompleteWire.OutputID=Device->GetID();
				}
				else
				{
					ClearIncompleteWire();
				}
			}
			else
			{
				// make sure we don't make a input->input connection
				if (m_IncompleteWire.InputChild==-1)				
				{					
					m_IncompleteWire.InputChild=ChildNum;
					m_IncompleteWire.InputPort=Port;
					m_IncompleteWire.InputID=Device->GetID();
				}
				else
				{
					ClearIncompleteWire();
				}
			}

			// if both have now been set...
			if (m_IncompleteWire.InputChild!=-1 && m_IncompleteWire.OutputChild!=-1)
			{
				m_WireVec.push_back(m_IncompleteWire);	
	
				// send the connect callback
				cb_Connection(this,(void*)&m_IncompleteWire);
				m_Graph.AddConnection(m_IncompleteWire.OutputID,m_IncompleteWire.InputID);
				
				// Turn on both ports
				Fl_DeviceGUI* ODGUI = (Fl_DeviceGUI*)(child(m_IncompleteWire.OutputChild));
				ODGUI->AddConnection(m_IncompleteWire.OutputPort+ODGUI->GetInfo()->NumInputs);
					
				Fl_DeviceGUI* IDGUI = (Fl_DeviceGUI*)(child(m_IncompleteWire.InputChild));
				IDGUI->AddConnection(m_IncompleteWire.InputPort);
				
				
				m_IncompleteWire.Clear();
				
				redraw();
			}
		}
	}
	else // Turned off the port
	{	
		// Find connections using this port
		bool Found=true;
		
		while (Found)
		{
			Found=false;
			
			for(vector<CanvasWire>::iterator i=m_WireVec.begin();
				i!=m_WireVec.end(); i++)
			{
				if ((Type==Fl_DeviceGUI::OUTPUT && i->OutputChild==ChildNum && i->OutputPort==Port) ||
					(Type==Fl_DeviceGUI::INPUT && i->InputChild==ChildNum && i->InputPort==Port))
				{
					// Turn off both ports
					Fl_DeviceGUI* ODGUI = (Fl_DeviceGUI*)(child(i->OutputChild));
					ODGUI->RemoveConnection(i->OutputPort+ODGUI->GetInfo()->NumInputs);
					
					Fl_DeviceGUI* IDGUI = (Fl_DeviceGUI*)(child(i->InputChild));
					IDGUI->RemoveConnection(i->InputPort);
									
					// send the unconnect callback	
					cb_Unconnect(this,(void*)&(*i));
					m_Graph.RemoveConnection(i->OutputID,i->InputID);		

					// Remove the wire
					m_WireVec.erase(i);
					
					Found=true;
					break;
				}
			}
		}
		
		redraw();
		
		// Clear the current selection
		m_IncompleteWire.Clear();
	}
}

void Fl_Canvas::ClearConnections(Fl_DeviceGUI* Device)
{
	// find out which child this comes from.
	int ChildNum=-1;	
	for(int n=0; n<children(); n++)
	{
		if(child(n)==Device)
		{
			ChildNum=n;
		}
	}
	
	if (ChildNum==-1) 
	{
		SpiralInfo::Alert("Clear connections callback can't find source child.");
		return;
	}
	
	bool removedall=false;
		
	while (!removedall)
	{
		removedall=true;
		
		for (vector<CanvasWire>::iterator i=m_WireVec.begin();
			 i!=m_WireVec.end(); i++)
		{	
			if (i->OutputChild==ChildNum ||
			    i->InputChild==ChildNum)
			{
				// Turn off both ports
				((Fl_DeviceGUI*)(child(i->OutputChild)))->RemoveConnection(i->OutputPort+
					((Fl_DeviceGUI*)(child(i->OutputChild)))->GetInfo()->NumInputs);
				((Fl_DeviceGUI*)(child(i->InputChild)))->RemoveConnection(i->InputPort);

				// send the unconnect callback
				cb_Unconnect(this,(void*)&(*i));
				m_Graph.RemoveConnection(i->OutputID,i->InputID);
				
				m_WireVec.erase(i);
				removedall=false;
				break;
			}
		}
	}
}
	
void Fl_Canvas::RemoveDevice(Fl_DeviceGUI* Device)
{
	// find out which child this comes from.
	int ChildNum=-1;	
	for(int n=0; n<children(); n++)
	{
		if(child(n)==Device)
		{
			ChildNum=n;
		}
	}
	
	if (ChildNum==-1) 
	{
		SpiralInfo::Alert("Remove device callback can't find source child.");
		return;
	}

	ClearConnections(Device);

	for (vector<CanvasWire>::iterator i=m_WireVec.begin();
		 i!=m_WireVec.end(); i++)
	{	
		if (i->OutputChild>ChildNum) i->OutputChild--;		
		if (i->InputChild>ChildNum)	 i->InputChild--;
	}
	
	remove(child(ChildNum));
	redraw(); 
}
	
void Fl_Canvas::Clear() 
{ 
	m_Graph.Clear();

	int kids=children();

	for(int n=0; n<kids; n++)
	{
		remove(child(0));		
	}	
	
	m_WireVec.clear(); 
	redraw(); 
}

	
	
istream &operator>>(istream &s, Fl_Canvas &o)
{
	int NumWires;
	s>>NumWires;
		
	for(int n=0; n<NumWires; n++)
	{
		CanvasWire NewWire;
		
		s>>NewWire.OutputID;
		s>>NewWire.OutputChild;
		s>>NewWire.OutputPort;
		s>>NewWire.InputID;
		s>>NewWire.InputChild;
		s>>NewWire.InputPort;	

		o.m_WireVec.push_back(NewWire);

		// Notify connection by callback
		o.cb_Connection(&o,(void*)&NewWire);
		o.m_Graph.AddConnection(NewWire.OutputID,NewWire.InputID);
			
		// Turn on both ports
		((Fl_DeviceGUI*)(o.child(NewWire.OutputChild)))->AddConnection(NewWire.OutputPort+
			((Fl_DeviceGUI*)(o.child(NewWire.OutputChild)))->GetInfo()->NumInputs);
		((Fl_DeviceGUI*)(o.child(NewWire.InputChild)))->AddConnection(NewWire.InputPort);
	}
		
	return s;
}

ostream &operator<<(ostream &s, Fl_Canvas &o)
{
	s<<o.m_WireVec.size()<<endl;

	for(vector<CanvasWire>::iterator i=o.m_WireVec.begin();
		i!=o.m_WireVec.end(); i++)
	{
		s<<i->OutputID<<" ";
		s<<i->OutputChild<<" ";
		s<<i->OutputPort<<" ";
		s<<i->InputID<<" ";
		s<<i->InputChild<<" ";
		s<<i->InputPort<<endl;	
	}

	return s;
}
