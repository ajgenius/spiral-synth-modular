/*  SpiralLoops
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

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <iostream>

#ifndef LOOPWIDGET
#define LOOPWIDGET

class Fl_Loop : public Fl_Group
{
public:
	Fl_Loop(int x, int y, int w, int h, const char* label=0);
	
	virtual void draw();
	virtual int  handle(int  event);
	
	void SetData(float const *set) {m_data=set;}
	void SetLength(const int Len);
	void SetPos(float *p) {m_Pos=p;}
	void SetUpdate(bool p) {m_Update=p;}
	void StopUpdate(bool p) {m_StopUpdate=p;}
	void SetWaveSize(float s) {m_WaveSize=s;}
	void SetMainWin(Fl_Window *s) {m_MainWin=s;}
	void SetIdle();
	void UnsetIdle();
	void DrawWav();
	void DrawWidgets();
	void DrawEveryThing();
	void DrawPosMarker();
	void SetSnap(bool s) {m_Snap=s;}
	void SetSnapAngle(int s) {m_SnapDegrees=s;}
	
	typedef void (cb_CopyBuf1)(Fl_Loop *, int, int);
	typedef void (cb_CopyBuf2)(Fl_Loop *, int);
	
	void SetupCopyBufFuncs(cb_CopyBuf1 *Cut,
					       cb_CopyBuf1 *Copy,
						   cb_CopyBuf2 *Paste,
						   cb_CopyBuf2 *PasteMix,
						   cb_CopyBuf1 *cb_ZeroRange,
						   cb_CopyBuf1 *cb_ReverseRange,
						   cb_CopyBuf1 *cb_Halve,
						   cb_CopyBuf1 *cb_Hold,
						   cb_CopyBuf1 *cb_SelectAll,
						   cb_CopyBuf1 *cb_NewTrigger,
						   cb_CopyBuf1 *cb_Move);
						   
	int GetX() {return x();}
	int GetY() {return y();}
	int GetW() {return w();}
	int GetH() {return h();}	
	
private:

	float const *m_data;

	Fl_Window *m_MainWin;

	int    m_Length;
	int    m_InnerRad;
	int    m_OuterRad;
	int    m_BorderRad;
	int    m_IndSX,m_IndSY,m_IndEX,m_IndEY;
	int    m_MidX,m_MidY;
	
	float  m_StartAngle;
	float  m_EndAngle;
	float  m_MoveAngle;
	int    m_RangeStart;
	int    m_RangeEnd;
	float  m_Angle;
	float *m_Pos;
	bool   m_Update;
	bool   m_StopUpdate;
	float  m_WaveSize;
	int    m_Move;
	int    m_LastMove;
	bool   m_Snap;
	int    m_SnapDegrees;
	int    m_PosMarkerCount;
	
	static void cb_Idle(void* o);
	
	cb_CopyBuf1 *cb_Cut;
	cb_CopyBuf1 *cb_Copy;
	cb_CopyBuf2 *cb_Paste;
	cb_CopyBuf2 *cb_PasteMix;
	cb_CopyBuf1 *cb_ZeroRange;
	cb_CopyBuf1 *cb_ReverseRange;
	cb_CopyBuf1 *cb_Halve;
	cb_CopyBuf1 *cb_Hold;
	cb_CopyBuf1 *cb_SelectAll;
	cb_CopyBuf1 *cb_NewTrigger;
	cb_CopyBuf1 *cb_Move;
	
};

#endif
