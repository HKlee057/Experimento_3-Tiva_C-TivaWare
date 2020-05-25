//*****************************************************************************
//Hector Alejandro Klée González
//Carné 17118
//Ejercicio 3 - TivaWare
//Electrónica Digital 2
//*****************************************************************************
//Librerías
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
//*****************************************************************************
//Variables utilizadas
//*****************************************************************************
uint8_t action = 0;
uint8_t send_button = 0;
uint8_t count = 0;
uint8_t send_Val = 0;
uint64_t console;
uint64_t response;
//*****************************************************************************
//Funciones Prototipo
//*****************************************************************************
void UARTInit (void);
void UARTespInit (void);
void colors (void);
//*****************************************************************************
//Main Void
//*****************************************************************************
int main(void)
{
    // Configuración de Reloj a 40 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |  SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    // Habilitación de Puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Habilitación de las entradas y salidas del Puerto F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    // Weak Pull-Up para primer botón
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    // Weak Pull-Up para segundo botón
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    // Función de incialización de UART1
    UARTInit();
    UARTespInit();
    // Loop infinitio
    while (1){
        // Función de cambio de color
        colors();
    }
}
//*****************************************************************************
//Inicialización de UART0
//*****************************************************************************
void UARTInit (void)
{
    // Habilitación de entradas y salidas del Puerto A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Habilitación del módulo UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Habilitación de pines de control
    GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
    // Configuraciones del UART
    UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    IntEnable(INT_UART0);
    UARTEnable(UART0_BASE);
}
//*****************************************************************************
//Handler UART0
//*****************************************************************************
void UART0IntHandler(void){
    // Limpiar bandera
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // Lectura de dato ingresado en la consola
    console = UARTCharGet(UART0_BASE);
    // Envío de respuesta de ESP8266
    UARTCharPut (UART0_BASE, response);
}
//*****************************************************************************
//Inicialización de UART1
//*****************************************************************************
void UARTespInit (void)
{
    SysCtlPeripheralReset(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_GPIOB);
    // Habilitación del módulo UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    // Habilitación de entradas y salidas del Puerto B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Habilitar los pines correspondientes
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    // Habilitación de pines de control
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Configuraciones del UART
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_TX);
    IntEnable(INT_UART1);
    UARTEnable(UART1_BASE);
}
//*****************************************************************************
//Handler UART1
//*****************************************************************************
void UART1IntHandler(void){
    // Limpiar bandera
    UARTIntClear(UART1_BASE, UART_INT_RX | UART_INT_TX);
    UARTCharPut (UART1_BASE, console);
    response = UARTCharGet(UART1_BASE);
    while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) == 0)
    {
        send_button = 1;
    }
    if (send_button == 1)
    {
        SysCtlDelay(500000);
        UARTCharPut (UART1_BASE, send_Val);
        send_button = 0;
    }
}
//*****************************************************************************
//Cambio de colores con botón
//*****************************************************************************
void colors (void){
    while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0)
    {
        action = 1;
    }
    if (action == 1)
    {
        SysCtlDelay(500000);
        count++;
        if (count == 1)
        {
            // Azul
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
            send_Val = 1;
        } else if (count == 2)
        {
            // Verde
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            send_Val = 2;
        } else if (count == 3)
        {
            // Turquesa
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            send_Val = 3;
        } else if (count == 4)
        {
            // Rojo
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
            send_Val = 4;
        } else if (count == 5)
        {
            // Violeta
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
            send_Val = 5;
        } else if (count == 6)
        {
            // Amarillo
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            send_Val = 6;
        } else if (count == 7)
        {
            // Blanco
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            send_Val = 7;
        } else if (count == 8)
        {
            // Negro
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
            send_Val = 8;
            count = 0;
        }
        action = 0;
    }
}
