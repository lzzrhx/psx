@ECHO OFF

ECHO Compiling and generating executable...
psymake

ECHO Building IMG file...
BUILDCD -l -iGAME.IMG CDLAYOUT.CTI

ECHO Converting GAME.IMG to GAME.ISO...
STRIPISO S 2352 GAME.IMG GAME.ISO

ECHO Bundling the license into the CD ISO...
PSXLICENSE /eu /i GAME.ISO

MKDIR BUILD
MOVE GAME.ISO BUILD\

psymake clean
DEL GAME.IMG
DEL CDW900E.TOC
DEL QSHEET.TOC

ECHO ISO file built successfully!