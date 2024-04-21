#ifndef QUEST_H
#define QUEST_H

#include "jamultypes.h"
#include "mgldraw.h"
#include "randomizer.h"


#define QUEST_HILL	0	// save halloween hill (win the game)
#define QUEST_TREES	1	// kill terror trees
#define QUEST_CAT	2	// find the cat
#define	QUEST_BOOTS	3	// buy muck-walking shoes
#define QUEST_SHROOM 4	// find 10 mushrooms
#define QUEST_DAISY	5	// get the doom daisy
#define QUEST_DARK	6	// light the torch
#define QUEST_ZOMBIES 7	// kill the zombies
#define QUEST_CROPS	8	// kill pumpkins
#define QUEST_ORBS	9	// enter mystery lake
#define QUEST_FRANK 10	// complete mystery lake
#define QUEST_WITCH 11	// kill the happy stick witch
#define QUEST_FARLEY 12	// hire farley as a sidekick
#define QUEST_SILVER 13	// get silver slingshot
#define QUEST_WOLF	14	// kill Larry werewolf
#define QUEST_GHOST 15	// get ghost potion
#define QUEST_TOWER 16	// complete the haunted tower
#define QUEST_BUSTS	17	// get vampire busts
#define QUEST_RESCUE 18	// get the girl out of the cave
#define QUEST_DOLLS	19	// collect the dolls

#define NUM_QUESTS 20

char *QuestName(byte quest);

extern RandoItem randoReward[11];

typedef struct Convo
{
	public:
		char line[4][64];
		byte action,next;
} Convo;

void BeginChatting(byte tag);
void UpdateChat(void);
void RenderChat(MGLDraw *mgl);
void GiveRandoItem(int index);
void LoadRandoItems();

#endif
