@ECHO OFF

ECHO Compiling and generating executable...
psymake

ECHO Building ISO file...
MKPSXISO -y CDLAYOUT.XML

MKDIR BUILD
MOVE GAME.ISO BUILD\
MOVE GAME.CUE BUILD\
psymake clean

ECHO ISO file built successfully!