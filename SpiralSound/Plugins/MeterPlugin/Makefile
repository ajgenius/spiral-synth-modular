# Generated automatically from Makefile.in by configure.
#############################################################################
# Makefile for building MeterPlugin.so
#############################################################################

####### Compiler, tools and options

CC	=	gcc
CXX	=	g++
CFLAGS	=	-pipe -Wall -O3 -ffast-math -DNO_DEBUG -Wno-unused  -fPIC
CXXFLAGS=	-pipe -Wall -O3 -ffast-math -DNO_DEBUG -Wno-unused  -fPIC -fPIC
INCPATH	=	-I/usr/X11R6/include
LINK	=	g++ -shared
LFLAGS	=
LIBS	=   -L/usr/X11R6/lib -lfltk -lGL -lXext -lX11 -lm -ldl
MOC     = moc
UIC	=

TAR	=	tar -cf
GZIP	=	gzip -9f

INSTALL	= /usr/bin/install -c

###### Autoconf variables

prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man

####### Files

HEADERS =	../SpiralPlugin.h \
		../SpiralPluginGUI.h \
		../Widgets/Fl_DragBar.H \
		../Widgets/Fl_SevenSeg.H \
		../Widgets/Fl_VU_Meter.h \
		../../Sample.h \
		MeterPlugin.h \
		MeterPluginGUI.h
SOURCES =	../SpiralPlugin.C \
		../SpiralPluginGUI.C \
		../Widgets/Fl_DragBar.cxx \
		../Widgets/Fl_SevenSeg.cxx \
		../Widgets/Fl_VU_Meter.cxx \
		../../Sample.C \
		MeterPlugin.C \
		MeterPluginGUI.C
OBJECTS =	../SpiralPlugin.o \
		../SpiralPluginGUI.o \
		../Widgets/Fl_DragBar.o \
		../Widgets/Fl_SevenSeg.o \
		../Widgets/Fl_VU_Meter.o \
		../../Sample.o \
		MeterPlugin.o \
		MeterPluginGUI.o
INTERFACES =
UICDECLS =
UICIMPLS =
SRCMOC	=
OBJMOC	=
DIST	=
TARGET	=	MeterPlugin.so

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

tmake:
	tmake MeterPlugin.pro

dist:
	$(TAR) MeterPlugin.tar MeterPlugin.pro $(SOURCES) $(HEADERS) $(INTERFACES) $(DIST)
	$(GZIP) MeterPlugin.tar

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core

install:
	$(INSTALL) $(TARGET) $(libdir)/SpiralPlugins

####### Sub-libraries


###### Combined headers


####### Compile

../SpiralPlugin.o: ../SpiralPlugin.C \
		../SpiralPlugin.h \
		../../Sample.h

../SpiralPluginGUI.o: ../SpiralPluginGUI.C \
		../SpiralPluginGUI.h \
		../Widgets/Fl_DragBar.H \
		../SpiralPlugin.h \
		../../Sample.h

../Widgets/Fl_DragBar.o: ../Widgets/Fl_DragBar.cxx \
		../Widgets/Fl_DragBar.H

../Widgets/Fl_VU_Meter.o: ../Widgets/Fl_VU_Meter.cxx \
		../Widgets/Fl_VU_Meter.h

../Widgets/Fl_SevenSeg.o: ../Widgets/Fl_SevenSeg.cxx \
		../Widgets/Fl_SevenSeg.H

../../Sample.o: ../../Sample.C \
		../../Sample.h \
		../../SpiralInfo.h

MeterPlugin.o: MeterPlugin.C \
		MeterPlugin.h \
		../SpiralPlugin.h \
		../../Sample.h \
		../../SpiralInfo.h \
		../../Sample.h \
		MeterPluginGUI.h \
		../Widgets/Fl_DragBar.H \
		../Widgets/Fl_VU_Meter.h \
		../Widgets/Fl_SevenSeg.H \
                ../SpiralPluginGUI.h \
		SpiralIcon.xpm

MeterPluginGUI.o: MeterPluginGUI.C \
		MeterPluginGUI.h \
		../Widgets/Fl_DragBar.H \
		../Widgets/Fl_VU_Meter.h \
		../Widgets/Fl_SevenSeg.H \
		MeterPlugin.h \
		../SpiralPlugin.h \
		../../Sample.h \
		../../SpiralInfo.h \
		../../Sample.h \
		../SpiralPluginGUI.h

