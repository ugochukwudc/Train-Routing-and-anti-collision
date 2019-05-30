/*
 * SVC.h
 *
 *  Created on: 2014-10-27
 *      Author: Vignesh
 */

#ifndef SVC_H_
#define SVC_H_

void SVCall(void);
void SVCHandler(struct stack_frame *argptr);

#endif /* SVC_H_ */
