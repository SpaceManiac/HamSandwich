#ifndef SCORE_H
#define SCORE_H

#include "mgldraw.h"

#define SCORE_OBJECT  5
#define SCORE_MONSTER 10
#define SCORE_BOMB	  1
#define SCORE_PENCIL  15

void Score_Init(void);
byte Score_Update(void);
void Score_BlowUp(byte pts,byte color);
void Score_Render(void);
byte Score_ComboGoing(void);

#endif