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

#include "MatrixPluginGUI.h"
#include <FL/fl_draw.h>
#include <FL/fl_draw.H>
#include <FL/fl_file_chooser.h>

static const int GUI_COLOUR = 179;
static const int GUIBG_COLOUR = 144;
static const int GUIBG2_COLOUR = 145;

////////////////////////////////////////////
Fl_MatrixButton::Fl_MatrixButton(int x, int y, int w, int h, char* n) : 
Fl_Button(x,y,w,h,n),
m_Volume(NULL),
cb_VolChange(NULL),
cb_context(NULL)
{
	m_SliderHidden=true;
	m_VolVal=255;
}
	
void Fl_MatrixButton::draw()
{
	if (m_Volume)
	{
		m_VolVal=255-m_Volume->value();
		fl_color((char)m_VolVal,(char)m_VolVal,255);
		selection_color(fl_color());
		
		if (cb_VolChange) cb_VolChange(this,cb_context);
	}
	Fl_Button::draw();
}

int Fl_MatrixButton::handle(int event)
{
	if (value()==true && event==FL_PUSH && Fl::event_button()==3)
	{
		if (m_SliderHidden) 
		{
			m_Volume = new Fl_Slider(x(),y()+h(),w(),50,"");
			m_Volume->type(4);
			m_Volume->selection_color(GUI_COLOUR);
			m_Volume->maximum(255);
    		m_Volume->step(1);
    		m_Volume->value(255-m_VolVal);
			m_Volume->show();
			parent()->add(m_Volume);
			parent()->redraw();
			m_SliderHidden=false;
		}
		else 
		{
			m_Volume->hide();
			m_VolVal=m_Volume->value();
			parent()->remove(m_Volume);
			parent()->redraw();
			m_Volume=NULL;
			m_SliderHidden=true;
		}
		
		return 1;
	}
	
	if (Fl::event_button()!=3) return Fl_Button::handle(event);
	
	return 1;
}

////////////////////////////////////////////

MatrixPluginGUI::MatrixPluginGUI(int w, int h,MatrixPlugin *o,ChannelHandler *ch,const HostInfo *Info) :
SpiralPluginGUI(w,h,o,ch),
m_LastLight(0)
{	
	//size_range(10,10);
	m_NoteCut = new Fl_Button (5, h-30, 85, 20,"NoteCut");
	m_NoteCut->type(1);
	m_NoteCut->labelsize(10);
	m_NoteCut->value(0);
	m_NoteCut->callback((Fl_Callback*)cb_NoteCut);
	add(m_NoteCut);
	
	m_Pattern = new Fl_Counter(5, 20, 40, 20, "Pattern");
    m_Pattern->labelsize(10);
	m_Pattern->type(FL_SIMPLE_COUNTER);
	m_Pattern->step(1);
	m_Pattern->value(0);
	m_Pattern->callback((Fl_Callback*)cb_Pattern);
	add(m_Pattern);
	
	m_Length = new Fl_Counter(5, 55, 40, 20, "Length");
    m_Length->labelsize(10);
	m_Length->type(FL_SIMPLE_COUNTER);
	m_Length->step(1);
	m_Length->value(64);
	m_Length->callback((Fl_Callback*)cb_Length);
	add(m_Length);
	
	m_Speed = new Fl_Knob (50, 60, 40, 40, "Speed");
    m_Speed->color(GUI_COLOUR);
	m_Speed->type(Fl_Knob::DOTLIN);
    m_Speed->labelsize(10);
    m_Speed->maximum(200);
    m_Speed->step(0.01);
    m_Speed->value(1.0);   
	m_Speed->callback((Fl_Callback*)cb_Speed);
	add(m_Speed);
	
	m_SpeedVal = new Fl_Counter (5, 125, 85, 20, "");
	m_SpeedVal->labelsize(10);
	m_SpeedVal->value(10);
	m_SpeedVal->type(FL_SIMPLE_COUNTER);
	m_SpeedVal->step(1);
	m_SpeedVal->callback((Fl_Callback*)cb_Speed);
	add(m_SpeedVal);
	
	m_Octave = new Fl_Counter(5, 90, 40, 20, "Octave");
    m_Octave->labelsize(10);
	m_Octave->type(FL_SIMPLE_COUNTER);
	m_Octave->step(1);
	m_Octave->value(0);
	m_Octave->callback((Fl_Callback*)cb_Octave);
	add(m_Octave);
		
	m_CopyBtn = new Fl_Button (5, 150, 40, 20, "Copy");
	m_CopyBtn->labelsize (10);
	m_CopyBtn->callback ((Fl_Callback*)cb_CopyBtn);
	add (m_CopyBtn);

	m_PasteBtn = new Fl_Button (50, 150, 40, 20, "Paste");
	m_PasteBtn->labelsize (10);
	m_PasteBtn->deactivate();
	m_PasteBtn->callback ((Fl_Callback*)cb_PasteBtn);
	add (m_PasteBtn);

	m_ClearBtn = new Fl_Button (5, 175, 85, 20, "Clear");
	m_ClearBtn->labelsize (10);
	m_ClearBtn->callback ((Fl_Callback*)cb_ClearBtn);
	add (m_ClearBtn);

	m_TransUpBtn = new Fl_Button (5, 200, 40, 20, "Up");
	m_TransUpBtn->labelsize (10);
	m_TransUpBtn->callback ((Fl_Callback*)cb_TransUpBtn);
	add (m_TransUpBtn);

	m_TransDnBtn = new Fl_Button (50, 200, 40, 20, "Down");
	m_TransDnBtn->labelsize (10);
	m_TransDnBtn->callback ((Fl_Callback*)cb_TransDnBtn);
	add (m_TransDnBtn);

	m_TransLbl = new Fl_Box (5, 216, 85, 20, "Transpose");
	m_TransLbl->labelsize(10);
	add (m_TransLbl);

	int xoff=95;
	int yoff=40;
	int butsize=7;
	int n=0;
	
	fl_color(150,150,150);
	int markercol=fl_color();
	
	for(int x=0; x<MATX; x++)
	for(int y=0; y<MATY; y++)
	{
		Numbers[n]=n;
		m_Matrix[x][y] = new Fl_MatrixButton(xoff+x*butsize,yoff+y*butsize,butsize+1,butsize+1,"");
		m_Matrix[x][y]->type(1);
		m_Matrix[x][y]->box(FL_BORDER_BOX);
		if ((x%8)==0) m_Matrix[x][y]->color(markercol);
		else m_Matrix[x][y]->color(FL_GRAY);
		
		m_Matrix[x][y]->selection_color(FL_WHITE);
		m_Matrix[x][y]->callback((Fl_Callback*)cb_Matrix,(void*)&Numbers[n]);
		m_Matrix[x][y]->SetVolCallback((Fl_Callback*)cb_MatVol,(void*)&Numbers[n]);
		add(m_Matrix[x][y]);
		n++;
	}
	
	xoff=93;
	for(int x=0; x<MATX; x++)
	{
		m_Flash[x] = new Fl_LED_Button(xoff+x*butsize,20,15,15,"");
		m_Flash[x]->selection_color(FL_WHITE);
		add(m_Flash[x]);
	}
	
	end();
}

