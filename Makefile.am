AUTOMAKE_OPTIONS = foreign
SUBDIRS = JSON libspiralcore GUI Plugins

INCLUDES =                                              \
                $(FLTK_CXXFLAGS)                        \
                $(FLTK_CFLAGS)                          \
                -I$(top_builddir)/GUI/Widgets           \
                -I$(top_srcdir)/libspiralcore/include

LIBS = 							\
	$(FLTK_LIBS)					\
	-lm -ldl -lpthread				\
	$(top_builddir)/GUI/Widgets/libSpiralWidgets.la \
	$(top_builddir)/GUI/Widgets/libSpiralPlugin.la\
                $(top_builddir)/libspiralcore/libspiralcore.la


EXTRA_DIST = autogen.sh	
SpiralPlugins = $(libdir)/SpiralPlugins

bin_PROGRAMS = SpiralSynthModular
SpiralSynthModular_SOURCES = \
		SpiralSynthPluginLocation.h \
		SpiralInfo.h		\
		SpiralInfo.C		\
		SpiralSynthModular.h \
		SpiralSynthModular.C \
		SettingsWindow.h \
		SettingsWindow.C \
		GraphSort.h \
		GraphSort.C \
		main.cpp

check_PROGRAMS = plugin-registry-test plugin-class-host
TESTS = plugin-registry-test
plugin_registry_test_SOURCES = tests/plugin-registry.cpp GUI/Widgets/PluginClass.cpp
plugin_registry_test_CPPFLAGS = -I$(top_srcdir)
plugin_registry_test_LDADD =
plugin_class_host_SOURCES = tests/plugin-class-host.cpp SpiralInfo.C GraphSort.C
plugin_class_host_CPPFLAGS = $(INCLUDES) -I$(top_srcdir)
plugin_class_host_LDADD = $(LIBS)

EXTRA_DIST += tests/run-plugin-loading.py tests/rejected-plugin.cpp docs/plugin-registry.md
