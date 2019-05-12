#ifndef SOUNDEDIT_H
#define SOUNDEDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"

void SoundEdit_Init(world_t *wrld);
void SoundEdit_InitPicker(byte modeFrom,world_t *wrld);
void SoundEdit_Exit(void);

void SoundEdit_Update(int mouseX,int mouseY,MGLDraw *mgl);
void SoundEdit_Render(int mouseX,int mouseY,MGLDraw *mgl);

void SoundEdit_Key(char k);

void SoundEdit_Help(void);

#endif
