build:
	mkdir build
	ccpsx -O3 -Xo$80010000 *.c -obuild/main.cpe,build/main.sym,build/mem.map
	cpe2x /ce build/main.cpe
