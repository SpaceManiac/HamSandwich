#include "winpch.h"
#include "levelscan.h"
#include "monster.h"
#include "sound.h"
#include "vars.h"
#include "shop.h"
#include "control.h"
#include "appdata.h"
#include "player.h"
#include "string_extras.h"

static SDL_RWops *scanF;

static char lvlFlagName[][16]={
	"Snowing",
	"Raining",
	"Hub Level",
	"Secret Level",
	"Torch Lit",
	"Lantern Lit",
	"Star Background",
	"Underwater",
	"Underlava",
	"Stealth",
};

static char bulletName[][20]={
	"Anything",
	"Hammer",
	"Bouncy Hammer",
	"Missile",
	"Flame",
	"Laser",
	"Acid",
	"Cherry Bomb",
	"Explosion",
	"Red Bullet",
	"Megabeam Source",
	"Megabeam Part",
	"Megabeam End",
	"Evil Flame",
	"Spore",
	"Mushroom",
	"Grenade",
	"Grenade Boom",
	"SDZ Shockwave",
	"Missile Boom",
	"Snowball",
	"Big Snowball",
	"Ice Spike",
	"Rock",
	"Cactus Spine",
	"Evil Hammer",
	"Power Shell",
	"Earsplitter Boom",
	"Lightning",
	"Spear",
	"Machete",
	"Landmine",
	"Evil Spear",
	"Orbiter",
	"Green Bullet",
	"Ball Lightning",
	"Zap Wand Shock",
	"Mind Control",
	"Reflect Shield",
	"Swap Gun",
	"Water Shot",
	"Orbit Bomber",
	"Harpoon",
	"Scanner",
	"Scanner Shot",
	"Torpedo",
	"Dirt Spike",
	"Paper",
	"Scanner Lock",
	"Bubble",
	"Freeze Ray",
	"Bubble Pop",
	"Harmless Boom",
	"Cheese Hammer",
	"Evil Freeze",
	"Lunachick Ray",
	"Bouncy Lunachick",
	"Life Blip",
	"Energy Blip",
	"Sitting Flame",
	"Floaty Flame",
	"Flamebringer Shot",
	"Black Hole Shot",
	"Black Hole",
	"Evil Green Bullet",
	"Evil Sitting Flame",
};

void PrintFX(word flags)
{
	if(flags&EF_NOFX)
		SDL_RWprintf(scanF," (noFX)\n");
	else
		SDL_RWprintf(scanF,"\n");
}

void PrintLessMore(word flags)
{
	if(flags&TF_LESS)
		SDL_RWprintf(scanF," or less");
	else if(flags&TF_MORE)
		SDL_RWprintf(scanF," or more");
	else
		SDL_RWprintf(scanF," exactly");
}

