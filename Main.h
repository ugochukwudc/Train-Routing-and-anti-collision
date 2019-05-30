/*
 * Main.h
 *
 *  Created on: 2014-12-01
 *      Author: Ugochukwu
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <string.h>

#include "Memory_Management.h"  /* stuffs from assignment 1*/
#include "process.h"			/* taken from course website and designed by Dr. Larry Hughes  */
#include "SVC.h"				/* taken from course website and designed by Dr. Larry Hughes (combined within this module itself)*/
#include "systick_and_uart.h"   /* stuffs from assignment 2 */
#include "Vt_100.h"				/* for Terminal output config */
#include "spi.h"
#include "gpiof.h"

/*********** #assign 4 definitions *************/
#define PORT_F_QSIZE   128
#define PORT_F			'F'
#define QEMPTY 			0
#define NUM_MQ         11
#define MSG_BLOCK_SIZE 128
#define MSG_SIZE	   116
#define PROC_BLOCKED   2
#define NEG_ONE		   -1

/* section states */
#define NOT_OCCUPIED 0
#define OCCUPIED 1
#define NO_SWITCH 0
#define NO_LOCO 0
#define NO_WAITING 0

/* switch states */
//#define STRAIGHT 1
//#define THROWN   0
#define NUM_LOCOMOTIVES 2
#define NUM_SWITCHES 9
#define NUM_SECTIONS 16
#define NUM_HS 32

#define CLK  0
#define CCLK 1

/*********** #virtual switch defines *********/
#define DONE  0
#define POFF  2
#define PON   1
#define SET_SW 3
#define TEST_SW 4
#define SEND_INFO 5
#define WAIT_INFO 6
#define WAITING 7
#define SET_DIR 8

/*********** #define files ***********/
#define ERROR_CHAR				'0'
#define NUL 					'\0'
#define NICE_ERROR				'0'
#define NICE_SUC				'1'
#define ASCII_ESC 				0x1b
#define NUM_TO_ASCII			48
#define COLUMN_START			5
#define IDLE_LINE				1
#define IDLE_COLUMN				75
#define NUM_CHAR				10
#define WAIT_COUNTER			800000
#define DESI_INTERRUPT			1600000
#define TRUE 					1
#define SUCCESS					1
#define FALSE 					0
#define FAILURE					0

#define PROCESS_STACK_SIZE 		1024   /* process stack size in decimal bytes */
#define PCB_SIZE 				256	  /* pcb size in decimal bytes */
#define MAX_PRIORITY 			4
#define TOT_PRIORITY 			5

#define UART_INT_TX         	0x020       // Transmit Interrupt Mask
#define UART_INT_RX         	0x010       // Receive Interrupt Mask
#define INT_UART0           	5           // UART0 Rx and Tx interrupt index (decimal)

#define ENABLE_INT() 			__asm("	cpsie	i")
#define DISABLE_INT()  			__asm("	cpsid	i")


/**************STRUCTURES *****************/

struct Message_Entry
{
	int Sender_ID;
	struct Message_Entry * Next;
	int Size;
	char * Msg_Ptr;
};

struct MQ_Entry
{
	int Owner_PID;
	struct Message_Entry * head;
	struct Message_Entry * tail;
	struct pcb * Blocked_PCB;
};

struct section_entry{
	unsigned occupied;
	unsigned occupied_loco;
	unsigned near_switch;
	unsigned waiting_state;
	unsigned waiting_loco;
};

struct switch_entry
{
	unsigned switch_state;
	unsigned thrown_loco;
};

struct next_HS_entry
{
	unsigned first;
	unsigned second;
};

struct master_to_loco
{
	unsigned char hall_num; /* hall sensor number */
};

struct loco_to_loco
{
	unsigned char cmd; /* 0xEE */
	unsigned from_loco; /* sender loco */
	unsigned to_loco; /* receiver loco */
	unsigned section; /* section that is free to use */
};

