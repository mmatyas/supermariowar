
del /f /q *.bin
del /f /q *.exp
del /f /q *.lib
del /f /q *.suo
del /f /q Makefile
del /f /q configure
del /f /q DOES_WORK
del /f /q smw_icon.res
del /f /q SuperMarioWar_console.exe

rd /s /q _src
rd /s /q debian
rd /s /q macosx
rd /s /q Projects
rd /s /q Scripts
rd /s /q zzz_test

del /f /q maps\ZZleveleditor.map
rd /s /q maps\test
rd /s /q maps\moved

del /f /q worlds\ZZworldeditor.txt

del /f /q /s /a:H thumbs.db

del /f /q maps\cache\*.png
del /f /q maps\screenshots\*.png

FOR /r . %%f IN (.svn) DO RD /s /q "%%f"