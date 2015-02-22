/*
 * CS120Project.c
 *
 * Created: 5/15/2013 3:01:19 PM
 *  Author: Fernando Gonzalez
 *  Class : CS 120B 
 *  Section: 023
 *  
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char rows = 0x01;
unsigned char columns = 0x01;

unsigned char locationr;
unsigned char locationc;
unsigned char uncovered;

unsigned char sevenSeg;

unsigned char gameMatrix[8][8];   //Will keep records of bomb vs. no bomb.
unsigned char updateMatrix[8][8]; //Will update locations that have been selected.
unsigned char winMatrix[8][8];    //Outputs a smile face when the player wins.
unsigned char MatrixRows[8];      //Has the Combinations for the rows.


void game_setup()
{
	uncovered = 0;
	unsigned char i;
	unsigned char j;
	
	for(i=0; i < 8; i++){
		for(j=0; j < 8; j++){
			gameMatrix[j][i] = 0;
			updateMatrix[j][i] = 0;
		}
	}

	unsigned char p;
	unsigned char bombr = 0;
	unsigned char bombc = 0;
	for(p=0; p < 8; p++){
		gameMatrix[bombc][bombr] = 1;
		bombr++;
		bombc++;
	}
}

/*
void transmit_all_data(unsigned char redData, unsigned char blueData)
{
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x88;
		
		// set SER = next bit of data to be sent.
		PORTC |= ((blueData >> i) & 0x01);
		PORTC |= (((redData >> i) & 0x01)<<4);
		
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x22;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x44;
	
	// clears all lines in preparation of a new transmission
	PORTC &= 0x0F;
	PORTC &= 0xF0;
}
*/

