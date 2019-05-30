/*
 * Stopwatch.c
 *
 * ECED 4402 - Real Time Systems
 * Assignment 2: Stopwatch
 * Vignesh Ramasamy
 * 14th October 2014
 *
 * The following program implements stopwatch module as explained in http://lh.ece.dal.ca/eced4402/a2.pdf
 */

#include "Main.h"

#define DEBUG

// UART0 & PORTA Registers
#define GPIO_PORTA_AFSEL_R	(*((volatile unsigned long *)0x40004420))	// GPIOA Alternate Function Select Register
#define GPIO_PORTA_DEN_R	(*((volatile unsigned long *)0x4000451C))	// GPIOA Digital Enable Register
#define UART0_DR_R      	(*((volatile unsigned long *)0x4000C000))	// UART0 Data Register
#define UART0_FR_R      	(*((volatile unsigned long *)0x4000C018))	// UART0 Flag Register
#define UART0_IBRD_R   		(*((volatile unsigned long *)0x4000C024))	// UART0 Integer Baud-Rate Divisor Register
#define UART0_FBRD_R     	(*((volatile unsigned long *)0x4000C028))	// UART0 Fractional Baud-Rate Divisor Register
#define UART0_LCRH_R    	(*((volatile unsigned long *)0x4000C02C))	// UART0 Line Control Register
#define UART0_CTL_R    		(*((volatile unsigned long *)0x4000C030))	// UART0 Control Register
#define UART0_IFLS_R    	(*((volatile unsigned long *)0x4000C034))	// UART0 Interrupt FIFO Level Select Register
#define UART0_IM_R      	(*((volatile unsigned long *)0x4000C038))	// UART0 Interrupt Mask Register
#define UART0_MIS_R      	(*((volatile unsigned long *)0x4000C040))	// UART0 Masked Interrupt Status Register
#define UART0_ICR_R      	(*((volatile unsigned long *)0x4000C044))	// UART0 Interrupt Clear Register

#define INT_UART0           	5           // UART0 Rx and Tx interrupt index (decimal)
#define UART_FR_TXFF        	0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE        	0x00000010  // UART Receive FIFO Empty
#define UART_RX_FIFO_ONE_EIGHT	0x00000038  // UART Receive FIFO Interrupt Level at >= 1/8
#define UART_TX_FIFO_SVN_EIGHT	0x00000007  // UART Transmit FIFO Interrupt Level at <= 7/8
#define UART_LCRH_WLEN_8    	0x00000060  // 8 bit word length
#define UART_LCRH_FEN       	0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN     	0x00000301  // UART RX/TX Enable
#define UART_INT_TX         	0x020       // Transmit Interrupt Mask
#define UART_INT_RX         	0x010       // Receive Interrupt Mask
#define UART_INT_RT         	0x040       // Receive Timeout Interrupt Mask
#define UART_CTL_EOT        	0x00000010  // UART End of Transmission Enable
#define EN_RX_PA0           	0x00000001  // Enable Receive Function on PA0
#define EN_TX_PA1           	0x00000002  // Enable Transmit Function on PA1
#define EN_DIG_PA0          	0x00000001  // Enable Digital I/O on PA0
#define EN_DIG_PA1          	0x00000002  // Enable Digital I/O on PA1

// Clock Gating Registers
#define SYSCTL_RCGC1_R		(*((volatile unsigned long *)0x400FE104))
#define SYSCTL_RCGC2_R		(*((volatile unsigned long *)0x400FE108))

#define SYSCTL_RCGC1_UART0  	0x00000001  // UART0 Clock Gating Control
#define SYSCTL_RCGC2_GPIOA  	0x00000001  // Port A Clock Gating Control

// Clock Configuration Register
#define SYSCTRL_RCC_R       	(*((volatile unsigned long *)0x400FE060))

#define CLEAR_USRSYSDIV		0xF83FFFFF	// Clear USRSYSDIV Bits
#define SET_BYPASS		0x00000800	// Set BYPASS Bit

#define NVIC_EN0_R		(*((volatile unsigned long *)0xE000E100))	// Interrupt 0-31 Set Enable Register
#define NVIC_EN1_R		(*((volatile unsigned long *)0xE000E104))	// Interrupt 32-54 Set Enable Register

