/*
 * Kernel_and_Process.c
 *
 * ECED 4402 - Real Time Systems
 * Assignment 3: Kernel Design and Priority Processes
 * 06th November 2014
 *
 * The following program implements kernel design and priority processes as explained in http://lh.ece.dal.ca/eced4402/a2.pdf
 * Many of the codes were taken from the course website written by Dr.Larry Hughes
 */

#define Assignment_4
/*********** #include files ***********/
#include "Main.h"
/****************** global variables ********************/
unsigned first_process;
int CUR_PRI;
volatile int count = 0;
enum kernelcallcodes {GETID, NICE, TERMINATE, PBIND, PSEND, PRECV};

int INT_QHEAD = 1;
int INT_QTAIL = 0;

unsigned char portf_int_queue[PORT_F_QSIZE];

int CC_HS_0[] = {DONE}; /* dummy one */

/* used in this software */
int CC_HS_1[] = {POFF,6, DONE};
//int CC_HS_2[] = {PON, 2, DONE}; /* should be changed */
int CC_HS_2[] = {PON, 2, TEST_SW, 1, 8, DONE, PON, 13};
int CC_HS_3[] = {POFF,1, DONE};
int CC_HS_4[] = {PON, 3, DONE};
int CC_HS_5[] = {POFF,2, DONE};
int CC_HS_6[] = {PON, 4, DONE};
int CC_HS_7[] = {POFF,3, DONE};
int CC_HS_8[] = {TEST_SW, 5, 16, PON, 5, DONE, PON, 16, DONE}; /* should be changed */
int CC_HS_9[] = {POFF,4, TEST_SW, 6, 8, DONE, PON, 12}; /* should be changed */
int CC_HS_10[]= {PON, 6, DONE};
int CC_HS_11[]= {POFF,5, DONE};
int CC_HS_12[]= {PON, 1, DONE};

int CC_HS_25[] = {POFF, 2, SET_SW, 1, STRAIGHT, DONE};
int CC_HS_26[] = {POFF,13, SEND_INFO, 1, WAITING, 1, WAIT_INFO, 1, SET_SW, 1, THROWN, SET_DIR, CLK, PON, 13, DONE};

int CC_HS_31[] = {POFF, 4, SET_SW, 5, STRAIGHT, DONE};
int CC_HS_32[] = {POFF,16, SEND_INFO, 5, WAITING, 4, WAIT_INFO, 1, SET_SW, 5, THROWN, SET_DIR, CLK, PON, 16, DONE};

int CC_HS_24[] = {POFF, 5, SET_SW, 6, STRAIGHT, DONE, POFF,12, SEND_INFO, 6, WAITING, 5, WAIT_INFO, 6, SET_SW, 6, THROWN, SET_DIR, CLK, PON, 12, PON, 5, DONE};

int C_HS_26[] = {DONE};
int C_HS_25[] = {PON, 1};
int C_HS_2[] = {SET_SW, 1, STRAIGHT, SET_DIR, CCLK, PON, 1, PON, 2};

int C_HS_32[] = {DONE};
int C_HS_31[] = {PON, 4};
int C_HS_8[]  = {SET_SW, 5, STRAIGHT, SET_DIR, CCLK, PON, 4, PON, 5};

int C_HS_24[] = {DONE};
int C_HS_9[] = {SET_SW, 6, STRAIGHT, SET_DIR, CCLK, PON, 5};

/* not used in this software */
int CC_HS_13[] = {DONE};
int CC_HS_14[] = {DONE};
int CC_HS_15[] = {DONE};
int CC_HS_16[] = {DONE};
int CC_HS_17[] = {DONE};
int CC_HS_18[] = {DONE};
int CC_HS_19[] = {DONE};
int CC_HS_20[] = {DONE};
int CC_HS_21[] = {DONE};
int CC_HS_22[] = {DONE};
int CC_HS_23[] = {DONE};
int CC_HS_29[] = {DONE};
int CC_HS_30[] = {DONE};
int CC_HS_27[] = {DONE};
int CC_HS_28[] = {DONE};

int C_HS_0[] = {DONE};
int C_HS_1[] = {DONE};
int C_HS_3[] = {DONE};
int C_HS_4[] = {DONE};
int C_HS_5[] = {DONE};
int C_HS_6[] = {DONE};
int C_HS_7[] = {DONE};
int C_HS_10[] = {DONE};
int C_HS_11[] = {DONE};
int C_HS_12[] = {DONE};
int C_HS_13[] = {DONE};
int C_HS_14[] = {DONE};
int C_HS_15[] = {DONE};
int C_HS_16[] = {DONE};
int C_HS_17[] = {DONE};
int C_HS_18[] = {DONE};
int C_HS_19[] = {DONE};
int C_HS_20[] = {DONE};
int C_HS_21[] = {DONE};
int C_HS_22[] = {DONE};
int C_HS_23[] = {DONE};
int C_HS_29[] = {DONE};
int C_HS_30[] = {DONE};
int C_HS_27[] = {DONE};
int C_HS_28[] = {DONE};

int *HALLS[NUM_HS+1][2] = { {C_HS_0,CC_HS_0},
							{C_HS_1,CC_HS_1},
							{C_HS_2,CC_HS_2},
							{C_HS_3,CC_HS_3},
							{C_HS_4,CC_HS_4},
							{C_HS_5,CC_HS_5},
							{C_HS_6,CC_HS_6},
							{C_HS_7,CC_HS_7},
							{C_HS_8,CC_HS_8},
							{C_HS_9,CC_HS_9},
							{C_HS_10,CC_HS_10},
							{C_HS_11,CC_HS_11},
							{C_HS_12,CC_HS_12},
							{C_HS_13,CC_HS_13},
							{C_HS_14,CC_HS_14},
							{C_HS_15,CC_HS_15},
							{C_HS_16,CC_HS_16},
							{C_HS_17,CC_HS_17},
							{C_HS_18,CC_HS_18},
							{C_HS_19,CC_HS_19},
							{C_HS_20,CC_HS_20},
							{C_HS_21,CC_HS_21},
							{C_HS_22,CC_HS_22},
							{C_HS_23,CC_HS_23},
							{C_HS_24,CC_HS_24},
							{C_HS_25,CC_HS_25},
							{C_HS_26,CC_HS_26},
							{C_HS_27,CC_HS_27},
							{C_HS_28,CC_HS_28},
							{C_HS_29,CC_HS_29},
							{C_HS_30,CC_HS_30},
							{C_HS_31,CC_HS_31},
							{C_HS_32,CC_HS_32},
						 };

/************** #assign 4 global structures ************/

struct MQ_Entry MQ[NUM_MQ];

struct locomotive_entry loco[NUM_LOCOMOTIVES+1];

struct switch_entry switches[NUM_SWITCHES+1] = {{STRAIGHT,NO_LOCO}, /* dummy one */
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO},
		{STRAIGHT,NO_LOCO}
};

struct section_entry section[NUM_SECTIONS+1] = {{NOT_OCCUPIED, NO_LOCO, NO_SWITCH, NO_WAITING, NO_LOCO}, /* dummy one */
		{NOT_OCCUPIED, NO_LOCO, 1, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 5, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 5, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 5, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 6, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 1, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 7, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 7, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 7, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 7, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 7, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 7, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 4, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 2, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 9, NO_WAITING, NO_LOCO},
		{NOT_OCCUPIED, NO_LOCO, 5, NO_WAITING, NO_LOCO},
};


