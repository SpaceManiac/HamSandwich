#ifndef TERRAINEDIT_H
#define TERRAINEDIT_H

#include "world.h"
#include "map.h"
#include "game.h"
#include "filedialog.h"
#include "mapdialog.h"
#include "editor.h"

void TerrainEdit_Init(world_t *wrld);
void TerrainEdit_InitPicker(byte fromMode,world_t *wrld);
void TerrainEdit_Exit(void);

void TerrainEdit_Update(int mouseX,int mouseY,MGLDraw *mgl);
void TerrainEdit_Render(int mouseX,int mouseY,MGLDraw *mgl);

void TerrainEdit_Key(char k);

void TerrainEdit_Help(void);

#endif
