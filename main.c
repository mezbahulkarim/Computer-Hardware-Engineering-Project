//CODED BY AMIRTAHA AMANZADI AND MEZBAHUL KARIM

#include <pic32mx.h>
#include <stdint.h>
#include "dodger_project.h"

int main(void) { 

    /*Initialize display*/
    display_hardware_init();
	display_controller_init();
	
	display_intro();
	
	button_init();
	
	show_snake();
	
	display_playing_field();
	delay(200);
		
		while(1){
			
			display_playing_field();
			remove_snake();
			generate_obstacle();
			generate_food();
			write_snake();
		
		}
    return 0;
}

/* Set ports and stuff to get the buttons working with interrupts
 * Comments are straight out of the manual.
 */
 void button_init(void) {
		TRISE &= 0xff00;	//for LEDs
		PORTE = 7;			//initial value for LEDs
    /* Change notice (CN) pins provide PIC32 abivlity to generate interrupts to processor in response
     * to the change of state on selected input pins (corresopnding TRISxbits must be 1).
     * Enabled pin values are compared with values sampled during the last read operation of the designated PORTx reg.
     * If pin value is different from last value read, mismatch condition is generated. The enabled pins all generate
     * the same interrupt signal (ORed together). Enabled pins are "sampled" on every internal system clock cycle, SYSCLK.
     * - Section 12 i/o. 12.3.3
     *   1. Disable CPU itnerrupts
     *   2. Set desired CN I/O pin as an input by setting TRISx corresponding to 1
*/    TRISDSET = 0xf0;  /*
     *   3. Enable CN module by setting the ON bit (CNCONx<15>) = 1
     */ CNCONSET = 0x8000; /*
     *   4. Enable individual CN input pins, enable optional pull-ups or pull-downs, dvs CN14, Cn15, CN16 (se chipkit data sheet för 34,35,36pin)
     */ CNENSET = 0x1C000; /*
     *   5. Read the corresponding PORTx registers to clear the CN interrupt
     *   6. Configure the CN interrupt priority bits, CNIP<2:0> (IPC6<20:18>) and subpriority bits CNIS<1:0> (IPC6<17:16>
     */ IPC(6) |= 0x1F0000; /*
     *   7. Clear the CN interrupt flag bit by setting CNIF bit (IFS1<0> = 0
     */ IFSCLR(1) = 0x1; /*
     *   8. Configure CN pin interrupt for a specific edge detect using EDGEDETECT bit in CNCONx reg, and set up. We dont EDGEDETECT? So skip, or set 0. CNCONbit11
     *   edge control using the CNENx/CNNEx bits (?)
     *   9. Enable CN interrupt enable bit, by setting CNIE bit (IEC1<0>) = 1
     */ IECSET(1) = 0x1; /*
     *   10. Enable CPU interrupts (noteself: use assembly stuff from lab3)
     */ enable_interrupts();
}

