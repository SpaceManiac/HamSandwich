#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <stdio.h>
#include "display.h"
#include "bitflags.h"

constexpr int FNAMELEN = 32;

enum FileDialogFlags : word
{
	FM_NEW          = 1 << 0,
	FM_LOAD         = 1 << 1,
	FM_SAVE         = 1 << 2,
	FM_EXIT         = 1 << 3,
	FM_ASKLOAD      = 1 << 4,
	FM_PLAYSONGS    = 1 << 5,
	FM_PICMOVIE     = 1 << 6,
	FM_MERGE        = 1 << 7,
	FM_SAVEPACK     = 1 << 8,
	FM_SOUNDS       = 1 << 9,
};
BITFLAGS(FileDialogFlags)

void InitFileDialog(const char *dir, const char *ext, FileDialogFlags menuItemsToShow,const char *defaultName);
void ExitFileDialog(void);
void RenderFileDialog(int msx,int msy,MGLDraw *mgl);
byte FileDialogKey(char key);
word FileDialogClick(int msx,int msy);
void FileDialogScroll(int msz);

const char *GetFilename(const char *header);
word FileDialogCommand(void);
void AddDLWToFilename(void);

#endif