#define TRUE 	1
#define FALSE 	0
#define ON 		1
#define OFF 	0
#define INCORRECT 0
#define CORRECT 1

// SysTick NVIC Registers

#define NVIC_ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))  // SysTick Control and Status Register (STCTRL)
#define NVIC_ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))  // Systick Reload Value Register (STRELOAD)

// SysTick defines

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

#define SYSTICK_CHAR			'S'
#define SYSTICK_DATA			'*'
#define UART_CHAR				'U'
#define INPUT_QSIZE 			1024
#define OUTPUT_QSIZE 			1024
#define MAX_FUNCTIONS 			3
#define NUL 					'\0'
#define QEMPTY 					0
#define QFULL 					1
#define SUCCESS 				2

// ASCII defines

#define ASCII_CARRIAGE_RETURN 	0x0d
#define ASCII_BACK_SPACE 		0x08
#define ASCII_DELETE 			0x7f
#define ASCII_SPACE 			0x20
#define ASCII_LINE_FEED 		0x0a
#define ASCII_COLON 			0x3a
#define ASCII_PERIOD 			0x2e
#define ASCII_ESC 				0x1b
#define ASCII_SEVEN				0x37
#define ASCII_EIGHT				0x38
#define ASCII_ZERO				0x30
#define ASCII_TWO				0x32
#define ASCII_NINE				0x39

// Other defines

#define TIME_LENGTH				4
#define	SET_LENGTH				14
#define GO_LENGTH				2
#define HOUR_LOCATION			4
#define MIN_LOCATION 			7
#define SEC_LOCATION 			10
#define DESI_LOCATION			13
#define	DESI_PER_HOUR			36000
#define DESI_PER_MIN			600
#define DESI_PER_SEC			10
#define NUM_BASE				10
#define START_LOCATION 			0
#define INSTRLEN				64
#define OUTSTRLEN   			64
#define TIME_STR_LEN			11
#define ECHO_STR_LEN			2
#define C_CLOCK 				0
#define S_CLOCK 				1
#define RESET 					0
#define IDLE 					0
#define BUSY 					1
#define START 					0
#define WATCH_CHAR_COUNT 		11
#define TOTAL_TIME_ENTRIES 		2
#define MAX_DESI 				864000
#define DESI_INTERRUPT			1600000

// interrupt ena/dis defines

#define ENABLE_INT() 			__asm("	cpsie	i")
#define DISABLE_INT()  			__asm("	cpsid	i")

// string defines
#define WELCOME_STRING 			"Welcome to the clock application!!!\n\n\r\r\0"
#define HELP_STRING_TITLE 		"Enter any of the following commands:\n\r\r\0"
#define HELP_STRING_TIME 		"TIME - Display the current time-of-day\n\r\r\0"
#define HELP_STRING_SET 		"SET - Set time-of-day in hh:mm:ss.d format\n\r\r\0"
#define HELP_STRING_GO 			"GO - Start the stop watch (press any key to stop)\n\n\r\r\0"
#define ERROR_MESSAGE 			"\n\r\rInvalid command or value\n\r\r\0"
#define NEXT_LINE 				"\n\r\r"

/* Globals */
volatile char Data;  	/* Input data from UART receive */
int stop_watch_status = OFF;
int uart_state=IDLE;    /* Initial UART state is set to IDLE */
unsigned int input_config[NUM_INPUT_INT];

/* queue globals and defines */

int INPUT_QHEAD 		= 1;
int INPUT_QTAIL 		= 0;
int OUTPUT_QHEAD 		= 1;
int OUTPUT_QTAIL 		= 0;

/* save and restore cursors */

char save_cursor[6]		={'\n','\r','\r',ASCII_ESC,ASCII_SEVEN,NUL};
char restore_cursor[3]	={ASCII_ESC,ASCII_EIGHT,NUL};

/* Function initialisations */
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

/* Gloabl structure */

unsigned long desi_counts[TOTAL_TIME_ENTRIES] = {0,0}; /* desi counts for time-of-day and stopwatch times initialized to zero*/

