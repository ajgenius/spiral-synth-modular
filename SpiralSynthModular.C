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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/fl_file_chooser.h>
#include <FL/fl_ask.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Tooltip.h>

#include "SpiralSynthModular.h"
#include "SpiralSound/PluginManager.h"

#include "GUI/SSM.xpm"
#include "GUI/load.xpm"
#include "GUI/save.xpm"
#include "GUI/new.xpm"
#include "GUI/options.xpm"
#include "GUI/edit.xpm"
#include "GUI/comment.xpm"

const static string LABEL = "SpiralSynthModular 0.1.1b1";
static string TITLEBAR;

static const int FILE_VERSION = 3;
static int Numbers[512];

static const int MAIN_WIDTH     = 700;
static const int MAIN_HEIGHT    = 300;
static const int SLIDER_WIDTH   = 15;
static const int TOOLBOX_HEIGHT = MAIN_HEIGHT-40;
static const int TOOLBOX_WIDTH  = 132+SLIDER_WIDTH;
static const int ICON_DEPTH     = 3;
static const int COMMENT_ID     = -1;

using namespace std;

map<int,DeviceWin*> SynthModular::m_DeviceWinMap;
bool SynthModular::m_CallbackUpdateMode = false;

//////////////////////////////////////////////////////////

DeviceWin::~DeviceWin()
{	
} 

//////////////////////////////////////////////////////////
		
SynthModular::SynthModular():
m_NextID(0),
m_NextPluginButton(1),
m_NextPluginButtonXPos(5),
m_NextPluginButtonYPos(20)
{ 	
	m_Info.BUFSIZE    = SpiralInfo::BUFSIZE;
	m_Info.FRAGSIZE   = SpiralInfo::FRAGSIZE;
	m_Info.FRAGCOUNT  = SpiralInfo::FRAGCOUNT;	
	m_Info.SAMPLERATE = SpiralInfo::SAMPLERATE;
	m_Info.OUTPUTFILE = SpiralInfo::OUTPUTFILE;
	m_Info.MIDIFILE   = SpiralInfo::MIDIFILE; 
	m_Info.POLY       = SpiralInfo::POLY;
	//m_Info.GUI_COLOUR = SpiralInfo::GUI_COLOUR;
	
	for (int n=0; n<512; n++) Numbers[n]=n;	
}

//////////////////////////////////////////////////////////

SynthModular::~SynthModular()
{ 
	ClearUp();
	PluginManager::Get()->PackUpAndGoHome();
}

//////////////////////////////////////////////////////////

void SynthModular::ClearUp()
{
	for(map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin();
		i!=m_DeviceWinMap.end(); i++)
	{
		if (i->second->m_DeviceGUI->GetPluginWindow())
		{
			i->second->m_DeviceGUI->GetPluginWindow()->hide();
			m_MainWindow->remove(i->second->m_DeviceGUI->GetPluginWindow());			
		}
		// deleted by Canvas::Remove()? seems to cause random crashes
		//delete i->second->m_DeviceGUI;
		delete i->second->m_Device;
	}
	
	m_Canvas->Clear();
	m_DeviceWinMap.clear();
	m_NextID=0;
}

//////////////////////////////////////////////////////////

void SynthModular::Update()
{	
	// run the plugins
	list<int> ExecutionOrder = m_Canvas->GetGraph()->GetSortedList();
	for (list<int>::reverse_iterator i=ExecutionOrder.rbegin(); 
		 i!=ExecutionOrder.rend(); i++)
	{
		// use the graphsort order to remove internal latency
		map<int,DeviceWin*>::iterator di=m_DeviceWinMap.find(*i);
		if (di!=m_DeviceWinMap.end() && di->second->m_Device) 
		{
			di->second->m_Device->Execute();
		}
	}
}

//////////////////////////////////////////////////////////

void SynthModular::UpdatePluginGUIs()
{
	// see if any need deleting		
	for (map<int,DeviceWin*>::iterator i=m_DeviceWinMap.begin(); 
		 i!=m_DeviceWinMap.end(); i++)
	{	
		if (i->second->m_DeviceGUI->Killed())
		{
			if (i->second->m_DeviceGUI->GetPluginWindow())
			{
				i->second->m_DeviceGUI->GetPluginWindow()->hide();
				m_MainWindow->remove(i->second->m_DeviceGUI->GetPluginWindow());
			}
			i->second->m_DeviceGUI->Clear();
			m_Canvas->RemoveDevice(i->second->m_DeviceGUI);			
			// deleted by Canvas::Remove()? seems to cause random crashes 
			//delete i->second->m_DeviceGUI;			
			if (i->second->m_Device) delete i->second->m_Device;
			m_DeviceWinMap.erase(i);
			break;
		}
	}
}

//////////////////////////////////////////////////////////

