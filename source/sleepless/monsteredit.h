#ifndef MONSTEREDIT_H
#define MONSTEREDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"

void MonsterEdit_Init(byte modeFrom,world_t *wrld);
void MonsterEdit_Exit(void);

void MonsterEdit_Update(int mouseX,int mouseY,MGLDraw *mgl);
void MonsterEdit_Render(int mouseX,int mouseY,MGLDraw *mgl);

void MonsterEdit_Key(char k);

void MonsterEdit_Help(void);

#endif
