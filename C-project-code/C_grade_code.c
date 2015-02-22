/*
 * CS120Project.c
 *
 * Created: 5/22/2013 3:01:19 PM
 *  Author: Fernando
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>

unsigned char rows = 0x01;
unsigned char columns = 0x01;

unsigned char gameMatrix[8][8];
unsigned char update_rows = 0x00;
unsigned char update_columns = 0x00;


//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
	if(c==0){return b;}
	a = b;
	b = c;
}
return 0;
}
//--------End find GCD function ----------------------------------------------

unsigned char curr_key;

unsigned char GetKeypadKey() {

	PORTD = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTD to stabilize before checking
if (GetBit(PIND,0)==0) { return('1'); }
	if (GetBit(PIND,1)==0) { return('4'); }
	if (GetBit(PIND,2)==0) { return('7'); }
	if (GetBit(PIND,3)==0) { return('*'); }

	// Check keys in col 2
	PORTD = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTD to stabilize before checking
	if (GetBit(PIND,0)==0) { return('2'); }
	if (GetBit(PIND,1)==0) { return('5'); }
	if (GetBit(PIND,2)==0) { return('8'); }
	if (GetBit(PIND,3)==0) { return('0'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTD = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PIND,0)==0) { return('3'); }
	if (GetBit(PIND,1)==0) { return('6'); }
	if (GetBit(PIND,2)==0) { return('9'); }
	if (GetBit(PIND,3)==0) { return('#'); }
	// ... *****FINISH*****

	// Check keys in col 4	
	PORTD = 0x7F;
	asm("nop");
	if (GetBit(PIND,0)==0) { return('A'); }
	if (GetBit(PIND,1)==0) { return('B'); }
	if (GetBit(PIND,2)==0) { return('C'); }
	if (GetBit(PIND,3)==0) { return('D'); }
	// ... *****FINISH*****

	return('\0'); // default value

}

enum Keypad_States{k0} keypad_state;
	
int keypad_tick(int state){
	keypad_state = state;
	switch(keypad_state){//transitions
		case k0:
			keypad_state = k0;
			break;
	}
	switch(keypad_state){//actions
		case k0:
			curr_key = GetKeypadKey();
			break;
	}
	return keypad_state;
}

enum Button_States{no_press, press} button_state;
	unsigned char prev_column = 0x01;
	unsigned char prev_row = 0x01;
	
int button_press(int state){
	button_state = state;
	switch(button_state){
		case no_press:
			if( curr_key == '\0'){
				button_state = no_press;
			} else if( curr_key != '\0'){
				button_state = press;
				switch(curr_key){
					case 'B':
						if(GetBit(rows, 0)){}
						else{
							rows = rows >> 1;
							prev_row = prev_row >> 1;
							transmit_data_red(~(rows | update_rows));
							//transmit_data_red(~(prev_row));
							//PORTA = prev_column;
							}
						break;
					case '3':
						if(GetBit(columns,0)){}
						else{
							columns = columns >> 1;
							//prev_column = prev_column >>1;
							PORTA = (columns | update_columns);}
						break;
					case '6':
						if(GetBit(rows,7)){}
						else{
							rows = rows << 1;
							prev_row = prev_row << 1;
							transmit_data_red(~(rows | update_rows));
							//PORTA = prev_column;
							//transmit_data_red(~(prev_row));
							}
						break;
					case '9':
						if(GetBit(columns, 7)){}
						else{
							columns = columns << 1;
							prev_column = prev_column << 1;
							PORTA = (columns | update_columns);}
						break;
					case '*':
						update_rows = rows | update_rows;
						update_columns = columns | update_columns;
						break;
				}
				break;
			}
			case press:
				if(curr_key == '\0'){
					button_state = no_press;
				} else if(curr_key != '\0'){
					button_state = press;
				}
				break;
	}
	return button_state;
}

void transmit_data_green(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

void transmit_data_red(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x80;
		// set SER = next bit of data to be sent.
		PORTC |= (((data >> i) & 0x01)<<4);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x20;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x40;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

int main(void)
{
	 DDRA = 0xFF; PORTA = 0x00;
	 DDRC = 0xFF; PORTC = 0x00;
	 DDRD = 0xF0; PORTD = 0x0F; 
	 
	 PORTA = columns;
	
		// Period for the tasks
		unsigned long int SMTick1_calc = 100;
		unsigned long int SMTick2_calc = 1;

		//Calculating GCD
		unsigned long int tmpGCD = 1;
		tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);

		//Greatest common divisor for all tasks or smallest time unit for tasks.
		unsigned long int GCD = tmpGCD;

		//Recalculate GCD periods for scheduler
		unsigned long int SMTick1_period = SMTick1_calc/GCD;
		unsigned long int SMTick2_period = SMTick2_calc/GCD;

		//Declare an array of tasks
		static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Keypad
	task1.state = k0;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &keypad_tick;//Function pointer for the tick.

	// Task 2 Tone
	task2.state = no_press;//Task initial state.
	task2.period = SMTick2_period;//Task Period.
	task2.elapsedTime = SMTick2_period;//Task current elapsed time.
	task2.TickFct = &button_press;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator
	
	transmit_data_green(0xFF);
	transmit_data_red(~(0x01));
	
	unsigned check = 0;
	 while(1) {
			// Scheduler code
			for ( i = 0; i < numTasks; i++ ) {
				// Task is ready to tick
				if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
					// Setting next state for task
					tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
					// Reset the elapsed time for next tick.
					tasks[i]->elapsedTime = 0;
				}
				tasks[i]->elapsedTime += GCD;
			}
			while(!TimerFlag);
			TimerFlag = 0;
    }
}