SpiralWindowType *SynthModular::CreateWindow()
{
	int xoff=0, yoff=10, but=64, gap=MAIN_HEIGHT/4, n=0;
	
	m_TopWindow = new SpiralWindowType(MAIN_WIDTH, MAIN_HEIGHT*2, LABEL.c_str());
	m_TopWindow->resizable(m_TopWindow);
	
	m_TopTile = new Fl_Tile(0,0,MAIN_WIDTH, MAIN_HEIGHT*2, "");
	m_TopWindow->add(m_TopTile);

	m_MainWindow = new Fl_Tile(0,0,MAIN_WIDTH, MAIN_HEIGHT, "");
	m_MainWindow->color(SpiralSynthModularInfo::GUICOL_Canvas);
 	//m_MainWindow->callback((Fl_Callback*)cb_Close);
	m_MainWindow->user_data((void*)(this));
	m_TopTile->add(m_MainWindow);
	
	m_MainButtons = new Fl_Group(0, 0, but, MAIN_HEIGHT, "");
    m_MainButtons->type(1);
	m_MainButtons->color(SpiralSynthModularInfo::GUICOL_Tool);
	m_MainButtons->box(FL_FLAT_BOX);
	m_MainButtons->user_data((void*)(this));
	m_MainWindow->add(m_MainButtons); 

	m_AppScroll = new Fl_Scroll(but, 0, MAIN_WIDTH-but, MAIN_HEIGHT, "");	
	m_AppScroll->scrollbar.align(FL_ALIGN_RIGHT);
	m_MainWindow->add(m_AppScroll);
	//m_MainWindow->resizable(m_AppScroll);

	m_AppGroup = new Fl_Group(-5000, -5000, 10000, 10000, "AppGroup");
    m_AppGroup->type(1);
	m_AppGroup->box(FL_FLAT_BOX);
    m_AppGroup->labeltype(FL_ENGRAVED_LABEL);
    m_AppGroup->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	m_AppGroup->color(SpiralSynthModularInfo::GUICOL_Canvas);
	m_AppScroll->add(m_AppGroup);   	

	m_Load = new Fl_Button(xoff, 5+yoff, but, but, "");
	m_Load->box(FL_NO_BOX);
	Fl_Pixmap *tPix = new Fl_Pixmap(load_xpm);
	m_Load->image(tPix->copy(tPix->w(),tPix->h()));		
	m_Load->selection_color(SpiralSynthModularInfo::GUICOL_Tool);
    m_Load->tooltip("Load a design file");		 
	m_Load->callback((Fl_Callback*)cb_Load);
	m_MainButtons->add(m_Load);
	n++;
	
	m_Save = new Fl_Button(xoff, n*gap+yoff, but, but, "");
	m_Save->box(FL_NO_BOX);
	tPix = new Fl_Pixmap(save_xpm);
	m_Save->image(tPix->copy(tPix->w(),tPix->h()));	
	delete tPix;	
	m_Save->selection_color(SpiralSynthModularInfo::GUICOL_Tool);
 	m_Save->tooltip("Save a design file");		 
	m_Save->callback((Fl_Callback*)cb_Save);
	m_MainButtons->add(m_Save);
	n++;
		
	m_New = new Fl_Button(xoff, n*gap+yoff, but, but, "");
	m_New->box(FL_NO_BOX);
	tPix = new Fl_Pixmap(new_xpm);
	m_New->image(tPix->copy(tPix->w(),tPix->h()));	
	delete tPix;	
	m_New->selection_color(SpiralSynthModularInfo::GUICOL_Tool);
  	m_New->tooltip("New design");		 
	m_New->callback((Fl_Callback*)cb_New);
	m_MainButtons->add(m_New);
	n++;
				
	m_Options = new Fl_Button(xoff, n*gap+yoff, but, but, "");
	m_Options->box(FL_NO_BOX);
	tPix = new Fl_Pixmap(options_xpm);
	m_Options->image(tPix->copy(tPix->w(),tPix->h()));
	delete tPix;		
	m_Options->selection_color(SpiralSynthModularInfo::GUICOL_Tool);
 	m_Options->tooltip("Options");			 
	m_Options->callback((Fl_Callback*)cb_Rload);
	m_MainButtons->add(m_Options);
	n++;
	
	/*m_OpenEditor = new Fl_Button(5+xoff, n*gap+yoff, but, but, "");
	m_OpenEditor->box(FL_NO_BOX);
	tPix = new Fl_Pixmap(edit_xpm);
	m_OpenEditor->image(tPix->copy(tPix->w(),tPix->h()));	
	delete tPix;	
	m_OpenEditor->selection_color(SpiralSynthModularInfo::GUICOL_Tool);	
	m_OpenEditor->tooltip("Open/Close Editor");		 
	m_OpenEditor->callback((Fl_Callback*)cb_OpenEditor);
	m_MainButtons->add(m_OpenEditor);
	n++;*/
			
	/////////////////

	m_EditorWindow = new Fl_Tile(0,MAIN_HEIGHT,MAIN_WIDTH, MAIN_HEIGHT, "");
	m_EditorWindow->color(SpiralSynthModularInfo::GUICOL_Tool);
	m_TopTile->add(m_EditorWindow);
	
	int edy = MAIN_HEIGHT;
	Fl_Group *Left = new Fl_Group(0,MAIN_HEIGHT,TOOLBOX_WIDTH,MAIN_HEIGHT);
	m_EditorWindow->add(Left); 
	m_EditorWindow->resizable(Left);
	
	m_ToolBox = new Fl_Scroll(0,0+edy,TOOLBOX_WIDTH, TOOLBOX_HEIGHT, "");
    m_ToolBox->type(Fl_Scroll::VERTICAL_ALWAYS);
	m_ToolBox->box(FL_FLAT_BOX);
    m_ToolBox->labeltype(FL_ENGRAVED_LABEL);
    m_ToolBox->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	m_ToolBox->scrollbar.align(FL_ALIGN_LEFT);
	m_ToolBox->color(SpiralSynthModularInfo::GUICOL_Tool);
	m_ToolBox->user_data((void*)(this));
	Left->add(m_ToolBox);  
//	m_EditorWindow->resizable(m_ToolBox);
		
	m_ToolPack = new Fl_Pack(SLIDER_WIDTH+5,5+edy,TOOLBOX_WIDTH-10, TOOLBOX_HEIGHT-40,"");
    m_ToolPack->type(FL_VERTICAL);
	m_ToolPack->box(FL_NO_BOX);
	m_ToolPack->color(SpiralSynthModularInfo::GUICOL_Tool);
    m_ToolPack->user_data((void*)(this));
	m_ToolBox->add(m_ToolPack);  
	//m_EditorWindow->resizable(m_ToolBox); 
	
	xoff=0; yoff=MAIN_HEIGHT+TOOLBOX_HEIGHT;
	m_Buttons = new Fl_Group(xoff, yoff, TOOLBOX_WIDTH, MAIN_HEIGHT*2-TOOLBOX_HEIGHT, "");
    m_Buttons->type(1);
	m_Buttons->color(SpiralSynthModularInfo::GUICOL_Tool);
	m_Buttons->box(FL_FLAT_BOX);
	m_Buttons->user_data((void*)(this));
	Left->add(m_Buttons); 

	m_CanvasScroll = new Fl_Scroll(TOOLBOX_WIDTH, 0+edy, MAIN_WIDTH-TOOLBOX_WIDTH, MAIN_HEIGHT, "");	
	m_EditorWindow->add(m_CanvasScroll);
	m_EditorWindow->resizable(m_CanvasScroll);
	
	m_Canvas = new Fl_Canvas(-5000, -5000, 10000, 10000, "Canvas");
    m_Canvas->type(1);
	m_Canvas->box(FL_FLAT_BOX);
    m_Canvas->labeltype(FL_ENGRAVED_LABEL);
    m_Canvas->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	m_Canvas->color(SpiralSynthModularInfo::GUICOL_Canvas);
	m_Canvas->user_data((void*)(this));
	m_Canvas->SetConnectionCallback((Fl_Callback*)cb_Connection);
	m_Canvas->SetUnconnectCallback((Fl_Callback*)cb_Unconnect);
	m_Canvas->SetAddDeviceCallback((Fl_Callback*)cb_NewDeviceFromMenu);
	m_CanvasScroll->add(m_Canvas);   

	m_NewComment = new Fl_Button(TOOLBOX_WIDTH/2-16, MAIN_HEIGHT*2-25, 32, 20, "");
	m_NewComment->box(FL_NO_BOX);
	m_Canvas->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	tPix = new Fl_Pixmap(comment_xpm);
	m_NewComment->image(tPix->copy(tPix->w(),tPix->h()));	
	delete tPix;	
	m_NewComment->color(SpiralSynthModularInfo::GUICOL_Button);
	m_NewComment->selection_color(SpiralSynthModularInfo::GUICOL_Tool);
	m_NewComment->type(0);
	m_NewComment->shortcut(FL_F + 10);		
	m_NewComment->tooltip("New comment in editor");		 
	m_NewComment->user_data((void*)(this));
	m_NewComment->callback((Fl_Callback*)cb_NewComment);
	m_Buttons->add(m_NewComment);

	m_SettingsWindow = new SettingsWindow;
	m_SettingsWindow->RegisterApp(this);
	
	return m_TopWindow;
}

