#ifndef ADDONWEB_H
#define ADDONWEB_H

#include "mgldraw.h"

void Download_Addons(void);
byte Update_Download(MGLDraw *mgl,char key,byte tap,int msx,int msy,byte btn);
void Render_Download(MGLDraw *mgl,int msx,int msy);

#endif
