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

#include "LogicPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <stdio.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////

LogicPluginGUI::LogicPluginGUI(int w, int h,LogicPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch)
{	
	int Width=32,Depth=20,Pos=0,Count=0;
	
	m_AND = new Fl_Button(5, 15, Width, Depth, "AND");
    m_AND->type(1);
	m_AND->value(1);
	m_AND->labelsize(10);
    m_AND->selection_color(GUI_COLOUR);
 	m_AND->callback((Fl_Callback*)cb_AND);

	m_OR = new Fl_Button(5, 35, Width, Depth,"OR");
    m_OR->type(1);
	m_OR->labelsize(10);
    m_OR->selection_color(GUI_COLOUR);
	m_OR->callback((Fl_Callback*)cb_OR);
	
	m_NOT = new Fl_Button(5, 55, Width, Depth,"NOT");
    m_NOT->type(1);
	m_NOT->labelsize(10);
    m_NOT->selection_color(GUI_COLOUR);
 	m_NOT->callback((Fl_Callback*)cb_NOT);
	
	m_NAND = new Fl_Button(5, 75, Width, Depth,"NAND");
    m_NAND->type(1);
 	m_NAND->labelsize(10);
  	m_NAND->selection_color(GUI_COLOUR);
	m_NAND->callback((Fl_Callback*)cb_NAND);
	
	m_NOR = new Fl_Button(37, 15, Width, Depth,"NOR");
    m_NOR->type(1);
	m_NOR->labelsize(10);
    m_NOR->selection_color(GUI_COLOUR);
	m_NOR->callback((Fl_Callback*)cb_NOR);
	
	m_XOR = new Fl_Button(37, 35, Width, Depth,"XOR");
    m_XOR->type(1);
	m_XOR->labelsize(10);
    m_XOR->selection_color(GUI_COLOUR);
 	m_XOR->callback((Fl_Callback*)cb_XOR);
	
	m_XNOR = new Fl_Button(37, 55, Width, Depth,"XNOR");
    m_XNOR->type(1);
	m_XNOR->labelsize(10);
    m_XNOR->selection_color(GUI_COLOUR);
	m_XNOR->callback((Fl_Callback*)cb_XNOR);
	
	end();
}

void LogicPluginGUI::ClearButtons()
{
	m_AND->value(false);
	m_OR->value(false);
	m_NOT->value(false);
	m_NAND->value(false);
	m_NOR->value(false);
	m_XOR->value(false);
	m_XNOR->value(false);
}
	
void LogicPluginGUI::UpdateValues(SpiralPlugin *o)
{
	LogicPlugin* Plugin = (LogicPlugin*)o;
	ClearButtons();
	switch (Plugin->GetOperator())
	{
		case LogicPlugin::AND  : m_AND->value(true); break;
		case LogicPlugin::OR   : m_OR->value(true); break;
		case LogicPlugin::NOT  : m_NOT->value(true); break;
		case LogicPlugin::NAND : m_NAND->value(true); break;
		case LogicPlugin::NOR  : m_NOR->value(true); break;
		case LogicPlugin::XOR  : m_XOR->value(true); break;
		case LogicPlugin::XNOR : m_XNOR->value(true); break;
	}
}

//// Callbacks ////
inline void LogicPluginGUI::cb_AND_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::AND);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_AND(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_AND_i(o,v);}

inline void LogicPluginGUI::cb_OR_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::OR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_OR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_OR_i(o,v);}

inline void LogicPluginGUI::cb_NOT_i(Fl_Button* o, void* v)
{
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::NOT);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_NOT(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_NOT_i(o,v);}

inline void LogicPluginGUI::cb_NAND_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::NAND);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_NAND(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_NAND_i(o,v);}

inline void LogicPluginGUI::cb_NOR_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::NOR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_NOR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_NOR_i(o,v);}

inline void LogicPluginGUI::cb_XOR_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::XOR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_XOR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_XOR_i(o,v);}

inline void LogicPluginGUI::cb_XNOR_i(Fl_Button* o, void* v)
{	
	if (o->value())
	{
		ClearButtons();
		o->value(true);
		m_GUICH->Set("Operator",(int)LogicPlugin::XNOR);
	}
	else
	{
		o->value(true);
	}
}
void LogicPluginGUI::cb_XNOR(Fl_Button* o, void* v)
{ ((LogicPluginGUI*)(o->parent()))->cb_XNOR_i(o,v);}

const string LogicPluginGUI::GetHelpText(const string &loc){
    return string("")
    + "1001010111010101101111101010100101010101010100010100100101\n"
    + "0010101010111010010010101010001010011110001010101000101010\n"
	+ "1110111101101001000010101010111110101010101010101111010101\n"
	+ "0011011111010101101000001010101010001010100001100111010111";
}