struct input_queue_entry					   /* queue to stack UART receive and SYSTICK interrupts */
{
	char device;
	char data;
};
struct input_queue_entry input_queue_list[INPUT_QSIZE];
struct input_queue_entry input_dequeued_entry;	/* used in main function to access the dequeued entry */


struct output_queue_entry					/* queue to stack the output to the user terminal window */
{
	char output_str[OUTSTRLEN];
	int index;
};
struct output_queue_entry output_queue_list[OUTPUT_QSIZE];

struct fentry
{
	int (* funct) (char *,int);
	char *name;
	int size;
};
struct fentry flist[MAX_FUNCTIONS] = {
		{time, "TIME", 4},
		{set, "SET", 3},
		{go, "GO", 2}
};	/* list of functions to be implemented in this application */

/* systick functions */

void SysTickStart(void)
{
	// Set the clock source to internal and enable the counter
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}

void SysTickStop(void)
{
	// Clear the enable bit to stop the counter
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_ENABLE);
}

void SysTickPeriod(unsigned long Period)
{
	// Must be between 0 and 16777216
	NVIC_ST_RELOAD_R = Period - 1;
}

void SysTickIntEnable(void)
{
	// Set the interrupt bit in STCTRL
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_INTEN;
}

void SysTickIntDisable(void)
{
	// Clear the interrupt bit in STCTRL
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_INTEN);
}

/* UART functions */

void UART0_Init(void)
{
	/* Initialize UART0 */
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; 	/* Enable Clock Gating for UART0 */
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; 	/* Enable Clock Gating for PORTA */



	UART0_CTL_R &= ~UART_CTL_UARTEN;      	/* Disable the UART */

	// Setup the BAUD rate
	UART0_IBRD_R = 8;	/* IBRD = int(16,000,000 / (16 * 115,200)) = int(8.68055) */
	UART0_FBRD_R = 44;	/* FBRD = int(0.68055 * 64 + 0.5) = 44.055 */

	UART0_LCRH_R = (UART_LCRH_WLEN_8);	/* WLEN: 8, no parity, one stop bit, without FIFOs */
#ifdef RECV_TIMER
	UART0_IFLS_R &= ~(UART_RX_FIFO_ONE_EIGHT); 		/* Set to interrupt at >= 1/8th of RX FIFO */
	UART0_IFLS_R &= ~(UART_TX_FIFO_SVN_EIGHT); 		/* Set to interrupt at <= 7/8th of TX FIFO */
#endif
	UART0_CTL_R = UART_CTL_UARTEN;        /* Enable the UART and End of Transmission Interrupts */

	GPIO_PORTA_AFSEL_R |= EN_RX_PA0 | EN_TX_PA1;    	/* Enable Receive and Transmit on PA1-0 */
	GPIO_PORTA_DEN_R |= EN_DIG_PA0 | EN_DIG_PA1;   		/* Enable Digital I/O on PA1-0 */

}

void IntEnable(unsigned long InterruptIndex)
{
	/* Indicate to CPU which device is to interrupt */
	if(InterruptIndex < 32)
		NVIC_EN0_R = 1 << InterruptIndex;		/* Enable the interrupt in the EN0 Register */
	else
		NVIC_EN1_R = 1 << (InterruptIndex - 32);	/* Enable the interrupt in the EN1 Register */
}

void UART0_IntEnable(unsigned long flags)
{
	/* Set specified bits for interrupt */
	UART0_IM_R |= flags;
}

void SetupPIOSC(void)
{
	/* Set BYPASS, clear USRSYSDIV and SYSDIV */
	SYSCTRL_RCC_R = (SYSCTRL_RCC_R & CLEAR_USRSYSDIV) | SET_BYPASS ;	// Sets clock to PIOSC (= 16 MHz)
}

/* Input queue functions */

