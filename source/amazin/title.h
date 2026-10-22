#ifndef TITLE_H
#define TITLE_H

#include "jamultypes.h"
#include "jamulspr.h"

enum class QuitAction
{
	None,
	HamumuWebsite,
	BuyOnline,
	RegisterTxt,
};

extern QuitAction g_QuitAction;
extern sprite_set_t* g_ButtonsJsp;

void TitleInit();
void CongratsInit();
void SplashInit(byte which);

#endif // TITLE_H
