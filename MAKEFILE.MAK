build:
	ccpsx -O3 -Xo$80010000 ./SRC/*.c -oMAIN.CPE,MAIN.SYM,MEM.MAP
	cpe2x /ce MAIN.CPE
clean:
	del MAIN.CPE
	del MAIN.SYM
	del MAIN.EXE
	del MEM.MAP
