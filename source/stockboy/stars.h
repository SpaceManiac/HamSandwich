#ifndef STARS_H
#define STARS_H

#include "mgldraw.h"

#define NUM_STARS 50

#define STARSRCX	(40*FIXAMT)
#define STARSRCY	(440*FIXAMT)
#define STARDESTX	(620*FIXAMT)
#define STARDESTY	(440*FIXAMT)

#define GOODIE_STARS	5
#define EMPMONTH_STARS	5
#define EMPYEAR_STARS	10
#define TRAINING_STARS	10

void InitStars(byte lvl,word minStar);
void UpdateStars(void);
void LaunchStar(int x,int y);
byte StarsLeft(void);
void RenderStars(MGLDraw *mgl);
void RenderStartStar(int x,int y,MGLDraw *mgl);
void RenderEndStar(MGLDraw *mgl);
void RenderEndStar2(MGLDraw *mgl);
void LaunchStarAngle(int x,int y,byte ang);

#endif