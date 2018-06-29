#include "types.h"
#include "x86.h"
#include "memlayout.h"
#include "user.h"
#include "VBE.h"
#include "spinlock.h"
#include "windows.h"

struct spinlock screen_lock;
struct spinlock buf1_lock;
struct spinlock buf2_lock;


void vesamodeinit()
{
    SCREEN_PHYSADDR = KERNBASE + 0x1028;
    VESA_ADDR = *((uint*)SCREEN_PHYSADDR);
    SCREEN_WIDTH = *((ushort*)(KERNBASE + 0x1012));
    SCREEN_HEIGHT = *((ushort*)(KERNBASE + 0x1014));
}

