/*
 * Memory_Management.h
 *
 *  Created on: 2014-10-27
 *      Author: Vignesh
 */

#ifndef MEMORY_MANAGEMENT_H_
#define MEMORY_MANAGEMENT_H_

void initialize ();
char* allocate (int size);
int deallocate (char *address);
void show(void);
void help(void);
#endif /* MEMORY_MANAGEMENT_H_ */

