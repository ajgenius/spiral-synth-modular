** SpiralSynthModular **

Last changed Saturday March 23 2002.

SpiralSynthModular is open source software, distributed under the General
Public License (GPL). See the file COPYING.

It uses the OSS /dev/dsp driver, (although this is configurable) and the GUI 
it uses is FLTK (www.fltk.org).

See www.pawfal.org for more details.
    
** Installing **
	
To build the SpiralSynthModular application, and all the plugins:

./configure 
make
make install

Check out the designs in the Examples directory.

The GUI requires FLTK to be installed (www.fltk.org), Which in turn 
require Mesa or OpenGL.

*** Jack support ***

This release of SSM comes with an experimental JackPlugin. JACK stands for
Jack Audio Connection Kit and aims to connect Linux audio apps together.
For more information on Jack, and other apps supporting it, see:
http://jackit.sourceforge.net

To build the JackPlugin, use the configure option:
./configure --enable-jack

To load the plugin, add it to .SpiralSynthModular's plugin list.

*** Setting up your .SpiralSynthModular file ***

This file contains all the setup variables for the program. Thes values can be 
edited with the options window whilst running SSM, but they will default to the
values in the file.
The following variables in the are the ones to play with to get a good playback 
on your machine. This is usually tradeoff between quality and responsiveness 
(latency). You can build designs and songs at low quality (22050 samplerate etc) 
and then record them at higher settings. The wav file will sound fine, even if 
the realtime output doesn't.

BufferSize        = 512         - Size of sample buffer to be calculated each cycle.
FragmentSize      = 256         - Subbuffer size sent to the soundcard
FragmentCount     = -1          - Subbuffer count, -1 computes a suitable size,
                                  setting to 4 or 8 gets better latencies.
Samplerate        = 44100       - Sets the samplerate


Disclaimer:
No representations are made about the suitability of this software 
for any purpose. This software is provided "as is" without express 
or implied warranty. 
(Damage to ears, speakers or buildings are not my fault :))

Dave Griffiths - dave@pawfal.org
www.pawfal.org
