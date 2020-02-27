### DDA-v2

A simple program that displays the current disk access (BIOS, MS-DOS, or FastDisk) currently in use.  It relies on the same method that the Control Panel '386 Enhanced' applet uses.  Compile with Open Watcom.

#### Methodology

    1.  Begins as a normal Windows 3.1 app, with all the necessary boilerplate. 
    2.  On WM_CREATE, we call Int2F (AX=1684h, BX=0021h) to get the protected-mode entry point for the PageFile VxD.
    3.  On return, we use an OpenWatcom macro to convert ES:DI to a far-pointer in memory.
    4.  We then load the registers for PageFile's "Get Swap Info" call (AX=0001, DS:SI=DS:DI=pointer to swap filename buffer) and use a bit on inline assembly to do the far call (FF 15 farptrMemoryLocation);
    5.  We store the returned AX value for later processing.
    6.  On WM_PAINT, we interpret the stored value as follows:
        -1 == Carry flag was set on return from the PageFile call, which indicates some kind of error.
         0 == Paging is disabled (this really means that 32-bit disk access is disabled).
         1 == Paging is using a temporary file through MS-DOS.
         2 == Paging is using a permanent file through the BIOS.
         3 == Paging is using a permanent file through FastDisk (32-bit disk access).
        
        