void transmit_data_blue(unsigned char blueData) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((blueData >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

void transmit_data_red(unsigned char redData) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x80;
		// set SER = next bit of data to be sent.
		PORTC |= (((redData >> i) & 0x01)<<4);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x20;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x40;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

void transmit_data_7Seg(unsigned char SegData) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x08;
		// set SER = next bit of data to be sent.
		PORTB |= ((SegData >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}

void transmit_data_columns(unsigned char columnData) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTD = 0x08;
		// set SER = next bit of data to be sent.
		PORTD |= ((columnData >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTD |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTD |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTD &= 0xF0;
}

void adjacentBombs()
{
	unsigned char BombColumns = locationc;
	unsigned char BombRows = locationr;
	
	unsigned char BombCount = 0;
	
	if((BombColumns - 1 > 0) && (gameMatrix[BombColumns - 1][BombRows] == 1)){
		BombCount++;
	}
	if((BombColumns - 1 > 0) && (BombRows + 1 < 7) &&  (gameMatrix[BombColumns - 1][BombRows + 1] == 1)){
		BombCount++;
	}	
	if((BombRows + 1 < 7) && (gameMatrix[BombColumns][BombRows + 1] == 1)){
		BombCount++;
	}	
	if((BombColumns + 1 < 7) && (BombRows + 1 < 7) && (gameMatrix[BombColumns + 1][BombRows + 1] == 1)){
		BombCount++;	
	}
	if((BombColumns + 1 < 7) && (gameMatrix[BombColumns + 1][BombRows] == 1)){
		BombCount++;	
	}
	if((BombColumns + 1 < 7) && (BombRows - 1 > 0) && (gameMatrix[BombColumns + 1][BombRows - 1] == 1)){
		BombCount++;
	}
	if((BombRows - 1 > 0) && (gameMatrix[BombColumns][BombRows - 1] == 1)){
		BombCount++;
	}	
	if((BombColumns - 1 > 0) && (BombRows - 1 > 0) && (gameMatrix[BombColumns - 1][BombRows - 1] == 1)){
		BombCount++;
	}
	
	//Output to 7 Segment display.
	if(BombCount == 0){
		//transmit_data_7Seg(0xE7);
		sevenSeg = 0xE7;
	}
	else if(BombCount == 1){
		transmit_data_7Seg(0x84);
	}
	else if(BombCount == 2){
		transmit_data_7Seg(0xD3);
	}	
	else if(BombCount == 3){
		transmit_data_7Seg(0xD6);
	}
	else if(BombCount == 4){
		transmit_data_7Seg(0xB4);
	}
	else if(BombCount == 5){
		transmit_data_7Seg(0x76);
	}
	else if(BombCount == 6){
		transmit_data_7Seg(0x77);
	}
	else if(BombCount == 7){
		transmit_data_7Seg(0xC4);
	}
	else if(BombCount == 8){
		transmit_data_7Seg(0xF7);
	}
}

void gamelost()
{
	transmit_data_blue(0xFF);
	
	unsigned char lostr = 0x00;
	unsigned char lostc = 0x00;
	
	lostr = SetBit(lostr,locationr, 1);
	lostc = SetBit(lostc, locationc, 1);
	while(1)
	{
		//PORTD = lostc;
		//transmit_all_data(~(lostr), 0xFF);
		transmit_data_red(~(lostr));
	}
}

void gamewon()
{
	unsigned char i;
	unsigned char j;
		
	for(i=0; i < 8; i++){
		for(j=0; j < 8; j++){
			updateMatrix[j][i] = 0;
		}
	}
	updateMatrix[3][2] = 1;
	updateMatrix[3][3] = 1;
	updateMatrix[5][2] = 1;
	updateMatrix[5][3] = 1;
		
	updateMatrix[2][5] = 1;
	updateMatrix[6][5] = 1;
		
	updateMatrix[3][6] = 1;
	updateMatrix[4][6] = 1;
	updateMatrix[5][6] = 1;		
	while (1)
	{
		updateBoard();
	}	
}

void updateBoard()
{
		unsigned char r;
		unsigned char c;
		
		unsigned char MatrixRows[8];
		
		
		for(c = 0; c < 8; c++){
			for(r = 0; r < 8; r++){
				if (updateMatrix[c][r] !=0){
					MatrixRows[c] = SetBit(MatrixRows[c], r, 1);
				}				
				else{
					MatrixRows[c] = SetBit(MatrixRows[c], r, 0);
				}
			}
		}		
		
		unsigned char updateColumns = 0x01;
		PORTD = updateColumns;
		for(c = 0; c < 8; c++){
			//PORTD = 0x00;
			//transmit_all_data(0xFF, ~(MatrixRows[c]));
			transmit_data_blue(~(MatrixRows[c]));
			PORTD = updateColumns;
			updateColumns = updateColumns << 1;
		}	
		PORTD = 0x00;			
}

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
	
	/* Keypad arrangement
			PC4 PC5 PC6 PC7
		col  1   2   3   4
	row
	PC0 1   1 | 2 | 3 | A
	PC1 2   4 | 5 | 6 | B
	PC2 3   7 | 8 | 9 | C
	PC3 4   * | 0 | # | D
	*/
	
	PORTA = 0xEF; // Enable c0ol 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTD to stabilize before checking
	if (GetBit(PINA,0)==0) { return('1'); }
	if (GetBit(PINA,1)==0) { return('4'); }
	if (GetBit(PINA,2)==0) { return('7'); }

	// Check keys in col 2
	PORTA = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTD to stabilize before checking
	if (GetBit(PINA,0)==0) { return('2'); }
	if (GetBit(PINA,1)==0) { return('5'); }
	if (GetBit(PINA,2)==0) { return('8'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTA = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINA,0)==0) { return('3'); }
	if (GetBit(PINA,1)==0) { return('6'); }
	if (GetBit(PINA,2)==0) { return('9'); }

	return('\0'); // default value

}

enum Keypad_States{k0} keypad_state;
	
int keypad_tick(int state){
	keypad_state = state;
	switch(keypad_state){//transitions
		case -1:
			keypad_state = k0;
			break;
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
	
int button_press(int state){
	button_state = state;
	switch(button_state){
		case -1:
			button_state = no_press;
		case no_press:
			if( curr_key == '\0')
			{
				button_state = no_press;
			} 
			else if( curr_key != '\0')
			{
				button_state = press;
				switch(curr_key){
					case '6':
						if(GetBit(rows, 0))
						{}
						else
						{
								if(updateMatrix[locationc][locationr] != 2)
								{
									updateMatrix[locationc][locationr] = 0;
									rows = rows >> 1;
									locationr--;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}									
								}	
								else
								{
									rows = rows >> 1;
									locationr--;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}						
							}
						break;
					case '2':
						if(GetBit(columns,0)){}
						else
						{
								if(updateMatrix[locationc][locationr] != 2)
								{
									updateMatrix[locationc][locationr] = 0;
									columns = columns >> 1;
									locationc--;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}			
								else
								{
									columns = columns >> 1;
									locationc--;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}													
							}
						break;
					case '5':
						if(GetBit(rows,7))
						{}
						else
						{
								if(updateMatrix[locationc][locationr] != 2)
								{
									updateMatrix[locationc][locationr] = 0;
									rows = rows << 1;
									locationr++;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}		
								else
								{
									rows = rows << 1;
									locationr++;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}														
							}
						break;
					case '8':
						if(GetBit(columns, 7))
						{}
						else
						{
								if(updateMatrix[locationc][locationr] != 2)
								{
									updateMatrix[locationc][locationr] = 0;
									columns = columns << 1;
									locationc++;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}					
								else
								{
									columns = columns << 1;
									locationc++;
									if(updateMatrix[locationc][locationr] != 2)
									{
										updateMatrix[locationc][locationr] = 1;
									}
								}			
							}
						break;
					case '7':
						if(gameMatrix[locationc][locationr] == 1)
						{
							gamelost();
						}
						else if (updateMatrix[locationc][locationr] != 2)
						{
							updateMatrix[locationc][locationr] = 2;
							uncovered++;
							adjacentBombs();
						}						
						if(uncovered == 56)
						{
							gamewon();
						}
						break;
					case'1':
						game_setup();
						transmit_data_blue(~(0x01));
						transmit_data_red(0xFF);
						transmit_data_7Seg(0x00);
						rows = 0x01;
						columns = 0x01;
						locationc = 0;
						locationr = 0;
						updateMatrix[0][0] = 1;
						break;
				}
				break;
			}
			case press:
				if(curr_key == '\0')
				{
					button_state = no_press;
				} else if(curr_key != '\0')
				{
					button_state = press;
				}
				break;
	}
	return button_state;
}

enum Update_States{up0} update_state;
	
int update_tick(int state)
{
	update_state = state;
		switch(update_state)
		{ 
			//States
			case -1:
				game_setup();
				transmit_data_blue(~(0x01));
				transmit_data_red(0xFF);
				transmit_data_7Seg(0x00);
				//transmit_all_data(0xFF, ~(0x01));
				locationc = 0;
				locationr = 0;
				updateMatrix[0][0] = 1;
				update_state = up0;
			break;
			case up0:
				update_state = up0;
			break;
		}
		switch(update_state)
		{ //Actions
			case up0:
				updateBoard();
			break;
		}
	return update_state;
}

int main(void)
{
	 DDRA = 0xF0; PORTA = 0x0F;     // Is used for the key pad 
	 DDRB = 0xFF; PORTB = 0X00;     // LED character ???
	 DDRC = 0xFF; PORTC = 0x00;     // Color for the Matrix
	 DDRD = 0xFF; PORTD = 0x00;     // Columns for the Matrix
	 
	 //PORTD = columns;
	
		// Period for the tasks
		unsigned long int SMTick1_calc = 5;
		unsigned long int SMTick2_calc = 100;
		unsigned long int SMTick3_calc = 5;

		//Calculating GCD
		unsigned long int tmpGCD = 1;
		tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
		tmpGCD = findGCD(tmpGCD, SMTick3_calc);

		//Greatest common divisor for all tasks or smallest time unit for tasks.
		unsigned long int GCD = tmpGCD;

		//Recalculate GCD periods for scheduler
		unsigned long int SMTick1_period = SMTick1_calc/GCD;
		unsigned long int SMTick2_period = SMTick2_calc/GCD;
		unsigned long int SMTick3_period = SMTick3_calc/GCD;

		//Declare an array of tasks
		static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Keypad
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &keypad_tick;//Function pointer for the tick.

	// Task 2 Button Press
	task2.state = -1;//Task initial state.
	task2.period = SMTick2_period;//Task Period.
	task2.elapsedTime = SMTick2_period;//Task current elapsed time.
	task2.TickFct = &button_press;//Function pointer for the tick.
	
	// Task 3 Update
	task3.state = -1;//Task initial state.
	task3.period = SMTick3_period;//Task Period.
	task3.elapsedTime = SMTick3_period;//Task current elapsed time.
	task3.TickFct = &update_tick;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	unsigned short i; // Scheduler for-loop iterator
	
	 while(1) {
		 transmit_data_red(0xFF);
		 transmit_data_blue(~(0x01));
		 transmit_data_columns(0x08);
		 /*
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
		*/
    }
}