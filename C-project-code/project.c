#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>
#include <stdlib.h>
#include <stdio.h>
#include <keypad.h>
#include <bit.h>
#include <seven_seg.h>
#include <scheduler.h>

unsigned char power;
unsigned char color;
unsigned char currentlight;
unsigned char keypadinput;
unsigned char count = 0x00;
unsigned char x;
unsigned char timer = 0x09;
unsigned char sevenseg;

void init_PWM() {
	TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS22);
}

void set_PWM(double frequency) {
	unsigned long temp;
	if (frequency < 1)
	OCR2 = 0;
	else
	OCR2 = (int)(16000000 / (128 * frequency)) - 1;
}

void transmit_data(unsigned char data) {

	int i;

	for (i = 0; i < 8 ; ++i) {

		// Sets SRCLR to 1 allowing data to be set

		// Also clears SRCLK in preparation of sending data

		PORTD = 0x08;

		// set SER = next bit of data to be sent.

		PORTD |= ((data >> i) & 0x01);

		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register

		PORTD |= 0x02;

	}

	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register

	PORTD |= 0x04;

	// clears all lines in preparation of a new transmission

	PORTD = 0x00;
}

void transmit_color(unsigned char colordata) {

	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x08;
		// set SER = next bit of data to be sent.
		PORTA |= ((colordata >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTA = 0x00;
}

void transmit_7segdisplay(unsigned char sevensegvalue) {
	
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x80;
		// set SER = next bit of data to be sent.
		PORTA |= ((Write7Seg(sevensegvalue) >> i) & 0x01)<<4;
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x20;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x40;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
	
}

int powerrow [4] = {1,2,4,8};
int colorrow [4] = {0xFE, 0xFD, 0xFB, 0xF7};

enum states1 {STATE_INIT, STATE_RANDOM_GEN, STATE_MATCH};
int randdots(int state)
{
   switch(state)
   {
      case -1:
      state = STATE_INIT;
      break;

      case STATE_INIT:
	  if(!(GetBit(PIND, 5)) && timer == 0)
	  {
		  state = STATE_INIT;
	  }
	  else if(!(GetBit(PIND,5)) && timer > 0)
	  {	  
			state = STATE_RANDOM_GEN;
	  }	  
      break;

      case STATE_RANDOM_GEN:
	  if( timer > 0)
	  {
			state = STATE_MATCH;
			timer--;
	  }
	  else if( timer == 0)
	  {
		  state = STATE_INIT;
	  }	  	  
      break;
	  
	  case STATE_MATCH:
	  if( timer > 0)
	  {
		  state = STATE_RANDOM_GEN;
	  }
	  else if (timer == 0)
	  {
		  state = STATE_INIT;
	  }	  	  
	  break;
   }
   switch(state)
   {
      case STATE_INIT:
      power = 0x00;
      color = 0xFF;
      break;

      case STATE_RANDOM_GEN:
      color = colorrow[rand()%4];
      power = powerrow[rand()%4];
      break;

	  case STATE_MATCH:
	  if(power == 8 && color == 0xF7)
	  {
		  currentlight = 0x0A;
	  }
	  else if(power == 8 && color == 0xFB)
	  {
		  currentlight = 0x03;
	  }
	  else if(power == 8 && color == 0xFD)
	  {
		  currentlight = 0x02;
	  }
	  else if(power == 8 && color == 0xFE)
	  {
		  currentlight = 0x01;
	  }
	  else if(power == 4 && color == 0xF7)
	  {
		  currentlight = 0x0B;
	  }
	  else if(power == 4 && color == 0xFB)
	  {
		  currentlight = 0x06;
	  }
	  else if(power == 4 && color == 0xFD)
	  {
		  currentlight = 0x05;
	  }
	  else if(power == 4 && color == 0xFE)
	  {
		  currentlight = 0x04;
	  }
	  else if(power == 2 && color == 0xF7)
	  {
		  currentlight = 0x0C;
	  }
	  else if(power == 2 && color == 0xFB)
	  {
		  currentlight = 0x09;
	  }
	  else if(power == 2 && color == 0xFD)
	  {
		  currentlight = 0x08;
	  }
	  else if(power == 2 && color == 0xFE)
	  {
		  currentlight = 0x07;
	  }
	  else if(power == 1 && color == 0xF7)
	  {
		  currentlight = 0x0D;
	  }
	  else if(power == 1 && color == 0xFB)
	  {
		  currentlight = 0x0F;
	  }
	  else if(power == 1 && color == 0xFD)
	  {
		  currentlight = 0x00;
	  }
	  else if(power == 1 && color == 0xFE)
	  {
		  currentlight = 0x0E;
	  }
	  break;
      default:
      break;
   }
   return state;
}

enum states2{STATE_CHECK, STATE_INC};
int POINTCNT (int state)
{
	switch(state)
	{
		case -1:
		state = STATE_CHECK;
		break;
	
		case STATE_CHECK:
		if(currentlight == keypadinput)
		{
			count++;
			state = STATE_INC;
		}
		else
		{
			state = STATE_CHECK;
		}
		break;
	
		case STATE_INC:
		if(currentlight == keypadinput)
		{
			state = STATE_INC;	
		}
		else
		{
			state = STATE_CHECK;
		}
		break;
	}
	switch(state)
	{
		case STATE_CHECK:
		break;
		
		case STATE_INC:
		break;
		
		default:
		break;
	}
	return state;	
			
}

enum States_Speaker { STATE_SPEAKER_INIT, STATE_SPEAKER_OFF, STATE_SPEAKER_ON_1, STATE_SPEAKER_ON_2, STATE_SPEAKER_ON_3};
int SpeakerTick( int state )
{
	//Transitions
	switch( state )
	{
		case -1:
			state = STATE_SPEAKER_OFF;
			break;

		case STATE_SPEAKER_OFF:
			//start if D5 pressed
			if( !GetBit(PIND, 5) && timer > 0 )
			{
				state = STATE_SPEAKER_ON_1;
			}
			else if ( timer == 0)
			{
				//stay
			}

			break;

		case STATE_SPEAKER_ON_1:
			//continue if A0 held
			if( timer > 0)
			{
				state = STATE_SPEAKER_ON_2;
			}
			else if(timer == 0) //go back to wait
			{
				state = STATE_SPEAKER_OFF;
			}

			break;
		
		case STATE_SPEAKER_ON_2:
		//continue if A0 held
		if( timer > 0)
		{
			state = STATE_SPEAKER_ON_3;
		}
		else if(timer == 0) //go back to wait
		{
			state = STATE_SPEAKER_OFF;
		}

		break;

		default:
			state = STATE_SPEAKER_OFF;
			break;
	}

	//Actions
	switch( state )
	{

		case STATE_SPEAKER_OFF:
			set_PWM( 0 );
			break;

		case STATE_SPEAKER_ON_1:
			set_PWM( 261.63 );
			break;
			
		case STATE_SPEAKER_ON_2:
			set_PWM( 293.66 );
			break;
		
		case STATE_SPEAKER_ON_3:
			set_PWM( 329.63 );
			break;

		default:
			break;
	}

	return state;
}

int COLOR(int state)
{
   transmit_color (color);
   return 0;
}

int POWER(int state)
{
   transmit_data (power);
   return 0;
}

int SEVENSEGDISPLAY(int state)
{
	transmit_7segdisplay(timer);
	return 0;
}

int KEYPAD (int state)
{
	x = GetKeypadKey();
	switch (x) {
		case '\0': keypadinput = 0xFF; break;// All 5 LEDs on
		case '1': keypadinput = 0x01; break; // hex equivalent
		case '2': keypadinput = 0x02; break;
		case '3': keypadinput = 0x03; break;
		case '4': keypadinput = 0x04; break;
		case '5': keypadinput = 0x05; break;
		case '6': keypadinput = 0x06; break;
		case '7': keypadinput = 0x07; break;
		case '8': keypadinput = 0x08; break;
		case '9': keypadinput = 0x09; break;
		case 'A': keypadinput = 0x0A; break;
		case 'B': keypadinput = 0x0B; break;
		case 'C': keypadinput = 0x0C; break;
		case 'D': keypadinput = 0x0D; break;
		case '*': keypadinput = 0x0E; break;
		case '0': keypadinput = 0x00; break;
		case '#': keypadinput = 0x0F; break;
		default: keypadinput = 0x1B; break; // Should never occur. Middle LED off.
	};
	return 0;
}

int main()
{
	init_PWM();
   DDRD = 0x9F; PORTD = 0x60;
   DDRA = 0xFF; PORTA = 0x00;
   DDRC = 0xF0; PORTC = 0x0F;
   DDRB = 0xFF; PORTB = 0x00;
   unsigned long int randdotstime = 500;
   unsigned long int outputtime = 1;
   unsigned long int keypadtime = 100;
   unsigned long int scoreoutput = 1;
   unsigned long int sevensegoutput = 1;
   unsigned long int speakertick = 1000;
   
   unsigned long tmpGCD = 1;
   tmpGCD = findGCD (randdotstime, outputtime);
   tmpGCD = findGCD (tmpGCD, keypadtime);
   tmpGCD = findGCD (tmpGCD, scoreoutput);
   tmpGCD = findGCD (tmpGCD, sevensegoutput);
   tmpGCD = findGCD (tmpGCD, speakertick);
   unsigned long int GCD = tmpGCD;

   task tasks[7];
   const unsigned short numTasks = 7;

   tasks[0].state = -1;//Task initial state.
   tasks[0].period = randdotstime;//Task Period.
   tasks[0].elapsedTime = randdotstime;//Task current elapsed time.
   tasks[0].TickFct = &randdots;//Function pointer for the tick.

   // Task 2
   tasks[1].state = -1;//Task initial state.
   tasks[1].period = outputtime;//Task Period.
   tasks[1].elapsedTime = outputtime;//Task current elapsed time.
   tasks[1].TickFct = &POWER;//Function pointer for the tick.

   tasks[2].state = -1;//Task initial state.
   tasks[2].period = outputtime;//Task Period.
   tasks[2].elapsedTime = outputtime;//Task current elapsed time.
   tasks[2].TickFct = &COLOR;//Function pointer for the tick.

   tasks[3].state = -1;//Task initial state.
   tasks[3].period = keypadtime;//Task Period.
   tasks[3].elapsedTime = keypadtime;//Task current elapsed time.
   tasks[3].TickFct = &KEYPAD;//Function pointer for the tick.

   tasks[4].state = -1;//Task initial state.
   tasks[4].period = outputtime;//Task Period.
   tasks[4].elapsedTime = outputtime;//Task current elapsed time.
   tasks[4].TickFct = &POINTCNT;//Function pointer for the tick.
   
   tasks[5].state = -1;//Task initial state.
   tasks[5].period = sevensegoutput;//Task Period.
   tasks[5].elapsedTime = sevensegoutput;//Task current elapsed time.
   tasks[5].TickFct = &SEVENSEGDISPLAY;//Function pointer for the tick.
   
   tasks[6].state = -1;//Task initial state.
   tasks[6].period = speakertick;//Task Period.
   tasks[6].elapsedTime = speakertick;//Task current elapsed time.
   tasks[6].TickFct = &SpeakerTick;//Function pointer for the tick.
   
   TimerSet(GCD);
   TimerOn();
   while(1)
   {

      unsigned short i;
      // Scheduler code
      for ( i = 0; i < numTasks; i++ ) {
         // Task is ready to tick
         if ( tasks[i].elapsedTime == tasks[i].period ) {
            // Setting next state for task
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            // Reset the elapsed time for next tick.
            tasks[i].elapsedTime = 0;
         }
         tasks[i].elapsedTime += GCD;
      }
	  PORTB = count;
      while (!TimerFlag);	// Wait 1 sec
      TimerFlag = 0;
   }
}