struct pkt_info
{
	unsigned char start_pkt; /* 0xAA */
	unsigned char cmd; /* 0xA0 */
	unsigned char arg1; /* Hall sensor number written by target Atmel (1 through 32) */
	unsigned char arg2; /* fill byte to keep packet sizes constant 0xFF */
	unsigned char end_pkt; /* 0x55 */
	unsigned char recv_result;

	unsigned char atmel_num;
};

struct hall_poll_fmt
{
	unsigned char start_pkt; /* 0xAA */
	unsigned char cmd; /* 0xA0 */
	unsigned char fill; /* Hall sensor number written by target Atmel (1 through 32) */
	unsigned char hall_num; /* fill byte to keep packet sizes constant 0xFF */
	unsigned char end_pkt; /* 0x55 */
	unsigned char recv_result; /* 0xFF */
};

struct hall_ack_fmt
{
	unsigned char start_pkt; /* 0xAA */
	unsigned char cmd; /* 0xC0 */
	unsigned char hall_num; /* hall sensor number to be acknowledged */
	unsigned char fill; /* fill byte to keep packet sizes constant 0xFF */
	unsigned char end_pkt; /* 0x55 */
	unsigned char recv_result; /* 0xFF */
};

struct sect_ctrl_fmt
{
	unsigned char start_pkt; /* 0xAA */
	unsigned char cmd; /* 0xB0 */
	unsigned char sect_num; /* section number (1 through 16) */
	unsigned char dir_mag;  /*direction and magnitude */
	unsigned char end_pkt; /* 0x55 */
	unsigned char recv_result; /* 0xFF */
};

struct sw_ctrl_fmt
{
	unsigned char start_pkt; /* 0xAA */
	unsigned char cmd; /* 0xD0 */
	unsigned char sw_num; /* switch number (1 through 9) */
	unsigned char state; /* 0 - turn; 1- straight */
	unsigned char end_pkt; /* 0x55 */
	unsigned char recv_result; /* 0xFF */
};

struct dir
{
	unsigned switch_num;
	unsigned switch_action;
	unsigned direction;
	unsigned next;
};


struct locomotive_entry
{
	unsigned current_section;
	//unsigned int dest_section;
	unsigned speed;
	unsigned direction;
	unsigned state;
	unsigned last_HS;
};


struct kcallargs
{
	unsigned int code;
	unsigned int rtnvalue;
	unsigned int arg1;
	unsigned int arg2;
	char * arg3;
	unsigned int arg4;
};



extern unsigned int input_config[];

/****************** assignment 4 functions *****************/
void hall_sensor_queue_reset(void);
void hall_sensor_ack_all(void);
void pon(unsigned int Section, unsigned int direction, unsigned int magnitude);
void poff(unsigned int Section);
void poff_all();
void sw_ctrl(unsigned switch_num, unsigned state);
int get_HS_num(unsigned Atmel_num);

void set_master_to_loco(char * pkt_mem, unsigned char hall_num);
void set_loco_to_loco(char * pkt_mem, unsigned from_loco, unsigned to_loco, unsigned section);
void set_pon_pkt(char * pkt_mem, unsigned section, unsigned direction, unsigned magnitude);
void set_poff_pkt(char * pkt_mem, unsigned int section);
void set_switch_pkt(char * pkt_mem, unsigned switch_num, unsigned state);
void set_hall_poll_pkt(char * pkt_mem, unsigned char atmel_num);
void set_hall_ack_pkt(char * pkt_mem, unsigned char hall_num);

void send_pkt(char * out_mem, char * in_mem);

void int_enqueue(unsigned char atmel_num);

/****************** function definitions ******************/
int reg_proc(void (*func_name)(), unsigned pid, unsigned priority);

unsigned proc_getid();
unsigned proc_nice(unsigned);
void proc_terminate();
void proc_remove();

unsigned output_function (unsigned, unsigned, char);
void putch(unsigned, unsigned, char);
void write_str_temp(char *app_str);

void UART_init();
void SysTick_init();


#endif /* MAIN_H_ */