struct next_HS_entry next_HS[NUM_HS+1][2] = {{0, 0, 0,0}, /* dummy one */
		{12,12, 2,2},  /* HS_1 */
		{1,1, 3,25},  /* HS_2 */
		{2,2, 4, 27},  /* HS_3 */
		{3,3, 5, 5},  /* HS_4 */
		{4,4, 6, 6},  /* HS_5 */
		{5, 17,7, 7},  /* HS_6 */
		{6, 26,8, 8},  /* HS_7 */
		{7,7, 9,31},  /* HS_8 */
		{8,8, 10,24},  /* HS_9 */
		{9,9, 11,11},  /* HS_10 */
		{10,10, 12,12},  /* HS_11 */
		{11,11, 1,1},  /* HS_12 */
		{24, 29, 14,14},  /* HS_13 */
		{13, 13, 15,15},  /* HS_14 */
		{14, 14, 16,16},  /* HS_15 */
		{15, 15, 17,17},  /* HS_16 */
		{16, 16, 18, 6},  /* HS_17 */
		{17, 17, 19,19},  /* HS_18 */
		{18, 18, 20,20},  /* HS_19 */
		{19, 19, 21,21},  /* HS_20 */
		{20, 20, 22,22},  /* HS_21 */
		{21, 21, 23,23},  /* HS_22 */
		{22, 22, 24,24},  /* HS_23 */
		{23, 9, 13,13},  /* HS_24 */
		{2,2, 26,26},  /* HS_25 */
		{25, 25, 7 ,7 },  /* HS_26 */
		{3, 3, 28,28},  /* HS_27 */
		{27, 27,  0,0}, /* HS_28 */
		{30,30, 13,13},  /* HS_29 */
		{0,0, 29,29}, /* HS_30 */
		{8,8, 32,32},  /* HS_31 */
		{31,31, 0,0},  /* HS_32 */
};

/****************** global structures ******************/
struct pcb *running[TOT_PRIORITY];
struct pcb *prev_pcb;


struct CUP cup = {ASCII_ESC,'[','0','0',';','0','0','H',NUL};


/***************** UART initiate function ****************/
void UART_init()
{
	SetupPIOSC(); 									// Set Clock Frequency to 16MHz (PIOSC)
	UART0_Init();  									// Initialize UART0
	IntEnable(INT_UART0);							// Enable UART0 interrupts
	UART0_IntEnable(UART_INT_RX | UART_INT_TX);		// Enable Receive and Transmit interrupts
}


/***************** Systick initiate function *************/
void SysTick_init()
{
	SysTickPeriod(DESI_INTERRUPT);  // interrupt every 10th of a second */
	SysTickIntEnable();
	SysTickStart();
}

/*********** getid routine *************/
unsigned proc_getid()
{
	volatile struct kcallargs args;
	args.code = GETID;
	assignR7((volatile unsigned long)&args);
	SVC();
	return args.rtnvalue;
}

/*********** nice routine ******************/
unsigned proc_nice(unsigned new_priority)
{
	volatile struct kcallargs args;
	args.code = NICE;
	args.arg1 = new_priority;
	assignR7((volatile unsigned long)&args);
	SVC();
	return args.rtnvalue;
}

/*********** terminate routine *************/
void proc_terminate()
{
	volatile struct kcallargs args;
	args.code = TERMINATE;
	assignR7((volatile unsigned long)&args);
	SVC();
}

/************ bind routine ****************/
int proc_bind(unsigned p_mq_num)
{
	volatile struct kcallargs args;
	args.code = PBIND;
	args.arg1 = p_mq_num;
	assignR7((volatile unsigned long)&args);
	SVC();
	return args.rtnvalue;
}

/*********** send routine ******************/
int proc_send(unsigned p_smq, unsigned p_dmq, char * p_msg_ptr, unsigned p_msg_size)
{
	volatile struct kcallargs args;
	args.code = PSEND;
	args.arg1 = p_smq;
	args.arg2 = p_dmq;
	args.arg3 = p_msg_ptr;
	args.arg4 = p_msg_size;
	assignR7((volatile unsigned long)&args);
	SVC();
	return args.rtnvalue;
}

/********** recv routine *******************/
int proc_recv(unsigned p_smq, unsigned p_dmq, char * p_msg_ptr, unsigned p_msg_size)
{
	volatile struct kcallargs args;
	args.code = PRECV;
	args.arg1 = p_smq;
	args.arg2 = p_dmq;
	args.arg3 = p_msg_ptr;
	args.arg4 = p_msg_size;
	assignR7((volatile unsigned long)&args);
	SVC();
	return args.rtnvalue;
}
/*********** process routines *************/

void start_loco()
{
	proc_bind(10);
	char * out_pkt_mem = (char*) allocate(128);
	set_pon_pkt(out_pkt_mem,loco[1].current_section,loco[1].direction,loco[1].speed);
	proc_send(10,5,out_pkt_mem,7);
	//pon(loco[1].current_section,loco[1].direction,loco[1].speed);
	section[loco[1].current_section].occupied = OCCUPIED;
	section[loco[1].current_section].occupied_loco = 1;

	/*pon(loco[2].current_section,loco[2].direction,loco[2].speed);
	section[loco[2].current_section].occupied = OCCUPIED;
	section[loco[2].current_section].occupied_loco = 2;*/

	/*
	set_pon_pkt(out_pkt_mem,sect,loco[2].direction,loco[2].speed);
	proc_send(10,5,out_pkt_mem,7);
	//pon(loco[1].current_section,loco[1].direction,loco[1].speed);
	section[loco[1].current_section].occupied = OCCUPIED;
	section[loco[1].current_section].occupied_loco = 2;
	*/

	deallocate(out_pkt_mem);
}

void loco_1()
{
	unsigned int sect, sw_num, sw_state;
	proc_bind(1);

	char * out_pkt_mem = allocate(128);
	char * in_pkt_mem = allocate(128);

	struct master_to_loco * in_pkt_fmt = (struct master_to_loco *)in_pkt_mem;
	struct loco_to_loco * out_pkt_fmt = (struct loco_to_loco *)out_pkt_mem;

	/* start the initial section */
	set_pon_pkt(out_pkt_mem,loco[1].current_section,loco[1].direction,loco[1].speed);
	proc_send(1,5,out_pkt_mem,7);

/*******one function send loco number
 * 		and other parameters needed
 * 		so we dont repeat code
 */
	while(1)
	{
		/* wait for the master to send some message */
		while(proc_recv(3,1,in_pkt_mem,6) == PROC_BLOCKED);

		loco[1].last_HS = in_pkt_fmt -> hall_num;

		/* Set the VM PC using the hall sensor number from master */
		int  * PC = HALLS[in_pkt_fmt -> hall_num][loco[1].direction];

		/* do the VM operation */
		while(*PC != DONE)
		{
			switch(*PC++)
			{
			case TEST_SW:
				sw_num=*PC++;
				if(switches[sw_num].switch_state == THROWN)
					PC = PC + *PC;
				else
					PC++;
				break;

			case POFF:
				sect = *PC++;
				//poff(sect);
				set_poff_pkt(out_pkt_mem,sect);
				proc_send(1,5,out_pkt_mem,7);
				section[sect].occupied = NOT_OCCUPIED;
				section[sect].occupied_loco = NO_LOCO;

				/* if any other loco is sided and waiting on that section, then message that loco to proceed */
				if(section[sect].waiting_loco != NO_LOCO)
				{
					/* setup the information in packet to sent to the sided loco that its free to go */
					set_loco_to_loco(out_pkt_mem, 2, 1, sect);
					/* send the packet to the waiting loco */
					proc_send(1,section[sect].waiting_loco,out_pkt_mem,sizeof(struct loco_to_loco));
				}

				break;

			case PON:
				sect = *PC++;

				/* check whether the section is free */
				if(section[sect].occupied == NOT_OCCUPIED) /* section is free */
				{
					//pon(sect, dir, mag);
					/* set and send pon pkt */
					set_pon_pkt(out_pkt_mem,sect,loco[1].direction,loco[1].speed);
					proc_send(1,5,out_pkt_mem,7);

					/* set the section state in this software */
					section[sect].occupied = OCCUPIED;
					section[sect].occupied_loco = 1;

				}

				else /* a loco is in the forward section */
				{
					unsigned waiting_for = section[sect].occupied_loco;
					/* throw the next available switch for the loco in before this loco */
					set_switch_pkt(out_pkt_mem,section[sect].near_switch,THROWN);
					proc_send(1,5,out_pkt_mem,7);

					/* change the state of the switch in this software */
					switches[section[sect].near_switch].switch_state = THROWN;
					switches[section[sect].near_switch].thrown_loco = 1;

					/* wait for the siding loco in before to complete siding and send a message back */
					while(proc_recv(waiting_for,1,in_pkt_mem,sizeof(struct loco_to_loco)) == PROC_BLOCKED);

					/* once the other loco is done with siding, then pon the section for this loco */
					set_pon_pkt(out_pkt_mem,sect,loco[1].direction,loco[1].speed);
					proc_send(1,5,out_pkt_mem,7);

					/* change the state of the section in this software */
					section[sect].occupied = OCCUPIED;
					section[sect].occupied_loco = 1;
				}
				break;

			case SET_SW:
				sw_num = *PC++;
				sw_state = *PC++;
				set_switch_pkt(out_pkt_mem,sw_num,sw_state);
				proc_send(1,5,out_pkt_mem,7);
				switches[sw_num].switch_state = sw_state;
				break;

			case SEND_INFO:
				sw_num = *PC++;

				/* set the packet info to be sent to another loco indicating its free to go */
				set_loco_to_loco(out_pkt_mem, 2,switches[sw_num].thrown_loco,0);

				/* send the packet to the another loco */
				proc_send(1,switches[sw_num].thrown_loco,out_pkt_mem,6);
				break;

			case WAITING:
				sect = *PC++;

				/* set the waiting loco of the section to this loco */
				section[sect].waiting_loco = 1;
				break;

			case WAIT_INFO:
				sw_num = *PC++;

				/* wait for the information for the other loco */
				while(proc_recv(switches[sw_num].thrown_loco,1,out_pkt_mem,6) == PROC_BLOCKED);

				/* may require to reset switches[sw_num].thrown_loco */
				break;

			case SET_DIR:
				loco[1].direction = *PC++;
				break;

			default:
				break;
			}
		}
	}
}

