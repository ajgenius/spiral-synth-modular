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

#include "Fl_CommentGUI.h"
#include <FL/fl_draw.H>
#include "../../SpiralSound/SpiralInfo.h"
#include "../../GUI/Widgets/PawfalInput.h"

Fl_CommentGUI::Fl_CommentGUI(const DeviceGUIInfo& Info, SpiralGUIType *PW, Fl_Pixmap *Icon) :
Fl_DeviceGUI(Info,PW,Icon),
m_Comment("double click to edit.")
{
	box(FL_NO_BOX);
	m_DragBar->box(FL_NO_BOX);
	m_DragBar->label(m_Comment.c_str());
	m_DragBar->color(SpiralInfo::GUICOL_Device);
//	m_DragBar->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
}

int Fl_CommentGUI::handle(int event)
{
	if (Fl::event_button()==1 && event==FL_PUSH && Fl::event_clicks()==1)
	{
		char t[1024];
		if (Pawfal_Input("and your comment is:", m_Comment.c_str(), t))
		{
			string temp(t);
			if (temp!="")
			{
				m_Comment=temp;	
				m_DragBar->label(m_Comment.c_str());
				int width,height;
				fl_font(fl_font(),10);
				fl_measure(m_Comment.c_str(),width,height);
				m_DragBar->size(width,height);
				resize(x(),y(),width,height);
				redraw();
				parent()->redraw();
				//int x,y;
				//fl_measure(m_Comment.c_str(),x,y);
				//m_DragBar->size(x,y);
			}
		}
	}
	
	return Fl_DeviceGUI::handle(event);
}			
	
void Fl_CommentGUI::Setup(const DeviceGUIInfo& Info, bool FirstTime)
{
	Fl_DeviceGUI::Setup(Info,FirstTime);
}

void Fl_CommentGUI::Clear()
{
	Fl_DeviceGUI::Clear();
}

void Fl_CommentGUI::StreamOut(ostream &s)
{
	s<<1<<" ";
	s<<m_Comment.size()<<" ";
	s<<m_Comment<<endl;
}

void Fl_CommentGUI::StreamIn(istream &s)
{
	unsigned int size,version;
	string temp;
	
	s>>version;
	s>>size;
	size++; // see below
	char *str = new char[size+1];
	s.read(str,size);
	str[size]='\0';
	// have to move the pointer on by one, as the string
	// starts with " ", as the file pointer starts on
	// a blank space (after streaming the size)
	m_Comment=str+1; 
	delete[] str;
	
	m_DragBar->label(m_Comment.c_str());
	int width,height;
	fl_font(fl_font(),10);
	fl_measure(m_Comment.c_str(),width,height);
	m_DragBar->size(width,height);
	resize(x(),y(),width,height);
}
