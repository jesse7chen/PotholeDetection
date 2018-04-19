#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "config.h"

# ifdef DEMO
void delayedExecuteInit(int ticks);

void delayedExecute(void (*desired_function)(void), int time);

void executeFunction(void);

# elif TIME_TEST
void stopwatchInit(void);

void stopwatchStart(void);

double stopwatchStop(void); 
# endif


#endif /* _STOPWATCH_H_ */
