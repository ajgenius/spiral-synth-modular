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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Counter.H>

#include "PoshSamplerPlugin.h"
#include "../SpiralPluginGUI.h"
#include "../Widgets/Fl_Knob.H"

#ifndef MixerGUI
#define MixerGUI

class Fl_WaveDisplay : public Fl_Widget
{
	public:
		Fl_WaveDisplay(int x,int y,int w,int h, char* Name);
		~Fl_WaveDisplay();		
		virtual void draw();		
		virtual int handle(int event);		
		void SetSample(Sample* s) { m_Sample=s; }
		long GetRangeStart()      { return m_StartPos; }
		long GetRangeEnd()        { return m_EndPos; }
		long GetViewStart()       { return m_ViewStart; }
		void SetViewStart(long s) { m_ViewStart=s; }
		long GetViewEnd()         { return m_ViewEnd; }
		void SetViewEnd(long s)   { m_ViewEnd=s; }
		void SetPlayPos(long s)   { m_PlayPos=s; if(m_PosMarker) redraw(); }
		void SetPlayStart(long s) { m_PlayStart=s; }
		long GetPlayStart()       { return m_PlayStart; }
		void SetLoopStart(long s) { m_LoopStart=s; }
		long GetLoopStart()       { return m_LoopStart; }
		void SetLoopEnd(long s)   { m_LoopEnd=s; }
		long GetLoopEnd()         { return m_LoopEnd; }
		void SetPosMarker(bool s) { m_PosMarker=s; }
		
	private:	
		Sample *m_Sample;
		long m_StartPos;
		long m_EndPos;

		long m_ViewStart;
		long m_ViewEnd;
		long m_PlayPos;
		
		long m_PlayStart;
		long m_LoopStart;
		long m_LoopEnd;
		bool m_PosMarker;
};


class PoshSamplerPluginGUI : public SpiralPluginGUI
{
public:
	PoshSamplerPluginGUI(int w, int h, PoshSamplerPlugin *o,const HostInfo *Info);
	
	virtual void UpdateValues();
	virtual SpiralPlugin* GetPlugin() { return m_Plugin; }
	
	void SetPlayPos(long s) { m_Display->SetPlayPos(s); }
	int  GetCurrentSample() { return (int)m_SampleNum->value(); }
	
	PoshSamplerPlugin *m_Plugin;	
private:
	
	int Numbers[NUM_SAMPLES];
	
	Fl_Button* m_Load;
	Fl_Button* m_Save;
	Fl_Button* m_PosMarker;
	Fl_Knob* m_Volume;
	Fl_Knob* m_Pitch;
	Fl_Knob* m_Octave;
	Fl_Button* m_Loop;
	Fl_Button* m_PingPong;
	Fl_Button* m_Record;
	Fl_Counter* m_Note;
	Fl_WaveDisplay* m_Display;
	Fl_Counter* m_SampleNum;
	
	Fl_Button* m_Cut;
	Fl_Button* m_Copy;
	Fl_Button* m_Paste;
	Fl_Button* m_Mix;
	Fl_Button* m_Crop;
	Fl_Button* m_Reverse;
	Fl_Button* m_Amp;
	
	//// Callbacks ////
	inline void cb_Load_i(Fl_Button* o, void* v);
	static void cb_Load(Fl_Button* o, void* v); 
	inline void cb_Save_i(Fl_Button* o, void* v);
	static void cb_Save(Fl_Button* o, void* v); 
	inline void cb_PosMarker_i(Fl_Button* o, void* v);
	static void cb_PosMarker(Fl_Button* o, void* v); 
	inline void cb_Record_i(Fl_Button* o, void* v);
	static void cb_Record(Fl_Button* o, void* v); 
	inline void cb_Volume_i(Fl_Knob* o, void* v);
	static void cb_Volume(Fl_Knob* o, void* v); 
	inline void cb_Pitch_i(Fl_Knob* o, void* v);
	static void cb_Pitch(Fl_Knob* o, void* v); 
	inline void cb_Octave_i(Fl_Knob* o, void* v);
	static void cb_Octave(Fl_Knob* o, void* v); 
	inline void cb_Loop_i(Fl_Button* o, void* v);
	static void cb_Loop(Fl_Button* o, void* v); 
	inline void cb_PingPong_i(Fl_Button* o, void* v);
	static void cb_PingPong(Fl_Button* o, void* v); 
	inline void cb_Note_i(Fl_Counter* o, void* v);
	static void cb_Note(Fl_Counter* o, void* v); 
	inline void cb_SampleNum_i(Fl_Counter* o, void* v);
	static void cb_SampleNum(Fl_Counter* o, void* v); 

	inline void cb_Cut_i(Fl_Button* o, void* v);
	static void cb_Cut(Fl_Button* o, void* v); 
	inline void cb_Copy_i(Fl_Button* o, void* v);
	static void cb_Copy(Fl_Button* o, void* v); 
	inline void cb_Paste_i(Fl_Button* o, void* v);
	static void cb_Paste(Fl_Button* o, void* v); 
	inline void cb_Mix_i(Fl_Button* o, void* v);
	static void cb_Mix(Fl_Button* o, void* v); 
	inline void cb_Crop_i(Fl_Button* o, void* v);
	static void cb_Crop(Fl_Button* o, void* v); 
	inline void cb_Reverse_i(Fl_Button* o, void* v);
	static void cb_Reverse(Fl_Button* o, void* v); 
	inline void cb_Amp_i(Fl_Button* o, void* v);
	static void cb_Amp(Fl_Button* o, void* v); 
	inline void cb_WaveDisplay_i(Fl_WaveDisplay* o, void* v);
	static void cb_WaveDisplay(Fl_WaveDisplay* o, void* v); 
};

#endif
