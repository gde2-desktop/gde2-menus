AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(PYTHON_CFLAGS) \
	-I$(srcdir)/../libmenu \
	-DGDE2MENU_I_KNOW_THIS_IS_UNSTABLE \
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = \
	$(WARN_CFLAGS) \
	$(NO_STRICT_ALIASING_CFLAGS)

gde2menudir = $(pyexecdir)
gde2menu_LTLIBRARIES = gde2menu.la

gde2menu_la_SOURCES = gde2menu.c
gde2menu_la_LDFLAGS = -module -avoid-version -fPIC -export-symbols-regex initgde2menu
gde2menu_la_LIBADD = $(GLIB_LIBS) $(PYTHON_LIBS) $(top_builddir)/libmenu/libgde2-menu.la

-include $(top_srcdir)/git.mk
