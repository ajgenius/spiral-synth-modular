/*  SpiralSynthModular
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

#ifndef SPIRALSYNTHMODULAR
#define SPIRALSYNTHMODULAR

#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Tile.H>
#include <sstream>
#include <iostream>
#include <map>
#include "SpiralSynthModularInfo.h"
#include "GUI/Widgets/Fl_DeviceGUI.h"
#include "GUI/Widgets/Fl_CommentGUI.h"
#include "GUI/Widgets/Fl_Canvas.h"
#include "SpiralSound/Plugins/SpiralPlugin.h"
#include "SpiralSound/ChannelHandler.h"
#include "SettingsWindow.h"

const static string VER_STRING = "0.2.0";

typedef Fl_Double_Window SpiralWindowType;
// typedef Fl_Window SpiralWindowType;

class SettingsWindow;

class DeviceWin
{
public:
	DeviceWin() : m_DeviceGUI(NULL) {}
	~DeviceWin();
	
	int 		  m_PluginID;
	SpiralPlugin* m_Device;
	Fl_DeviceGUI* m_DeviceGUI;
};

class SynthModular
{
public:
	SynthModular();
	~SynthModular();
	
	SpiralWindowType * CreateWindow();
	void LoadPlugins(string pluginPath);
	void Update();
	void AddDevice(int n, int x, int y);
	void AddComment(int n);
	void ClearUp();	
	void UpdateHostInfo();	
	bool CallbackMode() { return m_CallbackUpdateMode; }
	bool IsBlockingOutputPluginReady() { return m_BlockingOutputPluginIsReady; }
	void UpdatePluginGUIs();
	void LoadPatch(const char *fn);	
	
	void PauseAudio() 
	{
		m_CH.Set("PauseAudio",true);
		m_CH.Wait();
	}

	void ResumeAudio()
	{
		m_CH.Set("PauseAudio",false);
	}
	
	// only for audio thread
	bool IsPaused() { return m_PauseAudio; }
	
private:

	DeviceWin* NewDeviceWin(int n, int x, int y);
	DeviceWin* NewComment(int n, int x, int y);

	HostInfo m_Info;

	static DeviceGUIInfo BuildDeviceGUIInfo(PluginInfo &PInfo);

	// currently static, to allow the callback 
	// cb_UpdatePluginInfo to access the map. 
	static map<int,DeviceWin*> m_DeviceWinMap;
	
	int m_NextID;
	int m_NextPluginButton;
	int m_NextPluginButtonXPos;
	int m_NextPluginButtonYPos;
	
	static bool m_CallbackUpdateMode;
	static bool m_BlockingOutputPluginIsReady;
	string m_FilePath;
	
	// Main GUI stuff
	void CreateGUI(int xoff=0, int yoff=0, char *name="");
		
	Fl_Group 		*m_MainButtons;		
	Fl_Button       *m_Load;
	Fl_Button       *m_Save;
	Fl_Button       *m_New;
	Fl_Button       *m_OpenEditor;	
	Fl_Button       *m_Options;
	Fl_Group	 	*m_AppGroup;	
	Fl_Scroll		*m_AppScroll;
	
	Fl_Canvas 		*m_Canvas;	
	Fl_Scroll		*m_CanvasScroll;
	Fl_Scroll 		*m_ToolBox;
	Fl_Group 		*m_Buttons;		
	Fl_Button       *m_NewComment;
		
	Fl_Box          *m_GroupName;
	Fl_Button       *m_PluginGroupLeft;
	Fl_Button       *m_PluginGroupRight;

	class ToolBox
	{
	public:
		ToolBox(Fl_Scroll *parent, void* user);
		~ToolBox() {}
		
		void AddIcon(Fl_Button *Icon);
		Fl_Pack *GetToolPack() { return m_ToolPack; }
		
	private:
		Fl_Pack *m_ToolPack;
		Fl_Pack *m_IconPack;
		int m_Icon;
	
	};
	
	map<string,ToolBox*> m_PluginGroupMap;
	map<string,ToolBox*>::iterator m_CurrentGroup;
	
	SettingsWindow  *m_SettingsWindow;
	
	SpiralWindowType* m_TopWindow;
	Fl_Tile* m_TopTile;
	
	Fl_Group* m_MainWindow;
	Fl_Group* m_EditorWindow;

	vector<Fl_Button*> m_DeviceVec;
	
	ChannelHandler m_CH; // used for threadsafe communication
	bool m_PauseAudio;
	
	inline void cb_NewDevice_i(Fl_Button* o, void* v);
    static void cb_NewDevice(Fl_Button* o, void* v);
	inline void cb_NewDeviceFromMenu_i(Fl_Canvas* o, void* v);
	static void cb_NewDeviceFromMenu(Fl_Canvas* o, void* v);
	inline void cb_NewComment_i(Fl_Button* o, void* v);
    static void cb_NewComment(Fl_Button* o, void* v);
	inline void cb_Load_i(Fl_Button* o, void* v);
    static void cb_Load(Fl_Button* o, void* v);
	inline void cb_Save_i(Fl_Button* o, void* v);
    static void cb_Save(Fl_Button* o, void* v);
	inline void cb_New_i(Fl_Button* o, void* v);
    static void cb_New(Fl_Button* o, void* v);
	inline void cb_Connection_i(Fl_Canvas* o, void* v);
	static void cb_Connection(Fl_Canvas* o, void* v);
	inline void cb_Unconnect_i(Fl_Canvas* o, void* v);
	static void cb_Unconnect(Fl_Canvas* o, void* v);
	inline void cb_Close_i(Fl_Window* o, void* v);
	static void cb_Close(Fl_Window* o, void* v);
	inline void cb_PluginGroupLeft_i(Fl_Button* o, void* v);
    static void cb_PluginGroupLeft(Fl_Button* o, void* v);
	inline void cb_PluginGroupRight_i(Fl_Button* o, void* v);
    static void cb_PluginGroupRight(Fl_Button* o, void* v);
	
	inline void cb_Rload_i(Fl_Button* o, void* v);
    static void cb_Rload(Fl_Button* o, void* v);
	
    static void cb_Update(void* o, bool Mode);	
    static void cb_Blocking(void* o, bool Mode);	
	static void cb_UpdatePluginInfo(int ID, void *PluginInfo);

	
	friend istream &operator>>(istream &s, SynthModular &o);
	friend ostream &operator<<(ostream &s, SynthModular &o);	
};

istream &operator>>(istream &s, SynthModular &o);
ostream &operator<<(ostream &s, SynthModular &o);

#endif