/******************************************************************************************/
void loco_2()
{
	unsigned int sect, sw_num, sw_state;
	proc_bind(2);

	char * out_pkt_mem = allocate(128);
	char * in_pkt_mem = allocate(128);

	struct master_to_loco * in_pkt_fmt = (struct master_to_loco *)in_pkt_mem;

	/* start the initial section */
	set_pon_pkt(out_pkt_mem,loco[2].current_section,loco[2].direction,loco[2].speed);
	proc_send(2,5,out_pkt_mem,7);

	while(1)
	{
		/* wait for the master to send some message */
		while(proc_recv(3,2,in_pkt_mem,6) == PROC_BLOCKED);

		loco[2].last_HS = in_pkt_fmt -> hall_num;

		/* Set the VM PC using the hall sensor number from master */
		int  * PC = HALLS[in_pkt_fmt -> hall_num][loco[2].direction];

		/* do the VM operation */
		while(*PC != DONE)
		{
			switch(*PC++)
			{
			case TEST_SW:
				sw_num=*PC++;
				if(switches[sw_num].switch_state == THROWN)
					PC = PC + *PC;
				else
					PC++;
				break;

			case POFF:
				sect = *PC++;
				//poff(sect);
				set_poff_pkt(out_pkt_mem,sect);
				proc_send(2,5,out_pkt_mem,7);
				section[sect].occupied = NOT_OCCUPIED;
				section[sect].occupied_loco = NO_LOCO;

				/* if any other loco is sided and waiting on that section, then message that loco to proceed */
				if(section[sect].waiting_loco != NO_LOCO)
				{
					/* setup the information in packet to sent to the sided loco that its free to go */

					/* send the packet to the waiting loco */
					proc_send(2,section[sect].waiting_loco,out_pkt_mem,7);
				}


				break;

			case PON:
				sect = *PC++;

				/* check whether the section is free */
				if(section[sect].occupied == NOT_OCCUPIED) /* section is free */
				{
					//pon(sect, dir, mag);
					/* set and send pon pkt */
					set_pon_pkt(out_pkt_mem,sect,loco[2].direction,loco[2].speed);
					proc_send(2,5,out_pkt_mem,7);

					/* set the section state in this software */
					section[sect].occupied = OCCUPIED;
					section[sect].occupied_loco = 2;

				}

				else /* a loco is in the forward section */
				{
					/* thrown the next available switch for the loco in before this loco */
					set_switch_pkt(out_pkt_mem,section[sect].near_switch,THROWN);
					proc_send(2,5,out_pkt_mem,7);

					/* change the state of the switch in this software */
					switches[section[sect].near_switch].switch_state = THROWN;
					switches[section[sect].near_switch].thrown_loco = 2;

					/* wait for the siding loco in before to complete siding and send a message back */
					while(proc_recv(section[sect].occupied_loco,2,in_pkt_mem,6) == PROC_BLOCKED);

					/* once the other loco is done with siding, then pon the section for this loco */
					set_pon_pkt(out_pkt_mem,sect,loco[2].direction,loco[2].speed);
					proc_send(2,5,out_pkt_mem,7);

					/* change the state of the section in this software */
					section[sect].occupied = OCCUPIED;
					section[sect].occupied_loco = 2;
				}
				break;

			case SET_SW:
				sw_num = *PC++;
				sw_state = *PC++;
				set_switch_pkt(out_pkt_mem,sw_num,sw_state);
				proc_send(2,5,out_pkt_mem,7);
				switches[sw_num].switch_state = sw_state;
				break;

			case SEND_INFO:
				sw_num = *PC++;

				/* set the packet info to be sent to another loco indicating its free to go */

				/* send the packet to the another loco */
				proc_send(2,switches[sw_num].thrown_loco,out_pkt_mem,6);
				break;

			case WAITING:
				sect = *PC++;

				/* set the waiting loco of the section to this loco */
				section[sect].waiting_loco = 2;
				break;

			case WAIT_INFO:
				sw_num = *PC++;

				/* wait for the information for the other loco */
				while(proc_recv(switches[sw_num].thrown_loco,2,out_pkt_mem,6) == PROC_BLOCKED);

				/* may require to reset switches[sw_num].thrown_loco */
				break;

			case SET_DIR:
				loco[2].direction = *PC++;
				break;

			default:
				break;
			}
		}
	}
}

void train_master()
{
	proc_bind(3);

	//unsigned int hn, sect, dir, mag, sw_num, sw_state;
	//dir = CCLK;
	//mag = 4;

	char * out_pkt_mem = allocate(128);
	char * in_pkt_mem = allocate(128);

	struct hall_poll_fmt * in_pkt_fmt = (struct hall_poll_fmt *)in_pkt_mem;

	while(1)
	{
		int TP1 = (INT_QTAIL+1) % PORT_F_QSIZE;
		while(TP1 == INT_QHEAD);

		INT_QTAIL = TP1;

		/* set the hall poll packet and send it to the Tx_Rx process */
		set_hall_poll_pkt(out_pkt_mem,portf_int_queue[INT_QTAIL]);
		//INT_QTAIL+=1;
		//hn = get_HS_num(portf_int_queue[INT_QTAIL]);
		proc_send(3,5,out_pkt_mem,7);

		/* wait for the Tx_Rx process to send back the input packet from atmel */
		while(proc_recv(5,3,in_pkt_mem,6) == PROC_BLOCKED);
		//hn = (unsigned int) in_pkt_fmt -> hall_num;

		/* send message to the appropriate locomotive */
		set_master_to_loco(out_pkt_mem, in_pkt_fmt -> hall_num);

		unsigned i;

		for(i=1;i<=NUM_LOCOMOTIVES;i++)
		{
			if(next_HS[loco[i].last_HS][loco[i].direction].first == in_pkt_fmt -> hall_num ||
			   next_HS[loco[i].last_HS][loco[i].direction].second == in_pkt_fmt -> hall_num)
				{
					proc_send(3,i,out_pkt_mem,6); /* need to send only 1 */
					break;
				}
		}
	}
}

