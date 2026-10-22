#ifndef QUEST_H
#define QUEST_H

#include "jamultypes.h"
#include "mgldraw.h"

#define QUEST_WOOD		0	// collect 10 logs for the Woodsman
#define QUEST_PRESENTS	1	// collect all 99 presents
#define QUEST_PROVE		2	// kill 20 monsters to prove to Kinyova that you're good
#define QUEST_PROVE2	3	// kill 50 more
#define QUEST_HERBS		4	// collect 10 frostbane herbs for the healer
#define QUEST_SHARPEN	5	// find the sharpening stone and return it to the woodsman
#define QUEST_MIMIC		6	// tame mimic
#define QUEST_CRYSTAL	7	// get the crystal of binding
#define QUEST_AXES		8	// collect axes for axes r us
#define QUEST_BOBSWIFE	9	// find Black Market Bob's wife

#define QUEST_BOBSWIFE2 10	// unlock bob's wife
#define QUEST_GURU		11	// Guru find all talents
#define QUEST_RATS		12	// Rat Problem, kill all rats in cabin basement
#define QUEST_UNDIES	13	// Asbestos Underwear
#define QUEST_GLADIATOR 14	// reach Gladiator rank 5
#define QUEST_TOYS		15	// keep toys away from Mickey
#define QUEST_BOOK		16	// return the library book
#define QUEST_LETTER	17	// deliver a letter for Healiana
#define QUEST_ROPES		18	// save ropemaker
#define QUEST_BALLOON	19	// get ropes for balloon

#define QUEST_HANDY		20	// bring tools to handyman
#define QUEST_SOUP		21	// tin cans for ropewife
#define QUEST_BEARS		22	// slay cave bears
#define QUEST_BIGBEAR	23	// slay cave bear boss
#define QUEST_DEVILGRASS 24	// collect devil grass
#define QUEST_DELIVERY	25	// bring Curaga the item from Magic Guy
#define QUEST_TITANS	26	// kill ice hounds
#define QUEST_SKILLS	27	// find every skill
#define QUEST_FACTORY	28	// destroy toy factory
#define QUEST_FACTORY2	29	// destroy factory power core
// here are the 'split quests'
#define QUEST_ALLIES	30	// obtain some allies (join Onion or Bunny)
#define QUEST_QUESTS	31	// do 3 quests for your side
#define QUEST_CLOCKJUNK 32	// clockwork or junksmith
#define QUEST_EASYMINER 33	// candy from a baby or mine stuff
#define QUEST_HIGHWAY	34	// highway robbery or patrol
#define QUEST_SHAKEDOWN 35	// shutdown or shakedown
#define QUEST_SUPPLIES	36	// destroy enemy supply base
#define QUEST_RUMBLE	37	// rumble with enemies
#define QUEST_LEADER	38	// slay the enemy leader
#define QUEST_FURNACE	39  // fix the furnace

#define QUEST_FRAGILE	40	// sounding crystal needed to get one furnace part
#define QUEST_PLANTS	41	// second furnace part by killing gourdzilla
#define QUEST_FOREST	42	// third furnace part by cleansing forest
#define QUEST_BOKBOK	43	// get bokboks for sharonda, earn binding crystal
#define QUEST_HIPPY		44	// collect 20 Seeds
#define QUEST_TITANS2	45	// kill titanic titans
#define QUEST_BARON		46	// kill baron
#define QUEST_TOYS2		47	// collect toy repair items
#define QUEST_CHAMPION  48	// beat final battle in arena
#define QUEST_HAPPYSTICK 49	// kill happy stick bird
#define NUM_QUESTS 50

// quest events
#define QE_GET	 0
#define QE_KILL  1
#define QE_SKILL 2
#define QE_TALENT 3
#define QE_QUEST 4
#define QE_VAR	5

char *QuestName(byte quest);

typedef struct Convo
{
	public:
		char line[180];
		byte action;
		word next;
} Convo;

void BeginChatting(byte tag);
void UpdateChat(byte inShop);
void RenderChat(MGLDraw *mgl);
void RenderChatNoName(MGLDraw *mgl);
void NewChat(word ch);

void PrintQuestInfo(byte quest,int x,int y);
void ExitChatStuff(void);

void QuestReportEvent(byte event,int val);
void ReportQuestReport(byte quest);

#endif
