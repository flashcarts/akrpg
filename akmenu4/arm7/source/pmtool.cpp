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









#include "pmtool.h"


#define PM_REG_READ (1 << 7)
#define PM_REG_WRITE (0 << 7)

u8 PM_ReadRegister( u8 registerIndex )
{
    SerialWaitBusy();

    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
    REG_SPIDATA = registerIndex | PM_REG_READ;

    SerialWaitBusy();

    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz ;
    REG_SPIDATA = 0;

    SerialWaitBusy();

    return(REG_SPIDATA & 0xff);
}

void PM_WriteRegister( u8 registerIndex, u8 controlCode )
{
    SerialWaitBusy();

    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz | SPI_CONTINUOUS;
    REG_SPIDATA = registerIndex | PM_REG_WRITE;

    SerialWaitBusy();

    REG_SPICNT = SPI_ENABLE | SPI_DEVICE_POWER | SPI_BAUD_1MHz;
    REG_SPIDATA = controlCode;
}