void tx_rx_pkt(void)
{
	char * output_pkt_mem = allocate(128);
	struct hall_poll_fmt * output_pkt_fmt = (struct hall_poll_fmt *)output_pkt_mem;

	char * input_pkt_mem = allocate(128);
	struct hall_poll_fmt * input_pkt_fmt = (struct hall_poll_fmt *)input_pkt_mem;

	char * output_ack_pkt_mem = allocate(128);
	struct hall_ack_fmt * output_ack_pkt_fmt = (struct hall_ack_fmt *)output_ack_pkt_mem;

	char * input_ack_pkt_mem = allocate(128);
	struct hall_ack_fmt * input_ack_pkt_fmt = (struct hall_ack_fmt *)input_ack_pkt_mem;

	proc_bind(5);

	while(1)
	{
		while(proc_recv(1,5,output_pkt_mem,7) == PROC_BLOCKED);

		send_pkt(output_pkt_mem,input_pkt_mem);

		if(output_pkt_fmt -> cmd == 0xA0)
		{
			set_hall_ack_pkt(output_ack_pkt_mem,input_pkt_fmt -> hall_num);
			send_pkt(output_ack_pkt_mem,input_ack_pkt_mem);
			proc_send(5,3,input_pkt_mem,6);
		}
	}
}

unsigned output_function (unsigned Line, unsigned Column, char character)
{
	int i = 0;
	int j = 0;
	for (i = 0 ; i<NUM_CHAR ; i++)
	{
		for(j = 0; j<WAIT_COUNTER; j++);
		putch(Line,Column++,character);
	}
	return Column;
}

/*********** idle process routines  *************/
void Idle_Proc(void) /* Spin the cursor in the corner of the screen */
{
	int j = 0;
	printf("\nI am in idle process");
	while(1)
	{
		for(j = 0; j<WAIT_COUNTER; j++);
		putch(IDLE_LINE,IDLE_COLUMN,'|');
		for(j = 0; j<WAIT_COUNTER; j++);
		putch(IDLE_LINE,IDLE_COLUMN,'/');
		for(j = 0; j<WAIT_COUNTER; j++);
		putch(IDLE_LINE,IDLE_COLUMN,'-');
		for(j = 0; j<WAIT_COUNTER; j++);
		putch(IDLE_LINE,IDLE_COLUMN,'\\');
	}
}

/************ putch routines ********************/
void putch(unsigned Line, unsigned Column, char Character)
{
	cup.l_10 = (char)((Line/10)+NUM_TO_ASCII);
	cup.l_1  = (char)((Line%10)+NUM_TO_ASCII);
	cup.c_10 = (char)((Column/10)+NUM_TO_ASCII);
	cup.c_1  = (char)((Column%10)+NUM_TO_ASCII);
	write_str((char *)&cup);
	echo(Character);
}

/************ systick handler ********************/
void SysTickHandler(void)
{
	count++;
	if(count%10 == 0)  /* Context switch every second */
	{
		save_registers();	/* save the explicit registers in the process's PSP */
		running[CUR_PRI] -> sp = get_PSP();  /* set the SP in PCB to the new PSP */
		running[CUR_PRI] = running[CUR_PRI] -> next; /* execute the next process in the queue */
		set_PSP(running[CUR_PRI] -> sp);  /* set the cpu's PSP to the SP stored in the PCB of new process */
		restore_registers();  /* restore the explicit registers from the new process SP */
	}
}

/************ SVC handler *********************/
void SVCall(void)
{
	/* Supervisor call (trap) entry point
	 * Using MSP - trapping process either MSP or PSP (specified in LR)
	 * Source is specified in LR: F9 (MSP) or FD (PSP)
	 * Save r4-r11 on trapping process stack (MSP or PSP)
	 * Restore r4-r11 from trapping process stack to CPU
	 * SVCf
	 * Handler is called with r0 equal to MSP or PSP to access any arguments
	 */

	/* Save LR for return via MSP or PSP */
	__asm(" 	PUSH 	{LR}");

	/* Trapping source: MSP or PSP? */
	__asm(" 	TST 	LR,#4");	/* Bit 4: MSP (0) or PSP (1) */
	__asm(" 	BNE 	RtnViaPSP");	/* EQ zero - MSP; NE zero - PSP */

	/* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
	__asm(" 	PUSH 	{r4-r11}");
	__asm(" 	MRS	r0,msp");
	__asm(" 	BL	SVCHandler");	/* r0 is MSP */
	__asm(" 	POP	{r4-r11}");
	__asm(" 	POP 	{PC}");

	/* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
	__asm("RtnViaPSP:");
	__asm(" 	mrs 	r0,psp");
	__asm("		stmdb 	r0!,{r4-r11}");	/* Store multiple, decrement before */
	__asm("		msr	psp,r0");
	__asm(" 	BL	SVCHandler");	/* r0 Is PSP */

	/* Restore r4..r11 from trapping process stack  */
	__asm(" 	mrs 	r0,psp");
	__asm("		ldmia 	r0!,{r4-r11}");	/* Load multiple, increment after */
	__asm("		msr	psp,r0");
	__asm(" 	POP 	{PC}");

}

