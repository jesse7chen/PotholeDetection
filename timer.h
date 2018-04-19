#ifndef _TIMER_H_
#define _TIMER_H_

void timer_init(int ticks);

void timerStart(void);

void timerEnd(void);

int getTimerStatus(void);

void threadWait(int ticks);

//void delayedExecute(void (*desired_function)(void), int ticks);

#endif /* _TIMER_H_ */
