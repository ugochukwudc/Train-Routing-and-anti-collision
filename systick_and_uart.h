/*
 * interrupt_routines.h
 *
 *  Created on: 2014-10-27
 *      Author: Vignesh
 */

#ifndef INTERRUPT_ROUTINES_H_
#define INTERRUPT_ROUTINES_H_

/* Function initialisations */
#define NUM_INPUT_INT 4
#define NUM_SECTIONS 16
#define MAX_SPEED 7


void UART0_Init(void);
void IntEnable(unsigned long InterruptIndex);
void UART0_IntEnable(unsigned long flags);
void UART0_IntHandler(void);
void SetupPIOSC(void);

void SysTickIntDisable(void);
void SysTickHandler(void);
void SysTickIntDisable(void);
void SysTickIntEnable(void);
void SysTickPeriod(unsigned long Period);
void SysTickStop(void);
void SysTickStart(void);

int input_enqueue(char device, char data);
int input_dequeue();
void write_str (char *);


void time(char *input_time,int string_length);
void set(char *input_time,int string_length);
void go(char *input_time,int string_length);

void display_help(void);
void converttoupper(char *lcstr, char *ucstr);
char* calc_watch(int clock_index);
void increment_desi_count(int clock_index);
void echo (char echo_char);

void CLI_input(void);
int check_CLI_input(void);
void get_CLI_input(void);

#endif /* INTERRUPT_ROUTINES_H_ */