void Scan_Trigger(world_t *world,Map *map,int num,trigger_t *me,char *effText)
{
	SDL_RWprintf(scanF,"TRG%d: ",num);

	if(me->flags&TF_NOT) SDL_RWprintf(scanF,"NOT ");
	switch(me->type)
	{
		case TRG_STEP:
			SDL_RWprintf(scanF,"If %s steps within %d tiles of (%03d,%03d)",MonsterName(me->value),me->value2,me->x,me->y);
			break;
		case TRG_STEPRECT:
			SDL_RWprintf(scanF,"If %s steps inside (%03d,%03d)-(%03d,%03d)",MonsterName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case TRG_HAVEITEM:
			SDL_RWprintf(scanF,"If player has %d of item %s",me->value2,GetItem(me->value)->name);
			PrintLessMore(me->flags);
			break;
		case TRG_PASSLEVELS:
			SDL_RWprintf(scanF,"If player has passed %d levels",me->value);
			PrintLessMore(me->flags);
			break;
		case TRG_PASSLEVEL:
			SDL_RWprintf(scanF,"If player has passed \"%s\"",world->map[me->value]->name);
			break;
		case TRG_SHOOT:
			SDL_RWprintf(scanF,"If item/wall at (%03d,%03d) is hit by ",me->x,me->y);
			if(me->flags&TF_LESS)
				SDL_RWprintf(scanF,"Good");
			else if(me->flags&TF_MORE)
				SDL_RWprintf(scanF,"Evil");
			else
				SDL_RWprintf(scanF,"Any");
			SDL_RWprintf(scanF," shots");
			break;
		case TRG_TIMED:
			SDL_RWprintf(scanF,"Trigger once every %0.2f seconds after %0.2f seconds elapse",(float)me->value/30.0f,(float)me->value2/30.0f);
			break;
		case TRG_DELAY:
			SDL_RWprintf(scanF,"Trigger continuously after %0.2f seconds have passed",(float)me->value/30.0f);
			break;
		case TRG_MONSTER:
			SDL_RWprintf(scanF,"If there are %d of monster %s",me->value2,MonsterName(me->value));
			PrintLessMore(me->flags);
			break;
		case TRG_CHAIN:
			SDL_RWprintf(scanF,"If the special at (%03d,%03d) is triggered",me->x,me->y);
			break;
		case TRG_KILL:
			SDL_RWprintf(scanF,"If a %s is beaten",MonsterName(me->value));
			break;
		case TRG_VAR:
			SDL_RWprintf(scanF,"If variable %s is %d",VarName(me->value),me->value2);
			PrintLessMore(me->flags);
			break;
		case TRG_FLOOR:
			SDL_RWprintf(scanF,"If the floor at (%03d,%03d) is %03d",me->x,me->y,me->value);
			break;
		case TRG_RANDOM:
			SDL_RWprintf(scanF,"Random chance: %0.2f%%",(float)me->value/(float)FIXAMT);
			break;
		case TRG_FLOORRECT:
			SDL_RWprintf(scanF,"If the floor at (%03d,%03d)-(%03d,%03d) is entirely %03d",me->x,me->y,((word)me->value2)%256,((word)me->value2)/256,me->value);
			break;
		case TRG_LIFE:
			SDL_RWprintf(scanF,"If %s at (%03d,%03d) has %d life",MonsterName(me->value),me->x,me->y,me->value2);
			PrintLessMore(me->flags);
			break;
		case TRG_STEPTILE:
			SDL_RWprintf(scanF,"If %s steps on the tile %03d",MonsterName(me->value),me->value2);
			break;
		case TRG_GETITEM:
			SDL_RWprintf(scanF,"If player gets item at (%03d,%03d)",me->x,me->y);
			break;
		case TRG_ITEM:
			SDL_RWprintf(scanF,"If the item at (%03d,%03d) is %s",me->x,me->y,GetItem(me->value)->name);
			break;
		case TRG_AWAKE:
			SDL_RWprintf(scanF,"If %s at (%03d,%03d) is ",MonsterName(me->value),me->x,me->y);
			if(me->flags&TF_LESS)
				SDL_RWprintf(scanF,"all awake");
			else if(me->flags&TF_MORE)
				SDL_RWprintf(scanF,"all asleep");
			else
				SDL_RWprintf(scanF,"some awake");
			break;
		case TRG_ITEMS:
			SDL_RWprintf(scanF,"If there are %d of item %s",me->value2,GetItem(me->value)->name);
			PrintLessMore(me->flags);
			break;
		case TRG_COMPMAP:
			SDL_RWprintf(scanF,"If map at (%03d,%03d)-(%03d,%03d) matches (%03d,%03d)-(%03d,%03d)",me->x,me->y,((word)me->value2)%256,((word)me->value2)/256,
						((word)me->value)%256,((word)me->value)/256,(((word)me->value)%256)+((((word)me->value2)%256)-me->x),
						(((word)me->value)/256)+((((word)me->value2)/256)-me->y));
			if(me->flags&TF_LESS)
				SDL_RWprintf(scanF,"(ignoring monsters)");
			else
				SDL_RWprintf(scanF,"exactly");
			break;
		case TRG_COMPVAR:
			SDL_RWprintf(scanF,"If var %s is var %s",VarName(me->value),VarName(me->value2));
			PrintLessMore(me->flags);
			break;
		case TRG_MONSINRECT:
			SDL_RWprintf(scanF,"If any %s are in (%03d,%03d)-(%03d,%03d)",MonsterName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case TRG_ITEMRECT:
			SDL_RWprintf(scanF,"If any %s are in (%03d,%03d)-(%03d,%03d)",GetItem(me->value)->name,me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case TRG_DIFFICULTY:
			SDL_RWprintf(scanF,"If difficulty is ");
			if(me->value==0)
				SDL_RWprintf(scanF,"Normal");
			else if(me->value==1)
				SDL_RWprintf(scanF,"Hard");
			else
				SDL_RWprintf(scanF,"Lunatic");
			PrintLessMore(me->flags);
			break;
		case TRG_KEYPRESS:
			SDL_RWprintf(scanF,"If player ");
			if(me->flags&TF_LESS)
				SDL_RWprintf(scanF,"taps ");
			else if(me->flags&TF_MORE)
				SDL_RWprintf(scanF,"releases ");
			else
				SDL_RWprintf(scanF,"is holding ");

			switch(me->value)
			{
				case CONTROL_UP:
					SDL_RWprintf(scanF,"Up");
					break;
				case CONTROL_DN:
					SDL_RWprintf(scanF,"Down");
					break;
				case CONTROL_LF:
					SDL_RWprintf(scanF,"Left");
					break;
				case CONTROL_RT:
					SDL_RWprintf(scanF,"Right");
					break;
				case CONTROL_B1:
					SDL_RWprintf(scanF,"Fire");
					break;
				case CONTROL_B2:
					SDL_RWprintf(scanF,"Special");
					break;
			}
			break;
		case TRG_PLAYAS:
			SDL_RWprintf(scanF,"If player is playing as ");
			if(me->value==PLAY_BOUAPHA)
				SDL_RWprintf(scanF,"Bouapha");
			else if(me->value==PLAY_LUNATIC)
				SDL_RWprintf(scanF,"Lunatic");
			else if(me->value==PLAY_HAPPY)
				SDL_RWprintf(scanF,"Happy Stick Man");
			else if(me->value==PLAY_SHROOM)
				SDL_RWprintf(scanF,"Shtupid Shroom");
			else if(me->value==PLAY_LUNACHIK)
				SDL_RWprintf(scanF,"Lunachick");
			else if(me->value==PLAY_MECHA)
				SDL_RWprintf(scanF,"Mechabouapha");
			break;
		case TRG_MONSCOLOR:
			SDL_RWprintf(scanF,"If %s at (%03d,%03d) is painted %d",MonsterName(me->value),me->x,me->y,me->value2);
			break;
		case TRG_EQUATION:
			SDL_RWprintf(scanF,"If \"%s\" is %d",effText,me->value);
			PrintLessMore(me->flags);
			break;
		case TRG_EQUVAR:
			SDL_RWprintf(scanF,"If \"%s\" is var %s",effText,VarName(me->value));
			PrintLessMore(me->flags);
			break;
	}
	if(me->flags&TF_AND)
		SDL_RWprintf(scanF," AND\n");
	else
		SDL_RWprintf(scanF," OR\n");
}

void Scan_Effect(world_t *world,Map *map,int num,effect_t *me)
{
	SDL_RWprintf(scanF,"EFF%d: ",num);
	switch(me->type)
	{
		case EFF_NONE:
			break;
		case EFF_MESSAGE:
			SDL_RWprintf(scanF,"Message \"%s\"",me->text);
			if (me->flags & EF_TOGGLE)
				SDL_RWprintf(scanF, " (big)");
			PrintFX(me->flags);
			break;
		case EFF_SOUND:
			SDL_RWprintf(scanF,"Sound Effect \"%s\"\n",GetSoundInfo(me->value)->name);
			break;
		case EFF_SONG:
			SDL_RWprintf(scanF,"Play Song \"%s\"\n",me->text);
			break;
		case EFF_WINLEVEL:
			SDL_RWprintf(scanF,"Win level and go to \"%s\" at (%03d,%03d)\n",world->map[me->value]->name,me->x,me->y);
			break;
		case EFF_GOTOMAP:
			SDL_RWprintf(scanF,"Go to level \"%s\" at (%03d,%03d)\n",world->map[me->value]->name,me->x,me->y);
			break;
		case EFF_TELEPORT:
			SDL_RWprintf(scanF,"Teleport ");
			if(me->flags&EF_PLAYER)
				SDL_RWprintf(scanF,"player");
			else if(me->flags&EF_TAGGED)
				SDL_RWprintf(scanF,"tagged");
			else
				SDL_RWprintf(scanF,"target");
			SDL_RWprintf(scanF," to (%03d,%03d)",me->x,me->y);
			PrintFX(me->flags);
			break;
		case EFF_CHANGETILE:
			if(me->flags&EF_TOGGLE)
				SDL_RWprintf(scanF,"Toggle ");
			else
				SDL_RWprintf(scanF,"Change ");

			if(me->flags&EF_CONTIGUOUS)
				SDL_RWprintf(scanF,"touching tiles");
			else if(me->flags&EF_ALL)
				SDL_RWprintf(scanF,"all same tiles");
			else
				SDL_RWprintf(scanF,"single tile");

			SDL_RWprintf(scanF," at (%03d,%03d) to floor %03d and wall %03d",me->x,me->y,me->value,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_SUMMON:
			SDL_RWprintf(scanF,"Summon %s at (%03d,%03d) with item %s",MonsterName(me->value),me->x,me->y,GetItem(me->value2)->name);
			PrintFX(me->flags);
			break;
		case EFF_LIGHT:
			SDL_RWprintf(scanF,"Make ");
			if(me->flags&EF_PERMLIGHT)
				SDL_RWprintf(scanF,"permanent");
			else
				SDL_RWprintf(scanF,"temporary");
			SDL_RWprintf(scanF," light at (%03d,%03d) at brightness %d and radius %d",me->x,me->y,me->value,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_PICTURE:
			SDL_RWprintf(scanF,"Show image %s (mode: ",me->text);
			switch(me->value)
			{
				case TEXTFILE_NORMAL:
					SDL_RWprintf(scanF,"Standard");
					break;
				case TEXTFILE_YERFDOG:
					SDL_RWprintf(scanF,"Yerfdog");
					break;
				case TEXTFILE_COMPUTER:
					SDL_RWprintf(scanF,"Computer");
					break;
			}
			SDL_RWprintf(scanF,")");
			PrintFX(me->flags);
			break;
		case EFF_ITEM:
			if(me->flags&EF_TOGGLE)
				SDL_RWprintf(scanF,"Toggle ");
			else
				SDL_RWprintf(scanF,"Change ");

			if(me->flags&EF_CONTIGUOUS)
				SDL_RWprintf(scanF,"touching items");
			else if(me->flags&EF_ALL)
				SDL_RWprintf(scanF,"all same items");
			else
				SDL_RWprintf(scanF,"single item");

			SDL_RWprintf(scanF," at (%03d,%03d) to %s",me->x,me->y,GetItem(me->value)->name);
			PrintFX(me->flags);
			break;
		case EFF_SWAPMAP:
			if(me->flags&EF_TOGGLE)
				SDL_RWprintf(scanF,"Swap ");
			else
				SDL_RWprintf(scanF,"Copy ");

			SDL_RWprintf(scanF,"map from (%03d,%03d)-(%03d,%03d) to (%03d,%03d)-(%03d,%03d)\n",me->x,me->y,((word)me->value)%256,((word)me->value)/256,
					((word)me->value2)%256,((word)me->value2)/256,
				(((word)me->value2)%256)+((((word)me->value)%256)-me->x),
				(((word)me->value2)/256)+((((word)me->value)/256)-me->y));
			break;
		case EFF_KILLMONS:
			SDL_RWprintf(scanF,"Destroy %s at (%03d,%03d)",MonsterName(me->value),me->x,me->y);
			PrintFX(me->flags);
			break;
		case EFF_CHANGEMONS:
			SDL_RWprintf(scanF,"Change %s at (%03d,%03d) to %s",MonsterName(me->value),me->x,me->y,MonsterName(me->value2));
			PrintFX(me->flags);
			break;
		case EFF_CHANGETEAM:
			SDL_RWprintf(scanF,"Change %s at (%03d,%03d) to team ",MonsterName(me->value),me->x,me->y);
			switch(me->value2)
			{
				case 0:
					SDL_RWprintf(scanF,"Good");
					break;
				case 1:
					SDL_RWprintf(scanF,"Evil");
					break;
				case 2:
					SDL_RWprintf(scanF,"Toggle");
					break;
			}
			PrintFX(me->flags);
			break;
		case EFF_DELETESPCL:
			SDL_RWprintf(scanF,"Delete special at (%03d,%03d)\n",me->x,me->y);
			break;
		case EFF_VAR:
			SDL_RWprintf(scanF,"Set var %s",VarName(me->value));
			SDL_RWprintf(scanF," to \"%s\"\n",me->text);
			break;
		case EFF_LIGHTRECT:
			SDL_RWprintf(scanF,"Make ");
			if(me->flags&EF_PERMLIGHT)
				SDL_RWprintf(scanF,"permanent");
			else
				SDL_RWprintf(scanF,"temporary");
			SDL_RWprintf(scanF," light in (%03d,%03d)-(%03d,%03d) at brightness %d",me->x,me->y,((word)me->value)%256,((word)me->value)/256,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_LEVELFLAG:
			SDL_RWprintf(scanF,"Change level flag %s to ",lvlFlagName[me->value]);
			switch(me->value2)
			{
				case 0:
					SDL_RWprintf(scanF,"On\n");
					break;
				case 1:
					SDL_RWprintf(scanF,"Off\n");
					break;
				case 2:
					SDL_RWprintf(scanF,"Toggle\n");
					break;
			}
			break;
		case EFF_OLDTOGGLE:
			SDL_RWprintf(scanF,"Oldtoggle tiles at (%03d,%03d) to floor %03d and wall %03d",me->x,me->y,me->value,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_LIFE:
			SDL_RWprintf(scanF,"Set ");
			if(me->flags&EF_PERMLIGHT)
				SDL_RWprintf(scanF,"max");
			else
				SDL_RWprintf(scanF,"current");
			SDL_RWprintf(scanF," life of %s at (%03d,%03d) to %d",MonsterName(me->value),me->x,me->y,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_WEAPON:
			SDL_RWprintf(scanF,"Force player's weapon to %s and ",WeaponName(me->value));
			if(me->value2==0)
				SDL_RWprintf(scanF,"don't");
			else
				SDL_RWprintf(scanF,"do");
			SDL_RWprintf(scanF," reload if same\n");
			break;
		case EFF_TAGTARGET:
			SDL_RWprintf(scanF,"Tag the targeted monster\n");
			break;
		case EFF_TAGMONS:
			SDL_RWprintf(scanF,"Tag %s at (%03d,%03d)\n",MonsterName(me->value),me->x,me->y);
			break;
		case EFF_MONSITEM:
			SDL_RWprintf(scanF,"Give %s at (%03d,%03d) the item ",MonsterName(me->value),me->x,me->y);
			if(me->value2==ITM_RANDOM)
				SDL_RWprintf(scanF,"Random");
			else
				SDL_RWprintf(scanF,"%s",GetItem(me->value2)->name);
			PrintFX(me->flags);
			break;
		case EFF_TILEVAR:
			SDL_RWprintf(scanF,"Change ");

			if(me->flags&EF_CONTIGUOUS)
				SDL_RWprintf(scanF,"touching tiles");
			else if(me->flags&EF_ALL)
				SDL_RWprintf(scanF,"all same tiles");
			else
				SDL_RWprintf(scanF,"single tile");

			SDL_RWprintf(scanF," at (%03d,%03d) to floor %s",me->x,me->y,VarName(me->value));
			PrintFX(me->flags);
			break;
		case EFF_LIFEAMT:
			SDL_RWprintf(scanF,"Change life of %s at (%03d,%03d) by %d",MonsterName(me->value),me->x,me->y,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_AI:
			SDL_RWprintf(scanF,"Change %s at (%03d,%03d) to AI of %s",MonsterName(me->value),me->x,me->y,MonsterName(me->value2));
			PrintFX(me->flags);
			break;
		case EFF_NAME:
			SDL_RWprintf(scanF,"Change name of %s at (%03d,%03d) to \"%s\"\n",MonsterName(me->value),me->x,me->y,me->text);
			break;
		case EFF_COLOR:
			SDL_RWprintf(scanF,"Color swap %s at (%03d,%03d) from %d to %d\n",MonsterName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case EFF_MONSBRIGHT:
			SDL_RWprintf(scanF,"Change brightness of %s at (%03d,%03d) to %d\n",MonsterName(me->value),me->x,me->y,me->value2);
			break;
		case EFF_PLAYAS:
			SDL_RWprintf(scanF,"Force player to play as ");
			if(me->value==PLAY_BOUAPHA)
				SDL_RWprintf(scanF,"Bouapha\n");
			else if(me->value==PLAY_LUNATIC)
				SDL_RWprintf(scanF,"Lunatic\n");
			else if(me->value==PLAY_HAPPY)
				SDL_RWprintf(scanF,"Happy Stick Man\n");
			else if(me->value==PLAY_SHROOM)
				SDL_RWprintf(scanF,"Shtupid Shroom\n");
			else if(me->value==PLAY_LUNACHIK)
				SDL_RWprintf(scanF,"Lunachick\n");
			else if(me->value==PLAY_MECHA)
				SDL_RWprintf(scanF,"Mechabouapha\n");
			break;
		case EFF_MONSGRAPHICS:
			SDL_RWprintf(scanF,"Change graphics of %s at (%03d,%03d) to %s",MonsterName(me->value),me->x,me->y,me->text);
			PrintFX(me->flags);
			break;
		case EFF_ITEMGRAPHICS:
			SDL_RWprintf(scanF,"Set custom item graphics to %s\n",me->text);
			break;
		case EFF_VARBAR:
			SDL_RWprintf(scanF,"Set ");
			if(me->flags&EF_PERMLIGHT)
				SDL_RWprintf(scanF,"max");
			else
				SDL_RWprintf(scanF,"current");
			SDL_RWprintf(scanF," varbar to %s (color %d)\n",VarName(me->value),((word)me->value2)%256);
			break;
		case EFF_MAKEBULLET:
			SDL_RWprintf(scanF,"Summon bullet %s at (%03d,%03d) facing \"%s\"\n",bulletName[me->value2], me->x, me->y, me->text);
			break;
		default:
			SDL_RWprintf(scanF,"Unhandled effect type %d\n",me->type);
			break;
	}
}

void Scan_Special(world_t *world,Map *map,int num,special_t *me)
{
	int i;

	SDL_RWprintf(scanF,"#%03d (%03d,%03d) -----------\n",num,me->x,me->y);

	// triggers
	for(i=0;i<NUM_TRIGGERS;i++)
	{
		if(me->trigger[i].type!=TRG_NONE)
			Scan_Trigger(world,map,i,&me->trigger[i],me->effect[i].text);
	}
	SDL_RWprintf(scanF,"	  ----\n");
	// effects
	for(i=0;i<NUM_EFFECTS;i++)
	{
		if(me->effect[i].type!=EFF_NONE)
			Scan_Effect(world,map,i,&me->effect[i]);
	}
	SDL_RWprintf(scanF,"\n");
}

void Scan_Badguy(world_t *world,Map *map,int num,mapBadguy_t *me)
{
	SDL_RWprintf(scanF,"#%03d (%03d,%03d) %s (item: ",num,me->x,me->y,MonsterName(me->type));
	switch(me->item)
	{
		case ITM_RANDOM:
			SDL_RWprintf(scanF,"random");
			break;
		case ITM_NONE:
			SDL_RWprintf(scanF,"none");
			break;
		default:
			SDL_RWprintf(scanF,"%s",GetItem(me->item)->name);
			break;
	}
	SDL_RWprintf(scanF,")\n");
}

byte Scan_Level(world_t *world,Map *map)
{
	int i;
	word itemCount[MAX_ITEMS];
	int totalMons;
	char s[64];

	sprintf(s,"level%s.txt",map->name);
	auto f = AppdataOpen_Write(s);
	scanF = f.get();
	if(!scanF)
		return 0;

	SDL_RWprintf(scanF,"World: %s\nLevel: %s\n\nBADGUY COUNT\n---------\n",world->map[0]->name,map->name);

	totalMons=0;
	for(i=0;i<NUM_MONSTERS;i++)
		itemCount[i]=0;

	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			itemCount[map->badguy[i].type]++;
			totalMons++;
		}
	}

	SDL_RWprintf(scanF,"Total monsters: %d\n\n",totalMons);
	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(itemCount[i]>0)
			SDL_RWprintf(scanF,"%s: %d\n",MonsterName(i),itemCount[i]);
	}

	totalMons=0;
	memset(itemCount,0,MAX_ITEMS*sizeof(word));
	for(i=0;i<map->width*map->height;i++)
	{
		if(map->map[i].item!=ITM_NONE)
		{
			itemCount[map->map[i].item]++;
			totalMons++;
		}
	}
	SDL_RWprintf(scanF,"\nITEM COUNT\n---------\n");
	SDL_RWprintf(scanF,"Total items: %d\n\n",totalMons);
	for(i=0;i<MAX_ITEMS;i++)
	{
		if(itemCount[i]>0)
			SDL_RWprintf(scanF,"%s: %d\n",GetItem(i)->name,itemCount[i]);
	}

	SDL_RWprintf(scanF,"\nSPECIALS IN DETAIL\n---------\n");
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].x!=255)
			Scan_Special(world,map,i,&map->special[i]);
	}
	SDL_RWprintf(scanF,"\nBADGUYS IN DETAIL\n---------\n");
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			Scan_Badguy(world,map,i,&map->badguy[i]);
		}
	}

	scanF = nullptr;
	f.reset();
	AppdataSync();
	return 1;
}

static word var_checks[16],var_sets[16],tileVars;

void Find_Text_Vars(char *txt,byte check)
{
	int i;

	for(i=0;i<(int)strlen(txt)-1;i++)
	{
		if((txt[i]=='G' || txt[i]=='g') && txt[i+1]>='0' && txt[i+1]<='7')
		{
			if(check)
				var_checks[(txt[i+1]-'0')]++;
			else
				var_sets[(txt[i+1]-'0')]++;
		}
		if((txt[i]=='V' || txt[i]=='v') && txt[i+1]>='0' && txt[i+1]<='7')
		{
			if(check)
				var_checks[(txt[i+1]-'0')+VAR_LOCAL]++;
			else
				var_sets[(txt[i+1]-'0')+VAR_LOCAL]++;
		}
	}
}

byte Scan_Vars(world_t *world)
{
	int i,j,k;

	auto f = AppdataOpen_Write("var_scan.txt");
	scanF = f.get();
	if(!scanF)
		return 0;

	SDL_RWprintf(scanF,"World: %s\n\n\nVAR REFERENCES\n---------\n",world->map[0]->name);

	for(i=0;i<16;i++)
	{
		var_checks[i]=0;
		var_sets[i]=0;
	}
	tileVars=0;

	for(i=0;i<world->numMaps;i++)
	{
		for(j=0;j<MAX_SPECIAL;j++)
		{
			if(world->map[i]->special[j].x!=255)
			{
				for(k=0;k<NUM_TRIGGERS;k++)
				{
					switch(world->map[i]->special[j].trigger[k].type)
					{
						case TRG_COMPVAR: // value is one, compared to value2
							var_checks[world->map[i]->special[j].trigger[k].value2]++;
							[[fallthrough]];
						case TRG_VAR:	// value
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							break;
						case TRG_EQUVAR:	// value, and a text thing
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							[[fallthrough]];
						case TRG_EQUATION:	// text thing
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							break;
					}
				}
				for(k=0;k<NUM_EFFECTS;k++)
				{
					switch(world->map[i]->special[j].effect[k].type)
					{
						case EFF_VAR:	// sets value, reads text
							var_sets[world->map[i]->special[j].effect[k].value]++;
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							break;
						case EFF_TILEVAR:	// sets a tilevar, checks V
							tileVars++;
							var_checks[world->map[i]->special[j].effect[k].value]++;
							break;
						case EFF_ITEM:	// changes items, said items might change variables
							if(world->map[i]->special[j].effect[k].value!=ITM_NONE &&
								(GetItem(world->map[i]->special[j].effect[k].value)->effect==IE_INCVAR ||
								GetItem(world->map[i]->special[j].effect[k].value)->effect==IE_DECVAR))
							{
								var_sets[GetItem(world->map[i]->special[j].effect[k].value)->effectAmt]++;
							}
							break;
						case EFF_SUMMON:	// summon a monster, who might hold a var-changing item
						case EFF_MONSITEM:	// give a monster a possibly var-changing item
							if(world->map[i]->special[j].effect[k].value2!=ITM_NONE &&
								world->map[i]->special[j].effect[k].value2!=ITM_RANDOM &&
								(GetItem(world->map[i]->special[j].effect[k].value2)->effect==IE_INCVAR ||
								GetItem(world->map[i]->special[j].effect[k].value2)->effect==IE_DECVAR))
							{
								var_sets[GetItem(world->map[i]->special[j].effect[k].value2)->effectAmt]++;
							}
							break;
					}
				}
			}
		}
		// find var change items on the map
		for(j=0;j<world->map[i]->width*world->map[i]->height;j++)
		{
			if(GetItem(world->map[i]->map[j].item)!=NULL &&
				(GetItem(world->map[i]->map[j].item)->effect==IE_INCVAR ||
				GetItem(world->map[i]->map[j].item)->effect==IE_DECVAR))
				var_sets[GetItem(world->map[i]->map[j].item)->effectAmt]++;
		}
		// find var change items held by monsters
		for(j=0;j<MAX_MAPMONS;j++)
		{
			if(world->map[i]->badguy[j].type!=MONS_NONE && GetItem(world->map[i]->badguy[j].item)!=NULL &&
				(GetItem(world->map[i]->badguy[j].item)->effect==IE_INCVAR ||
				GetItem(world->map[i]->badguy[j].item)->effect==IE_DECVAR))
				var_sets[GetItem(world->map[i]->badguy[j].item)->effectAmt]++;
		}
	}

	SDL_RWprintf(scanF,"Tile vars: %d\n\nVars being checked:\n",tileVars);
	for(i=0;i<16;i++)
	{
		if(var_checks[i]>0)
		{
			if(i<8)
				SDL_RWprintf(scanF,"G%d: %d\n",i,var_checks[i]);
			else
				SDL_RWprintf(scanF,"V%d: %d\n",i-8,var_checks[i]);
		}
	}
	SDL_RWprintf(scanF,"\nVars being set:\n");
	for(i=0;i<16;i++)
	{
		if(var_sets[i]>0)
		{
			if(i<8)
				SDL_RWprintf(scanF,"G%d: %d\n",i,var_sets[i]);
			else
				SDL_RWprintf(scanF,"V%d: %d\n",i-8,var_sets[i]);
		}
	}

	SDL_RWprintf(scanF,"\nLEVEL DETAILS\n----------------\n\n");

	for(i=0;i<world->numMaps;i++)
	{
		SDL_RWprintf(scanF,"Level %d: %s -----------------\n\n",i,world->map[i]->name);

		for(j=0;j<16;j++)
		{
			var_checks[j]=0;
			var_sets[j]=0;
		}
		tileVars=0;

		for(j=0;j<MAX_SPECIAL;j++)
		{
			if(world->map[i]->special[j].x!=255)
			{
				for(k=0;k<NUM_TRIGGERS;k++)
				{
					switch(world->map[i]->special[j].trigger[k].type)
					{
						case TRG_COMPVAR: // value is one, compared to value2
							var_checks[world->map[i]->special[j].trigger[k].value2]++;
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Compare %s to %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].trigger[k].value),VarName(world->map[i]->special[j].trigger[k].value2));
							break;
						case TRG_VAR:	// value
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Check value of %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].trigger[k].value));
							break;
						case TRG_EQUVAR:	// value, and a text thing
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Equation compare \"%s\" to %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									world->map[i]->special[j].effect[k].text,VarName(world->map[i]->special[j].trigger[k].value));
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							break;
						case TRG_EQUATION:	// text thing
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Equation check \"%s\"\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									world->map[i]->special[j].effect[k].text);
							break;
					}
				}
				for(k=0;k<NUM_EFFECTS;k++)
				{
					switch(world->map[i]->special[j].effect[k].type)
					{
						case EFF_VAR:	// sets value, reads text
							var_sets[world->map[i]->special[j].effect[k].value]++;
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Sets %s to \"%s\"\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].effect[k].value),world->map[i]->special[j].effect[k].text);
							break;
						case EFF_TILEVAR:	// sets a tilevar, checks V
							tileVars++;
							var_checks[world->map[i]->special[j].effect[k].value]++;
							SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Tilevar reads %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].effect[k].value));
							break;
						case EFF_ITEM:	// changes items, said items might change variables
							if(world->map[i]->special[j].effect[k].value!=ITM_NONE &&
								(GetItem(world->map[i]->special[j].effect[k].value)->effect==IE_INCVAR ||
								GetItem(world->map[i]->special[j].effect[k].value)->effect==IE_DECVAR))
							{
								var_sets[GetItem(world->map[i]->special[j].effect[k].value)->effectAmt]++;
								SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Creates item that incs/decs %s\n",j,
									world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(GetItem(world->map[i]->special[j].effect[k].value)->effectAmt));
							}
							break;
						case EFF_SUMMON:	// summon a monster, who might hold a var-changing item
							if(world->map[i]->special[j].effect[k].value2!=ITM_NONE &&
								world->map[i]->special[j].effect[k].value2!=ITM_RANDOM &&
								(GetItem(world->map[i]->special[j].effect[k].value2)->effect==IE_INCVAR ||
								GetItem(world->map[i]->special[j].effect[k].value2)->effect==IE_DECVAR))
							{
								var_sets[GetItem(world->map[i]->special[j].effect[k].value2)->effectAmt]++;
								SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Summons monster holding item that incs/decs %s\n",j,
									world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(GetItem(world->map[i]->special[j].effect[k].value2)->effectAmt));
							}
							break;
						case EFF_MONSITEM:	// give a monster a possibly var-changing item
							if(world->map[i]->special[j].effect[k].value2!=ITM_NONE &&
								world->map[i]->special[j].effect[k].value2!=ITM_RANDOM &&
								(GetItem(world->map[i]->special[j].effect[k].value2)->effect==IE_INCVAR ||
								GetItem(world->map[i]->special[j].effect[k].value2)->effect==IE_DECVAR))
							{
								var_sets[GetItem(world->map[i]->special[j].effect[k].value2)->effectAmt]++;
								SDL_RWprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Changes monster item to item that incs/decs %s\n",j,
									world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(GetItem(world->map[i]->special[j].effect[k].value2)->effectAmt));
							}
							break;
					}
				}
			}
		}

		// find var change items on the map
		for(j=0;j<world->map[i]->width*world->map[i]->height;j++)
		{
			if(GetItem(world->map[i]->map[j].item)!=NULL &&
				(GetItem(world->map[i]->map[j].item)->effect==IE_INCVAR ||
				GetItem(world->map[i]->map[j].item)->effect==IE_DECVAR))
			{
				var_sets[GetItem(world->map[i]->map[j].item)->effectAmt]++;
				SDL_RWprintf(scanF,"ITM (%03d,%03d): '%s' incs/decs %s\n",j%world->map[i]->width,j/world->map[i]->width,
						GetItem(world->map[i]->map[j].item)->name,VarName(GetItem(world->map[i]->map[j].item)->effectAmt));
			}
		}
		// find var change items held by monsters
		for(j=0;j<MAX_MAPMONS;j++)
		{
			if(world->map[i]->badguy[j].type!=MONS_NONE && GetItem(world->map[i]->badguy[j].item)!=NULL &&
				(GetItem(world->map[i]->badguy[j].item)->effect==IE_INCVAR ||
				GetItem(world->map[i]->badguy[j].item)->effect==IE_DECVAR))
			{
				var_sets[GetItem(world->map[i]->badguy[j].item)->effectAmt]++;
				SDL_RWprintf(scanF,"MONS (%03d,%03d): '%s' holds a '%s' that incs/decs %s\n",j%world->map[i]->width,j/world->map[i]->width,
						MonsterName(world->map[i]->badguy[j].type),
						GetItem(world->map[i]->badguy[j].item)->name,
						VarName(GetItem(world->map[i]->badguy[j].item)->effectAmt));
			}
		}

		SDL_RWprintf(scanF,"Tile vars: %d\n\nVars being checked:\n",tileVars);
		for(j=0;j<16;j++)
		{
			if(var_checks[j]>0)
			{
				if(j<8)
					SDL_RWprintf(scanF,"G%d: %d\n",j,var_checks[j]);
				else
					SDL_RWprintf(scanF,"V%d: %d\n",j-8,var_checks[j]);
			}
		}
		SDL_RWprintf(scanF,"\nVars being set:\n");
		for(j=0;j<16;j++)
		{
			if(var_sets[j]>0)
			{
				if(j<8)
					SDL_RWprintf(scanF,"G%d: %d\n",j,var_sets[j]);
				else
					SDL_RWprintf(scanF,"V%d: %d\n",j-8,var_sets[j]);
			}
		}
		SDL_RWprintf(scanF,"\n");
	}

	scanF = nullptr;
	f.reset();
	AppdataSync();
	return 1;
}
