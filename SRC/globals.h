#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <libspu.h>
#include <libcd.h>
#include "inline_n.h"
#include "lzss.h"

#define OT_LEN 8192
#define PRIMBUFF_LEN 131072
#define TRACK_PULL 100
#define TRACK_PUSH 30000

void EmptyOT(u_short currbuff);
void SetOTAt(u_short currbuff, u_int i, u_long value);
u_long* GetOTAt(u_short currbuff, u_int i);
void IncrementNextPrim(u_int size);
void SetNextPrim(char* value);
void ResetNextPrim(u_short currbuff);
char* GetNextPrim(void);

#endif