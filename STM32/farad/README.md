http://www.emcu.it/STM32F0xx/STM32F0-ValueLine/STM32F030_marketing_pres.pdf
p. 9: Cortex-M0 doesn't have a FPU

find file in git commit history:
git log --all -- **/Makefile.*
now you have commit ids; use them in the place of 7787 below
git checkout 7787 -- Makefile

todo:
now uart outputs some garbage, letters are not in the correct order
what about changing to_send: mutex? membar? dma interrupt + timer? polling?


interrupt number values: stm32f091xc.h

make flash, then continue, wait till it got stuck on waiting till
TC !=0, "next" in gdb, then continue again and it should work

address+size wrapped at the beginning;
when continued in gdb, it was like a+s wrapped or landed in 0xffffffff;
it is an issue of RCC_FLASH configure function

error 2:
program was stuck on waiting for TXE, but TXE interrupt handled this event in the
background and TXE was seemingly always set (when it was reset, interrupt fired out
and filled TDR, so TXE was reset etc.)

if gdb throws you to 0xfffffffX and openocd tells that address+size wrapped,
just step through the program init - maybe somewhere the timing is wrong and
something doesnt have enough time to load?

puppy linux:  install libnewlib-arm, otherwise #include_next stdint.h throws an error
TL;DR:
    CLion shouldnt be building CMSIS each time
    use small variables to increase the size of trig-val table
    store fixed-size as Q1.7 on int8_t
    range: [-2, 2) with 0.0078125 = 0.0245 rad step
    .
    error for distance=80cm and angle=0.0245 rad:
    x = sqrt(2*(80cm)^2 - 2*80cm*80cm*cos(angle))
    x = sqrt(2*6400cm^2(1 - cos(0.0245 rad)))
    x = 1.95995cm
    2cm is totally enough for a robot at the second end of the arena.
    .
    error for distance=5cm and angle=0.0245 rad:
    x = sqrt(2*(5cm)^2 - 2*80cm*80cm*cos(angle))
    x = 0.12250cm
    it's also totally enough for us
    .
    we need DMAT to send accurate data about robot's position
    we need DMAR to collect lots of data from sensors
    
About sin:
    Angle values should be in [-pi, pi], never beyond
    We will use Q30 number format, so the range of our variables
    will be in [-2, 2). It would be convenient to
    change the angle range to [-pi, pi) then.
    pre- and post-process Q30 to int transformations will then be
    functions that satisfies:
        pre(int): [-pi, pi) -> [-2, 2)
        post(Q30): [-2, 2) -> [-pi, pi)
    And we gonna have to remember that our Q30 number is
    shifted left by 30 bits.
    The precision we need is for the robot to detect an object from 80cm
    with error around +=1cm. Then sin2x = sqrt(80cm^2+1cm^2)= sq(6401cm2) ~ 80cm
    sin2x=80cm, 2x=asin(80cm)=0.0125 rad = 1/80rad
    So LSB in our fixed-point format has to correspond to max. 1/80=rad
    Lets say that we store variables as Q1.7 on 8bits, so we can store
    256 values. LSB is 2^-7=0.007 and it corresponds to 0.0245 rad
    0.0245 is greater than 0.0125, so we get precision like +-2cm for 80cm distance,
    but thats totally ok and increase in precision isn't worth doubling the memory
    needed for the trig values table.
    
Todo:
    Write prerequisites and steps to build, compile, flash and debug from:
        -Windows
        -Linux
        -Cleanup old makefiles
        -Write what errors can occur, ex. cannot find entry symbol Reset_Handler; defaulting to 08000000
        -Connect arm-gdb to clion
        -Won't OpenOCD work?
    Make a good structure for the project:
        -where to put headers
        -what structure to have cmsis sources, cmsis headers, other stm files, linker, tests, my own libs and main sources?

ERROR:
2018-10-07T22:15:35 WARN C:\Users\Jerry\Desktop\stlink-master\src\usb.c: Couldn't find any ST-Link/V2 devices
SOLUTION:
port is busy

Windows:
    needed: st-link gdb server from ST
    build and compile project, run this server in the background
    run arm-none-eabi gdb, connect to server's port (default: 4242 as on 01.10.2018)
    target extended-remote localhost 4242
    kill
    load build/main.elf
    file build/main.elf
    run
    help: https://github.com/texane/stlink
    Don't use ANSI nor gnu89 standards:
        -ANSI doesn't suppport inline keyword present in CMSIS libs
        -gnu89 does, but doesn't support uint8_t, int8_t types - we want them
    Use C99 instead
    Harmless build warning:
    c:/progra~2/gnutoo~1/72018-~1/bin/../lib/gcc/arm-none-eabi/7.3.1/../../../../arm-none-eabi/bin/ld.exe: warning: cannot find entry symbol Reset_Handler; defaulting to 08000000
