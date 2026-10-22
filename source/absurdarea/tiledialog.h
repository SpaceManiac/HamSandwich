#ifndef TILEDIALOG_H
#define TILEDIALOG_H

#include "winpch.h"
#include "display.h"
#pragma pack(1)

void InitTileDialog(void);
void ExitTileDialog(void);
void RenderTileDialog(int msx,int msy,MGLDraw *mgl);
byte TileDialogKey(char key);
byte TileDialogClick(int msx,int msy);

#endif