//////////////////////////////////////////////////////////

void SynthModular::LoadPlugins()
{

	int Width  = 40;
	int Height = 40;
	
	int SWidth  = 392;
	int SHeight = 187;
		
	Fl_Pixmap pic(SSM_xpm);
	
	Fl_Double_Window* Splash = new Fl_Double_Window((Fl::w()/2)-(SWidth/2),
									                (Fl::h()/2)-(SHeight/2),
									                SWidth,SHeight,"SSM");
	Splash->border(0);
	
	Fl_Box* pbut = new Fl_Box(0,8,SWidth,SHeight,"");
	pbut->box(FL_NO_BOX);
	pic.label(pbut);
	Fl_Box *splashtext = new Fl_Box(5,SHeight-20,200,20,"Loading...");
	splashtext->labelsize(10);
	splashtext->box(FL_NO_BOX);
	splashtext->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
	Splash->add(pbut);
	Splash->add(splashtext);
	Splash->show();

	int ID=-1;
	int Icon=0;
	Fl_Pack *IconPack;

	IconPack = new Fl_Pack(0,0,TOOLBOX_WIDTH-SLIDER_WIDTH,Height,"");
    IconPack->type(FL_HORIZONTAL);
	IconPack->color(SpiralSynthModularInfo::GUICOL_Tool);
	IconPack->user_data((void*)(this));  
	m_ToolPack->add(IconPack);

	for (vector<string>::iterator i=SpiralSynthModularInfo::PLUGINVEC.begin();
		 i!=SpiralSynthModularInfo::PLUGINVEC.end(); i++)
	{
		string Fullpath=SpiralSynthModularInfo::PLUGIN_PATH+*i;

		ID=PluginManager::Get()->LoadPlugin(Fullpath.c_str());
		if (ID!=PluginError)
		{	
			if (Icon>=ICON_DEPTH)
			{
				Icon=0;
				IconPack = new Fl_Pack(0,0,TOOLBOX_WIDTH-SLIDER_WIDTH,Height,"");
	   	 		IconPack->type(FL_HORIZONTAL);
				IconPack->color(SpiralSynthModularInfo::GUICOL_Tool);
				IconPack->user_data((void*)(this));
				m_ToolPack->add(IconPack); 
			}
		
			Fl_Button *NewButton = new Fl_Button(0,0,Width,Height,"");
			NewButton->labelsize(10);
			
			Fl_Pixmap *tPix = new Fl_Pixmap(PluginManager::Get()->GetPlugin(ID)->GetIcon());
			NewButton->image(tPix->copy(tPix->w(),tPix->h()));		
			delete tPix;	
			
			IconPack->add(NewButton);
			NewButton->type(0);	
			NewButton->box(FL_PLASTIC_UP_BOX);				
			NewButton->align(FL_ALIGN_INSIDE|FL_ALIGN_TOP);
			NewButton->color(SpiralSynthModularInfo::GUICOL_Button);
			NewButton->selection_color(SpiralSynthModularInfo::GUICOL_Tool);
			
			string tooltip=*i;
            // find the first / if there is one, and get rid of everything before and including it
			unsigned int p = tooltip.find ('/');
			if (p < tooltip.length()) tooltip.erase (0, p);
			// find last . and get rid of everything after and including it
			p = tooltip.rfind ('.');
			unsigned int l = tooltip.length ();
			if (p < l) tooltip.erase (p, l);
			m_Canvas->AddPluginName (tooltip, PluginManager::Get()->GetPlugin(ID)->ID);
			splashtext->label (tooltip.c_str());
               		Splash->redraw();
			NewButton->tooltip (tooltip.c_str());
			
			NewButton->callback((Fl_Callback*)cb_NewDevice,&Numbers[ID]);			
			NewButton->show();
			m_DeviceVec.push_back(NewButton);			
			Icon++;
		
			m_ToolBox->redraw();
		
			m_NextPluginButton++;
			Fl::check();
		}
	}
	
	Splash->hide();
	delete Splash;
}

