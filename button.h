#ifndef _BUTTON_H_
#define _BUTTON_H_

// Report button on PIO1_9
#define REPORT_B (1UL << 9)

void initButtons(void);

int readButtons(void);

int getLastPressed(void);

void resetLastPressed(void);

static int rawButtonPresses(int button);

static int reportButtonPressed(void);


#endif /* _BUTTON_H_ */
