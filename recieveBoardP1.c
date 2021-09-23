#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"

void GPIOF_Handler(void);

int state=0;                            //Estados de indicación
int fss;
unsigned char inputSignal[8];           //Vector de resultado


void main(void) //recieveBoard
{
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //Encender puerto B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    //Encender puerto E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    //Encender puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    //Definición de puertos de salida y entrada
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4); //Start/end , data1, data2

    //Interrupción start/end
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE, GPIOF_Handler);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);                         //Clk
    IntMasterEnable();

    while(1)
    {
        if(state>=4)
        {
            //Send Result to LED's & reset state variable
            if(inputSignal[0])
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_PIN_0);
            else
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, 0x0);

            if(inputSignal[1])
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);
            else
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x0);

            if(inputSignal[2])
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
            else
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x0);

            if(inputSignal[3])
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3);
            else
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0x0);

            if(inputSignal[4])
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
            else
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x0);

            if(inputSignal[5])
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);
            else
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x0);

            if(inputSignal[6])
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            else
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

            if(inputSignal[7])
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            else
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

            state=0;
        }
    }
}

void GPIOF_Handler(void)
{
    fss = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);
        if(fss)        //E5 de poncho
        {
            inputSignal[0]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
            inputSignal[1]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4);
            state=1;
        }
        else if(state==1)
        {
            inputSignal[2]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
            inputSignal[3]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4);
            state=2;
        }
        else if(state==2)
        {
            inputSignal[4]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
            inputSignal[5]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4);
            state=3;
        }
        else if(state==3)
        {
            inputSignal[6]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
            inputSignal[7]=GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4);
            state=4;
        }
        GPIOIntStatus(GPIO_PORTF_BASE, true);
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}
