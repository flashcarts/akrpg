/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#include <nds.h>

#include <nds/bios.h>
#include <nds/arm7/touch.h>
#include <nds/arm7/clock.h>

#include "brightnesscontrol.h"
#include "suspendcontrol.h"
#include "../../share/timetool.h"
#include "../../share/ipctool.h"
#include "../../share/memtool.h"


#define waitMs(t) swiDelay( 8 * 1000 * t )

//---------------------------------------------------------------------------------
void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format)
{
//---------------------------------------------------------------------------------
    SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
    SCHANNEL_SOURCE(channel) = (u32)data;
    SCHANNEL_LENGTH(channel) = bytes >> 2 ;
    SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}


//---------------------------------------------------------------------------------
s32 getFreeSoundChannel()
{
//---------------------------------------------------------------------------------
    int i;
    for (i=0; i<16; i++) {
        if ( (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 ) return i;
    }
    return -1;
}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
    static int heartbeat = 0;

    uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0, batt=0, aux=0;
    int t1=0, t2=0;
    uint32 temp=0;
    uint8 ct[sizeof(IPC->time.curtime)];
    u32 i;

    // Update the heartbeat
    heartbeat++;

    // Read the touch screen

    but = REG_KEYXY;

    if (!(but & (1<<6))) {

        touchPosition tempPos = touchReadXY();

        x = tempPos.x;
        y = tempPos.y;
        xpx = tempPos.px;
        ypx = tempPos.py;
    }

    z1 = touchRead(TSC_MEASURE_Z1);
    z2 = touchRead(TSC_MEASURE_Z2);


    batt = touchRead(TSC_MEASURE_BATTERY);
    aux  = touchRead(TSC_MEASURE_AUX);

    // Read the date and the time
    ct[0] = 0;
    rtcGetTimeAndDate((uint8 *)ct + 1);

    // Read the temperature
    temp = touchReadTemperature(&t1, &t2);

    // Update the IPC struct
    //IPC->heartbeat    = heartbeat;
    IPC->buttons        = but;
    IPC->touchX            = x;
    IPC->touchY            = y;
    IPC->touchXpx        = xpx;
    IPC->touchYpx        = ypx;
    IPC->touchZ1        = z1;
    IPC->touchZ2        = z2;
    IPC->battery        = batt;
    IPC->aux            = aux;

    for(i=0; i<sizeof(ct); i++) {
        IPC->time.curtime[i] = ct[i];
    }

    IPC->temperature = temp;
    IPC->tdiode1 = t1;
    IPC->tdiode2 = t2;


    //sound code  :)
    TransferSound *snd = IPC->soundData;
    IPC->soundData = 0;

    if (0 != snd) {

        for (i=0; i<snd->count; i++) {
            s32 chan = getFreeSoundChannel();

            if (chan >= 0) {
                startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
            }
        }
    }

}


static void prepairReset()
{
    powerON(POWER_SOUND);

    //reset DMA
    zeroMemory( (void *)0x40000B0, 0x30 );

    REG_IME = IME_DISABLE;
    REG_IE = 0;
    REG_IF = ~0;

    IPC_ARM7 = IPC_MSG_ARM7_READY_BOOT;
}


int main(int argc, char ** argv)
{
    IPC_ARM9 = IPC_MSG_NONE;
    IPC_ARM7 = IPC_MSG_NONE;

    //// Reset the clock if needed
    //rtcReset();

    //enable sound
    powerON(POWER_SOUND);
    SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
    IPC->soundData = 0;

    irqInit();
    irqSet(IRQ_VBLANK, VblankHandler);
    irqEnable(IRQ_VBLANK);

    // Keep the ARM7 out of main RAM
    while( true )
    {
        swiWaitForVBlank();
        vu32 ipc_msg = IPC_ARM9;
        switch( ipc_msg )
        {
        case IPC_MSG_ARM7_REBOOT:
            {
                vu32 var_27ffe08 = *(vu32 *)0x027FFE08;
                vu32 arm9Dest = *(vu32 *)0x027FFE24;
                prepairReset();
                waitMs( 100 ); // wait ARM9 Reset

                *(vu32 *)0x027FFE24 = arm9Dest;        // let arm9 go
                waitMs( 10 );                        // !!!! wait ARM9 code ldr pc, #0x027ffe24 run !!!!
                *(vu32 *)0x027FFE08 = var_27ffe08;    // !!!! this will overwrite ARM9 code ldr pc, #0x027ffe24 !!!!
                swiSoftReset();
                break;
            }
        case IPC_MSG_SET_BRIGHTNESS:
            {
                u8 level = (u8 )(IPC_ARM9_P1 & 0xff) & 3;
                setBrightness( level );
                IPC_ARM9 = IPC_MSG_NONE;
                break;
            }
        case IPC_MSG_GET_BRIGHTNESS:
            {
                u8 level = getBrightness();
                IPC_ARM9_P1 = level;
                IPC_ARM9 = IPC_MSG_NONE;
                break;
            }
        case IPC_MSG_SUSPEND:
            {
                suspend();
            }
            break;
        case IPC_MSG_WAKEUP:
            {
                wakeup();
            }
        default:
            {}
        }
    }
}