int input_enqueue(char device, char data)	/* function that fills an entry in input queue */
{
	DISABLE_INT();				/* globally disable interrupts */

	if (INPUT_QHEAD == INPUT_QTAIL)	/* queue is full */
	{
		#ifdef DEBUG	/* if the debug is defined print the information to the admin through console port */
		printf("Input queue is full during enqueue of %c and queue head is %i and queue tail is %i\n",INPUT_QHEAD,INPUT_QTAIL);
		#endif
		ENABLE_INT();
		return QFULL;
	}

	else				/* queue has atleast one free space */
	{
		input_queue_list[INPUT_QHEAD].data = data;		/* store the information in the queue head */
		input_queue_list[INPUT_QHEAD].device = device;
		INPUT_QHEAD = (INPUT_QHEAD + 1) % INPUT_QSIZE;	/* increment the queue head to point to next free entry in the queue */
		ENABLE_INT();
		return SUCCESS;
	}

}

int input_dequeue() 	/* function that removes an entry in input queue */
{
	DISABLE_INT();

	int TP1 = (INPUT_QTAIL+1) % INPUT_QSIZE;
	if (TP1 == INPUT_QHEAD)	/* if the queue is empty */
	{
		ENABLE_INT();
		return QEMPTY;
	}

	else		/* there is atleast one entry in the queue to be dequeued */
	{
		input_dequeued_entry.data = input_queue_list[TP1].data;	//store the dequeued entry in a location
		input_dequeued_entry.device = input_queue_list[TP1].device;
		INPUT_QTAIL = TP1;		/* increment the queue tail to point to next entry in the queue to be dequeued */
		ENABLE_INT();
		return SUCCESS;
	}

}

/* Output queue functions */

void write_str(char *app_str)
{
	DISABLE_INT();

	if (OUTPUT_QHEAD == OUTPUT_QTAIL) /* Queue is full */
	{
	#ifdef DEBUG 	/* if the debug is defined print the information to the admin through console port */
		printf("Output queue is full during enqueue of %s and queue head is %i and queue tail is %i\n",app_str, OUTPUT_QHEAD, OUTPUT_QTAIL);
	#endif
		ENABLE_INT();
		return;
	}

	else /* there is atleast one free space in the queue */
	{
	int i = 0;
	while(app_str[i] != NUL)	/* copy the string to the output queue entry pointed by head until a null char is detected */
	{
		output_queue_list[OUTPUT_QHEAD].output_str[i] = app_str[i];
		i++;
	}

	output_queue_list[OUTPUT_QHEAD].output_str[i] = NUL;	/* terminate the queue string with a null character */
	output_queue_list[OUTPUT_QHEAD].index = 0;	/* set the string pointer to start from the first char of the string */
	OUTPUT_QHEAD = (OUTPUT_QHEAD+1)%OUTPUT_QSIZE;	/* increment the head to point to the next available free space in the queue */

	if(uart_state == IDLE)		/* when the UART is available for transmission */
	{
		OUTPUT_QTAIL = (OUTPUT_QTAIL+1)%OUTPUT_QSIZE; /* increment the tail to point to the next entry to be transmitted */
		UART0_DR_R = output_queue_list[OUTPUT_QTAIL].output_str[output_queue_list[OUTPUT_QTAIL].index];	/* transmit the first char of the string */
		uart_state = BUSY;	/* set the UART state to not available */
	}

	ENABLE_INT();
	return;
	}
}

/* Interrupt handlers */

void UART0_IntHandler(void)
{
	/*
	 * Simplified UART ISR - handles receive and xmit interrupts
	 * Application signalled when data received
	 */
	if (UART0_MIS_R & UART_INT_RX)
	{
		/* RECV done - clear interrupt and make char available to application */
		UART0_ICR_R |= UART_INT_RX;
		Data = UART0_DR_R;
		input_enqueue(UART_CHAR,Data);
	}
#ifdef RECV_TIMER
	if (UART0_MIS_R & UART_INT_RT)
	{
		/* Receive T/O occurred - clear interrupt */
		UART0_ICR_R |= UART_INT_RT;
	}
#endif
	if (UART0_MIS_R & UART_INT_TX)
	{
		/* XMIT done - clear interrupt */
		UART0_ICR_R |= UART_INT_TX;
		output_queue_list[OUTPUT_QTAIL].index++; /* increment the string pointer to point to next char of the string */
		if(output_queue_list[OUTPUT_QTAIL].output_str[output_queue_list[OUTPUT_QTAIL].index] != NUL) /* if index do not points to a null char (last char)*/
			UART0_DR_R = output_queue_list[OUTPUT_QTAIL].output_str[output_queue_list[OUTPUT_QTAIL].index]; //transmit next char

		else if (output_queue_list[OUTPUT_QTAIL].output_str[output_queue_list[OUTPUT_QTAIL].index] == NUL) /* if index points to a null char (last char)*/
		{
			int TP1=(OUTPUT_QTAIL+1)%OUTPUT_QSIZE;
			if (TP1!=OUTPUT_QHEAD)	/* there is atleast one entry in the queue to be transmitted */
			{
				OUTPUT_QTAIL=TP1;	/* set the tail to point to the entry */
				UART0_DR_R = output_queue_list[OUTPUT_QTAIL].output_str[output_queue_list[OUTPUT_QTAIL].index];	//transmit the first char of the string
			}

			else /* there is no entry in the queue to be transmitted */
				uart_state = IDLE;
		}
	}
}

