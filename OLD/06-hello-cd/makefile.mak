build:
	ccpsx -O3 -Xo$80010000 *.c -oMAIN.CPE,MAIN.SYM,MEM.MAP
	cpe2x /ce MAIN.CPE
