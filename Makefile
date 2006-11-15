all : smw leveledit


COMMON_OBJS:=build/MapList.o build/SFont.o build/dirlist.o build/gfx.o \
           build/global.o build/input.o build/map.o \
           build/movingplatform.o build/path.o build/savepng.o
SMW_OBJS:= build/HashTable.o build/ai.o build/eyecandy.o \
           build/gamemodes.o build/main.o build/map.o build/menu.o \
           build/object.o build/player.o build/sfx.o build/splash.o \
           build/uicontrol.o build/uimenu.o
LEVELEDIT_OBJS:=build/leveleditor.o

include configuration
#here because of one .c file among a .cpp project (o_O)

build/%.o : _src/%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

smw : $(COMMON_OBJS) $(SMW_OBJS)
	$(CXX) $(CFLAGS) $^ $(LDFLAGS) -o $@

leveledit : $(COMMON_OBJS) $(LEVELEDIT_OBJS)
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
	cp -r macosx/smw.icns gfx maps music sfx tours \
		'$@/Contents/Resources/'

appbundle : Super\ Mario\ War.app

install : install-data install-bin install-leveledit

install-data : all
	mkdir -p $(DESTDIR)/usr/share/smw/
	cp -ravx sfx $(DESTDIR)/usr/share/smw/
	cp -ravx gfx $(DESTDIR)/usr/share/smw/
	cp -ravx music $(DESTDIR)/usr/share/smw/
	cp -ravx maps $(DESTDIR)/usr/share/smw/
	cp -ravx tours $(DESTDIR)/usr/share/smw/
	rm -rf $(DESTDIR)/usr/share/smw/*/CVS
	rm -rf $(DESTDIR)/usr/share/smw/*/*/CVS
	rm -rf $(DESTDIR)/usr/share/smw/*/*/*/CVS
	rm -rf $(DESTDIR)/usr/share/smw/*/*/*/*/CVS
	rm -rf $(DESTDIR)/usr/share/smw/*/.cvsignore
	rm -rf $(DESTDIR)/usr/share/smw/*/*/.cvsignore
	rm -rf $(DESTDIR)/usr/share/smw/*/*/*/.cvsignore
	rm -rf $(DESTDIR)/usr/share/smw/*/*/*/*/.cvsignore
	chmod a+w $(DESTDIR)/usr/share/smw/maps -R

install-bin : all
	#assume DESTDIR is the prefix for installing
	mkdir -p $(DESTDIR)/usr/bin/
	cp smw $(DESTDIR)/usr/bin/

install-leveledit : all
	mkdir -p $(DESTDIR)/usr/bin/
	cp leveledit $(DESTDIR)/usr/bin/smw-leveledit

clean :
	rm -rf build/* 
	rm -f smw
	rm -f leveledit
	rm -f smw.exe
	rm -f leveledit.exe
	rm -f options.bin
	rm -rf 'Super Mario War.app'

dpkg :
	rm -f ~/src/*.dsc
	rm -f ~/src/*.diff.gz
	rm -f ~/src/*.deb
	rm -f ~/src/*.changes
	cvs-buildpackage -W ~/src -UHEAD -THEAD -rfakeroot
	sudo reprepro -b /webroot/apt include sid $(HOME)/src/*.changes
