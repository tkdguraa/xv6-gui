unsigned short SCREEN_WIDTH;
unsigned short SCREEN_HEIGHT;
unsigned short* SCREEN_PHYSADDR;
unsigned short* VESA_ADDR;

// Get the VESA mode information
#ifndef VBE_H
#define VBE_H

void vesamodeinit();
void test();
#endif 