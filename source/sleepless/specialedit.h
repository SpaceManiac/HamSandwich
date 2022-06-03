#ifndef SPECIALEDIT_H
#define SPECIALEDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"

void SpecialEdit_Init(int s);
void SpecialEdit_Exit(void);

void SpecialEdit_Update(int mouseX,int mouseY,int scroll,MGLDraw *mgl);
void SpecialEdit_Render(int mouseX,int mouseY,MGLDraw *mgl);

void SpecialEdit_Key(char k);

void SetSpecialCoords(int x,int y);
void SetSpecialRect(int x,int y,int x2,int y2);

void SpecialEdit_Help(void);

#endif
