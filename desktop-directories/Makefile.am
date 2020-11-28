directorydir = $(datadir)/gde2/desktop-directories

# Should be moved to gde2-control-center:
directory_in_controlcenterfiles = \
	gde2-hardware.directory.in \
	gde2-internet-and-network.directory.in \
	gde2-look-and-feel.directory.in \
	gde2-personal.directory.in \
	gde2-system.directory.in

directory_in_files = \
	gde2-audio-video.directory.in \
	gde2-development.directory.in \
	gde2-education.directory.in \
	gde2-game.directory.in \
	gde2-graphics.directory.in \
	gde2-network.directory.in \
	gde2-office.directory.in \
	gde2-settings.directory.in \
	gde2-settings-system.directory.in \
	gde2-system-tools.directory.in \
	gde2-utility.directory.in \
	gde2-utility-accessibility.directory.in \
	gde2-other.directory.in \
	gde2-menu-applications.directory.in \
	gde2-menu-system.directory.in \
	$(directory_in_controlcenterfiles)

directory_DATA = $(directory_in_files:.directory.in=.directory)

@INTLTOOL_DIRECTORY_RULE@

EXTRA_DIST= $(directory_in_files)

DISTCLEANFILES = $(directory_DATA)

-include $(top_srcdir)/git.mk
