#include "Vt_100.h"




/*************************** Track Layout ******************************/
			/*0         10		  20		30		  40		*/
			/* 1234567990123456789012345678901234567890123456 */
#define ROW1  "                    |:::::::13:::::::|        " //"       0"
#define ROW2  "                   /                  \\      " //"       1"
#define ROW3  "                 :|::::03::::|:::02::::|:     " //"       2"
#define ROW4  "                :      \\        /        :   " //"       3"
#define ROW5  "               :        \\      /          :  " //"       4"
#define ROW6  "              :   :|:::09:::|:::08:::|:    :  " // "      5"						
#define ROW7  "             :   :           /         :    : " // "		 6"
#define ROW8  "            :   :       |::14::|        :    :" //"       7"
#define ROW9  "           04  10                       07   01" //"      8"
#define ROW10 "            :   :            |::15::|   :    :  " //"     9"
#define ROW11 "             :   :                  \\  :    :   " //"    10"
#define ROW12 "              :   :|:::11:::|:::12:::|:	   :    " //"    11"
#define ROW13 "               :              /           :     " //"    12"
#define ROW14 "                :            /           :      " //"    13"
#define ROW15 "                 :|::::05::::|:::06::::|:       " //"    14"
#define ROW16 "                  \\                            " //"    15"
#define ROW17 "                   |::::::16:::::::|            " //"    16"

/******************************************************************************/

/************globals**********************/
/* initialize reverse video structure */
struct video reversevid = {ESC,'[','7','m', NUL};
/* initialize normal video structure */
struct video normalvid = {ESC,'[','0','m', NUL};
/* Define and initialize an escape sequence for cursor position */
//struct CUP cup = {ESC, SQBRAC, ZERO, ZERO, SEMICOL, ZERO, ZERO, CAP_H , NUL};

/* array holds the row and column number for each section  on the display */
struct display sect_disp[] = {{1,1,"  "}, // dummy display structure corresponds to section 0
								{9,46,"01"}, // section 1
								{3,34,"02"}, // section 2
								{3,24,"03"}, // section 3
								{9,12,"04"}, // section 4
								{15,24,"05"},
								{15,34,"06"},
								{9,41,"07"},
								{6,33,"08"},
								{6,24,"09"},
								{9,16,"10"},
								{12,24,"11"},
								{12,33,"12"},
								{1,29,"13"},
								{8,28,"14"},
								{10,33,"15"},
								{17,27,"16"},
							};

/* array holds the rows and column numbers for the switches*/

struct display swi_disp[] = { {1,1,"  "}, // dummy display structure corresponds to SWO
								{2,39,"\\"}, // for switch 1
								{4,33,"/"}, // for switch 2
								{4,24,"\\"}, //for switch 3
								{2,20,"/"}, // for switch 4
								{16,19,"\\"}, //for swicth 5
								{14,30,"/"}, // for switch 6
								{5,25,"\\"}, //for switch 7
								{13,31,"/"}
							};


/**************code for vt100() ******************/
 
/*
* function is called to set the line and column variables of the cup variable
* line and column variables
*/
void vt_100(unsigned int l, unsigned int c)
{
       /*
       - To position the cursor at line 3 and column 75
       - Note that this should be done dynamically for any line/column pair
       */
       cup . l_10 = ZERO + (int) l/DEC; //tens value of line
       cup . l_1  = ZERO + (int) l%DEC; // unit value of line
       cup . c_10 = ZERO + (int) c/DEC; // tens value of col
       cup . c_1  = ZERO + (int) c%DEC; // unit value of col
 
}


/* called to display a line of string dline
 * at line l
 * sets the cursor to position l and calls wrt_str
 */
void disp_line(unsigned int l, char* dline)
{
	/* set column & set row */
	//update_cup(l);
	vt_100(l,NUL);
	write_str((char*) &cup);
	write_str(dline);
}

/* function is called to highlight a section
 * using reverse video on the display
 * the section number of the section to be highlighted
 *
 */
void highlight_sect(enum SECTIONS s)
{
	/* use section number to retrieve row and col for the section
	 *  on the display from the sect_disp[] array
	 */
	vt_100(sect_disp[s].rw, sect_disp[s].cl);
	write_str((char*) &cup);
	write_str((char*)&reversevid);
	write_str(sect_disp[s].str);
}

/* called to print counter reverse video effect on a section */
void unhighlight_sect(enum SECTIONS s)
{
	/* use section number to retrieve row and col for the section
	 *  on the display from the sect_disp[] array
	 */
	vt_100(sect_disp[s].rw, sect_disp[s].cl);
	write_str((char*) &cup);
	write_str((char*)&normalvid);
	write_str(sect_disp[s].str);
}

/* display switches
 * function called to display a switch on the display, prints a
 */
void disp_switches(enum SWITCHES sw, enum SWITCH_STATE st)
{
	/* set cursor position to switch col and row */
	vt_100(swi_disp[sw].rw, swi_disp[sw].cl);
	write_str((char*) &cup);
	if(st == STRAIGHT)
		write_str(BLANK);
	else /* switch is turned */
		write_str(swi_disp[sw].str);

}

/* function to initialize the display at the start of the program
 * prints the track layout to the screen
 */
void track_init(void)
{

	unsigned int i =START_DISP_LINE;

	/**********write rows to screen**********/
	/* initialize it once and change the state of the sections
	 * i.e use a reverse video when the loc is in a section
	 * print a switch when it is thrown
	 * do not show the switch when it is straight
	 */


	disp_line(i++, ROW1);
	disp_line(i++, ROW2);
	disp_line(i++, ROW3);
	disp_line(i++, ROW4);
	disp_line(i++, ROW5);
	disp_line(i++, ROW6);
	disp_line(i++, ROW7);
	disp_line(i++, ROW8);
	disp_line(i++, ROW9);
	disp_line(i++, ROW10);
	disp_line(i++, ROW11);
	disp_line(i++, ROW12);
	disp_line(i++, ROW13);
	disp_line(i++, ROW14);
	disp_line(i++, ROW15);
	disp_line(i++, ROW16);
	disp_line(i++, ROW17);

}

/* testing the display function
 * uses a for loop to highlight and unhighlight sections
 * and display switches in their state, prints the states of the switches
 */
void disp_test(void)
{
unsigned int i =START_DISP_LINE;

track_init();

/* testing the display func */
while(1)
{
	for(i=S1; i<17; i++)
	{
		highlight_sect((enum SECTIONS)i);
		unhighlight_sect((enum SECTIONS)i);
	}

	for(i=SW1; i<9; i++)
	{
		disp_switches((enum SWITCHES)i, STRAIGHT);
		disp_switches((enum SWITCHES)i, THROWN);
	}
}


}
