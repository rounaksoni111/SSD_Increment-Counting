// Visitor_counter.c

 
#define F_CPU 12000000ul
#include <avr/io.h>
#include <util/delay.h>

#define SSDPORT PORTD
#define SLPORT PORTC
#define SL1 PC1
#define SL2 PC2
#define SL3 PC3
#define SL4 PC4

//Function Declaration (with 4 arguments but no return type)
void display(int d1,int d2,int d3,int d4);

void inc_data();	//Function declaration to increment data

//Array holding patterns 0	  1		2	3	4	  5		6	7	8	 9
unsigned int digit[10]={0XFC,0X60,0XDA,0XF2,0X66,0XB6,0XBE,0XE0,0XFE,0XF6};

int data1,data2,data3,data4;	//4 global variables to hold data

unsigned int sensor_status; //Global variable holding sensor status
unsigned int reset_button;		//Global variable holding Reset Button Status

int main(void)
{
	DDRD=0XFF;			//Define PORTD as output (SSD data lines are connected)
	DDRC=0b0011110;		//Define PC1, PC2, PC3, PC4 as Output (Select lines)
	
	while (1)
	{
		display(data1,data2,data3,data4); //Function to display data on SSD
		
		sensor_status=	PINB & 0X01;	//Check & Store Sensor Status
		reset_button=	PINB & 0X02;		//Check & Store Button Status
		
		if (sensor_status==0X01)	//if sensor detects
		{
			_delay_ms(250);		//debounce time (it may vary)
			inc_data();			//increment your data by 1
		}
		
		else if (reset_button==0X02)	//if Reset button is pressed
		{
			_delay_ms(250);			//debounce time (it may vary)
			data1=data2=data3=data4=0;	//Reset all data to 0
		}	
	}
}

//display Function definition
void display(int d1,int d2,int d3,int d4)
{
	SLPORT=1<<SL2|1<<SL3|1<<SL4;	//SSD1 active
	SSDPORT=digit[d1];				//Data transmitted to SSD1
	_delay_ms(1);

	SLPORT=1<<SL1|1<<SL3|1<<SL4;	//SSD2 active
	SSDPORT=digit[d2];				//Data transmitted to SSD2
	_delay_ms(1);

	SLPORT=1<<SL1|1<<SL2|1<<SL4;	//SSD3 active
	SSDPORT=digit[d3];				//Data transmitted to SSD3
	_delay_ms(1);

	SLPORT=1<<SL1|1<<SL2|1<<SL3;	//SSD4 active
	SSDPORT=digit[d4];				//Data transmitted to SSD4
	_delay_ms(1);
}

//increment_data function definition
void inc_data()
{
	data4++;		//increment data4
	if (data4>9)
	{
		data4=0;	//if data4=10, reset data4 to 0
		data3++;	//increment data3
		
		if (data3>9)
		{
			data3=0;	//if data3=10, reset data3 to 0
			data2++;	//increment data2
			
			if (data2>9)
			{
				data2=0;	//if data2=10, reset data2 to 0
				data1++;	//increment data1
				
				if (data1>9)	//if data1=10
				{
					data1=data2=data3=data4=9;	//Keep all of them 9999.
				}
			}
		}
	}
}


































