## Makefile.am for katy

# this has all of the subdirectories that make will recurse into.  if
# there are none, comment this out
SUBDIRS = pics doc

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = katy katy_client

# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# the library search path. 
katy_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
katy_LDADD = $(LIB_KFILE)

# which sources should be compiled for katy
katy_SOURCES = main.cpp katyapp.cpp katy.cpp katyview.cpp \
	katypref.cpp katyiface.skel \
	textdocument.cpp texteditor.cpp 

# these are the headers for your project
noinst_HEADERS = katy.h katyapp.h katyview.h katypref.h \
	textdocument.h texteditor.h

# client stuff
katy_client_LDFLAGS = $(KDE_RPATH) $(all_libraries)
katy_client_LDADD = $(LIB_KDECORE)
katy_client_SOURCES = katy_client.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

# if you "make distclean", this files get removed. If you want to remove
# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(katy_SOURCES) && mv messages.po ../po/katy.pot

KDE_ICON = katy

 # this is where the kdelnk file will go 
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = katy.desktop
