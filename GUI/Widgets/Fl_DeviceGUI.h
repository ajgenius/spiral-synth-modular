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

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.h>
#include <iostream>
#include <vector>
#include <string>
#include "Fl_DragBar.H"
#include <stdio.h>

#ifndef DEVICEGUI
#define DEVICEGUI

#define WIRE_COL0 91
#define WIRE_COL1 FL_GREEN
#define WIRE_COL2 FL_YELLOW
#define WIRE_COL3 FL_BLUE
#define WIRE_COL4 FL_CYAN

using namespace std;

static const int TitleBarHeight = 12;
static const int PortGroupWidth = 12;
static const int PortSize=6;

struct DeviceGUIInfo
{
	int XPos;
	int YPos;	
	int Width;
	int Height;
	int NumInputs;
	int NumOutputs;
	vector<string> PortTips;
	string Name;
	vector<int> PortTypes;
};

class Fl_DeviceGUI : public Fl_Group
{
public:
	Fl_DeviceGUI(const DeviceGUIInfo& Info, Fl_Group *PW, Fl_Pixmap *Icon);
	
	virtual int handle(int event);
	virtual void draw();
	
	enum PortType {INPUT,OUTPUT};
		
	int           GetID()           			 { return m_ID; }	
	void          SetID(int s)      			 { m_ID=s; /*DisplayID(s);*/ } 		
	
	/*char did[8];
	void DisplayID(int s) { sprintf(did,"%d",s); label(did); }*/
											
	bool          Killed()          			 { return m_DelMe; }	
	int           GetPortX(int n)                { return m_PortVec[n]->x()+PortSize/2; }
	int           GetPortY(int n)                { return m_PortVec[n]->y()+PortSize/2; }
	void          ForcePortValue(int n, bool v)  { m_PortVec[n]->value(v); }	
	const         DeviceGUIInfo* GetInfo()       { return &m_Info; }
	Fl_Group*     GetPluginWindow()              { return m_PluginWindow; }
	
	// automatically called from the constructor, but may be redone at any time.
	virtual void  Setup(const DeviceGUIInfo& Info, bool FirstTime = false); 
	virtual void  Clear();
	int           GetPortType(int n)             { return m_Info.PortTypes[n]; }
		
protected:

	DeviceGUIInfo m_Info;

	Fl_DragBar* m_DragBar;
	Fl_Group*   m_PluginWindow;
	Fl_Pixmap*  m_Icon;
		
private:

	inline void cb_Port_i(Fl_Button* o, void* v);
	static void cb_Port(Fl_Button* o, void* v);
	
	vector<Fl_Button*> m_PortVec;
	
	static int Numbers[512];

	string m_Name;
	int    m_ID;
	bool   m_DelMe;
};

#endif
