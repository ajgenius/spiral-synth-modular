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
#include <FL/Fl_Button.H>
#include "DiskWriterPlugin.h"
#include "../SpiralPluginGUI.h"

#ifndef DISK_WRITER_GUI_H
#define DISK_WRITER_GUI_H

class DiskWriterPluginGUI : public SpiralPluginGUI
{
public:
	DiskWriterPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);

protected:
    const string GetHelpText(const string &loc);

private:

	Fl_Button		*Open;
	Fl_Button		*Record;

	//// Callbacks ////

	inline void cb_Record_i(Fl_Button* o, void* v);
	static void cb_Record(Fl_Button* o, void* v);
	inline void cb_Open_i(Fl_Button* o, void* v);
	static void cb_Open(Fl_Button* o, void* v);
};

#endif
