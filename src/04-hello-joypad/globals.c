#include "globals.h"

static u_long ot[2][OT_LEN]; // Ordering table
static char primbuff[2][PRIMBUFF_LEN]; // Primitive buffer
static char* nextprim; // Next primitive pointer

void EmptyOT(u_short currbuff) {
    ClearOTagR(ot[currbuff], OT_LEN);
}

void SetOTAt(u_short currbuff, u_int i, u_long value) {
    ot[currbuff][i] = value;
}

u_long* GetOTAt(u_short currbuff, u_int i) {
    return &ot[currbuff][i];
}

void IncrementNextPrim(u_int size) {
    nextprim += size;
}

void SetNextPrim(char* value) {
    nextprim = value;
}

void ResetNextPrim(u_short currbuff) {
    nextprim = primbuff[currbuff];
}

char* GetNextPrim(void) {
    return nextprim;
}