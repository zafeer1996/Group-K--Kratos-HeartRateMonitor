#include <p18f452.h>
#include "xlcd.h"
#include <delays.h>
#include <timers.h>
#include <portb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "HeartRate_Functions.h"
#include "ow.h"
#include "Speaker_Functions.h"
#define _XTAL_FREQ 4000000

////////////////////////////////////////////////////////////////////////////
char BPMMessage [8];
char HRVMessage [8];
char PulseMessage [8];

int T0_finished = 0;
int Beatcnt = 0;
int ccpcnt = 0;
int ccp1 = 0;
int ccp2 = 0;
int ccp3 = 0;
int ccpdif = 0;
int NN50 = 0;
int NNcnt  = 0;
int live;
int BPM;
unsigned int HRV;
 //////////////////////////////////////////////////////////////////////////////


void PORTB_con(void)
{
    OpenRB0INT(PORTB_CHANGE_INT_ON & RISING_EDGE_INT & PORTB_PULLUPS_OFF);
    //INTCON2bits.RBIP = 0;
}

void PORTB_end(void)
{
    CloseRB0INT();
}

void Heart_rate(void)
{
    
    HRV = 0;
    Beatcnt = 0;
    BPM = 0;
    ccpcnt = 1;
    ccp1 = 0;
    ccp2 = 0;
    ccpdif = 0;
    NN50 = 0;
    NNcnt  = 0;
    
    PORTDbits.RD3 = 1;
    T0_finished = 0;
    
    if (live==0)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x00);                                                     // Set Display data ram address to 0
        while(BusyXLCD());
        WriteCmdXLCD(0x01);
        while(BusyXLCD()); 

        putrsXLCD("Measuring...");
        while(BusyXLCD());
    }
    
    
    INTCONbits.GIEH = 1;        //enable high interrupts
    INTCONbits.GIEL = 1;        //enable low interrupts
    RCONbits.IPEN = 1;          // turn priority levels on
    INTCON2bits.TMR0IP = 0;        //timer 0 priority set to low
    INTCONbits.TMR0IF = 0;        //clears interrupt flag
    INTCONbits.TMR0IE = 1;        //enables tmr0 interrupt
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_256 & T0_EDGE_RISE );
    WriteTimer0(26472);         //writes value to overflow in 10 s
    
    while(1)
    {
        if(T0_finished == 1)
        {
            INTCONbits.GIEH = 0;        //disable high interrupts
            INTCONbits.GIEL = 0;        //disable low interrupts
            CloseTimer0();
            
            BPM = Beatcnt * 6 ;
            itoa(BPM, BPMMessage);
            
            NNcnt = Beatcnt-1;
            HRV = (NN50*100)/(NNcnt);
            itoa(HRV,HRVMessage);
            PORTDbits.RD3 = 0;
            break;
        }
    }  
}


void Write_HR_data(void)
{
    if (live==0)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x00);                                                     // Set Display data ram address to 0
        while(BusyXLCD());
        WriteCmdXLCD(0x01);
        while(BusyXLCD());  

        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x00);                                                     // Set Display data ram address to 0
        while(BusyXLCD());
        if (live!=3)putrsXLCD("Completed");
        while(BusyXLCD());
    }
    if (live==1)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x00);                                                     // Set Display data ram address to 0
        while(BusyXLCD());
        WriteCmdXLCD(0x01);
        while(BusyXLCD());  

        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x06);                                                     // Set Display data ram address to 0
        while(BusyXLCD());
        if (live!=3)putrsXLCD("LIVE");
        while(BusyXLCD());
    }
    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x40);                                                     // Set Display data ram address to 0
    while(BusyXLCD());
    if (live!=3)putrsXLCD("BPM:");

    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x44);
    while(BusyXLCD());  
    putsXLCD(BPMMessage);
    while(BusyXLCD()); 
    
    if(BPM<60)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x4C);
        while(BusyXLCD());  
        putrsXLCD("LOW!");
        speaker_alert();
        if (live==0) Delay1KTCYx(255);
    }
    if(BPM>100)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x4B);
        while(BusyXLCD());  
        putrsXLCD("HIGH!");
        speaker_alert();
        if (live==0) Delay1KTCYx(255);
    }
    
    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x10);                                                     // Set Display data ram address to 0
    while(BusyXLCD());
    if (live!=3)putrsXLCD("HRV:");
    while(BusyXLCD()); 

    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x14);
    while(BusyXLCD());  
    putsXLCD(HRVMessage);
    while(BusyXLCD());
    
    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x17);                                                     // Set Display data ram address to 0
    while(BusyXLCD());
    if (live!=3)putrsXLCD("%");
    while(BusyXLCD());
    
    if(HRV>25)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x1B);
        while(BusyXLCD());  
        putrsXLCD("HIGH!");
        speaker_alert(); 
    }
    
    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x51);                                                     // Set Display data ram address to 0
    while(BusyXLCD());
    if (live==0)putrsXLCD("AVG 60-100 BPM");
    while(BusyXLCD());  
   
}

void instructions(void)
{
    int count=5;
    int addr=0x50;
    char countdown[8];
    while(BusyXLCD());
    WriteCmdXLCD(0x01);
    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x00);                                                     
    while(BusyXLCD());
    putrsXLCD("Put Finger in HR");
    while(BusyXLCD());
    SetDDRamAddr(0x40);                                                     
    while(BusyXLCD());
    putrsXLCD("Sensor for 10s");
    Delay10KTCYx(255);
    while(BusyXLCD());
    SetDDRamAddr(0x10);
    while(BusyXLCD());
    putrsXLCD("TEST BEGINS IN");
    
    while (count!=0)
    {
        SetDDRamAddr(addr);
        itoa(count, countdown);
        while(BusyXLCD());
        putsXLCD(countdown);
        SetDDRamAddr(addr+1);
        while(BusyXLCD());
        putrsXLCD("..");
        Delay10KTCYx(100);
        count--;
        addr=addr+3;
    }
    
}

void Heart_Monitor (void)
{
    
    
        PORTB_con();
        TRISDbits.RD3 = 0;
        PORTDbits.RD3 = 1;
        instructions();
        Heart_rate();
        Write_HR_data();
        Delay10KTCYx(250);
        Delay10KTCYx(250);
   
       
}

void livereadings (void)
{
    while(BusyXLCD());                                                      // Wait if LCD busy
    SetDDRamAddr(0x00);                                                     // Set Display data ram address to 0
    while(BusyXLCD());
    WriteCmdXLCD(0x01);
    while(BusyXLCD()); 
    PORTB_con();
    TRISDbits.RD3 = 0;
    PORTDbits.RD3 = 1;
     if (live==1)
    {
        while(BusyXLCD());                                                      // Wait if LCD busy
        SetDDRamAddr(0x00);                                                     // Set Display data ram address to 0
        while(BusyXLCD());
        WriteCmdXLCD(0x01);
        while(BusyXLCD()); 

        if (live!=3)putrsXLCD("LIVE READINGS");
        while(BusyXLCD());
        SetDDRamAddr(0x40); 
        while(BusyXLCD());
        if (live!=3)putrsXLCD("Refresh Rate:10s");
        SetDDRamAddr(0x50); 
        while(BusyXLCD());
        if (live!=3)putrsXLCD("Measuring...");
        
    }
    while (live==1)
    {
        if (live!=3)Heart_rate();
        if (live!=3)Write_HR_data();
        if (live!=3)temp();
        
        
    }
    live=0;
    
}