//////////////////////////////////////////////////////////

DeviceGUIInfo SynthModular::BuildDeviceGUIInfo(PluginInfo &PInfo)
{
	DeviceGUIInfo Info;
	int Height=50;
	
	// tweak the size if we have too many ins/outs
	if (PInfo.NumInputs>4 || PInfo.NumOutputs>4)
	{
		if (PInfo.NumInputs<PInfo.NumOutputs)
		{
			Height=PInfo.NumOutputs*10+5;
		}
		else
		{
			Height=PInfo.NumInputs*10+5;
		}
	}
	
	// Make the guiinfo struct
	Info.XPos       = 0;
	Info.YPos       = 0;
	Info.Width      = 40;
	Info.Height     = Height;
	Info.NumInputs  = PInfo.NumInputs;
	Info.NumOutputs = PInfo.NumOutputs;
	Info.Name       = PInfo.Name;
	Info.PortTips   = PInfo.PortTips;
	Info.PortTypes  = PInfo.PortTypes;
	
	return Info;
}

//////////////////////////////////////////////////////////

DeviceWin* SynthModular::NewDeviceWin(int n, int x, int y)
{
	DeviceWin *nlw = new DeviceWin;
	const HostsideInfo* Plugin=PluginManager::Get()->GetPlugin(n);
		
	if (!Plugin) return NULL;
	
	nlw->m_Device=Plugin->CreateInstance();

	if (!nlw->m_Device)
	{
		return NULL;
	}
	nlw->m_Device->SetUpdateCallback(cb_Update);
	nlw->m_Device->SetParent((void*)this);

	PluginInfo PInfo    = nlw->m_Device->Initialise(&m_Info);		
	SpiralGUIType *temp = nlw->m_Device->CreateGUI();
	Fl_Pixmap *Pix      = new Fl_Pixmap(Plugin->GetIcon());
	nlw->m_PluginID     = n;

	if (temp) 
	{	
		temp->position(200,50);
		m_AppGroup->add(temp);		
		m_MainWindow->redraw();
	}
	DeviceGUIInfo Info=BuildDeviceGUIInfo(PInfo);
	Info.XPos       = x; //TOOLBOX_WIDTH+(rand()%400);
	Info.YPos       = y; //rand()%400;
	
	nlw->m_DeviceGUI = new Fl_DeviceGUI(Info, temp, Pix);
	m_Canvas->add(nlw->m_DeviceGUI);
	m_Canvas->redraw();

	return nlw;	
}

