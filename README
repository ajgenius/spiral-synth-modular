** SpiralSynthModular **

Last changed Saturday December 11 2002.

SpiralSynthModular is open source software, distributed under the General
Public License (GPL). See the file COPYING.


** Dependancies **

FLTK (www.fltk.org)
If you build FLTK from source use "./configure --enable-shared"

LADSPA plugins  (www.ladspa.org)

Sound output can be provided by either OSS or JACK (and therefore alsa).

See www.pawfal.org or sourceforge.net/projects/spiralmodular for more details.

** Installing **

To build the SpiralSynthModular application, and all the plugins:

./configure
make
make install
spiralsynthmodular

Check out the designs in the Examples directory.
For more examples be sure to check out the noize farm
(http://groups.yahoo.com/group/noize_farm) an open source repository of
ssm patches.

usage:
spiralsynthmodular options [patch.ssm]

options list:
-h : help
-v : print version
--NoGUI : run without GUI (only useful when loading patch from command line
--Realtime : spawn audio thread with FIFO scheduling (run as root)
--PluginPath <PATH> : look for plugins in the specified directory

*** LRDF support for LADSPA plugins ***

http://plugin.org.uk/releases/lrdf/
http://www.redland.opensource.ac.uk/raptor/

If you have the lrdf library installed on your system,
the LADSPAPlugin will automatically use it.

To disable detection of this this, use the configure option:
./configure --disable-liblrdf

Having a tempfs file system mounted on /dev/shm allows a single LADSPA
plugin database to be shared between instances.

To disable detection of this, use the configure option:
./configure --disable-posiz-shm

*** Jack support ***

http://jackit.sourceforge.net

To build the JackPlugin, use the configure option:
./configure --enable-jack

To load the plugin, add it to .SpiralSynthModular's plugin list.

*** Setting up your .SpiralSynthModular file ***

This file contains all the setup variables for the program. Thes values can be
edited with the options window whilst running SSM.
The following variables in the are the ones to play with to get a good playback
on your machine. This is usually tradeoff between quality and responsiveness
(latency). You can build designs and songs at low quality (22050 samplerate etc)
and then record them at higher settings. The wav file will sound fine, even if
the realtime output doesn't.

BufferSize        = 512         - Size of sample buffer to be calculated each cycle.
FragmentSize      = 256         - Subbuffer size sent to the soundcard
FragmentCount     = 8           - Subbuffer count, -1 computes a suitable size,
                                  setting to 4 or 8 gets better latencies.
Samplerate        = 44100       - Sets the samplerate


Disclaimer:
No representations are made about the suitability of this software
for any purpose. This software is provided "as is" without express
or implied warranty.
(Damage to ears, speakers or buildings are not my fault :))

Dave Griffiths - dave@pawfal.org
www.pawfal.org
