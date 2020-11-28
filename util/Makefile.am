noinst_PROGRAMS = gde2-menu-spec-test

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(srcdir)/../libmenu \
	-DGDE2MENU_I_KNOW_THIS_IS_UNSTABLE \
	-DGDE2LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(DISABLE_DEPRECATED_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

gde2_menu_spec_test_SOURCES = \
	test-menu-spec.c

gde2_menu_spec_test_LDADD = \
	$(GLIB_LIBS) \
	../libmenu/libgde2-menu.la

if HAVE_PYTHON
pyexampledir = $(pkgdatadir)/examples
pyexample_DATA = gde2-menus-ls.py
else
pyexampledir =
pyexample_DATA =
endif

EXTRA_DIST = $(pyexample_DATA)

-include $(top_srcdir)/git.mk

