#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

class MGLDraw;
struct world_t;

void InitExportDialog(const world_t* world, const char* filename);

void RenderExportDialog(MGLDraw *mgl, int msx, int msy);
bool ExportDialogClick(int msx, int msy);
bool ExportDialogKey(char key);
void ExportDialogScroll(int msz);

#endif
