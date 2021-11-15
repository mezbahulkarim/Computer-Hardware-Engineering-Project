//CODED BY AMIRTAHA AMANZADI

#include <stdint.h>
#include <stdlib.h> //needed for rand
#include <pic32mx.h> 
#include "dodger_project.h"
#include <math.h>
//#include <time.h>

/*
currently on level 1(small blocks), for level 2(big block)
two things should be changed:

1.Uncomment the for loop for the big block in remove_block()
and comment out the other loop.
2.Comment out the first two cases of the switch statement in
generate_obstacle() and modify randlane for only one case.
*/

/* Game options. Change to make game more difficult, easier, etc */
#define INITIAL_SPEED 35000				//less = faster speed
#define FOOD_GENERATION_SPEED 300		//less = food generate at faster rate
#define OBSTACLE_GENERATION_SPEED 350	//less = blocks generate at faster rate

/* GLOBALS */
int lifecount = 4;	//3 lives
int score = 0;
int speed = 0;
int pos_x = 0;
int pos_y = 3;
int food;
int snake_page = 2;
int timeoutcount = 0;
int timeforfood = 0;
int timeforblock = 0;
#define snake_length 8



/*custom delay, used for the game speed*/
void delay2(int a){
	
	if(a < 10000)		//speed limit
		 a = 10000;
	while(1){
	timeoutcount++;
	//IFS(0)= IFS(0) & 0xfffffeff;
	
	if(timeoutcount == a)
		break;
	}
	timeoutcount = 0;
}


/*generates random placed blocks*/	
void generate_obstacle(){
	
	if(timeforblock % OBSTACLE_GENERATION_SPEED == 0){
//		srand(time(NULL));
		int random = rand() % 4;
//		int randlane = rand() % 2;			//for small blocks	
		int randlane = rand() % 1;			//for big block only

	
	uint8_t* lane = field_pages[random];
	random = rand() % 125;
	int i, j, k;
	
	switch(randlane){
	
		// case 0:
		// //	if( !(lane[random] & block1[0]) && !(lane[random + 1] & block1[1]) &&			//checking for empty spaces
		// //		!(lane[random + 2] & block1[2]) && !(lane[random + 3] & block1[3]) )		  doesn't work.
				// for(i = 0; i < 4; i++)
					// lane[random + i] |= block1[i];		//or to only put obstacle
			// break;
		
		// case 1:
			// // if( !(lane[random] & block2[0]) && !(lane[random + 1] & block2[1]) &&
				// // !(lane[random + 2] & block2[2]) && !(lane[random + 3] & block2[3]) )
				// for(j = 0; j < 4; j++)
					// lane[random + j] |= block2[j];
			// break;
			
		case 0:
			for(k = 0; k < 8; k++)							//big block
				lane[random + k] |= block3[k];
			break;
		
		default:
			break;
	}
	}
	timeforblock++;
	return;			
}


/*generates randomly placed food*/
void generate_food(){
	if (timeforfood % FOOD_GENERATION_SPEED == 0){
//	srand(time(NULL));
	int random = rand() % 4;
	int randlane = rand() % 4;
	
	uint8_t* lane = field_pages[random];
	random = rand() % 127;
	
	switch(randlane){
	
		case 0:
			food = 3;
			break;
		
		case 1:
			food = 12;
			break;
		
		case 2:
			food = 48; 
			break;
		
		case 3:
			food = 192;
			break;
		
		default:
			break;
	}
		if(!(lane[random] & food) && !(lane[random + 1] & food)){		//check for empty space
			lane[random] |= food;				//writes food to display
			lane[random + 1] |= food;
		}	
	}
	timeforfood++;
	return;
}

/*shows how many lives you have left on the LEDs*/
void show_life(){
	
	switch(lifecount){
		case 3:
			PORTE = 3;
			break;
		case 2:
			PORTE = 1;
			break;
		case 1:
			PORTE = 0;
			break;
		default:
			break;
	}
}

/*removes the block after a hit*/
void remove_block(int blocknr, int hitcol){
	uint8_t* lane = field_pages[snake_page];
	int i, j;
	
	for(j = 0; j < 7; j++){				//length of big block = 7
		
		if(blocknr == 0x99)
			lane[hitcol - 6 + j] = 0;
		else
			lane[pos_x + j] = 0;
	}
	
	// for(i = 0; i < 4; i++){			//length of the small blocks = 4
		
		// if(blocknr == 0x39 || blocknr == 0x9c)	//hit from below or above in line
			// lane[hitcol - 3 + i] &= pos_y;
		
		// if(blocknr == 0x90)						//hit from below in page
			// lane[hitcol - 3 + i] &= 0x0f;
		// if(blocknr == 0x09)
			// lane[hitcol - 3 + i] &= 0xf0;		//hit from above in page
		
		// else									//hit from the head
			// lane[pos_x + i] = 0;
	// }
}

