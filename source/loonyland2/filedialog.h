#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <io.h>
#include "winpch.h"
#include "display.h"
#pragma pack(1)

void InitFileDialog(void);
void ExitFileDialog(void);
void RenderFileDialog(int msx,int msy,MGLDraw *mgl);
byte FileDialogKey(char key);
byte FileDialogClick(int msx,int msy);

#endif