void SVCHandler(struct stack_frame *argptr)
{
	/*
	 * Supervisor call handler
	 * Handle startup of initial process
	 * Handle all other SVCs such as getid, terminate, etc.
	 * Assumes first call is from startup code
	 * Argptr points to (i.e., has the value of) either:
   - the top of the MSP stack (startup initial process)
   - the top of the PSP stack (all subsequent calls)
	 * Argptr points to the full stack consisting of both hardware and software
   register pushes (i.e., R0..xPSR and R4..R10); this is defined in type
   stack_frame
	 * Argptr is actually R0 -- setup in SVCall(), above.
	 * Since this has been called as a trap (Cortex exception), the code is in
   Handler mode and uses the MSP
	 */
	static int firstSVCcall = TRUE;
	struct kcallargs *kcaptr;

	if (firstSVCcall)
	{
		/*
		 * Force a return using PSP
		 * This will be the first process to run, so the eight "soft pulled" registers
   (R4..R11) must be ignored otherwise PSP will be pointing to the wrong
   location; the PSP should be pointing to the registers R0..xPSR, which will
   be "hard pulled"by the BX LR instruction.
		 * To do this, it is necessary to ensure that the PSP points to (i.e., has) the
   address of R0; at this moment, it points to R4.
		 * Since there are eight registers (R4..R11) to skip, the value of the sp
   should be increased by 8 * sizeof(unsigned int).
		 * sp is increased because the stack runs from low to high memory.
		 */

		set_PSP(running[CUR_PRI] -> sp + 8 * sizeof(unsigned int));

		firstSVCcall = FALSE;	/* clear the flag */

		SysTick_init();			/* setup the systick */

		UART_init();			/* setup the uart */

		ENABLE_INT();			/* Enable Master (CPU) Interrupts */

		/*
	 - Change the current LR to indicate return to Thread mode using the PSP
	 - Assembler required to change LR to FFFF.FFFD (Thread/PSP)
	 - BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
		 */
		__asm("	movw 	LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
		__asm("	movt 	LR,#0xFFFF");  /* Upper 16 only */
		__asm("	bx 		LR");          /* Force return to PSP */
	}

	else /* Subsequent SVCs */
	{
		/*
		 * kcaptr points to the arguments associated with this kernel call
		 * argptr is the value of the PSP (passed in R0 and pointing to the TOS)
		 * the TOS is the complete stack_frame (R4-R10, R0-xPSR)
		 * in this example, R7 contains the address of the structure supplied by
   the process
		 * to get the address and store it in kcaptr, it is simply a matter of
   assigning the value of R7 (arptr -> r7) to kcaptr
		 */
		//DISABLE_INT();
		kcaptr = (struct kcallargs *) argptr -> r7;

		switch(kcaptr -> code) /* switch to requested operation that is specified in the code */
		{
		case GETID:
			kcaptr -> rtnvalue = running[CUR_PRI] -> id; /* set the return value to the process's ID */
			break;

		case NICE:
			running[CUR_PRI] -> sp = get_PSP(); /* store the stack pointer in the PCB */

			if(kcaptr -> arg1 > MAX_PRIORITY) /* the new priority is out of range */
			{
				kcaptr -> rtnvalue = NICE_ERROR; /* return error */
				break;
			}

			else if(kcaptr -> arg1 == CUR_PRI) /* the new priority is same as the current priority */
			{
				kcaptr -> rtnvalue = NICE_SUC; /* return success so that the process continues in the same process */
				break;
			}

			else if(kcaptr -> arg1 > CUR_PRI) /* the new priority is greater than the current priority */
			{

				struct pcb *temp;
				temp = running[CUR_PRI]; /* store current process in a temporary location */
				proc_remove(); /* removes the current process from the queue and set the running to its next */

				running[kcaptr -> arg1] = temp; /* fills process in the new higher priority queue */
				running[kcaptr -> arg1] -> next = temp;
				running[kcaptr -> arg1] -> prev = temp;

				CUR_PRI = kcaptr -> arg1; /* sets the current priority to new higher priority */
				set_PSP(running[CUR_PRI] -> sp); /* sets SP from the PCB of the process */
				kcaptr -> rtnvalue = NICE_SUC;
			}

			else if(kcaptr -> arg1 < CUR_PRI)
			{
				struct pcb *new_pcb;
				char *pcb_low_address=allocate(PCB_SIZE); /* allocate a PCB space */

				/* copy the entries of the process PCB to the new PCB */
				new_pcb = (struct pcb *) pcb_low_address;
				new_pcb-> sp = running[CUR_PRI] -> sp;
				new_pcb-> id = running[CUR_PRI] -> id;

				deallocate((char *)running[CUR_PRI]); /* deallocate the process PCB after copy */

				if(running[kcaptr -> arg1] == NUL) /* if the new priority queue is empty fill the process as running */
				{
					running[kcaptr -> arg1] = new_pcb;
					running[kcaptr -> arg1] -> next = new_pcb;
					running[kcaptr -> arg1] -> prev = new_pcb;
				}

				else /* if the process is not empty attach it to the end of the queue */
				{
					struct pcb *last_pcb;
					last_pcb = running[kcaptr ->arg1] -> prev; /* store the last entry in a temporary location */

					new_pcb -> next = running[kcaptr -> arg1]; /* attach the process at the end of the queue */
					last_pcb -> next = new_pcb;
					new_pcb -> prev = last_pcb;
					running[kcaptr -> arg1] -> prev = new_pcb;
				}

				proc_remove(); /* remove the process from the current queue and start the next process in round robin */
				set_PSP(running[CUR_PRI] -> sp); /* set the psp to new process's sp */
				kcaptr -> rtnvalue = NICE_SUC;
			}
			break;

		case TERMINATE:
			deallocate((char *)running[CUR_PRI] -> sp); /* deallocate both stack and PCB */
			deallocate((char *)running[CUR_PRI]);

			proc_remove(); /* removes the current process from the queue and set the running to its next */

			set_PSP(running[CUR_PRI] -> sp); /* set the psp to new process's sp */
			break;

		case PBIND:
			/* return error if the message queue is out of range or queue belongs to other process */
			if(kcaptr -> arg1 > NUM_MQ || MQ[kcaptr -> arg1].Owner_PID != 0)
				kcaptr -> rtnvalue = FAILURE;
			else
				MQ[kcaptr -> arg1].Owner_PID = running[CUR_PRI] -> id;
				kcaptr -> rtnvalue = SUCCESS;
			break;

		case PSEND:
			/* store the stack pointer in the PCB */
			running[CUR_PRI] -> sp = get_PSP();

			/* validity check */
			if(kcaptr -> arg1 > NUM_MQ || kcaptr -> arg2 > NUM_MQ)
				kcaptr -> rtnvalue = FAILURE;
			else if(MQ[kcaptr -> arg1].Owner_PID != running[CUR_PRI] -> id)
				kcaptr -> rtnvalue = FAILURE;

			/* queues are valid */
			else
			{
				/* allocate memory for message and update its sender ID and next in the queue */
				char * alloc_msg_space;
				alloc_msg_space = allocate(MSG_BLOCK_SIZE);
				struct Message_Entry * k_msg_entry = (struct Message_Entry *)alloc_msg_space;
				k_msg_entry -> Sender_ID = running[CUR_PRI] -> id;
				k_msg_entry -> Next = NULL;

				/* copy only upto the message size if the message to be sent is oversized */
				if(kcaptr -> arg4 > MSG_SIZE)
				k_msg_entry -> Size = MSG_SIZE;

				/* message size is appropriate */
				else
				k_msg_entry -> Size = kcaptr -> arg4;

				/* copy the message */
				k_msg_entry -> Msg_Ptr = alloc_msg_space + (4*sizeof(unsigned int));
				memcpy(k_msg_entry -> Msg_Ptr, kcaptr -> arg3, k_msg_entry -> Size);

				if(MQ[kcaptr -> arg2].Blocked_PCB == NULL)
				{

					if(MQ[kcaptr -> arg2].head == NULL)
					{
						MQ[kcaptr -> arg2].head = k_msg_entry;
						MQ[kcaptr -> arg2].tail = k_msg_entry;
					}
					else
					{
						MQ[kcaptr -> arg2].tail -> Next = k_msg_entry;
						MQ[kcaptr -> arg2].tail = k_msg_entry;
					}
				}

				else /* MQ[kcaptr -> arg2].Blocked_PCB != NULL */
				{
					if(MQ[kcaptr -> arg2].Blocked_PCB -> priority == CUR_PRI)
					{
						running[CUR_PRI] -> next -> prev = MQ[kcaptr -> arg2].Blocked_PCB;
						MQ[kcaptr -> arg2].Blocked_PCB -> next = running[CUR_PRI] -> next;
						MQ[kcaptr -> arg2].Blocked_PCB -> prev = running[CUR_PRI];
						running[CUR_PRI] -> next = MQ[kcaptr -> arg2].Blocked_PCB;
					}

					else if(MQ[kcaptr -> arg2].Blocked_PCB -> priority > CUR_PRI)
					{
						CUR_PRI = MQ[kcaptr -> arg2].Blocked_PCB -> priority;
						running[CUR_PRI] = MQ[kcaptr -> arg2].Blocked_PCB;
						running[CUR_PRI] -> next = MQ[kcaptr -> arg2].Blocked_PCB;
						running[CUR_PRI] -> prev = MQ[kcaptr -> arg2].Blocked_PCB;
					}

					MQ[kcaptr -> arg2].head = k_msg_entry;
					MQ[kcaptr -> arg2].tail = k_msg_entry;
					MQ[kcaptr -> arg2].Blocked_PCB = NULL;
				}
				set_PSP(running[CUR_PRI] -> sp); /* set the psp to new process's sp */
				kcaptr -> rtnvalue = SUCCESS;
		}
			break;

		case PRECV:
			/* store the stack pointer in the PCB */
			running[CUR_PRI] -> sp = get_PSP();

			/* Check validity */
			if(kcaptr -> arg1 > NUM_MQ || kcaptr -> arg2 > NUM_MQ)
				kcaptr -> rtnvalue = FAILURE;
			else if(MQ[kcaptr -> arg2].Owner_PID != running[CUR_PRI] -> id)
				kcaptr -> rtnvalue = FAILURE;

			/* there is atleast one message in the queue */
			else if(MQ[kcaptr -> arg2].head != NULL)
			{
				memcpy(kcaptr -> arg3, MQ[kcaptr -> arg2].head -> Msg_Ptr, MQ[kcaptr -> arg2].head -> Size);
				kcaptr -> rtnvalue = MQ[kcaptr -> arg2].head -> Size;

				deallocate(MQ[kcaptr -> arg2].head -> Msg_Ptr);
				MQ[kcaptr -> arg2].head = MQ[kcaptr -> arg2].head -> Next;
				kcaptr -> rtnvalue = SUCCESS;
			}

			else /* if MQ[kcaptr -> arg2].head == NULL */
			{
				MQ[kcaptr -> arg2].Blocked_PCB = running[CUR_PRI];
				proc_remove();
				kcaptr->rtnvalue = PROC_BLOCKED;
			}
			set_PSP(running[CUR_PRI] -> sp); /* set the psp to new process's sp */
			break;

		default:
			break; /* do nothing */
		}
		//ENABLE_INT();
	}
}

void proc_remove(void)
{

	if(running[CUR_PRI] == running[CUR_PRI] -> next) /* this is the last process in the queue */
	{
		running[CUR_PRI] = NUL;  /* clear the current priority queue */
		CUR_PRI -=1; 			 /* decrement the priority */
		while(running[CUR_PRI] == NUL) /* decrement until a non-null priority queue is found */
		{
			CUR_PRI--;
		}
	}

	else /*atleast one more process waiting in the queue */
	{
		running[CUR_PRI] -> next -> prev = running[CUR_PRI] -> prev; /* remove the process from the queue */
		running[CUR_PRI] -> prev -> next = running[CUR_PRI] -> next; /* link the prev and next process in the queue */
		running[CUR_PRI] = running[CUR_PRI] -> next;	/* set the running to the process's next */
	}
}

int reg_proc(void (*func_name)(), unsigned pid, unsigned priority)
{

	void (*term_func)() = proc_terminate; /* a pointer to the terminate function */
	char *stack_low_address=allocate(PROCESS_STACK_SIZE); /* allocate memory to the process'S stack */
	char *pcb_low_address=allocate(PCB_SIZE); /* allocate memory to the process's PCB */

	/* If any one of the allocation failed, then deallocate both and return a error message */
	if(stack_low_address==(char*)ERROR_CHAR || pcb_low_address==(char*)ERROR_CHAR)
	{

		deallocate(stack_low_address);
		deallocate(pcb_low_address);
		return FAILURE;
	}

	else /* both the allocation is successful */
	{
		unsigned long psp=(unsigned long)stack_low_address+PROCESS_STACK_SIZE; /* PSP points to the high address of the stack */

		psp = psp - sizeof(struct stack_frame); /* PSP is then decremented by size of stack frame to load the stack frame entries */

		struct stack_frame *stack_args;
		stack_args = (struct stack_frame *)psp;

		stack_args->r4 = 0x44444444;
		stack_args->r5 = 0x55555555;
		stack_args->r6 = 0x66666666;
		stack_args->r7 = 0x77777777;
		stack_args->r8 = 0x88888888;
		stack_args->r9 = 0x99999999;
		stack_args->r10 = 0xaaaaaaaa;
		stack_args->r11 = 0xbbbbbbbb;
		stack_args->r12 = 0xcccccccc;
		stack_args->r0 = 0x00000000;
		stack_args->r1 = 0x11111111;
		stack_args->r2 = 0x22222222;
		stack_args->r3 = 0x33333333;
		stack_args->xpsr = 0x01000000;
		stack_args->pc = (unsigned long) func_name; /* PC pointing to the process function */
		stack_args->lr = (unsigned long) term_func; /* LR pointing to the terminate function */

		/* Initialise the stacks pcb */
		struct pcb *pcbargs;
		pcbargs = (struct pcb *) pcb_low_address;
		pcbargs -> id = pid;
		pcbargs -> sp = (unsigned long) psp;
		pcbargs -> priority = priority;

		if(first_process==TRUE) /* if it is the first process in the queue then point next and prev to itself */
		{
			pcbargs -> next = pcbargs;
			pcbargs -> prev = pcbargs;
			prev_pcb = pcbargs;

			running[priority]=pcbargs; /* set running of current priority to the first process in the queue */
			first_process = FALSE; /* unset the flag for next processes */
		}

		else /* there is already at least one process in the current priority queue */
		{
			pcbargs -> next = prev_pcb -> next; /* attach the process at the end of the queue */
			prev_pcb -> next = pcbargs;
			pcbargs -> prev = prev_pcb;

			running[priority] -> prev = pcbargs;
			prev_pcb = pcbargs;

		}
		return SUCCESS;
	}
}

int main(void) {


	initialize();			/* Memory Initialisation */

	/* initialise interrupts */
	UART_init();			/* Setup the uart */
	//SPI_Init();				/* Setup the SPI */
	//PORTF_Init();			/* Setup port F */
	ENABLE_INT();			/* Enable Master (CPU) Interrupts */

	write_str("WORKS!");
	/* prompt and access input from the user and store it in the location */
	//get_CLI_input();

	/* initialize the track layout on on the display */
	track_init();

	while(1);

	loco[1].current_section = input_config[0];
	loco[1].speed = input_config[1];
	loco[1].direction  = CCLK;
	loco[1].last_HS = (loco[1].current_section*2) - 1;

	loco[2].current_section = input_config[2];
	loco[2].speed = input_config[3];
	loco[2].direction = CCLK;
	loco[2].last_HS = (loco[2].current_section*2) - 1;

	/* initialise and reset the hall sensor queue */
	//hall_sensor_ack_all();
	hall_sensor_queue_reset();
	hall_sensor_ack_all();

	/* power off all the sections */
	//poff_all();

	int i = 0;
	for(i=1;i<16;i++)
		poff(i);


	/* set all switches STRAIGHT */
	i = 0;
	for(i=1;i<9;i++)
		sw_ctrl(i,STRAIGHT);

	/* set SW8 THROWN */

	sw_ctrl(SW8,THROWN);

	//pon(255,CCLK,0);

	/* register loco processes */
	first_process = TRUE;
	//reg_proc(start_loco,10,4);
	reg_proc(tx_rx_pkt,5,4);

	first_process = TRUE;
	reg_proc(loco_1,1,3);
	//reg_proc(loco_2,2,4);
	/* register main process */

	first_process = TRUE;
	reg_proc(train_master,3,2);

	/*first_process = TRUE;
	reg_proc(Proc_b,2,3);
	first_process = TRUE;
	reg_proc(Proc_a,1,2);*/

	/* register idle process */
	first_process = TRUE;
	reg_proc(Idle_Proc,30,0);

	CUR_PRI = 4;
	/*pon(5,CCLK,7);
	pon(6,CCLK,7);
	pon(1,CCLK,7);
	pon(2,CCLK,7);
	pon(14,CCLK,7);*/

	//pon(5,CLK,7);
	//pon(6,CLK,7);
	//pon(7,CLK,7);

	/*sw_ctrl(1,1);
	sw_ctrl(2,0);*/

	/*poff(1);
	poff(2);
	poff(3);
	poff(4);
	poff(5);
	poff(6);
	poff(7);*/

	//pon(loco[1].current_section,CCLK,loco[1].speed);

	SVC();

	return 0;
}


/* power on the section in specific direction at specific speed */
void pon(unsigned int Section, unsigned int direction, unsigned int magnitude)
{
	/* memory for output section control packet */
	char * output_pkt_memory = allocate(128);
	struct sect_ctrl_fmt *output_pkt = (struct sect_ctrl_fmt *)output_pkt_memory;

	/* set the output section control packet */
	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xB0;
	output_pkt -> sect_num = (char)Section;
	if(direction == CCLK)
		magnitude +=128;
	output_pkt -> dir_mag = magnitude;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;

	/* memory for input packets from atmel */
	char * input_pkt_memory = allocate(128);
	char * input_pkt = input_pkt_memory;

	/* send section control packet until success */
	do
	{
		input_pkt = input_pkt_memory;
		output_pkt_memory = (char *)output_pkt;
		while(*output_pkt_memory)
		{
			//struct sect_ctrl_fmt *input_pkt = (struct sect_ctrl_fmt *)input_pkt_memory;;

			SPIDataTransmit(*output_pkt_memory++,1);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt != 0x99);

	/* higlight the section */
	highlight_sect((enum SECTIONS) Section);

	/* memory of packets deallocated */
	deallocate(input_pkt_memory);
	deallocate(output_pkt_memory);
}

/* power off all section at the beginning */
void poff_all()
{
	/* memory of output section control packet */
	char * output_pkt_memory = allocate(128);
	struct sect_ctrl_fmt *output_pkt = (struct sect_ctrl_fmt *)output_pkt_memory;

	/* set the output section control packet */
	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xB0;
	output_pkt -> sect_num = 0xFF;
	output_pkt -> dir_mag = 0x00;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;

	/* memory of input section control packet */
	char * input_pkt_memory = allocate(128);
	char * input_pkt = input_pkt_memory;

    /* send section control packet until success */
	do
	{
		input_pkt = input_pkt_memory;
		output_pkt_memory = (char *)output_pkt;
		while(output_pkt_memory != (char*)output_pkt+(6*sizeof(unsigned char)))
		{
			//struct sect_ctrl_fmt *input_pkt = (struct sect_ctrl_fmt *)input_pkt_memory;;

			SPIDataTransmit(*output_pkt_memory++,1);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt != 0x99);

	/* memory of packets deallocated */
	deallocate(input_pkt_memory);
	deallocate(output_pkt_memory);
}

void poff(unsigned int Section)
{
	/* memory of output section control packet */
	char * output_pkt_memory = allocate(128);
	struct sect_ctrl_fmt *output_pkt = (struct sect_ctrl_fmt *)output_pkt_memory;

	/* set the output section control packet */
	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xB0;
	output_pkt -> sect_num = (char)Section;
	output_pkt -> dir_mag = 0x00;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;

	char * input_pkt_memory = allocate(128);
	char * input_pkt = input_pkt_memory;

	do
	{
		input_pkt = input_pkt_memory;
		output_pkt_memory = (char *)output_pkt;
		while(output_pkt_memory != (char*)output_pkt+(6*sizeof(unsigned char)))
		{
			//struct sect_ctrl_fmt *input_pkt = (struct sect_ctrl_fmt *)input_pkt_memory;;

			SPIDataTransmit(*output_pkt_memory++,1);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt != 0x99);

	/* unhighlight section */
	unhighlight_sect((enum SECTIONS) Section);

	deallocate(input_pkt_memory);
	deallocate(output_pkt_memory);
}


void sw_ctrl(unsigned switch_num, unsigned state)
{
	char * output_pkt_memory = allocate(128);
	struct sw_ctrl_fmt * output_pkt = (struct sw_ctrl_fmt *) output_pkt_memory;

	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xD0;
	output_pkt -> sw_num = (char)switch_num;
	output_pkt -> state = (char)state;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;


	char * input_pkt_memory = allocate(128);
	char * input_pkt;
	do{
		input_pkt = input_pkt_memory;
		output_pkt_memory = (char *)output_pkt;
		while(output_pkt_memory != (char*)output_pkt + (6*sizeof(unsigned char)))
		{
			SPIDataTransmit(*output_pkt_memory++,2);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt!=0x99);

	/* update switch state on track layout display */
	disp_switches((enum SWITCHES) switch_num, (enum SWITCH_STATE) state);


	deallocate(output_pkt_memory);
	deallocate(input_pkt_memory);
}

void hall_sensor_queue_reset(void)
{
	char * output_poll_pkt_memory = allocate(128);
	struct hall_poll_fmt * output_poll_pkt = (struct hall_poll_fmt *) output_poll_pkt_memory;

	output_poll_pkt -> start_pkt = 0xAA;
	output_poll_pkt -> cmd = 0xAE;
	output_poll_pkt -> hall_num = 0xFF;
	output_poll_pkt -> fill = 0xFF;
	output_poll_pkt -> end_pkt = 0x55;
	output_poll_pkt -> recv_result = 0xFF;

	char * input_poll_pkt_memory = allocate(128);
	char * input_pkt = input_poll_pkt_memory;

		do
		{
			input_pkt = input_poll_pkt_memory;
			output_poll_pkt_memory = (char *)output_poll_pkt;
			while(*output_poll_pkt_memory)
			{
				SPIDataTransmit(*output_poll_pkt_memory++,1);
				WaitSPIBusy();
				*input_pkt++=SPIDataReceive();
			}
		}while(*--input_pkt != 0x99);

		do
		{
			input_pkt = input_poll_pkt_memory;
			output_poll_pkt_memory = (char *)output_poll_pkt;
			while(*output_poll_pkt_memory)
			{
				SPIDataTransmit(*output_poll_pkt_memory++,3);
				WaitSPIBusy();
				*input_pkt++=SPIDataReceive();
			}
		}while(*--input_pkt != 0x99);

	deallocate(output_poll_pkt_memory);
	deallocate(input_poll_pkt_memory);
}

void hall_sensor_ack_all(void)
{
	char * output_ack_pkt_memory = allocate(128);
	struct hall_ack_fmt * output_ack_pkt = (struct hall_ack_fmt *) output_ack_pkt_memory;

	output_ack_pkt -> start_pkt = 0xAA;
	output_ack_pkt -> cmd = 0xCE;
	output_ack_pkt -> hall_num = 0xFF;
	output_ack_pkt -> fill = 0xFF;
	output_ack_pkt -> end_pkt = 0x55;
	output_ack_pkt -> recv_result = 0xFF;

	char * input_ack_pkt_memory = allocate(128);
	char * input_pkt = input_ack_pkt_memory;

	do
	{
		input_pkt = input_ack_pkt_memory;
		output_ack_pkt_memory = (char *)output_ack_pkt;
		while(*output_ack_pkt_memory)
		{
			SPIDataTransmit(*output_ack_pkt_memory++,1);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt != 0x99);

	deallocate(output_ack_pkt_memory);
	deallocate(input_ack_pkt_memory);
}

int get_HS_num(unsigned Atmel_num)
{
	char * output_poll_pkt_memory = allocate(128);
	struct hall_poll_fmt * output_poll_pkt = (struct hall_poll_fmt *) output_poll_pkt_memory;

	output_poll_pkt -> start_pkt = 0xAA;
	output_poll_pkt -> cmd = 0xA0;
	output_poll_pkt -> fill = 0xFF;
	output_poll_pkt -> hall_num = 0xFF;
	output_poll_pkt -> end_pkt = 0x55;
	output_poll_pkt -> recv_result = 0xFF;

	char * input_poll_pkt_memory = allocate(128);
	char * input_pkt = input_poll_pkt_memory;

	do
	{
		input_pkt = input_poll_pkt_memory;
		output_poll_pkt_memory = (char *)output_poll_pkt;
		while(*output_poll_pkt_memory)
		{
			SPIDataTransmit(*output_poll_pkt_memory++,Atmel_num);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt != 0x99);

	struct hall_poll_fmt * input_poll_pkt = (struct hall_poll_fmt *)input_poll_pkt_memory;

	char * output_ack_pkt_memory = allocate(128);
	struct hall_ack_fmt * output_ack_pkt = (struct hall_ack_fmt *) output_ack_pkt_memory;

	output_ack_pkt -> start_pkt = 0xAA;
	output_ack_pkt -> cmd = 0xC0;
	output_ack_pkt -> hall_num = input_poll_pkt -> hall_num;
	output_ack_pkt -> fill = 0xFF;
	output_ack_pkt -> end_pkt = 0x55;
	output_ack_pkt -> recv_result = 0xFF;

	char * input_ack_pkt_memory = allocate(128);
	input_pkt = input_ack_pkt_memory;

	do
	{
		input_pkt = input_ack_pkt_memory;
		output_ack_pkt_memory = (char *)output_ack_pkt;
		while(*output_ack_pkt_memory)
		{
			SPIDataTransmit(*output_ack_pkt_memory++,1);
			WaitSPIBusy();
			*input_pkt++=SPIDataReceive();
		}
	}while(*--input_pkt != 0x99);

    unsigned Hall_sensor_num = input_poll_pkt -> hall_num;

    deallocate(output_poll_pkt_memory);
    deallocate(input_poll_pkt_memory);
    deallocate(output_ack_pkt_memory);
    deallocate(input_ack_pkt_memory);

    return Hall_sensor_num;
}

void GPIOPortF_IntHandler(void)
{
// Configured for Rising Edge triggered interrupts on Pins 0 and 1

// Clear the two interrupts
PORTF_GPIOICR_R -> IC |= (1<<BIT0) | (1<<BIT1);

if((PORTF_GPIODR_R -> DATA & 0x01) == 0x01)
{
// This indicates that Hall Sensors 1 - 8 has been triggered
// And that Atmel board #1 has signalled the Cortex via Pin 0 on PORTF
// The application can now process the interrupt (e.g. fetch the HS# via SPI)
// input_enqueue(PORT_F,get_HS_num(1));
	int_enqueue(0x01);
}

if((PORTF_GPIODR_R -> DATA & 0x02) == 0x02)
{
// This indicates that Hall Sensors 9 - 32 has been triggered
// And that Atmel board #3 has signalled the Cortex via Pin 1 on PORTF
// The application can now process the interrupt (e.g. fetch the HS# via SPI)
//input_enqueue(PORT_F,get_HS_num(3));
	int_enqueue(0x03);
}

}

void int_enqueue(unsigned char atmel_num)
{
	DISABLE_INT();				/* globally disable interrupts */

	if(INT_QHEAD == INT_QTAIL)
	{
		#ifdef DEBUG	/* if the debug is defined print the information to the admin through console port */
		printf("Input queue is full during enqueue of %c and queue head is %i and queue tail is %i\n",INPUT_QHEAD,INPUT_QTAIL);
		#endif
		ENABLE_INT();
	}

	else
	{
		portf_int_queue[INT_QHEAD] = atmel_num;
		INT_QHEAD = (INT_QHEAD+1) % PORT_F_QSIZE;
		ENABLE_INT();
	}
}

void set_master_to_loco(char * pkt_mem, unsigned char hall_num)
{
	struct master_to_loco * pkt_mem_fmt = (struct master_to_loco *)pkt_mem;
	pkt_mem_fmt -> hall_num = hall_num;
}

void set_loco_to_loco(char * pkt_mem, unsigned from_loco, unsigned to_loco, unsigned section)
{
	struct loco_to_loco * pkt_mem_fmt = (struct loco_to_loco *)pkt_mem;
	pkt_mem_fmt -> cmd = 0xEE;
	pkt_mem_fmt -> from_loco = from_loco;
	pkt_mem_fmt -> to_loco = to_loco;
	pkt_mem_fmt -> section = section;
}

void set_pon_pkt(char * pkt_mem, unsigned section, unsigned direction, unsigned magnitude)
{
	struct sect_ctrl_fmt *output_pkt = (struct sect_ctrl_fmt *)pkt_mem;
	struct pkt_info *pkt = (struct pkt_info *)pkt_mem;

	/* set the output section control packet */
	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xB0;
	output_pkt -> sect_num = (char)section;
	if(direction == CCLK)
		magnitude +=128;
	output_pkt -> dir_mag = magnitude;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;

	/* set the atmel number */
	pkt -> atmel_num = 0x01;
}

void set_poff_pkt(char * pkt_mem, unsigned int section)
{
	struct sect_ctrl_fmt *output_pkt = (struct sect_ctrl_fmt *)pkt_mem;
	struct pkt_info *pkt = (struct pkt_info *)pkt_mem;

	/* set the output section control packet */
	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xB0;
	output_pkt -> sect_num = (char)section;
	output_pkt -> dir_mag = 0x00;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;

	/* set the atmel number */
	pkt -> atmel_num = 0x01;
}

void set_switch_pkt(char * pkt_mem, unsigned switch_num, unsigned state)
{
	struct sw_ctrl_fmt *output_pkt = (struct sw_ctrl_fmt *)pkt_mem;
	struct pkt_info *pkt = (struct pkt_info *)pkt_mem;

	/* set the output section control packet */
	output_pkt -> start_pkt = 0xAA;
	output_pkt -> cmd = 0xD0;
	output_pkt -> sw_num = (char)switch_num;
	output_pkt -> state = (char)state;
	output_pkt -> end_pkt = 0x55;
	output_pkt -> recv_result = 0xFF;

	/* set the atmel number */
	pkt -> atmel_num = 0x02;
}

void set_hall_poll_pkt(char * pkt_mem, unsigned char atmel_num)
{
	struct hall_poll_fmt * output_poll_pkt = (struct hall_poll_fmt *) pkt_mem;
	struct pkt_info *pkt = (struct pkt_info *)pkt_mem;

	/* set the output section control packet */
	output_poll_pkt -> start_pkt = 0xAA;
	output_poll_pkt -> cmd = 0xA0;
	output_poll_pkt -> fill = 0xFF;
	output_poll_pkt -> hall_num = 0xFF;
	output_poll_pkt -> end_pkt = 0x55;
	output_poll_pkt -> recv_result = 0xFF;

	/* set the atmel number */
	pkt -> atmel_num = atmel_num;
}

void set_hall_ack_pkt(char * pkt_mem, unsigned char hall_num)
{
	struct hall_ack_fmt * output_ack_pkt = (struct hall_ack_fmt *) pkt_mem;
	struct pkt_info *pkt = (struct pkt_info *)pkt_mem;

	output_ack_pkt -> start_pkt = 0xAA;
	output_ack_pkt -> cmd = 0xC0;
	output_ack_pkt -> hall_num = hall_num;
	output_ack_pkt -> fill = 0xFF;
	output_ack_pkt -> end_pkt = 0x55;
	output_ack_pkt -> recv_result = 0xFF;

	/* set the atmel number */
	pkt -> atmel_num = 0x01;
}


void send_pkt(char * out_mem, char * in_mem)
{
	struct pkt_info * output_pkt_info = (struct pkt_info *) out_mem;
	unsigned char atmel_num = output_pkt_info -> atmel_num;

	char * input_pkt;
	char * output_pkt;

	do{
			input_pkt = in_mem;
			output_pkt = out_mem;
			while(output_pkt != (char*)out_mem + (6*sizeof(unsigned char)))
			{
				SPIDataTransmit(*output_pkt++,atmel_num);
				WaitSPIBusy();
				*input_pkt++=SPIDataReceive();
			}
		}while(*--input_pkt!=0x99);

}