//////////////////////////////////////////////////////////

void SynthModular::AddDevice(int n, int x=TOOLBOX_WIDTH+50, int y=400)
{
	//cerr<<"Adding "<<m_NextID<<endl;
	DeviceWin* temp = NewDeviceWin(n,x,y);
	if (temp)
	{
		int ID=m_NextID++;
		//cerr<<"adding device "<<ID<<endl;
		temp->m_DeviceGUI->SetID(ID);
		temp->m_Device->SetUpdateInfoCallback(ID,cb_UpdatePluginInfo);
		m_DeviceWinMap[ID]=temp;
	}
}

//////////////////////////////////////////////////////////

DeviceWin* SynthModular::NewComment(int n, int x=TOOLBOX_WIDTH+50, int y=400)
{	
	DeviceWin *nlw = new DeviceWin;
	
	nlw->m_Device=NULL;
	nlw->m_PluginID  = COMMENT_ID;
	
	DeviceGUIInfo Info;	
	
	Info.XPos       = x; 
	Info.YPos       = y; 
	Info.Width      = 50;
	Info.Height     = 20;
	Info.NumInputs  = 0;
	Info.NumOutputs = 0;
	Info.Name = "";
	
	nlw->m_DeviceGUI = new Fl_CommentGUI(Info, NULL, NULL);
			
	m_Canvas->add(nlw->m_DeviceGUI);
	m_Canvas->redraw();
	
	return nlw;
}

//////////////////////////////////////////////////////////

void SynthModular::AddComment(int n)
{
	//cerr<<"Adding "<<m_NextID<<endl;
	DeviceWin* temp = NewComment(n);
	if (temp)
	{
		int ID=m_NextID++;
		//cerr<<"adding comment "<<ID<<endl;
		temp->m_DeviceGUI->SetID(ID);
		m_DeviceWinMap[ID]=temp;
	}
}

//////////////////////////////////////////////////////////

void SynthModular::UpdateHostInfo()
{   
	std::stringstream str;		
	str<<*this;
	
	ClearUp();
	
	// update the settings
	m_Info.BUFSIZE    = SpiralInfo::BUFSIZE;
	m_Info.FRAGSIZE   = SpiralInfo::FRAGSIZE;
	m_Info.FRAGCOUNT  = SpiralInfo::FRAGCOUNT;	
	m_Info.SAMPLERATE = SpiralInfo::SAMPLERATE;
	m_Info.OUTPUTFILE = SpiralInfo::OUTPUTFILE;
	m_Info.MIDIFILE   = SpiralInfo::MIDIFILE; 
	m_Info.POLY       = SpiralInfo::POLY;
		
	str>>*this;
}

//////////////////////////////////////////////////////////

void SynthModular::cb_Update(void* o, bool mode)
{
	m_CallbackUpdateMode=mode;
	((SynthModular*)o)->Update();
}

//////////////////////////////////////////////////////////

