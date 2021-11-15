#include <stdint.h>
#include <pic32mx.h>        //mcb32 environment
#include "dodger_project.h"

/*hardware initialization of the display
(copied from Basic I/O Shield reference manual)*/
void display_hardware_init (void) {

    SPI2CON = 0;            
    SPI2BRG = 4;             

    SPI2STATCLR = 0x40;     
    SPI2CONSET = 0x40;      
    SPI2CONSET = 0x20;		//set bit 5 to 1, means master mode
    SPI2CONSET = 0x8000;   	//set bit 15 to 1, which turns SPI2 ON

//    ODCF = 0x0;
//    ODCG = 0x0;

    TRISFCLR = 0x70;        //set bits 4-5-6 to 0 to 0
    TRISGCLR = 0x200;       
}

/*controller initialization of the display*/
void display_controller_init (void) {
    
    PORTFCLR = 0x10;
    PORTFCLR = 0x40;             
                            
//	delay(1000);
                            
    spi2putbyte(0xAE);

    
    PORTGCLR = 0x200;       //rd4 low
    delay(10);
    PORTGSET = 0x200;       //rd4 high
    delay(10);
   
    spi2putbyte(0x8D);
    spi2putbyte(0x14);
    spi2putbyte(0xD9);
    spi2putbyte(0xF1);

    PORTFCLR = 0x20;        //clear bit 5
//    delay(1000);          //wait 1000ms

    spi2putbyte(0xA1);      //remap columns
    spi2putbyte(0xC8);      //remap rows

    spi2putbyte(0xDA);
    spi2putbyte(0x20);

    spi2putbyte(0xAF);		// send display on command
}


void display_playing_field(void) {
	ready_page_for_input(0);
        display_putbuffer(128, field_page0);
	ready_page_for_input(1);
        display_putbuffer(128, field_page1);
	ready_page_for_input(2);
        display_putbuffer(128, field_page2);
	ready_page_for_input(3);
        display_putbuffer(128, field_page3);
}

/* Helper function for display_playing_field (and others?)
 */
void ready_page_for_input(int page) {
	PORTFCLR = 0x10;
	spi2putbyte(0x22);  //setpage
	spi2putbyte(page);
	spi2putbyte(0x00);  //set lowest nib
	spi2putbyte(0x10);  //set highest nib
	PORTFSET = 0x10;    //data mode
}

void center_string(char* str, int page,int fnt) {
    char* pnt = str;
    int col;
    int strlen = 0;
    
    //count str length
    while (*pnt != '\0') {
        strlen++;
        pnt++;
    }

    //print str to screen (note; needs a screen update to be shown aswell)
    col = (128 - strlen*6) / 2 - 12;
    pnt = str;
    while (*pnt != '\0') {
        put_huntchar(*pnt, page, col, fnt);
        col += 10;
        pnt++;
    }
}

/* Puts a character with font HUNTER somewhere
 */
void put_huntchar(char ch, int page, int col, int font) {

    uint8_t* lane = field_pages[page];
    int huntnum = ((int) ch) - 32; //finds the right character in the font array
    int i;
    int k = 0;
    for (i = col; i < (col + 8); i++) {
		if(font == 1)
			lane[i] = hunter_font1[huntnum][k++];
		if(font == 2)
			lane[i] = hunter_font2[huntnum][k++];

    }
}

void display_text(char* str1, char* str2, int page1, int page2) {

    center_string(str1, page1, 1);
    center_string(str2, page2, 1);
    display_playing_field();
    delay(400);
    
    int i;
    for (i = 0; i < 130; i++) {
        scroll_playingfield();
        display_playing_field();
        delay(3);
    }
    display_clear();
}

void display_intro(){
	display_text("WELCOME TO", "DODGER!", 0, 2);
	display_text("DODGE MINES", "EAT BLOCKS", 0, 2);
	display_text("BTN2 UP", "BTN4 DOWN", 0, 2);
	display_text("BTN3", "SPEED UP", 0, 2);
	display_text("YOU HAVE", "3 LIVES", 0, 2);
	display_text("GOOD LUCK", "GAMER!", 0, 2);
}

/* Write data to display panel, "put buffer" */
void display_putbuffer(int number_bytes, uint8_t* buffer_to_send) {
    int i;
    for (i = 0; i < number_bytes; i++) {
        spi2putbyte(*buffer_to_send);       //send first byte in buffer, this function also waits to receive next byte
        buffer_to_send++;                   //increment pointer
    }
}


uint8_t spi2putbyte (uint8_t bytetowrite) {
    uint8_t bytetoread;
    /*Wait for transmitter to be ready*/
    while (!(SPI2STAT & 0x08));
    /*Write byte to buffer*/
    SPI2BUF = bytetowrite;

    /*Wait for receive byte*/
    while (!(SPI2STAT & 1));
    /*Read byte and return it - unnecessery for display which is readonly??*/
    bytetoread = SPI2BUF;
    return bytetoread;
}

void display_gameover(void) {
    int ipage;
    int icolumn;
    char* go_str = "GAME OVER!";
    char* score_str = "SCORE:";
    //count number of digits in score
    int scorecount = score / 10;
    int nrdigs = 0;
    while(scorecount) {
        scorecount /= 10;
        nrdigs++;
   }
    char* scr_str[nrdigs+1]; //dont forget \0
    sprintf(scr_str, "%d", score / 10);

    
    //scroll and clear playing field
    int i;
    for (i = 0; i < 128; i++) {
        field_page0[127] = 0;
        field_page1[127] = 0;
        field_page2[127] = 0;
        field_page3[127] = 0;
        scroll_playingfield();
        display_playing_field();
        delay(5);
    }

    display_clear();
    display_playing_field();
    delay(300);

    center_string(go_str, 0, 2);
    center_string(score_str, 2, 2);
    center_string(scr_str,3 ,2);
    display_playing_field();
    return;
}

void display_clear(void) {
    int ipage;
    int icolumn;
	

    for (ipage = 0; ipage < 4; ipage++) {
        PORTFCLR = 0x10;
        spi2putbyte(0x22);      //setpage
        spi2putbyte(ipage);
        spi2putbyte(0x0);       //set lowest nib
        spi2putbyte(0x10);      //set highest nib
        PORTFSET = 0x10;        //data mode
    	
        display_putbuffer(128, clear); //send one page of 0s to buffer
    }
} 