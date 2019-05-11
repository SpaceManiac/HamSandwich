#ifndef ITEMEDIT_H
#define ITEMEDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"

void ItemEdit_Init(byte modeFrom,world_t *wrld,byte picking);
void ItemEdit_Exit(void);

void ItemEdit_Update(int mouseX,int mouseY,MGLDraw *mgl);
void ItemEdit_Render(int mouseX,int mouseY,MGLDraw *mgl);

void ItemEdit_Key(char k);

void ItemEditHelp(void);

#endif