/*



//Control SSD with TIMER2 Interrupt


#define F_CPU 12000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SSDPORT PORTD
#define SLPORT PORTC
#define SL1 PC1
#define SL2 PC2
#define SL3 PC3
#define SL4 PC4

void inc_data();	//Function declaration to increment data
void init_timer2();	//Function to initialize TIMER-2 (1.002ms interrupt)

//Array holding patterns 0	  1		2	3	4	  5		6	7	8	 9
unsigned int digit[10]={0XFC,0X60,0XDA,0XF2,0X66,0XB6,0XBE,0XE0,0XFE,0XF6};

int data1,data2,data3,data4;	//4 global variables to hold data for SSD

unsigned int sensor_status; //Global variable holding sensor status
unsigned int reset_button;		//Global variable holding Reset Button Status
unsigned int interrupt_count;	//Global Variable to count number of interrupts

int main(void)
{
	DDRD=0XFF;			//Define PORTD as output
	DDRC=0b0011110;		//Define PC1, PC2, PC3, PC4 as Output
	init_timer2();		//Function to initialize TIMER-2
	while (1)
	{
		sensor_status=PINB & 0X01;	//Sensor Status Stored
		reset_button=PINB & 0X02;		//Button Status Stored
		
		if (sensor_status==0X01)	//if sensor detects
		{
			_delay_ms(250);		//Debounce time (it may vary)
			inc_data();			//function to increment data by 1
		}
		
		else if (reset_button==0X02) //if button is pressed
		{
			_delay_ms(250);				//Debounce time (it may vary)
			data1=data2=data3=data4=0;	//function to reset all data to 0
		}
	}
}

void inc_data()
{
	data4++;		//increment data4
	if (data4>9)
	{
		data4=0;	//if data4=10, reset data4 to 0
		data3++;	//increment data3
		
		if (data3>9)
		{
			data3=0;	//if data3=10, reset data3 to 0
			data2++;	//increment data2
			
			if (data2>9)
			{
				data2=0;	//if data2=10, reset data2 to 0
				data1++;	//increment data1
				
				if (data1>9)	//if data1=10
				{
					data1=data2=data3=data4=9;	//reset all to 0
				}
			}
		}
	}
}

void init_timer2()		
{
	TCCR2=(1<<WGM21)|(1<<CS22); //64 Pre Scaler | CTC mode
	OCR2=188;					//Initialize OCR2 to 188 (will generate 1.002ms)					
	TIMSK=(1<<OCIE2);			//Compare match (local) interrupt enable
	sei();						//global interrupt enable
//These settings of TIMER-2 will generate interrupt after every 1.002ms
}

ISR(TIMER2_COMP_vect) //1.002ms interrupt
{
	switch(interrupt_count)
	{
		case 0:
			SLPORT=1<<SL2|1<<SL3|1<<SL4;	//1st SSD Selected
			SSDPORT=digit[data1];			//Data transmitter to SSD
			interrupt_count++;				//interrupt count incremented by 1
			break;
		
		case 1:
			SLPORT=1<<SL1|1<<SL3|1<<SL4;	//2nd SSD Selected
			SSDPORT=digit[data2];			//Data transmitted to SSD
			interrupt_count++;				//interrupt count incremented by 1
			break;
		
		case 2:	
			SLPORT=1<<SL1|1<<SL2|1<<SL4;	//3rd SSD Selected
			SSDPORT=digit[data3];			//Data transmitted to SSD
			interrupt_count++;				//interrupt count incremented by 1
			break;
		
		case 3:

		SLPORT=1<<SL1|1<<SL3|1<<SL2;		//4th SSD Selected
		SSDPORT=digit[data4];				//Data transmitted to SSD
		interrupt_count=0;					//interrupt count reset to 0
		break;
	}

}





















//Bi-directional Counter

#define F_CPU 12000000ul
#include <avr/io.h>
#include <util/delay.h>

#define SSDPORT PORTD
#define SLPORT PORTC
#define SL1 PC1
#define SL2 PC2
#define SL3 PC3
#define SL4 PC4

//Function Declaration (with 4 arguments but no return type)
void display(int d1,int d2,int d3,int d4);
void inc_data();
void dec_data();

//Array holding patterns 0	  1		2	3	4	  5		6	7	8	 9	
unsigned int digit[10]={0XFC,0X60,0XDA,0XF2,0X66,0XB6,0XBE,0XE0,0XFE,0XF6};

int data1,data2,data3,data4;	//4 global variables to hold data 

unsigned int inc_sensor_status,dec_sensor_status;

int main(void)
{
	DDRD=0XFF;			//Define PORTD as output
	DDRC=0b0011110;		//Define PC1, PC2, PC3, PC4 as Output
	
    while (1) 
    {
			display(data1,data2,data3,data4); //Function to display data on SSD
			
			inc_sensor_status=PINB & 0X01;
			dec_sensor_status=PINB & 0X02;
			
			if (inc_sensor_status==0X01)
			{
				_delay_ms(250);
				inc_data();
			}
			
			if (dec_sensor_status==0X02)
			{
				_delay_ms(250);
				dec_data();
			}
	}
}

//display Function definition 

void display(int d1,int d2,int d3,int d4)
{
	SLPORT=1<<SL2|1<<SL3|1<<SL4;	//SSD1 active	
	SSDPORT=digit[d1];				//Data transmitted to SSD1
	_delay_ms(1);

	SLPORT=1<<SL1|1<<SL3|1<<SL4;	//SSD2 active
	SSDPORT=digit[d2];				//Data transmitted to SSD2
	_delay_ms(1);

	SLPORT=1<<SL1|1<<SL2|1<<SL4;	//SSD3 active
	SSDPORT=digit[d3];				//Data transmitted to SSD3
	_delay_ms(1);

	SLPORT=1<<SL1|1<<SL2|1<<SL3;	//SSD4 active	
	SSDPORT=digit[d4];				//Data transmitted to SSD4
	_delay_ms(1);
}

void inc_data()
{		
	data4++;		//increment data4
	if (data4>9)
	{
		data4=0;	//if data4=10, reset data4 to 0
		data3++;	//increment data3
				
		if (data3>9)
		{
			data3=0;	//if data3=10, reset data3 to 0
			data2++;	//increment data2
					
			if (data2>9)
			{
				data2=0;	//if data2=10, reset data2 to 0
				data1++;	//increment data1
						
				if (data1>9)	//if data1=10
				{
					data1=data2=data3=data4=9;	//reset all to 0
				}
			}
		}
	}
}



void dec_data()
{
	data4--;		//increment data4
	if (data4<0)
	{
		data4=9;	//if data4=10, reset data4 to 0
		data3--;	//increment data3
		
		if (data3<0)
		{
			data3=9;	//if data3=10, reset data3 to 0
			data2--;	//increment data2
			
			if (data2<0)
			{
				data2=9;	//if data2=10, reset data2 to 0
				data1--;	//increment data1
				
				if (data1<0)	//if data1=10
				{
					data1=data2=data3=data4=0;	//reset all to 0
				}
			}
		}
	}
}
*/



