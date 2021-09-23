#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"

uint32_t valorADC, valorADC2;
int Mask = 0x01;
int valor1;
int valor2;
int x=0;
int cont=0;

void GPIOF_Handler_mifuncion(void)
{
    x++;
    if(x==1)
    {
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);

        valor1 = valorADC2 & Mask;
        valorADC2 = valorADC2 >> 1;
        valor2 = valorADC2 & Mask;
        valorADC2 = valorADC2 >> 1;
        if(valor1==1)
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x0);
        }
        if(valor2==1)
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x0);
        }
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);      //Indicar que comience a leer
        SysCtlDelay(SysCtlClockGet() / 100);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x0);
        valor1 = valorADC2 & Mask;
        valorADC2 = valorADC2 >> 1;
        valor2 = valorADC2 & Mask;
        valorADC2 = valorADC2 >> 1;
        if(valor1==1)
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x0);
        }
        if(valor2==1)
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, GPIO_PIN_4);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4, 0x0);
        }
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);      //Indicar que comience lectura
        SysCtlDelay(SysCtlClockGet() / 100);

        if(x==4)//Nunca llega a 5
        {
            x=0;
        }
    }
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);                 //Indicar que finalice lectura
    GPIOIntStatus(GPIO_PORTF_BASE, true);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
}
void main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5); // dataGPIOs - GPIO 1 para clk signal - GPIO 5 Start - GPIO 2 & 4 Data
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1);

    //Interrupción Externa
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE, GPIOF_Handler_mifuncion);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
    IntMasterEnable();

    //ADC
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE,3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE,3);
        while(!ADCIntStatus(ADC0_BASE,3, false)){}
        ADCIntClear(ADC0_BASE,3);
        ADCSequenceDataGet(ADC0_BASE, 3,&valorADC);
        valorADC2 = valorADC >> 4;

        for(cont=0;cont<4;cont++)
        {
            GPIOF_Handler_mifuncion();
        }
    }

}


