all : smw leveledit worldedit


COMMON_OBJS:=build/FileIO.o build/FileList.o build/MapList.o \
           build/SFont.o build/TilesetManager.o build/dirlist.o \
           build/eyecandy.o build/gfx.o build/global.o build/input.o \
           build/map.o build/movingplatform.o build/path.o \
           build/savepng.o build/sfx.o
SMW_OBJS:= build/HashTable.o build/ai.o build/gamemodes.o build/main.o \
           build/menu.o build/modeoptionsmenu.o build/object.o \
           build/objectgame.o build/objecthazard.o build/player.o \
           build/splash.o build/uicontrol.o build/uicustomcontrol.o \
           build/uimenu.o build/world.o
LEVELEDIT_OBJS:=build/leveleditor.o
WORLDEDIT_OBJS:=build/modeoptionsmenu.o build/object.o \
                build/objecthazard.o build/uicontrol.o build/uimenu.o \
                build/world.o build/worldeditor.o

include configuration
#here because of one .c file among a .cpp project (o_O)

build/%.o : _src/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

smw : $(COMMON_OBJS) $(SMW_OBJS)
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@

leveledit : $(COMMON_OBJS) $(LEVELEDIT_OBJS)
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@

worldedit : $(COMMON_OBJS) $(WORLDEDIT_OBJS)
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@

build/SFont.o : _src/SFont.c
	$(CC) $(CFLAGS) -o $@ -c $<

build/SDLMain.o : macosx/SDLMain.m
	$(CC) $(CFLAGS) -o $@ -c $<

Super\ Mario\ War.app : smw
	mkdir -p '$@/Contents/Resources'
	mkdir -p '$@/Contents/MacOS'
	mkdir -p '$@/Contents/Frameworks'
	cp -r /Library/Frameworks/SDL.framework \
		/Library/Frameworks/SDL_image.framework \
		/Library/Frameworks/SDL_net.framework \
		/Library/Frameworks/SDL_mixer.framework \
		'$@/Contents/Frameworks/'
	cp smw '$@/Contents/MacOS/Super Mario War'
	cp macosx/Info.plist '$@/Contents/'
	echo -n 'APPL????' > '$@/Contents/PkgInfo'
	cp -r macosx/smw.icns gfx maps music sfx tours worlds \
		'$@/Contents/Resources/'

appbundle : Super\ Mario\ War.app

install : install-data install-bin install-leveledit install-worldedit

install-data : all
	mkdir -p $(DESTDIR)/usr/share/games/smw/
	cp -ravx gfx $(DESTDIR)/usr/share/games/smw/
	cp -ravx maps $(DESTDIR)/usr/share/games/smw/
	cp -ravx music $(DESTDIR)/usr/share/games/smw/
	cp -ravx sfx $(DESTDIR)/usr/share/games/smw/
	cp -ravx tours $(DESTDIR)/usr/share/games/smw/
	cp -ravx worlds $(DESTDIR)/usr/share/games/smw/
	rm -rf $(DESTDIR)/usr/share/games/smw/*/.svn
	rm -rf $(DESTDIR)/usr/share/games/smw/*/*/.svn
	rm -rf $(DESTDIR)/usr/share/games/smw/*/*/*/.svn
	rm -rf $(DESTDIR)/usr/share/games/smw/*/*/*/*/.svn
	rm -rf $(DESTDIR)/usr/share/games/smw/*/*/*/*/*/.svn
	rm -rf $(DESTDIR)/usr/share/games/smw/*/*/*/*/*/*/.svn
	chmod a+w $(DESTDIR)/usr/share/games/smw/maps -R
	chmod a+w $(DESTDIR)/usr/share/games/smw/worlds -R

install-bin : all
	#assume DESTDIR is the prefix for installing
	mkdir -p $(DESTDIR)/usr/games/
	cp smw $(DESTDIR)/usr/games/

install-leveledit : all
	mkdir -p $(DESTDIR)/usr/games/
	cp leveledit $(DESTDIR)/usr/games/smw-leveledit

install-worldedit : all
	mkdir -p $(DESTDIR)/usr/games/
	cp worldedit $(DESTDIR)/usr/games/smw-worldedit

clean :
	rm -rf build/*
	rm -f smw
	rm -f leveledit
	rm -f worldedit
	rm -f smw.exe
	rm -f leveledit.exe
	rm -f worldedit.exe
	rm -f options.bin
	rm -rf 'Super Mario War.app'

dpkg :
	rm -f ~/src/*.dsc
	rm -f ~/src/*.diff.gz
	rm -f ~/src/*.deb
	rm -f ~/src/*.changes
	cvs-buildpackage -W ~/src -UHEAD -THEAD -rfakeroot
	sudo reprepro -b /webroot/apt include sid $(HOME)/src/*.changes
