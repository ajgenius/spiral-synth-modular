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

#ifndef SPIRALSYNTHMODULARINFO
#define SPIRALSYNTHMODULARINFO

#include "SpiralSound/SpiralInfo.h"
#include <vector>

class SpiralSynthModularInfo : public SpiralInfo
{
public:
	static SpiralSynthModularInfo* Get();
	void SetColours();
	
	static string PLUGIN_PATH;
	static vector<string> PLUGINVEC;
	
	static int GUICOL_Tool;
	static int GUICOL_Button;
	static int GUICOL_Canvas;
	static int GUICOL_Device;
	static string BGIMG;

protected:
	SpiralSynthModularInfo();
	
	virtual string GetResFileName() { return ".SpiralSynthModular"; }
	
	static SpiralSynthModularInfo *m_SpiralSynthModularInfo;
	
	virtual void StreamInPrefs(istream &s);
	virtual void StreamOutPrefs(ostream &s);
};

#endif
