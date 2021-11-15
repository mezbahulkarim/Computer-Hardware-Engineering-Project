/*  Function declarations for use in SNAKE project  */

/* Initializing hardware (thats not Display related, see below)
 * and main function with main game loop
 * main.c
 */
void button_init(void);


/* Display related functions
 * display.c
 */
//initialization and data handling
void display_hardware_init(void);
void display_controller_init(void);
void display_putbuffer(int number_bytes, uint8_t* buffer_to_send);
uint8_t spi2putbyte(uint8_t bytetowrite);

//actually doing stuff on the screen
void display_playing_field(void);
void ready_page_for_input(int page);
void display_clear(void);
void put_huntchar(char ch, int page, int col, int font);
void center_string(char* str, int page, int fnt);
void display_gameover(void);
void display_text(char* str1, char* str2, int page1, int page2);
void display_intro();


/* Game logic functions
 * gamelogic.c
 */
void delay2(int a);
void write_snake();
void remove_snake();
void delete_snake();
void page_scroll(int pagenr);
void generate_obstacle();
void generate_food();
void game_over(void);
void show_life();
void remove_block(int blocknr, int hitcol);
void check_hit(int hit);
void show_snake();
void change_lane_down();
void change_lane_up();
void change_page_down();
void change_page_up();
void page_scroll(int pagenr);
void scroll_playingfield(void);
void button_interrupt(void);
extern int score; //used in display_gameover

/*  Helper assembly functions
 *  assembler.S
 */
void delay(int);    //insert milliseconds
void enable_interrupts(void);

/* Graphic data units, these are generally sent into the
 * display buffer
 * graphicsdata.c
 */
extern const uint8_t const  clear[512];
extern uint8_t field_page0[128];
extern uint8_t field_page1[128];
extern uint8_t field_page2[128];
extern uint8_t field_page3[128];
extern uint8_t* field_pages[4];
extern uint8_t block1[4];
extern uint8_t block2[4];
extern uint8_t block3[7];

/* Fonts
 * fonts.h
 */
extern const unsigned char hunter_font1[96][8];
extern const unsigned char hunter_font2[96][8];