/* Three command functions */

void time(char *input_time, int string_length) /* function called when user entered string "time" */
{
	if(string_length == TIME_LENGTH)	/* check that user entered only string "time" and no extra characters */
	{
	write_str(NEXT_LINE);
	write_str(calc_watch(C_CLOCK));	/* stack the next line, time-of-day and next line in the output queue */
	write_str(NEXT_LINE);
	}
	else
		write_str(ERROR_MESSAGE);	/* if not a valid "time" command then stack an error message */
}

void set(char *input_time, int string_length) /* function called when user entered string "time" */
{
	if(string_length == SET_LENGTH && set_command_error(input_time)==FALSE) /* check if the command is in the correct format */
	{
	write_str(NEXT_LINE); /* move the cursor to the next line */
	unsigned long int temp = atoi(&input_time[HOUR_LOCATION])*DESI_PER_HOUR;
	temp += atoi(&input_time[MIN_LOCATION])*DESI_PER_MIN;
	temp += atoi(&input_time[SEC_LOCATION])*DESI_PER_SEC;
	temp += atoi(&input_time[DESI_LOCATION]);		/* temp has the desi count for the time entered */
	if(temp<MAX_DESI)
		desi_counts[C_CLOCK]=temp;		/* store the count in C_CLOCK if it is less than 864000 */
	else
		write_str(ERROR_MESSAGE);		/* queue error message as the user entered a time greater than 23:59:59.9 */
	}

	else	/* invalid format of "set" command is used */
		write_str(ERROR_MESSAGE);		/* queue the error message for the user */
}

void go(char *input_time, int string_length) /* function called when user entered string "go" */
{
	if(string_length == GO_LENGTH) /* check that user entered only string "go" and no extra characters */
	{
	stop_watch_status=ON;
	write_str(save_cursor);			/* turn ON the stopwatch and save the cursor point to be restored everytime when the stopwatch is displayed */
	}

	else
		write_str(ERROR_MESSAGE); /* if not a valid "go" command then stack an error message */
}

/* other functions */

char* calc_watch(int clock_index)	/* converts the desi count to time in the form of hh:mm:ss.d */
{
	char temp_watch[TIME_STR_LEN];	/* temporary variable to hold the calculated hh:mm:ss.d string */
	unsigned long int temp=desi_counts[clock_index];	/* temporary variable to hold the desi count of the clock */
	int hours=temp/DESI_PER_HOUR;	/* calculate hh */
	int hh0=hours/NUM_BASE;			/* calculate first digit of hh */
	int hh1=hours%NUM_BASE;			/* calculate second digit of hh */
	temp=temp%(hours*DESI_PER_HOUR);	/* subtract the hours from the clock */
	int mins=temp/DESI_PER_MIN;		/* calculate mm */
	int mm0=mins/NUM_BASE;			/* calculate first digit of mm */
	int mm1=mins%NUM_BASE;			/* calculate second digit of mm */
	temp=temp%(mins*DESI_PER_MIN);  /* subtract the minutes from the clock */
	int secs=temp/DESI_PER_SEC;     /* calculate ss */
	int ss0=secs/NUM_BASE;			/* calculate first digit of ss */
	int ss1=secs%NUM_BASE;			/* calculate second digit of ss */
	int desi=temp%NUM_BASE;			/* calculate desi */
	sprintf(temp_watch,"%d%d:%d%d:%d%d.%d\0",hh0,hh1,mm0,mm1,ss0,ss1,desi); /*create a time string in hh:mm:ss.d format */
	return temp_watch;		/* return the time string */
}

