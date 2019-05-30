/*
 * Kernel code: Supervisor call (SVC) handler example
 * ECED 4402
 *
 * 23 Oct 2014 - Fix SVC call (again?) to test bit 3 in SVCall()
 * 11 Mar 2013 - Handle passing argument to kernel by register
 * 05 Mar 2013 - Fix first SVC call to access PSP correctly
 *             - Updated comments
 * 20 Feb 2012 - First version
 */
#include <stdio.h>
#include "process.h"
#include "SVC.h"