istream &operator>>(istream &s, SynthModular &o)
{	
	string dummy;
	int ver;
	s>>dummy>>dummy>>dummy>>ver;
	
	if (ver>FILE_VERSION)
	{
		SpiralInfo::Alert("Bad file, or more recent version.");
		return s;
	}

	if (ver>2)
	{
		int MainWinX,MainWinY,MainWinW,MainWinH;
		int EditWinX,EditWinY,EditWinW,EditWinH;
	
		s>>MainWinX>>MainWinY>>MainWinW>>MainWinH;
		s>>EditWinX>>EditWinY>>EditWinW>>EditWinH;
	
		//o.m_MainWindow->resize(MainWinX,MainWinY,MainWinW,MainWinH);
		//o.m_EditorWindow->resize(EditWinX,EditWinY,EditWinW,EditWinH);	
	}
	
	int Num, ID, PluginID, x,y,ps,px,py;
	s>>dummy>>Num;
	
	for(int n=0; n<Num; n++)
	{	
		//cerr<<"Loading Device "<<n<<endl;
		
		s>>dummy;    //cerr<<dummy<<" ";
		s>>ID;       //cerr<<ID<<" ";
		s>>dummy;    //cerr<<dummy<<" ";
		s>>PluginID; //cerr<<PluginID<<endl;
		s>>x>>y;
		if (ver>1) s>>ps>>px>>py;
		
		// Check we're not duplicating an ID
		if (o.m_DeviceWinMap.find(ID)!=o.m_DeviceWinMap.end())
		{
			SpiralInfo::Alert("Duplicate device ID found in file - aborting load");
			return s;
		}
				
		if (PluginID==COMMENT_ID)
		{		
			DeviceWin* temp = o.NewComment(PluginID, x, y);
			if (temp)
			{
				temp->m_DeviceGUI->SetID(ID);
				o.m_DeviceWinMap[ID]=temp;
				((Fl_CommentGUI*)(o.m_DeviceWinMap[ID]->m_DeviceGUI))->StreamIn(s); // load the plugin	
				if (o.m_NextID<=ID) o.m_NextID=ID+1;
			}
		}
		else
		{
			DeviceWin* temp = o.NewDeviceWin(PluginID, x, y);
			if (temp)
			{
				temp->m_DeviceGUI->SetID(ID);
				temp->m_Device->SetUpdateInfoCallback(ID,o.cb_UpdatePluginInfo);					
				o.m_DeviceWinMap[ID]=temp;
				o.m_DeviceWinMap[ID]->m_Device->StreamIn(s); // load the plugin
				if (ver>1 && o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow()) 
				{
					// position the plugin window in the main window
					o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow()->position(px,py);
						
					if (ps) o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow()->show();
					else o.m_DeviceWinMap[ID]->m_DeviceGUI->GetPluginWindow()->hide();
					
					// load external files
					o.m_DeviceWinMap[ID]->m_Device->LoadExternalFiles(o.m_FilePath+"_files/");
				}

				if (o.m_NextID<=ID) o.m_NextID=ID+1;
			}
			else 
			{
				// can't really recover if the plugin ID doesn't match a plugin, as
			    // we have no idea how much data in the stream belongs to this plugin
				SpiralInfo::Alert("Error in stream, can't really recover data from here on.");
				return s;
			}
		}	
	}

	s>>*o.m_Canvas;

	return s;
}

//////////////////////////////////////////////////////////

ostream &operator<<(ostream &s, SynthModular &o)
{
	s<<"SpiralSynthModular File Ver "<<FILE_VERSION<<endl;
		
	// make external files dir
	bool ExternalDirUsed=false;
	string command("mkdir "+o.m_FilePath+"_files");
	system(command.c_str());
	
	if (FILE_VERSION>2)
	{
		s<<o.m_MainWindow->x()<<" "<<o.m_MainWindow->y()<<" ";
		s<<o.m_MainWindow->w()<<" "<<o.m_MainWindow->h()<<" ";
		s<<o.m_EditorWindow->x()<<" "<<o.m_EditorWindow->y()<<" ";
		s<<o.m_EditorWindow->w()<<" "<<o.m_EditorWindow->h()<<endl;
	}
	
	// save out the SynthModular
	s<<"SectionList"<<endl;
	s<<o.m_DeviceWinMap.size()<<endl;
	
	for(map<int,DeviceWin*>::iterator i=o.m_DeviceWinMap.begin();
		i!=o.m_DeviceWinMap.end(); i++)
	{		
		s<<endl;
		s<<"Device ";
		s<<i->first<<" "; // save the id
		s<<"Plugin ";
		s<<i->second->m_PluginID<<endl;
		s<<i->second->m_DeviceGUI->x()<<" ";
		s<<i->second->m_DeviceGUI->y()<<" ";
		
		if (i->second->m_DeviceGUI->GetPluginWindow())
		{
			s<<i->second->m_DeviceGUI->GetPluginWindow()->visible()<<" ";
			s<<i->second->m_DeviceGUI->GetPluginWindow()->x()<<" ";
			s<<i->second->m_DeviceGUI->GetPluginWindow()->y()<<" ";
		}
		else
		{
			s<<0<<" "<<0<<" "<<0;
		}
		
		s<<endl;
		
		if (i->second->m_PluginID==COMMENT_ID)
		{
			// save the comment gui
			((Fl_CommentGUI*)(i->second->m_DeviceGUI))->StreamOut(s);
		}
		else
		{		
			// save the plugin
			i->second->m_Device->StreamOut(s); 
		}
		s<<endl;
		
		// save external files
		if (i->second->m_Device && i->second->m_Device->SaveExternalFiles(o.m_FilePath+"_files/"))
		{
			ExternalDirUsed=true;
		}
	}
	
	s<<endl<<*o.m_Canvas<<endl;
	
	// remove it if it wasn't used
	if (!ExternalDirUsed)
	{
		// i guess rmdir won't work if there is something in the dir
		// anyway, but best to be on the safe side. (could do rm -rf) :)
		string command("rmdir "+o.m_FilePath+"_files");
		system(command.c_str());
	}
	
	return s;
}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Close_i(Fl_Window* o, void* v)
{
	m_SettingsWindow->hide();
	delete m_SettingsWindow;
	m_EditorWindow->hide();
	delete m_EditorWindow;
	o->hide();
}

