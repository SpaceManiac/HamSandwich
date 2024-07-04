#include "winpch.h"
#include "levelscan.h"
#include <algorithm>
#include "monster.h"
#include "sound.h"
#include "vars.h"
#include "shop.h"
#include "control.h"
#include "appdata.h"
#include "bullet.h"
#include "player.h"

static FILE *scanF;

void PrintFX(word flags)
{
	if(flags&EF_NOFX)
		fprintf(scanF," (noFX)\n");
	else
		fprintf(scanF,"\n");
}

void PrintLessMore(word flags)
{
	if(flags&TF_LESS)
		fprintf(scanF," or less");
	else if(flags&TF_MORE)
		fprintf(scanF," or more");
	else
		fprintf(scanF," exactly");
}

void Scan_Trigger(world_t *world,Map *map,int num,trigger_t *me,char *effText)
{
	fprintf(scanF,"TRG%d: ",num);

	if(me->flags&TF_NOT) fprintf(scanF,"NOT ");
	switch(me->type)
	{
		case TRG_STEP:
			fprintf(scanF,"If %s steps within %d tiles of (%03d,%03d)",MonsterName(me->value),me->value2,me->x,me->y);
			break;
		case TRG_STEPRECT:
			fprintf(scanF,"If %s steps inside (%03d,%03d)-(%03d,%03d)",MonsterName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case TRG_HAVEITEM:
			fprintf(scanF,"If player has %d of item %s",me->value2,GetItem(me->value)->name);
			PrintLessMore(me->flags);
			break;
		case TRG_PASSLEVELS:
			fprintf(scanF,"If player has passed %d levels",me->value);
			PrintLessMore(me->flags);
			break;
		case TRG_PASSLEVEL:
			fprintf(scanF,"If player has passed \"%s\"",world->map[me->value]->name);
			break;
		case TRG_SHOOT:
			fprintf(scanF,"If item/wall at (%03d,%03d) is hit by ",me->x,me->y);
			if(me->flags&TF_LESS)
				fprintf(scanF,"Good");
			else if(me->flags&TF_MORE)
				fprintf(scanF,"Evil");
			else
				fprintf(scanF,"Any");
			fprintf(scanF," shots");
			break;
		case TRG_TIMED:
			fprintf(scanF,"Trigger once every %0.2f seconds after %0.2f seconds elapse",(float)me->value/30.0f,(float)me->value2/30.0f);
			break;
		case TRG_DELAY:
			fprintf(scanF,"Trigger continuously after %0.2f seconds have passed",(float)me->value/30.0f);
			break;
		case TRG_MONSTER:
			fprintf(scanF,"If there are %d of monster %s",me->value2,MonsterName(me->value));
			PrintLessMore(me->flags);
			break;
		case TRG_CHAIN:
			fprintf(scanF,"If the special at (%03d,%03d) is triggered",me->x,me->y);
			break;
		case TRG_KILL:
			fprintf(scanF,"If a %s is beaten",MonsterName(me->value));
			break;
		case TRG_VAR:
			fprintf(scanF,"If variable %s is %d",VarName(me->value),me->value2);
			PrintLessMore(me->flags);
			break;
		case TRG_FLOOR:
			fprintf(scanF,"If the floor at (%03d,%03d) is %03d",me->x,me->y,me->value);
			break;
		case TRG_RANDOM:
			fprintf(scanF,"Random chance: %0.2f%%",(float)me->value/(float)FIXAMT);
			break;
		case TRG_FLOORRECT:
			fprintf(scanF,"If the floor at (%03d,%03d)-(%03d,%03d) is entirely %03d",me->x,me->y,((word)me->value2)%256,((word)me->value2)/256,me->value);
			break;
		case TRG_LIFE:
			fprintf(scanF,"If %s at (%03d,%03d) has %d life",MonsterName(me->value),me->x,me->y,me->value2);
			PrintLessMore(me->flags);
			break;
		case TRG_STEPTILE:
			fprintf(scanF,"If %s steps on the tile %03d",MonsterName(me->value),me->value2);
			break;
		case TRG_GETITEM:
			fprintf(scanF,"If player gets item at (%03d,%03d)",me->x,me->y);
			break;
		case TRG_ITEM:
			fprintf(scanF,"If the item at (%03d,%03d) is %s",me->x,me->y,GetItem(me->value)->name);
			break;
		case TRG_AWAKE:
			fprintf(scanF,"If %s at (%03d,%03d) is ",MonsterName(me->value),me->x,me->y);
			if(me->flags&TF_LESS)
				fprintf(scanF,"all awake");
			else if(me->flags&TF_MORE)
				fprintf(scanF,"all asleep");
			else
				fprintf(scanF,"some awake");
			break;
		case TRG_ITEMS:
			fprintf(scanF,"If there are %d of item %s",me->value2,GetItem(me->value)->name);
			PrintLessMore(me->flags);
			break;
		case TRG_COMPMAP:
			fprintf(scanF,"If map at (%03d,%03d)-(%03d,%03d) matches (%03d,%03d)-(%03d,%03d)",me->x,me->y,((word)me->value2)%256,((word)me->value2)/256,
						((word)me->value)%256,((word)me->value)/256,(((word)me->value)%256)+((((word)me->value2)%256)-me->x),
						(((word)me->value)/256)+((((word)me->value2)/256)-me->y));
			if(me->flags&TF_LESS)
				fprintf(scanF,"(ignoring monsters)");
			else
				fprintf(scanF,"exactly");
			break;
		case TRG_COMPVAR:
			fprintf(scanF,"If var %s is var %s",VarName(me->value),VarName(me->value2));
			PrintLessMore(me->flags);
			break;
		case TRG_MONSINRECT:
			fprintf(scanF,"If any %s are in (%03d,%03d)-(%03d,%03d)",MonsterName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case TRG_ITEMRECT:
			fprintf(scanF,"If any %s are in (%03d,%03d)-(%03d,%03d)",GetItem(me->value)->name,me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case TRG_DIFFICULTY:
			fprintf(scanF,"If difficulty is %s", GetDifficultyName(me->value));
			PrintLessMore(me->flags);
			break;
		case TRG_KEYPRESS:
			fprintf(scanF,"If player ");
			if(me->flags&TF_LESS)
				fprintf(scanF,"taps ");
			else if(me->flags&TF_MORE)
				fprintf(scanF,"releases ");
			else
				fprintf(scanF,"is holding ");

			switch(me->value)
			{
				case CONTROL_UP:
					fprintf(scanF,"Up");
					break;
				case CONTROL_DN:
					fprintf(scanF,"Down");
					break;
				case CONTROL_LF:
					fprintf(scanF,"Left");
					break;
				case CONTROL_RT:
					fprintf(scanF,"Right");
					break;
				case CONTROL_B1:
					fprintf(scanF,"Fire");
					break;
				case CONTROL_B2:
					fprintf(scanF,"Special");
					break;
			}
			break;
		case TRG_PLAYAS:
			fprintf(scanF,"If player is playing as %s", GetPlayableCharacterName(me->value));
			break;
		case TRG_MONSCOLOR:
			fprintf(scanF,"If %s at (%03d,%03d) is painted %d",MonsterName(me->value),me->x,me->y,me->value2);
			break;
		case TRG_EQUATION:
			fprintf(scanF,"If \"%s\" is %d",effText,me->value);
			PrintLessMore(me->flags);
			break;
		case TRG_EQUVAR:
			fprintf(scanF,"If \"%s\" is var %s",effText,VarName(me->value));
			PrintLessMore(me->flags);
			break;
		case TRG_BULLETRECT:
			fprintf(scanF,"If any %s are in (%03d,%03d)-(%03d,%03d)",BulletName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
	}
	if(me->flags&TF_AND)
		fprintf(scanF," AND\n");
	else
		fprintf(scanF," OR\n");
}

void Scan_Effect(world_t *world,Map *map,int num,effect_t *me)
{
	fprintf(scanF,"EFF%d: ",num);
	switch(me->type)
	{
		case EFF_NONE:
			break;
		case EFF_MESSAGE:
			fprintf(scanF,"Message \"%s\"",me->text);
			if (me->flags & EF_TOGGLE)
				fprintf(scanF, " (big)");
			PrintFX(me->flags);
			break;
		case EFF_SOUND:
			fprintf(scanF,"Sound Effect \"%s\"\n",GetSoundInfo(me->value)->name);
			break;
		case EFF_SONG:
			fprintf(scanF,"Play Song \"%s\"\n",me->text);
			break;
		case EFF_WINLEVEL:
			fprintf(scanF,"Win level and go to \"%s\" at (%03d,%03d)\n",world->map[me->value]->name,me->x,me->y);
			break;
		case EFF_GOTOMAP:
			fprintf(scanF,"Go to level \"%s\" at (%03d,%03d)\n",world->map[me->value]->name,me->x,me->y);
			break;
		case EFF_TELEPORT:
			fprintf(scanF,"Teleport ");
			if(me->flags&EF_PLAYER)
				fprintf(scanF,"player");
			else if(me->flags&EF_TAGGED)
				fprintf(scanF,"tagged");
			else
				fprintf(scanF,"target");
			fprintf(scanF," to (%03d,%03d)",me->x,me->y);
			PrintFX(me->flags);
			break;
		case EFF_CHANGETILE:
			if(me->flags&EF_TOGGLE)
				fprintf(scanF,"Toggle ");
			else
				fprintf(scanF,"Change ");

			if(me->flags&EF_CONTIGUOUS)
				fprintf(scanF,"touching tiles");
			else if(me->flags&EF_ALL)
				fprintf(scanF,"all same tiles");
			else
				fprintf(scanF,"single tile");

			fprintf(scanF," at (%03d,%03d) to floor %03d and wall %03d",me->x,me->y,me->value,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_SUMMON:
			fprintf(scanF,"Summon %s at (%03d,%03d) with item %s",MonsterName(me->value),me->x,me->y,GetItem(me->value2)->name);
			PrintFX(me->flags);
			break;
		case EFF_LIGHT:
			fprintf(scanF,"Make ");
			if(me->flags&EF_PERMLIGHT)
				fprintf(scanF,"permanent");
			else
				fprintf(scanF,"temporary");
			fprintf(scanF," light at (%03d,%03d) at brightness %d and radius %d",me->x,me->y,me->value,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_PICTURE:
			fprintf(scanF,"Show image %s (mode: ",me->text);
			switch(me->value)
			{
				case TEXTFILE_NORMAL:
					fprintf(scanF,"Standard");
					break;
				case TEXTFILE_YERFDOG:
					fprintf(scanF,"Yerfdog");
					break;
				case TEXTFILE_COMPUTER:
					fprintf(scanF,"Computer");
					break;
			}
			fprintf(scanF,")");
			PrintFX(me->flags);
			break;
		case EFF_ITEM:
			if(me->flags&EF_TOGGLE)
				fprintf(scanF,"Toggle ");
			else
				fprintf(scanF,"Change ");

			if(me->flags&EF_CONTIGUOUS)
				fprintf(scanF,"touching items");
			else if(me->flags&EF_ALL)
				fprintf(scanF,"all same items");
			else
				fprintf(scanF,"single item");

			fprintf(scanF," at (%03d,%03d) to %s",me->x,me->y,GetItem(me->value)->name);
			PrintFX(me->flags);
			break;
		case EFF_SWAPMAP:
			if(me->flags&EF_TOGGLE)
				fprintf(scanF,"Swap ");
			else
				fprintf(scanF,"Copy ");

			fprintf(scanF,"map from (%03d,%03d)-(%03d,%03d) to (%03d,%03d)-(%03d,%03d)\n",me->x,me->y,((word)me->value)%256,((word)me->value)/256,
					((word)me->value2)%256,((word)me->value2)/256,
				(((word)me->value2)%256)+((((word)me->value)%256)-me->x),
				(((word)me->value2)/256)+((((word)me->value)/256)-me->y));
			break;
		case EFF_KILLMONS:
			fprintf(scanF,"Destroy %s at (%03d,%03d)",MonsterName(me->value),me->x,me->y);
			PrintFX(me->flags);
			break;
		case EFF_CHANGEMONS:
			fprintf(scanF,"Change %s at (%03d,%03d) to %s",MonsterName(me->value),me->x,me->y,MonsterName(me->value2));
			PrintFX(me->flags);
			break;
		case EFF_CHANGETEAM:
			fprintf(scanF,"Change %s at (%03d,%03d) to team ",MonsterName(me->value),me->x,me->y);
			switch(me->value2)
			{
				case 0:
					fprintf(scanF,"Good");
					break;
				case 1:
					fprintf(scanF,"Evil");
					break;
				case 2:
					fprintf(scanF,"Toggle");
					break;
			}
			PrintFX(me->flags);
			break;
		case EFF_DELETESPCL:
			fprintf(scanF,"Delete special at (%03d,%03d)\n",me->x,me->y);
			break;
		case EFF_VAR:
			fprintf(scanF,"Set var %s",VarName(me->value));
			fprintf(scanF," to \"%s\"\n",me->text);
			break;
		case EFF_LIGHTRECT:
			fprintf(scanF,"Make ");
			if(me->flags&EF_PERMLIGHT)
				fprintf(scanF,"permanent");
			else
				fprintf(scanF,"temporary");
			fprintf(scanF," light in (%03d,%03d)-(%03d,%03d) at brightness %d",me->x,me->y,((word)me->value)%256,((word)me->value)/256,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_LEVELFLAG:
			fprintf(scanF,"Change level flag %s to ",MapFlagName(me->value));
			switch(me->value2)
			{
				case 0:
					fprintf(scanF,"On\n");
					break;
				case 1:
					fprintf(scanF,"Off\n");
					break;
				case 2:
					fprintf(scanF,"Toggle\n");
					break;
			}
			break;
		case EFF_OLDTOGGLE:
			fprintf(scanF,"Oldtoggle tiles at (%03d,%03d) to floor %03d and wall %03d",me->x,me->y,me->value,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_LIFE:
			fprintf(scanF,"Set ");
			if(me->flags&EF_PERMLIGHT)
				fprintf(scanF,"max");
			else
				fprintf(scanF,"current");
			fprintf(scanF," life of %s at (%03d,%03d) to %d",MonsterName(me->value),me->x,me->y,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_WEAPON:
			fprintf(scanF,"Force player's weapon to %s and ",GetWeaponName(me->value));
			if(me->value2==0)
				fprintf(scanF,"don't");
			else
				fprintf(scanF,"do");
			fprintf(scanF," reload if same\n");
			break;
		case EFF_TAGTARGET:
			fprintf(scanF,"Tag the targeted monster\n");
			break;
		case EFF_TAGMONS:
			fprintf(scanF,"Tag %s at (%03d,%03d)\n",MonsterName(me->value),me->x,me->y);
			break;
		case EFF_MONSITEM:
			fprintf(scanF,"Give %s at (%03d,%03d) the item ",MonsterName(me->value),me->x,me->y);
			if(me->value2==ITM_RANDOM)
				fprintf(scanF,"Random");
			else
				fprintf(scanF,"%s",GetItem(me->value2)->name);
			PrintFX(me->flags);
			break;
		case EFF_TILEVAR:
			fprintf(scanF,"Change ");

			if(me->flags&EF_CONTIGUOUS)
				fprintf(scanF,"touching tiles");
			else if(me->flags&EF_ALL)
				fprintf(scanF,"all same tiles");
			else
				fprintf(scanF,"single tile");

			fprintf(scanF," at (%03d,%03d) to floor %s",me->x,me->y,VarName(me->value));
			PrintFX(me->flags);
			break;
		case EFF_LIFEAMT:
			fprintf(scanF,"Change life of %s at (%03d,%03d) by %d",MonsterName(me->value),me->x,me->y,me->value2);
			PrintFX(me->flags);
			break;
		case EFF_AI:
			fprintf(scanF,"Change %s at (%03d,%03d) to AI of %s",MonsterName(me->value),me->x,me->y,MonsterName(me->value2));
			PrintFX(me->flags);
			break;
		case EFF_NAME:
			fprintf(scanF,"Change name of %s at (%03d,%03d) to \"%s\"\n",MonsterName(me->value),me->x,me->y,me->text);
			break;
		case EFF_COLOR:
			fprintf(scanF,"Color swap %s at (%03d,%03d) from %d to %d\n",MonsterName(me->value),me->x,me->y,((word)me->value2)%256,((word)me->value2)/256);
			break;
		case EFF_MONSBRIGHT:
			fprintf(scanF,"Change brightness of %s at (%03d,%03d) to %d\n",MonsterName(me->value),me->x,me->y,me->value2);
			break;
		case EFF_PLAYAS:
			fprintf(scanF,"Force player to play as %s\n", GetPlayableCharacterName(me->value));
			break;
		case EFF_MONSGRAPHICS:
			fprintf(scanF,"Change graphics of %s at (%03d,%03d) to %s",MonsterName(me->value),me->x,me->y,me->text);
			PrintFX(me->flags);
			break;
		case EFF_ITEMGRAPHICS:
			fprintf(scanF,"Set custom item graphics to %s\n",me->text);
			break;
		case EFF_VARBAR:
			fprintf(scanF,"Set ");
			if(me->flags&EF_PERMLIGHT)
				fprintf(scanF,"max");
			else
				fprintf(scanF,"current");
			fprintf(scanF," varbar to %s (color %d)\n",VarName(me->value),((word)me->value2)%256);
			break;
		case EFF_MAKEBULLET:
			fprintf(scanF,"Summon bullet %s at (%03d,%03d) facing \"%s\"\n",BulletName(me->value2), me->x, me->y, me->text);
			break;
		default:
			fprintf(scanF,"Unhandled effect type %d\n",me->type);
			break;
	}
}

void Scan_Special(world_t *world,Map *map,int num,special_t *me)
{
	int i;

	fprintf(scanF,"#%03d (%03d,%03d) -----------\n",num,me->x,me->y);

	// triggers
	for(i=0;i<NUM_TRIGGERS;i++)
	{
		if(me->trigger[i].type!=TRG_NONE)
			Scan_Trigger(world,map,i,&me->trigger[i],me->effect[i].text);
	}
	fprintf(scanF,"      ----\n");
	// effects
	for(i=0;i<NUM_EFFECTS;i++)
	{
		if(me->effect[i].type!=EFF_NONE)
			Scan_Effect(world,map,i,&me->effect[i]);
	}
	fprintf(scanF,"\n");
}

void Scan_Badguy(world_t *world,Map *map,int num,mapBadguy_t *me)
{
	fprintf(scanF,"#%03d (%03d,%03d) %s (item: ",num,me->x,me->y,MonsterName(me->type));
	switch(me->item)
	{
		case ITM_RANDOM:
			fprintf(scanF,"random");
			break;
		case ITM_NONE:
			fprintf(scanF,"none");
			break;
		default:
			fprintf(scanF,"%s",GetItem(me->item)->name);
			break;
	}
	fprintf(scanF,")\n");
}

byte Scan_Level(world_t *world,Map *map)
{
	constexpr int LEN = std::max(MAX_ITEMS, NUM_MONSTERS);
	int i;
	word itemCount[LEN];
	int totalMons;

	scanF=AppdataOpen_Write("level_scan.txt");
	if(!scanF)
		return 0;

	fprintf(scanF,"World: %s\nLevel: %s\n\nBADGUY COUNT\n---------\n",world->map[0]->name,map->name);

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

	fprintf(scanF,"Total monsters: %d\n\n",totalMons);
	for(i=0;i<NUM_MONSTERS;i++)
	{
		if(itemCount[i]>0)
			fprintf(scanF,"%s: %d\n",MonsterName(i),itemCount[i]);
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
	fprintf(scanF,"\nITEM COUNT\n---------\n");
	fprintf(scanF,"Total items: %d\n\n",totalMons);
	for(i=0;i<MAX_ITEMS;i++)
	{
		if(itemCount[i]>0)
			fprintf(scanF,"%s: %d\n",GetItem(i)->name,itemCount[i]);
	}

	fprintf(scanF,"\nSPECIALS IN DETAIL\n---------\n");
	for(i=0;i<MAX_SPECIAL;i++)
	{
		if(map->special[i].x!=255)
			Scan_Special(world,map,i,&map->special[i]);
	}
	fprintf(scanF,"\nBADGUYS IN DETAIL\n---------\n");
	for(i=0;i<MAX_MAPMONS;i++)
	{
		if(map->badguy[i].type)
		{
			Scan_Badguy(world,map,i,&map->badguy[i]);
		}
	}

	fclose(scanF);
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

	scanF=AppdataOpen_Write("var_scan.txt");
	if(!scanF)
		return 0;

	fprintf(scanF,"World: %s\n\n\nVAR REFERENCES\n---------\n",world->map[0]->name);

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

	fprintf(scanF,"Tile vars: %d\n\nVars being checked:\n",tileVars);
	for(i=0;i<16;i++)
	{
		if(var_checks[i]>0)
		{
			if(i<8)
				fprintf(scanF,"G%d: %d\n",i,var_checks[i]);
			else
				fprintf(scanF,"V%d: %d\n",i-8,var_checks[i]);
		}
	}
	fprintf(scanF,"\nVars being set:\n");
	for(i=0;i<16;i++)
	{
		if(var_sets[i]>0)
		{
			if(i<8)
				fprintf(scanF,"G%d: %d\n",i,var_sets[i]);
			else
				fprintf(scanF,"V%d: %d\n",i-8,var_sets[i]);
		}
	}

	fprintf(scanF,"\nLEVEL DETAILS\n----------------\n\n");

	for(i=0;i<world->numMaps;i++)
	{
		fprintf(scanF,"Level %d: %s -----------------\n\n",i,world->map[i]->name);

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
							fprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Compare %s to %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].trigger[k].value),VarName(world->map[i]->special[j].trigger[k].value2));
							break;
						case TRG_VAR:	// value
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							fprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Check value of %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].trigger[k].value));
							break;
						case TRG_EQUVAR:	// value, and a text thing
							var_checks[world->map[i]->special[j].trigger[k].value]++;
							fprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Equation compare \"%s\" to %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									world->map[i]->special[j].effect[k].text,VarName(world->map[i]->special[j].trigger[k].value));
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							break;
						case TRG_EQUATION:	// text thing
							Find_Text_Vars(world->map[i]->special[j].effect[k].text,1);
							fprintf(scanF,"SPCL #%03d (%03d,%03d): TRG: Equation check \"%s\"\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
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
							fprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Sets %s to \"%s\"\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].effect[k].value),world->map[i]->special[j].effect[k].text);
							break;
						case EFF_TILEVAR:	// sets a tilevar, checks V
							tileVars++;
							var_checks[world->map[i]->special[j].effect[k].value]++;
							fprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Tilevar reads %s\n",j,world->map[i]->special[j].x,world->map[i]->special[j].y,
									VarName(world->map[i]->special[j].effect[k].value));
							break;
						case EFF_ITEM:	// changes items, said items might change variables
							if(world->map[i]->special[j].effect[k].value!=ITM_NONE &&
								(GetItem(world->map[i]->special[j].effect[k].value)->effect==IE_INCVAR ||
								GetItem(world->map[i]->special[j].effect[k].value)->effect==IE_DECVAR))
							{
								var_sets[GetItem(world->map[i]->special[j].effect[k].value)->effectAmt]++;
								fprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Creates item that incs/decs %s\n",j,
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
								fprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Summons monster holding item that incs/decs %s\n",j,
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
								fprintf(scanF,"SPCL #%03d (%03d,%03d): EFF: Changes monster item to item that incs/decs %s\n",j,
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
				fprintf(scanF,"ITM (%03d,%03d): '%s' incs/decs %s\n",j%world->map[i]->width,j/world->map[i]->width,
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
				fprintf(scanF,"MONS (%03d,%03d): '%s' holds a '%s' that incs/decs %s\n",j%world->map[i]->width,j/world->map[i]->width,
						MonsterName(world->map[i]->badguy[j].type),
						GetItem(world->map[i]->badguy[j].item)->name,
						VarName(GetItem(world->map[i]->badguy[j].item)->effectAmt));
			}
		}

		fprintf(scanF,"Tile vars: %d\n\nVars being checked:\n",tileVars);
		for(j=0;j<16;j++)
		{
			if(var_checks[j]>0)
			{
				if(j<8)
					fprintf(scanF,"G%d: %d\n",j,var_checks[j]);
				else
					fprintf(scanF,"V%d: %d\n",j-8,var_checks[j]);
			}
		}
		fprintf(scanF,"\nVars being set:\n");
		for(j=0;j<16;j++)
		{
			if(var_sets[j]>0)
			{
				if(j<8)
					fprintf(scanF,"G%d: %d\n",j,var_sets[j]);
				else
					fprintf(scanF,"V%d: %d\n",j-8,var_sets[j]);
			}
		}
		fprintf(scanF,"\n");
	}

	fclose(scanF);
	AppdataSync();
	return 1;
}