void MatrixPluginGUI::Update()
{	
	int Light=m_GUICH->GetInt("Step");
	if (Light!=m_LastLight)
	{
		m_Flash[Light]->value(1);
		m_Flash[m_LastLight]->value(0);
		m_LastLight=Light;
	}
}

void MatrixPluginGUI::UpdateValues(SpiralPlugin *o)
{
	MatrixPlugin *Plugin = (MatrixPlugin*)o;
	
	m_Pattern->value(Plugin->GetCurrent());
	m_Length->value(Plugin->GetPattern()->Length);
	m_Speed->value(Plugin->GetPattern()->Speed);
	m_SpeedVal->value((int)m_Speed->value());
	m_Octave->value(Plugin->GetPattern()->Octave);
	
	for(int x=0; x<MATX; x++)
	for(int y=0; y<MATY; y++)
	{
		m_Matrix[x][y]->value(Plugin->GetPattern()->Matrix[x][y]);
		m_Matrix[x][y]->SetVolume(Plugin->GetPattern()->Volume[x][y]);
	}        
}
	
void MatrixPluginGUI::UpdateMatrix()
{
	m_GUICH->Wait();
	m_GUICH->RequestChannelAndWait("Matrix");
	m_GUICH->GetData("Matrix",(void*)m_GUIMatrix);
	
	for(int x=0; x<MATX; x++)
	for(int y=0; y<MATY; y++)
	{
		m_Matrix[x][y]->value(m_GUIMatrix[(int)m_Pattern->value()].Matrix[x][y]);
		m_Matrix[x][y]->SetVolume(m_GUIMatrix[(int)m_Pattern->value()].Volume[x][y]);
	}        

}

inline void MatrixPluginGUI::cb_NoteCut_i(Fl_Button* o, void* v)
{ 
	m_GUICH->Set("NoteCut",o->value());
}
void MatrixPluginGUI::cb_NoteCut(Fl_Button* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_NoteCut_i(o,v);}

inline void  MatrixPluginGUI::cb_Matrix_i(Fl_Button* o, void* v)
{ 
	m_GUICH->Set("X",*(int*)v/MATY);
	m_GUICH->Set("Y",*(int*)v%MATY);

	if (o->value())	m_GUICH->SetCommand(MatrixPlugin::MAT_ACTIVATE);
	else m_GUICH->SetCommand(MatrixPlugin::MAT_DEACTIVATE);
	
	m_GUICH->Wait();
}
void  MatrixPluginGUI::cb_Matrix(Fl_Button* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_Matrix_i(o,v);}

