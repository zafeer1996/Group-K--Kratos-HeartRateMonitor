#include <p18f452.h>
#include <delays.h>
#include "xlcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "ow.h"

unsigned char MSB_Temp;
unsigned char LSB_Temp;
unsigned int MSB_Temp2 = 0;
unsigned int LSB_Temp2 =0;
unsigned int Temp_Int = 0;
unsigned char degree = 0xDF;
float floatFract = 0.0000;
int sign = 0;
int frat =0;
int x = 0;
char Temperature[20];
extern int live;


unsigned char ow_reset(void)
{
    short device_found=0;
    // DQ High
    OW_TRIS=INPUT;
    // DQ Low
    OW_LAT=0;
    OW_TRIS=OUTPUT;
    // delay 480us
    Delay10TCYx(48);
    // DQ High
    OW_TRIS=INPUT;
    // Wait for the sensors to respond
    Delay10TCYx(24);
    // Determine is a device has responded
    device_found = !OW_PIN;
    return device_found;
}
void ow_write_byte(unsigned char data)
{
    char i;
    for (i=0;i<8;i++)
    {
        // DQ Low
        OW_LAT=0;
        OW_TRIS=OUTPUT;
        // Keep it low for 10us to start the WRITE
        Delay10TCYx(1);
        // Keep low i.e. keep output mode and low if WRITE0
        // or release line i.e. make input to take it high if WRITE1
        OW_TRIS = data & 0x01;
        Delay10TCYx(5);
        // Release the line. Total of 60us
        OW_TRIS=INPUT;
        // Some recovery time between the bits 2us
        Delay10TCYx(2);
        data =data >>1;
    }
}

char ow_read_byte(void)
{
    char data=0;
    char i;
    for (i=0;i<8;i++)
    {
        // DQ Low. Initiate READ
        OW_LAT=0;
        OW_TRIS=OUTPUT;
        Nop();
        Nop();
        // DQ High. Release
        OW_TRIS=INPUT;
        // Wait for total of 10 us since READ initiation
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        // Read the bit and place it in the data at the right position
        data = (OW_PIN << i) | data;
        // Additional time to complete the minimum 60 us READ cycle
        Delay10TCYx(5);
    }
    return data;
}

void temp(void)
{
    
        if (live==0)
        {
            while( BusyXLCD() );
            SetDDRamAddr(0x02);
            putrsXLCD("TEMPERATURE:");

        }
        if (live==1)
        {
            while( BusyXLCD() );
            SetDDRamAddr(0x50);
            putrsXLCD("TEMP:");
        }

            TRISBbits.RB3=0; //RB3 Output transistor
            ow_reset();  //Reset sensor
            ow_write_byte(0xCC); // Skip ROM Check
            ow_write_byte(0x44); //Temperature conversion 
            PORTBbits.RB3 = 1;  //Strong pullup provides current to power device

            Delay10KTCYx(100);//1000ms conversion (750ms is recommended conversion time))

            PORTBbits.RB3 = 0; //Turn off strong pullup
            //Reading Device
            ow_reset(); //reset device
            ow_write_byte(0xCC); //skip ROM check
            ow_write_byte(0xBE); //Send read scratchpad on DS 1822P
            LSB_Temp = ow_read_byte(); //Read first byte, LS and store in LSB_Temp
            MSB_Temp = ow_read_byte(); //Read first byte, MS and store in MSB_Temp
            
            //Obtain int component
            LSB_Temp2 = LSB_Temp >> 4; 
            MSB_Temp2 = MSB_Temp << 4;
            Temp_Int = MSB_Temp2 | LSB_Temp2;

            //Obtain fraction component
            if(LSB_Temp & 0x01){floatFract += 0.0625;}
            if(LSB_Temp & 0x02){floatFract += 0.125;}
            if(LSB_Temp & 0x04){floatFract += 0.25;}
            if(LSB_Temp & 0x08){floatFract += 0.5;}
            frat =floatFract*1000;

            //Sign check
            sign = ((MSB_Temp >> 3 )& 0x3F);
            if(sign == 0){
                sprintf(Temperature,"+%d.%03d%cC",Temp_Int,frat,degree);

                Temp_Int = 0;
                frat= 0;
                floatFract =0.0;
            }
            else
            {
                sprintf(Temperature,"-%d.%03d%cC",Temp_Int,frat);
                Temp_Int = 0;
                frat= 0;
                floatFract =0.0;

            }

            while(BusyXLCD());
            if (live==0) SetDDRamAddr(0x12); //Set cursor to top line
            else if (live==1) SetDDRamAddr(0x55);
            while(BusyXLCD());
            Delay1KTCYx(110); //Give time for LCD to refresh, writing to it too quickly causes flicker issues
            while(BusyXLCD());
            if (live!=3) putsXLCD(Temperature);//Write top line value
            if (live!=3) sprintf(Temperature,"                   ");

            if (live==0) Delay10KTCYx(255);
            
    
 

    
       
}

