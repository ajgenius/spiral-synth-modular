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

#ifndef SplitSwitchPLUGIN
#define SplitSwitchPLUGIN

class SplitSwitchPlugin : public SpiralPlugin {
  public:
    SplitSwitchPlugin ();
    virtual ~SplitSwitchPlugin ();
    virtual PluginInfo& Initialise (const HostInfo *Host);
    virtual SpiralGUIType* CreateGUI ();
    virtual void Execute ();
    virtual void ExecuteCommands ();
    virtual void StreamOut (ostream &s);
    virtual void StreamIn (istream &s);
    int GetSwitch (void) { return m_SwitchPos + 1; }
    int GetChans (void) { return m_PluginInfo.NumOutputs - 1; }
    enum GUICommands {NONE, SETCHANS, SETSWITCH};
    struct GUIArgs {
      int Chans;
      int Switch;
      int Echo;
    };
  private:
    GUIArgs m_GUIArgs;
    void SetChans (int n);
    void SetSwitch (int n);
    int m_SwitchPos;
    bool m_Triggered;
    friend istream &operator>> (istream &s, SplitSwitchPlugin &o);
    friend ostream &operator<< (ostream &s, SplitSwitchPlugin &o);
};

istream &operator>> (istream &s, SplitSwitchPlugin &o);
ostream &operator<< (ostream &s, SplitSwitchPlugin &o);

#endif