inline void  MatrixPluginGUI::cb_MatVol_i(Fl_Button* o, void* v)
{ 
	m_GUICH->Set("X",*(int*)v/MATY);
	m_GUICH->Set("Y",*(int*)v%MATY);
	m_GUICH->Set("Volume",((Fl_MatrixButton*)o)->GetVolume());
	m_GUICH->SetCommand(MatrixPlugin::MAT_VOLUME);
}
void  MatrixPluginGUI::cb_MatVol(Fl_Button* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_MatVol_i(o,v);}

inline void MatrixPluginGUI::cb_Pattern_i(Fl_Counter* o, void* v)
{ 	
	if (o->value()<0) o->value(0);
	if (o->value()>NUM_PATTERNS-1) o->value(NUM_PATTERNS-1);
	m_GUICH->Set("Current",(int)o->value());
	UpdateMatrix();
}
void MatrixPluginGUI::cb_Pattern(Fl_Counter* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_Pattern_i(o,v);}

inline void MatrixPluginGUI::cb_Length_i(Fl_Counter* o, void* v)
{
	if (o->value()<1) o->value(1);
	if (o->value()>64) o->value(64);
 		
	m_GUICH->Set("Length",(int)o->value());
	m_GUICH->SetCommand(MatrixPlugin::MAT_LENGTH);

}
void MatrixPluginGUI::cb_Length(Fl_Counter* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_Length_i(o,v);}

inline void MatrixPluginGUI::cb_Speed_i(Fl_Knob* o, void* v)
{ 	
	// Round off value, but it should be a float for tweaking
	float value=o->value()+((int)o->value()-o->value());
	m_SpeedVal->value(value);
		
	m_GUICH->Set("Speed",(float)value);
	m_GUICH->SetCommand(MatrixPlugin::MAT_SPEED);
}
void MatrixPluginGUI::cb_Speed(Fl_Knob* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_Speed_i(o,v);}

inline void MatrixPluginGUI::cb_Octave_i(Fl_Counter* o, void* v)
{
	if (o->value()<0) o->value(0);
	if (o->value()>6) o->value(6);	
	
	m_GUICH->Set("Octave",(int)o->value());
	m_GUICH->SetCommand(MatrixPlugin::MAT_OCTAVE);
}
void MatrixPluginGUI::cb_Octave(Fl_Counter* o, void* v)
{ ((MatrixPluginGUI*)(o->parent()))->cb_Octave_i(o,v);}

inline void MatrixPluginGUI::cb_SpeedVal_i (Fl_Counter* o, void* v) 
{
       m_Speed->value(o->value());
}

void MatrixPluginGUI::cb_SpeedVal (Fl_Counter* o, void* v) 
{
     ((MatrixPluginGUI*)(o->parent())) -> cb_SpeedVal_i (o, v);
}

inline void MatrixPluginGUI::cb_CopyBtn_i (Fl_Button* o, void* v) 
{
       m_PasteBtn->activate();
	   m_GUICH->SetCommand(MatrixPlugin::COPY);
	   UpdateMatrix();
}

void MatrixPluginGUI::cb_CopyBtn (Fl_Button* o, void* v) 
{
     ((MatrixPluginGUI*)(o->parent())) -> cb_CopyBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_PasteBtn_i (Fl_Button* o, void* v) 
{
 	   m_GUICH->SetCommand(MatrixPlugin::PASTE);
	   UpdateMatrix();
}

void MatrixPluginGUI::cb_PasteBtn (Fl_Button* o, void* v) 
{
     ((MatrixPluginGUI*)(o->parent())) -> cb_PasteBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_ClearBtn_i (Fl_Button* o, void* v) 
{
  	   m_GUICH->SetCommand(MatrixPlugin::CLEAR);
	   UpdateMatrix();
}

void MatrixPluginGUI::cb_ClearBtn (Fl_Button* o, void* v) 
{
     ((MatrixPluginGUI*)(o->parent())) -> cb_ClearBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_TransUpBtn_i (Fl_Button* o, void* v) 
{
  	   m_GUICH->SetCommand(MatrixPlugin::TUP);
	   UpdateMatrix();
}

void MatrixPluginGUI::cb_TransUpBtn (Fl_Button* o, void* v) 
{
     ((MatrixPluginGUI*)(o->parent())) -> cb_TransUpBtn_i (o, v);
}

inline void MatrixPluginGUI::cb_TransDnBtn_i (Fl_Button* o, void* v) 
{
  	   m_GUICH->SetCommand(MatrixPlugin::TDOWN);
	   UpdateMatrix();
}

void MatrixPluginGUI::cb_TransDnBtn (Fl_Button* o, void* v) 
{
     ((MatrixPluginGUI*)(o->parent())) -> cb_TransDnBtn_i (o, v);
}
