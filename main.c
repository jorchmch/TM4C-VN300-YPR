#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> //atof

#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"
#include "utils/uartstdio.c"

// VARIABLES GLOBALES
#define SIZE    38  // 171 PARA 169

double YAW;
double PITCH;
double ROLL;

int count_0=0;
int NUMERO=0;
bool VAL=false;

int contador=0;

const uint8_t palabra[]="$VNSIH,0*06DE\n";
char DATARX[SIZE]="";
char delimitador[] = ",\n";
char *token;

// variables de rotacion
int tam=38;
int num=16;
int i,j;


void
ConfigureUART0(void)
{

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);

}

void ConfigureUART3(void){
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    MAP_GPIOPinConfigure(GPIO_PC7_U3TX);
    MAP_GPIOPinConfigure(GPIO_PC6_U3RX);
    MAP_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_7 | GPIO_PIN_6);
    MAP_UARTConfigSetExpClk(UART3_BASE, MAP_SysCtlClockGet(), 9600,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));

}

void ConfigureUART7(void){
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    MAP_GPIOPinConfigure(GPIO_PE0_U7RX);
    MAP_GPIOPinConfigure(GPIO_PE1_U7TX);
    MAP_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    MAP_UARTConfigSetExpClk(UART7_BASE, MAP_SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
}

void ConfigureInitial (void){


    MAP_FPULazyStackingEnable();
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);  //50mhz
    // MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //80mhz
}
int
main(void)
{

    ConfigureInitial();
    ConfigureUART3();   // FUNCION: COMUNICAR AL ARDUINO 9600 baudios
    ConfigureUART7();   // FUNCION: VN300 RECIBIR Y ENVIAR
    ConfigureUART0();   // FUNCION: CONSOLA

    UARTprintf("Se configuro UART0, UART7, PWM Y ADC\n");

    //PROBAR SysCtlDelay(SysCtlClockGet()/3); // 1 s COMO 1 SEGUNDO
    SysCtlDelay((15000000)*5); // 5000ms
    //UARTEnable(UART7_BASE);
    for(j = 0 ; j < 15 ; j++)   // ENVIA CON CHARPUT UART7
       {UARTCharPut( UART7_BASE, palabra[j]);
       UARTprintf("vuelta N° %d\n",j);


       }

    SysCtlDelay((15000000)*0.2); // 200ms



    while(1)
    {
        while (UARTCharsAvail(UART7_BASE))
            {       SysCtlDelay(3);
                while(NUMERO<SIZE){
                    DATARX[NUMERO]=UARTCharGet(UART7_BASE);
                    SysCtlDelay(3);
                    NUMERO++;

                }
            }
        NUMERO=0;
        //UARTprintf("datos :%s\n",DATARX);

        char salida[38]="";
        char salida2[38]="";
            for (i=0;i<(tam-num);i++){
                salida[i]=DATARX[i+num-1];
            }
            for (i=0;i<(num-2);i++){
                salida2[i]=DATARX[i];
            }
        strcat(salida,salida2);

       UARTprintf("EXIT FINAL: %s \n",salida);


      contador++;
      if (contador > 10){

              token = strtok(salida,delimitador);
              UARTprintf("TOKEN 1: %s \n",token);
              if (strcmp(token,"$VNYPR")==0){
                     VAL=true;
                     //UARTprintf("VAL TRUE\n");
                 } else { VAL=false;
                 //UARTprintf("VAL FALSE\n");
                 }


              while(VAL){
                    token = strtok(NULL, delimitador);
                    count_0++;
                    //UARTprintf("TOKEN bucle: %s \n",token);
                    switch (count_0)
                        {
                            case 1:
                              YAW=atof(token);
                              break;

                            case 2:
                              PITCH=atof(token);
                              break;

                            case 3:
                              ROLL=atof(token);
                              VAL=false;
                              count_0=0;
                              break;

                            default:
                                break;
                        }
                    }


      }

UARTprintf("==== fin de bucle ===== \n");
//count_0=0;

    }

}