/* service routine */

void converttoupper(char *lcstr, char *ucstr)
{
	/*
 - Convert a string with one or more lowercase characters to uppercase
 - Assumes ucstr is as long as lcstr
	 */

	/* Short version */
	while ((*ucstr++ = toupper(*lcstr++)));
}

void increment_desi_count(int clock_index)
{
	desi_counts[clock_index]=(desi_counts[clock_index]+1)%MAX_DESI;		/* increment the clocks desi count by 1 (round off after 864000 - desi per day)*/
}

int set_command_error(char *set_command)			/* check if the set command is of valid format set hh:mm:ss.d */
{
	/* OR connection is used rather than AND connection to avoid overload (OR connection breaks soon after the first state pass but AND connection executes every condition) */
	if(set_command[3] != ASCII_SPACE || set_command[6] != ASCII_COLON ||
	   set_command[9] != ASCII_COLON || set_command[12] != ASCII_PERIOD ||
	   set_command[4]<ASCII_ZERO || set_command[4]>ASCII_TWO ||
	   set_command[5]<ASCII_ZERO || set_command[5]>ASCII_NINE ||
	   set_command[7]<ASCII_ZERO || set_command[7]>ASCII_NINE ||
	   set_command[8]<ASCII_ZERO || set_command[8]>ASCII_NINE ||
	   set_command[10]<ASCII_ZERO || set_command[10]>ASCII_NINE ||
	   set_command[11]<ASCII_ZERO || set_command[11]>ASCII_NINE ||
	   set_command[13]<ASCII_ZERO || set_command[13]>ASCII_NINE )
		return TRUE;	/*if any of the above conditions exist that means the set command format is invalid and return error as TRUE */

	else
		return FALSE;	/*iff any of the above condition do not exist return error as FALSE */
}

void echo (char echo_char)	/* echo the character */
{
	char echo_str[ECHO_STR_LEN];
	echo_str[0]=echo_char;
	echo_str[1]=NUL;
	write_str(echo_str);	/* stack the char in the output queue with a null terminating char */
}

void display_help() /* display the help lines */
{
	write_str(HELP_STRING_TITLE);
	write_str(HELP_STRING_TIME);
	write_str(HELP_STRING_SET);
	write_str(HELP_STRING_GO);
}

void CLI_input(void)
{
	char input_string[INSTRLEN]; 		//user entered string in lower case
	int input_string_location = START;
	int index = 0;
	int User_Enter = FALSE;
	while(User_Enter == FALSE) /* Might also need to check the index < 6 */
	{
		while(input_dequeue() == QEMPTY);

		switch(input_dequeued_entry.data)
		{
		case ASCII_CARRIAGE_RETURN:
			echo(input_dequeued_entry.data);
			input_string[input_string_location] = NUL;
			input_config[index] = atoi(input_string);
			input_string_location = START;
			index++;
			User_Enter = TRUE;
			break;
		case ASCII_SPACE:
			echo(input_dequeued_entry.data);
			input_string[input_string_location] = NUL;
			input_config[index] = atoi(input_string);
			input_string_location = START;
			index++;
			break;
		case ASCII_DELETE:
			echo(input_dequeued_entry.data);
			input_string_location--;
			break;
		default:
			echo(input_dequeued_entry.data);
			input_string[input_string_location] = input_dequeued_entry.data;
			input_string_location = (input_string_location+1)%INSTRLEN;
			break;
		}
	}
}

int check_CLI_input()
{
	if(input_config[0] > NUM_SECTIONS || input_config[1] > MAX_SPEED ||
	   input_config[2] > NUM_SECTIONS || input_config[3] > MAX_SPEED ||
	   input_config[0] < 1 || input_config[1] < 1 || input_config[2] < 1 || input_config[3] <1)
		return INCORRECT;
	else
		return CORRECT;
}

void get_CLI_input(void)
{
	do
	{
		CLI_input();
	}while(check_CLI_input() == INCORRECT);
}
