/*  SpiralSound
 *  Copyleft (C) 2001 David Griffiths <dave@pawfal.org>
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

#include "../SpiralPlugin.h"
#include <FL/Fl_Pixmap.H>

#ifndef MatrixPLUGIN
#define MatrixPLUGIN

static const int MATX         = 64;
static const int MATY         = 32;
static const int NUM_PATTERNS = 16;

struct Pattern
{
	int Length;
	float Speed;
	int Octave;
	bool Matrix[MATX][MATY];
};

class MatrixPlugin : public SpiralPlugin
{
public:
 	MatrixPlugin();
	virtual ~MatrixPlugin();
	
	virtual PluginInfo &Initialise(const HostInfo *Host);
	virtual SpiralGUIType *CreateGUI();
	virtual void Execute();
	virtual void ExecuteCommands();
	virtual void StreamOut(ostream &s);
	virtual void StreamIn(istream &s);
		
	bool  GetNoteCut()               { return m_NoteCut; }
	int   GetCurrent()               { return m_Current; }
	float GetStepTime()              { return m_StepTime; }
	Pattern* GetPattern() 			 { return &m_Matrix[m_Current]; }
		
	enum GUICommands{NONE,MAT_LENGTH,MAT_SPEED,MAT_ACTIVATE,MAT_DEACTIVATE,
						MAT_OCTAVE,COPY,PASTE,CLEAR,TUP,TDOWN};
	
	struct GUIArgs
	{
		int Num;
		int Length;
		float Speed;
		int X,Y;
		int Octave;
	};
	
private:

	GUIArgs m_GUIArgs;

	void CopyPattern() { m_CopyPattern = m_Current; }
    void PastePattern();
    void ClearPattern();
    void TransposeUp();
    bool CanTransposeUp();
    void TransposeDown();
    bool CanTransposeDown();
	
	float m_TickTime;
	float m_StepTime;
	float m_Time;
	int   m_Step;
	bool  m_Loop;
	bool  m_NoteCut;
	
	int   m_Current;
	Pattern m_Matrix[NUM_PATTERNS];
	float m_TriggerLevel[NUM_PATTERNS];
	
	float m_CurrentNoteCV;
	float m_CurrentTriggerCV;
	
	bool  m_Triggered;
	bool  m_ClockHigh;
	int m_CopyPattern;
};

#endif
