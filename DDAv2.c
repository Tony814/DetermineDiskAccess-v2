#include "DDAv2.h"
#include <i86.h>

static char SwapFileName[128];
static void __far *PageFilePMEntryPoint;

short GetDiskAccess();

extern short CallPageFile(int Function, char* TempFileNameBuffer, char* PermFileNameBuffer);
#pragma aux CallPageFile = \
".386" \
"push ecx" \
"call dword PTR [PageFilePMEntryPoint]" \
"pop ecx" \
"jnc NoError" \
"mov ax, 0xFFFF" \
"NoError:" \
modify [ax si di] \
parm [ax] [ds si] [ds di] \
value [ax];

short GetDiskAccess()
{
    union REGS r;
    struct SREGS s;
    char TextBuffer[255];
    short PageFileStatus;

    //setup registers for Int2F call
    segread(&s);
    r.w.ax = 0x1684;
    r.w.bx = 0x0021;
    s.es = 0x0000;
    r.w.di = 0x0000;
    int86x(0x2F, &r, &r, &s);

    if (r.w.di == 0x0000 && s.es == 0x0000)
    {
        //PageFile not loaded?
        MessageBox(NULL,"INT2F/1684+0021 returned NULL entry point","DDAv2",MB_OK);
        return -1;
    }
    else
    {
        PageFilePMEntryPoint = MK_FP(s.es, r.w.di);
    }

    //Far call the pointer we received with 'GetSwapInfo' in AX
    PageFileStatus = CallPageFile(0x0001, &SwapFileName, &SwapFileName);

    /* will return:
     * -1 = CF set on return from PageFile VxD (error)
     *  0 = PageFile VxD loaded but paging disabled
     *  1 = Paging through MS-DOS
     *  2 = Paging through BIOS
     *  3 = Paging through FastDisk
     */

    if (PageFileStatus == -1)
    {
        MessageBox(NULL,"PageFile GetSwapFileInfo returned CF set","DDAv2",MB_OK);
    }

    return PageFileStatus;
    /*sprintf(TextBuffer, "ReturnValue = %X", PageFileStatus);
    MessageBox(NULL, TextBuffer, "DDAv2", MB_OK);
    return FALSE;
    */
}
