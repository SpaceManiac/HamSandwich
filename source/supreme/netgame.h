#ifndef NETGAME_H
#define NETGAME_H

#include "mgldraw.h"

void Init_Netgame(void);
void Exit_Netgame(void);
byte Update_Netgame(byte btn,word mouse_x,word mouse_y);
void Render_Netgame(void);

#endif
