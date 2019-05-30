/*
 * vt100.h
 */
 
 #ifndef VT_100
 #define VT_100
 
 #include <stdio.h>
 #include <string.h>
 
 
 /************** #defines *************/
 #define ESC 0x1b
 #define NUL 0x00
 #define ZERO 0
 #define NUL 0x00
 #define DEC 10
 #define CAP_H 'H'
 #define SEMICOL ';'
 #define SQBRAC '['
 #define BLANK " " // white space
 #define START_DISP_LINE 1
 
 /*************** Enumerations ***********/
 
/* Train track sections */
enum SECTIONS { S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15, S16}; // S0 is a dummy value
/* Train switches */
enum SWITCHES { SW0, SW1, SW2, SW3, SW4, SW5, SW6, SW7, SW8}; // SW0 is a dummy value
/* Switch states */
enum SWITCH_STATE { THROWN, STRAIGHT};
 
 /************ Structures ************/
  struct video
 {
 	char esc;
 	char sq_brkt;
 	char num;
 	char let_m;
 	char nul;
 };
 
 /* display structure */
struct display
{
	unsigned char rw; // row to display
	unsigned char cl; // column to start display
	char str[3]; // str to display
};

/* cursor position structure */
struct CUP
{
	char esc;
	char sqrbrkt;
	char l_10;
	char l_1;
	char semicol;
	char c_10;
	char c_1;
	char cmdchar;
	char null;
};
 
 extern struct video normalvid;
 extern struct video reversevid;
 extern struct CUP cup;
 

 /*****************functions****************/
 void track_init(); // called to initialize the track layout on the display
 void highlight_sect(enum SECTIONS ); // called to highlight a section
 void unhighlight_sect(enum SECTIONS ); // called to unhighlight a section
 void disp_switches(enum SWITCHES , enum SWITCH_STATE); // called to display switch state


 #endif
