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
#include "../Widgets/Fl_LED_Button.H"

#ifndef DISK_WRITER_GUI_H
#define DISK_WRITER_GUI_H

class DiskWriterPluginGUI : public SpiralPluginGUI
{
public:
	DiskWriterPluginGUI(int w, int h, SpiralPlugin *o, ChannelHandler *ch, const HostInfo *Info);

	virtual void UpdateValues(SpiralPlugin *o);

protected:
    const std::string GetHelpText(const std::string &loc);

private:

	Fl_Button		*Open;
	Fl_Button		*Record;

	Fl_LED_Button		*m_16bits;
	Fl_LED_Button		*m_24bits;
	Fl_LED_Button		*m_32bits;

	//// Callbacks ////

	inline void cb_Record_i(Fl_Button* o, void* v);
	static void cb_Record(Fl_Button* o, void* v);
	inline void cb_Open_i(Fl_Button* o, void* v);
	static void cb_Open(Fl_Button* o, void* v);

	inline void cb_16bits_i(Fl_Button* o, void* v);
	static void cb_16bits(Fl_Button* o, void* v);
	inline void cb_24bits_i(Fl_Button* o, void* v);
	static void cb_24bits(Fl_Button* o, void* v);
	inline void cb_32bits_i(Fl_Button* o, void* v);
	static void cb_32bits(Fl_Button* o, void* v);
};

#endif
