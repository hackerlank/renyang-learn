#############################################################################
# Makefile for building: scribble
# Generated by qmake (1.07a) (Qt 3.3.7) on: Fri Sep 19 10:53:15 2008
# Project:  scribble.pro
# Template: app
# Command: $(QMAKE) -o Makefile scribble.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -I/usr/include/mysql -I/usr/include/freetype2 -I/usr/include/postgresql -fno-exceptions -Wall -W -g -D_REENTRANT  -DQT_TABLET_SUPPORT -DQT_THREAD_SUPPORT -DQT_SHARED
CXXFLAGS = -pipe -I/usr/include/mysql -I/usr/include/freetype2 -I/usr/include/postgresql -fno-exceptions -Wall -W -g -D_REENTRANT  -DQT_TABLET_SUPPORT -DQT_THREAD_SUPPORT -DQT_SHARED
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/share/qt3/mkspecs/linux-g++ -I. -I/usr/include/freetype2 -I/usr/include/qt3 -I/usr/X11R6/include -I.moc/debug-shared-mt/
LINK     = g++
LFLAGS   = -fno-exceptions
LIBS     = $(SUBLIBS) -L/usr/share/qt3/lib -L/usr/X11R6/lib -lqt-mt -ljpeg -lpng -lz -lXi -lXrender -lXrandr -lXcursor -lXinerama -lXft -lfreetype -lfontconfig -lXext -lX11 -lm -lSM -lICE -ldl -lpthread
AR       = ar cqs
RANLIB   = 
MOC      = /usr/share/qt3/bin/moc
UIC      = /usr/share/qt3/bin/uic -L /usr/share/qt3/plugins
QMAKE    = /usr/share/qt3/bin/qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = .obj/debug-shared-mt/

####### Files

HEADERS = scribble.h
SOURCES = main.cpp \
		scribble.cpp
OBJECTS = .obj/debug-shared-mt/main.o \
		.obj/debug-shared-mt/scribble.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = .moc/debug-shared-mt/moc_scribble.cpp
OBJMOC = .obj/debug-shared-mt/moc_scribble.o
DIST	   = scribble.pro
QMAKE_TARGET = scribble
DESTDIR  = 
TARGET   = scribble

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(OBJCOMP) $(LIBS)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd /usr/share/qt3/src/moc && $(MAKE) )

Makefile: scribble.pro ../../.qmake.cache /usr/share/qt3/mkspecs/linux-g++/qmake.conf /usr/share/qt3/lib/libqt-mt.prl
	$(QMAKE) -o Makefile scribble.pro
qmake: 
	@$(QMAKE) -o Makefile scribble.pro

dist: 
	@mkdir -p .obj/debug-shared-mt/scribble && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) .obj/debug-shared-mt/scribble/ && ( cd `dirname .obj/debug-shared-mt/scribble` && $(TAR) scribble.tar scribble && $(GZIP) scribble.tar ) && $(MOVE) `dirname .obj/debug-shared-mt/scribble`/scribble.tar.gz . && $(DEL_FILE) -r .obj/debug-shared-mt/scribble

mocclean:
	-$(DEL_FILE) $(OBJMOC)
	-$(DEL_FILE) $(SRCMOC)

uiclean:

yaccclean:
lexclean:
clean: mocclean
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core $(TARGET)


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile

.obj/debug-shared-mt/main.o: main.cpp scribble.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/debug-shared-mt/main.o main.cpp

.obj/debug-shared-mt/scribble.o: scribble.cpp scribble.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/debug-shared-mt/scribble.o scribble.cpp

.obj/debug-shared-mt/moc_scribble.o: .moc/debug-shared-mt/moc_scribble.cpp  scribble.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/debug-shared-mt/moc_scribble.o .moc/debug-shared-mt/moc_scribble.cpp

.moc/debug-shared-mt/moc_scribble.cpp: $(MOC) scribble.h
	$(MOC) scribble.h -o .moc/debug-shared-mt/moc_scribble.cpp

####### Install

install:  

uninstall:  

