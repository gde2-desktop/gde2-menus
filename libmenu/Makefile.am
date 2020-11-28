lib_LTLIBRARIES = libgde2-menu.la

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	-DGDE2MENU_I_KNOW_THIS_IS_UNSTABLE	\
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

libgde2_menu_includedir = $(includedir)/gde2-menus
libgde2_menu_include_HEADERS = \
	gde2menu-tree.h

libgde2_menu_sources = \
	canonicalize.c \
	desktop-entries.c \
	entry-directories.c \
	gde2menu-tree.c \
	menu-layout.c \
	menu-monitor.c \
	menu-util.c

libgde2_menu_la_SOURCES = \
	$(libgde2_menu_sources) \
	canonicalize.h \
	desktop-entries.h \
	entry-directories.h \
	gde2menu-tree.h \
	menu-layout.h \
	menu-monitor.h \
	menu-util.h

libgde2_menu_la_LIBADD = \
	$(GLIB_LIBS)

libgde2_menu_la_LDFLAGS = \
	-version-info $(LIB_MENU_LT_VERSION) \
	-no-undefined \
	-export-symbols-regex gde2menu_tree

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgde2-menu.pc

EXTRA_DIST = \
	libgde2-menu.pc.in \
	libgde2-menu-uninstalled.pc.in

CLEANFILES =

# Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
# Note: we only include the headers here so far because there's no gtk-doc at all anyway
introspection_sources = $(libgde2_menu_include_HEADERS)

Gde2Menu-2.0.gir: libgde2-menu.la
Gde2Menu_2_0_gir_INCLUDES = GObject-2.0
Gde2Menu_2_0_gir_CFLAGS = $(AM_CPPFLAGS)
Gde2Menu_2_0_gir_LIBS = libgde2-menu.la
Gde2Menu_2_0_gir_SCANNERFLAGS = --pkg-export=libgde2-menu
Gde2Menu_2_0_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
INTROSPECTION_GIRS += Gde2Menu-2.0.gir

girdir = $(INTROSPECTION_GIRDIR)
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(INTROSPECTION_TYPELIBDIR)
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
