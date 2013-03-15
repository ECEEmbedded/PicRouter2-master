#include "debug.h"
#include <p18cxxx.h>

void DebugPrint(unsigned char out) {
    //Convert any numbers to just 1 or 0
    LATA = out;
}

void sendValuesToPins(char ra1, char ra2, char ra3) {
    if(ra1) {
        RA1 = ra1;
    }
    if(ra2) {
        RA2 = ra2;
    }
    if(ra3) {
        RA3 = ra3;
    }
}
