mkdir build
cd build

set LINKER_LIBS=comctl32.lib Gdi32.lib User32.lib Ole32.lib UxTheme.lib

cl /WX /W4 ../main.c /link %LINKER_LIBS% && main.exe

cd ..