/*checks hit from below and above*/
void check_hit(int hit){			
	uint8_t* lane = field_pages[snake_page];
	int i;
	int hitcolumn = pos_x - snake_length - 1;
	
	for(i = 0; i < snake_length; i++){
		hitcolumn++;
		//if(lane[pos_x - snake_length + i] & hit == hit){
		if(lane[pos_x - snake_length + i] == hit){
			remove_block(hit, hitcolumn);
			lifecount--;
			show_life();
			delay(150);
 			if(!lifecount)
				game_over();
			}	
	}
}

/*displays the snake in the beginning and for every page change*/
void show_snake(){
	uint8_t* lane = field_pages[snake_page];
	int i, j;
	
	if(pos_x == 0)
		for(i = 1; i <= snake_length; i++){
			lane[i] = pos_y;
			pos_x++;
	}
	else
		for(j = 0; j < snake_length; j++)
			lane[pos_x - snake_length + j] |= pos_y;
}

/*deletes each column of the snake without changing other bits*/
void delete_snake(){
	uint8_t* lane = field_pages[snake_page];
	int i;
	for(i = 0; i < snake_length; i++)
		lane[pos_x - snake_length + i] &= ~pos_y;
}

/*writes the snake to the next column*/
void write_snake() {

	uint8_t* lane = field_pages[snake_page];
	int i;

	if(pos_x == 128){
		delete_snake();
		pos_x = 0;
	}
	uint8_t temp = lane[pos_x] & pos_y;			//check for game over
	if(temp == 1 || temp == 8 ||				//hit straight, 4 different possibilities
		temp == 16 || temp == 128){
		lifecount--;
		show_life();
		delay(250);
		remove_block(0, 0);
		if(!lifecount)
			game_over();
	}
		  
	if(temp == 3 || temp == 12 ||				//check for eating food
		temp == 48 || temp == 192){
		score += 1000;
	}
	
	lane[pos_x] |= pos_y;				//writes the snake to the next column and keeps the other bits unchanged
	pos_x++;
	
	score++;							//keeps the score
	delay2(INITIAL_SPEED + speed);		//controls the game speed
	
}

/*remove the last column of the snake*/
void remove_snake() {
	uint8_t* lane = field_pages[snake_page];
	lane[pos_x - snake_length] &= ~pos_y;
}



void change_page_up(){
	snake_page--;
	if(snake_page == -1)
		snake_page = 3;
	uint8_t* lane = field_pages[snake_page];
	pos_y = 192;
}


void change_page_down(){
	snake_page++;
	if(snake_page == 4)
		snake_page = 0;
	uint8_t* lane = field_pages[snake_page];
	pos_y = 3;
}

/*snake goes two lines down*/
void change_lane_down(){
	uint8_t* lane = field_pages[snake_page];
	int i;

	if(pos_y == 192){				//check for change page
		delete_snake();				//deletes snake in the previous page
		change_page_down();
		check_hit(0x09);			//for small blocks
		check_hit(0x99);			//for big block
		show_snake();
	}
	else{
		check_hit(0x9c);
		delete_snake();
		pos_y *= 4;
		for(i = 0; i < snake_length; i++)
			lane[pos_x - snake_length + i] |= pos_y;		//writes the snake to the new line
	}
}

/*snake goes two lines up*/
void change_lane_up(){
	uint8_t* lane = field_pages[snake_page];
	int i;
	
	if(pos_y == 3){					//check for change page
		delete_snake();				//deletes snake in the previous page	
		change_page_up();
		check_hit(0x90);
		check_hit(0x99);			
		show_snake();
	}
	else{
		check_hit(0x39);
		delete_snake();
		pos_y /= 4;
		for(i = 0; i < snake_length; i++)
			lane[pos_x - snake_length + i] |= pos_y;
	}
}


void button_interrupt(void) {
	
	volatile int p = (PORTD >> 5) & 0x7;
	switch(p){
	case 4:			//button 4 -> down
		change_lane_down();
		break;
	case 2:			//button 3 -> increase speed
		speed -= 5000;
		break;
	case 1:			//button 2 -> up
		change_lane_up();
		break;
	default:
		break;
	}
 
    IFSCLR(1) = 0x1; //clear flag
    return;
}


void game_over() {
    
    CNCONCLR = 0x8000;			//disable buttons, otherwise you can write snake in weird places after gameover
    TRISDCLR = 0xf0;
	display_gameover();

    while(1); 
}


void scroll_playingfield() {
    page_scroll(0);
    page_scroll(1);
    page_scroll(2);
    page_scroll(3);
}

/*scrolls everything in a page one column to the left*/
void page_scroll(int pagenr) {
    uint8_t pagecopy[128]; 
    uint8_t* realpage = field_pages[pagenr];

    int cnt;
    int i = 0;
    for (cnt = 1; cnt < 128; cnt++) {
        pagecopy[i] = realpage[cnt];
        i++;
    }
    pagecopy[i] = 0;    //add new empty column at far right

    //copy new page
    for (cnt = 0; cnt < 128; cnt++) {
        realpage[cnt] = pagecopy[cnt];
    }
}