/*
#define F_CPU 12000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define SSDPORT PORTD
#define SLPORT PORTC
#define SL1 PC1
#define SL2 PC2
#define SL3 PC3
#define SL4 PC4


void inc_data();
void dec_data();
void init_timer2();

//Array holding patterns 0	  1		2	3	4	  5		6	7	8	 9
unsigned int digit[10]={0XFC,0X60,0XDA,0XF2,0X66,0XB6,0XBE,0XE0,0XFE,0XF6};

int data1,data2,data3,data4;	//4 global variables to hold data

unsigned int inc_sensor_status,dec_sensor_status;
unsigned int interrupt_count;

int main(void)
{
	DDRD=0XFF;			//Define PORTD as output
	DDRC=0b0011110;		//Define PC1, PC2, PC3, PC4 as Output
	init_timer2();
	while (1)
	{
		inc_sensor_status=PINB & 0X01;
		dec_sensor_status=PINB & 0X02;
		
		if (inc_sensor_status==0X01)
		{
			_delay_ms(250);
			inc_data();
		}
		
		if (dec_sensor_status==0X02)
		{
			_delay_ms(250);
			dec_data();
		}
	}
}

void inc_data()
{
	data4++;		//increment data4
	if (data4>9)
	{
		data4=0;	//if data4=10, reset data4 to 0
		data3++;	//increment data3
		
		if (data3>9)
		{
			data3=0;	//if data3=10, reset data3 to 0
			data2++;	//increment data2
			
			if (data2>9)
			{
				data2=0;	//if data2=10, reset data2 to 0
				data1++;	//increment data1
				
				if (data1>9)	//if data1=10
				{
					data1=data2=data3=data4=9;	//reset all to 0
				}
			}
		}
	}
}



void dec_data()
{
	data4--;		//increment data4
	if (data4<0)
	{
		data4=9;	//if data4=10, reset data4 to 0
		data3--;	//increment data3
		
		if (data3<0)
		{
			data3=9;	//if data3=10, reset data3 to 0
			data2--;	//increment data2
			
			if (data2<0)
			{
				data2=9;	//if data2=10, reset data2 to 0
				data1--;	//increment data1
				
				if (data1<0)	//if data1=10
				{
					data1=data2=data3=data4=0;	//reset all to 0
				}
			}
		}
	}
}


void init_timer2()
{
		TCCR2=(1<<WGM21)|(1<<CS22); //64 PS | CTC 
		OCR2=188;
		TIMSK=(1<<OCIE2);
		sei();
}



ISR(TIMER2_COMP_vect) //1.002ms interrupt
{
	switch(interrupt_count)
	{
		case 0:
			SLPORT=1<<SL2|1<<SL3|1<<SL4;
			SSDPORT=digit[data1];
			interrupt_count++;
			break;
		
		case 1:
			SLPORT=1<<SL1|1<<SL3|1<<SL4;
			SSDPORT=digit[data2];
			interrupt_count++;
			break;
		
		case 2:
			
			SLPORT=1<<SL1|1<<SL2|1<<SL4;
			SSDPORT=digit[data3];
			interrupt_count++;
			break;
			
		case 3:

		SLPORT=1<<SL1|1<<SL3|1<<SL2;
		SSDPORT=digit[data4];
		interrupt_count=0;
		break;
	}

}
*/