void SynthModular::cb_Close(Fl_Window* o, void* v)
{((SynthModular*)(o->user_data()))->cb_Close_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Load_i(Fl_Button* o, void* v)
{
	if (m_DeviceWinMap.size()>0 && !fl_ask("Load - Loose changes to current design?"))
	{
		return;
	}	
	
	char *fn=fl_file_chooser("Load a design", "*.ssm", NULL);
	
	if (fn && fn!='\0')
	{
		ifstream inf(fn);
		
		if (inf)
		{	
			m_FilePath=fn;	
			
			ClearUp();
			inf>>*this;
			
			TITLEBAR=LABEL+" "+fn;
			m_TopWindow->label(TITLEBAR.c_str());
		}
	}
}

void SynthModular::cb_Load(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_Load_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Save_i(Fl_Button* o, void* v)
{
	char *fn=fl_file_chooser("Save a design", "*.ssm", NULL);
	
	if (fn && fn!='\0')
	{	
		ifstream ifl(fn);
		if (ifl)
		{
			if (!fl_ask("File [%s] exists, overwrite?",fn))
			{
				return;
			}			
		}
	
		ofstream of(fn);
		
		if (of)
		{	
			m_FilePath=fn;
		
			of<<*this;
						
			TITLEBAR=LABEL+" "+fn;
			m_TopWindow->label(TITLEBAR.c_str());
		}
	}
}
void SynthModular::cb_Save(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_Save_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_New_i(Fl_Button* o, void* v)
{
	if (m_DeviceWinMap.size()>0 && !fl_ask("New - Loose changes to current design?"))
	{
		return;
	}	

	m_TopWindow->label(TITLEBAR.c_str());

	ClearUp();
}
void SynthModular::cb_New(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_New_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_NewDevice_i(Fl_Button* o, void* v)
{
	AddDevice(*((int*)v));
}
void SynthModular::cb_NewDevice(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_NewDevice_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_NewDeviceFromMenu_i(Fl_Canvas* o, void* v)
{
	AddDevice(*((int*)v),*((int*)v+1),*((int*)v+2));
}
void SynthModular::cb_NewDeviceFromMenu(Fl_Canvas* o, void* v)
{((SynthModular*)(o->user_data()))->cb_NewDeviceFromMenu_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_NewComment_i(Fl_Button* o, void* v)
{
	AddComment(-1);
}
void SynthModular::cb_NewComment(Fl_Button* o, void* v)
{((SynthModular*)(o->user_data()))->cb_NewComment_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_OpenEditor_i(Fl_Button* o, void* v)
{
	//if (m_EditorWindow->shown()) m_EditorWindow->hide();
	//else m_EditorWindow->show();
}
void SynthModular::cb_OpenEditor(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_OpenEditor_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Rload_i(Fl_Button* o, void* v)
{
	m_SettingsWindow->show();
	
	/*PluginManager::Get()->UnloadAll();

	m_ToolBox->remove(m_ToolPack);
	delete m_ToolPack;

	m_ToolPack = new Fl_Pack(5,20,TOOLBOX_WIDTH-10, TOOLBOX_HEIGHT-40,"");
    m_ToolPack->type(FL_VERTICAL);
	m_ToolPack->box(FL_NO_BOX);
	m_ToolPack->color(SpiralSynthModularInfo::GUICOL_Tool);
    m_ToolPack->user_data((void*)(this));
	m_ToolBox->add(m_ToolPack); 
	m_ToolBox->redraw();
	
	LoadPlugins();*/
}
void SynthModular::cb_Rload(Fl_Button* o, void* v)
{((SynthModular*)(o->parent()->user_data()))->cb_Rload_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Connection_i(Fl_Canvas* o, void* v)
{
	CanvasWire *Wire;
	Wire=(CanvasWire*)v;
			
	map<int,DeviceWin*>::iterator si=m_DeviceWinMap.find(Wire->OutputID);
	if (si==m_DeviceWinMap.end())
	{
		char num[32]; 
		sprintf(num,"%d",Wire->OutputID);
		SpiralInfo::Alert("Warning: Connection problem - can't find source "+string(num));
		return;
	}
	
	map<int,DeviceWin*>::iterator di=m_DeviceWinMap.find(Wire->InputID);
	if (di==m_DeviceWinMap.end())
	{
		char num[32]; 
		sprintf(num,"%d",Wire->InputID);
		SpiralInfo::Alert("Warning: Connection problem - can't find destination "+string(num));
		return;
	}

	Sample *sample=NULL;
	
	if (!si->second->m_Device->GetOutput(Wire->OutputPort,&sample))
	{ 
		char num[32]; 
		sprintf(num,"%d,%d",Wire->OutputID,Wire->OutputPort);
		SpiralInfo::Alert("Warning: Connection problem - can't find source output "+string(num));
		return; 
	}
	
	if (!di->second->m_Device->SetInput(Wire->InputPort,(const Sample*)sample))
	{ 
		char num[32]; 
		sprintf(num,"%d,%d",Wire->InputID,Wire->InputPort);
		SpiralInfo::Alert("Warning: Connection problem - can't find source input "+string(num));
		return;	
	}
	
}
void SynthModular::cb_Connection(Fl_Canvas* o, void* v)
{((SynthModular*)(o->user_data()))->cb_Connection_i(o,v);}

//////////////////////////////////////////////////////////

inline void SynthModular::cb_Unconnect_i(Fl_Canvas* o, void* v)
{
	CanvasWire *Wire;
	Wire=(CanvasWire*)v;
	
	//cerr<<Wire->InputID<<" "<<Wire->InputPort<<endl;
	
	map<int,DeviceWin*>::iterator di=m_DeviceWinMap.find(Wire->InputID);
	if (di==m_DeviceWinMap.end())
	{
		//cerr<<"Can't find destination device "<<Wire->InputID<<endl;
		return;
	}

	if (!di->second->m_Device->SetInput(Wire->InputPort,NULL))
	{ cerr<<"Can't find destination device's Input"<<endl; return;	}
	
}
void SynthModular::cb_Unconnect(Fl_Canvas* o, void* v)
{((SynthModular*)(o->user_data()))->cb_Unconnect_i(o,v);}

//////////////////////////////////////////////////////////

void SynthModular::cb_UpdatePluginInfo(int ID, void *PInfo)
{
	map<int,DeviceWin*>::iterator i=m_DeviceWinMap.find(ID);
	if (i!=m_DeviceWinMap.end())
	{
		DeviceGUIInfo Info=BuildDeviceGUIInfo(*((PluginInfo*)PInfo));
	
		(*i).second->m_DeviceGUI->Setup(Info);
		(*i).second->m_DeviceGUI->redraw();
	}
}

//////////////////////////////////////////////////////////

void SynthModular::LoadPatch(const char *fn)
{
    ifstream inf(fn);

    if (inf) 
	{	
	    m_FilePath=fn;	

	    ClearUp();
	    inf>>*this;

	    TITLEBAR=LABEL+" "+fn;
	    m_TopWindow->label(TITLEBAR.c_str());
    }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

int main(int argc, char **argv)
{	
	srand(time(NULL));
	SpiralSynthModularInfo::Get()->LoadPrefs();
	
	// get args
    string cmd_filename="";
    bool cmd_specd = false;
    if (argc>1) 
	{
        cmd_filename = argv[1];
        cmd_specd = true;
    }	
	
	Fl::visual(FL_DOUBLE|FL_RGB);
	
	SynthModular *synth=new SynthModular;
	
	Fl_Window* win = synth->CreateWindow();
	synth->LoadPlugins();
	win->xclass("");
	win->show(argc, argv); // prevents stuff happening before the plugins have loaded
	
	Fl_Tooltip::size(10);	
	Fl::visible_focus(false);
		
	// do we need to load a patch on startup? 
    if (cmd_specd) synth->LoadPatch(cmd_filename.c_str());        

    for (;;) 
	{
    	if (!Fl::check()) break;
		if (!synth->CallbackMode()) synth->Update();
		synth->UpdatePluginGUIs(); // deletes any if necc
		//else Fl::wait();
  	}
	
	delete synth;	
	return 1;
}

