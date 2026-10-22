#include "alchemy.h"
#include "leveldef.h"
#include "editor.h"
#include "player.h"
#include "game.h"
#include "quest.h"

void PlantAsbestos(Map *map,world_t *world,byte amt)
{
	int x,y;
	int tries;
	// asbestos grows only on snow, and is buried, must be dug up with shovel

	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		if(map->map[x+y*map->width].item==0 && map->map[x+y*map->width].wall==0 &&
			SnowCheck(map->map[x+y*map->width].floor))
		{
			amt--;
			map->map[x+y*map->width].item=IT_BURIEDASB;
		}
	}
}

void PlantBubbleweed(Map *map,world_t *world,byte amt)
{
	int x,y,tries;
	mapTile_t *m;
	// bubbleweed only grows adjacent to rivers

	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		m=&map->map[x+y*map->width];
		if(m->item==0 && m->wall==0 && (
			(m->floor>=129 && m->floor<=137) || (m->floor>=149 && m->floor<=157) || (m->floor>=169 && m->floor<=177) ||
			(m->floor>=193 && m->floor<=197)))
		{
			amt--;
			map->map[x+y*map->width].item=IT_HERBB;
		}
	}
}

void PlantCilantro(Map *map,world_t *world,byte amt)
{
	int x,y,tries;
	mapTile_t *m;
	// cilantro grows anywhere, but it's rare

	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		m=&map->map[x+y*map->width];
		if(m->item==0 && m->wall==0 && !(world->terrain[m->floor].flags&(TF_WATER|TF_ICE|TF_SKY|TF_CLIFF|TF_LAVA)))
		{
			amt--;
			map->map[x+y*map->width].item=IT_HERBC;
		}
	}
}

void PlantDevilgrass(Map *map,world_t *world,byte amt)
{
	int x,y,tries;
	mapTile_t *m;
	// devil grass grows only on stone
	// but since it only grows in caves where the place is all stone, let's simplify that

	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		m=&map->map[x+y*map->width];
		if(m->item==0 && m->wall==0 && !(world->terrain[m->floor].flags&(TF_WATER|TF_ICE|TF_SKY|TF_CLIFF|TF_LAVA)))
		{
			amt--;
			map->map[x+y*map->width].item=IT_HERBD;
		}
	}
}

void PlantElderberry(Map *map,world_t *world,byte amt)
{
	int x,y,tries;
	mapTile_t *m;
	// elderberry grows on cliffsides only
	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		m=&map->map[x+y*map->width];
		if(m->item==0 && m->wall==0 && (world->terrain[m->floor].flags&TF_CLIFF))
		{
			amt--;
			map->map[x+y*map->width].item=IT_HERBE;
		}
	}
}

void PlantFrostbane(Map *map,world_t *world,byte amt)
{
	int x,y,tries;
	mapTile_t *m;
	// frostbane grows on anything that isn't water, but only in the woods

	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		m=&map->map[x+y*map->width];
		if(m->item==0 && m->wall==0 && !(world->terrain[m->floor].flags&(TF_WATER|TF_ICE|TF_SKY|TF_CLIFF|TF_LAVA)))
		{
			amt--;
			map->map[x+y*map->width].item=IT_HERBF;
		}
	}
}

void AddCracks(Map *map,world_t *world,byte amt)
{
	int x,y,tries;
	mapTile_t *m;
	// cracks show up on any solid ground

	tries=0;
	while(amt>0 && (tries++<10000))
	{
		x=rand()%(map->width);
		y=rand()%(map->height);
		m=&map->map[x+y*map->width];
		if(m->item==0 && m->wall==0 && !(world->terrain[m->floor].flags&(TF_WATER|TF_ICE|TF_SKY|TF_CLIFF|TF_LAVA)))
		{
			amt--;
			if(player.levelNum>=LVL_GEYSER && player.levelNum<=LVL_TITANS)	// underground, use undercracks
				map->map[x+y*map->width].item=IT_CRACKUNDER;
			else
				map->map[x+y*map->width].item=IT_CRACK;
		}
	}
	if(player.levelNum==LVL_WESTWOOD && player.var[VAR_QUESTASSIGN+QUEST_SUPPLIES] && !player.var[VAR_QUESTDONE+QUEST_SUPPLIES] &&
		player.var[VAR_SNUGGLY]==1 && player.var[VAR_SUPPLIES]<20)	// dig up supplies randomly in westwood
	{
		amt=20-player.var[VAR_SUPPLIES];
		tries=0;
		while(amt>0 && (tries++<10000))
		{
			x=rand()%(map->width);
			y=rand()%(map->height);
			m=&map->map[x+y*map->width];
			if(m->item==0 && m->wall==0 && !(world->terrain[m->floor].flags&(TF_WATER|TF_ICE|TF_SKY|TF_CLIFF|TF_LAVA)))
			{
				amt--;
				map->map[x+y*map->width].item=IT_SUPCRACK;
			}
		}
	}
}

void PlantHerbs(Map *map,world_t *world)
{
	byte amt[7],max;
	int i;

	for(i=0;i<7;i++)
		if(levelDef[player.levelNum].herbCount[i]==0)
			amt[i]=0;
		else
		{
			if(i<6)	// only affects herbs
				max=(byte)((float)levelDef[player.levelNum].herbCount[i]+TalentBonus(TLT_HERBALISM)+0.5f);
			else
				max=(byte)((float)levelDef[player.levelNum].herbCount[i]);

			amt[i]=rand()%(max+1);
		}

	PlantAsbestos(map,world,amt[0]);
	PlantBubbleweed(map,world,amt[1]);
	PlantCilantro(map,world,amt[2]);
	PlantDevilgrass(map,world,amt[3]);
	PlantElderberry(map,world,amt[4]);
	PlantFrostbane(map,world,amt[5]);
	AddCracks(map,world,amt[6]);	// mining cracks
}

void GuyDropPotion(byte level,byte *type,int *val1,int *val2)
{
	// some potions are likelier than others, odds add to 100
	byte odds[20]={
		10, // Brain Juice
		5, // Mudder's Milk
		5, // Stone Soup
		10, // Life-Ade
		2, // Red Cow
		2, // Soula Cola
		2, // Dr. Deflector
		2, // Alien Blood
		2, // Fear Beer
		2, // Jolting Soda
		2, // Sparkling Water
		2, // Smart Sauce
		2, // Luckenbrau
		2, // Eau De Monster
		20, // Healenbrau
		20, // Magicola
		4, // Stamino
		2, // Rocket Sauce
		2, // Explosivo
		2, // Cool-Ade

	};
	int i;
	byte total,chose;
	byte val,upAmt;

	// decide which potion
	total=0;
	chose=rand()%100;
	for(i=0;i<20;i++)
	{
		total+=odds[i];
		if(chose<total)
		{
			(*type)=IT_POT1+i;
			break;
		}
	}
	if(i>=20)
		(*type)=IT_POT15;	// this should never happen

	// now how powerful?
	val=(level/5);	// start at level/5 base value

	if(val>9)
		val=9;

	upAmt=rand()%100;
	while(1)
	{
		if(upAmt<30)
			break;	// 30% chance that this is quite the power level we want
		else if(upAmt<65)
		{
			if(val>0)
				val--;	// 35% chance that it will go down one
		}
		else
		{
			if(val<9)
				val++;	// 35% chance of up
		}

		upAmt=rand()%100;
	}

	(*val1)=val;
	(*val2)=0;
}

int PotionDuration(byte type,byte lvl)
{
	int duration[20*2]={
		10*30,60*30, // Brain Juice
		10*30,120*30, // Mudder's Milk
		10*30,120*30, // Stone Soup
		10*30,60*30, // Life-Ade
		10*30,60*30, // Red Cow
		10*30,30*30, // Soula Cola
		5*30,30*30, // Dr. Deflector
		5*30,30*30, // Alien Blood
		10*30,45*30, // Fear Beer
		10*30,40*30, // Jolting Soda
		10*30,60*30, // Sparkling Water
		10*30,100*30, // Smart Sauce
		10*30,120*30, // Luckenbrau
		10*30,60*30, // Eau De Monster
		1,1, // Healenbrau
		1,1, // Magicola
		10*30,120*30, // Stamino
		10*30,60*30, // Rocket Sauce
		10*30,30*30, // Explosivo
		10*30,30*30, // Cool-Ade
	};

	type-=IT_POT1;
	return duration[type*2]+((duration[type*2+1]-duration[type*2])*lvl)/9;
}

int PotionEffect(byte type,byte lvl)
{
	int amt[20*2]={
		30,30*3-10, // Brain Juice (how many ticks are subtracted from normal MAGIC_RATE, which is 30*3)
		1,10, // Mudder's Milk (damage added)
		1,10, // Stone Soup (damage blocked)
		2*30,5, // Life-Ade (how many ticks between healing 1 life)
		2,2, // Red Cow
		1,1, // Soula Cola
		1,1, // Dr. Deflector
		1,10, // Alien Blood (damage it does)
		1*32,5*32, // Fear Beer
		2*30,3, // Jolting Soda
		1,3, // Sparkling Water
		10,200, // Smart Sauce
		25,500, // Luckenbrau
		1,1, // Eau De Monster
		5,50, // Healenbrau
		5,50, // Magicola
		1,1, // Stamino
		30,1, // Rocket Sauce
		5,20, // Explosivo
		1*32,4*32, // Cool-Ade
	};
	int val;

	type-=IT_POT1;
	val=amt[type*2]+((amt[type*2+1]-amt[type*2])*lvl)/9;

	// potions that get better with smaller numbers:
	// Jolting Soda, Life-Ade, Rocket Sauce
	if(type==17 || type==3 || type==9)
		val=val*(100-(int)TalentBonus(TLT_MIXOLOGY))/100;
	else
		val=val*(100+(int)TalentBonus(TLT_MIXOLOGY))/100;

	if(type==0)	// brain juice cannot go higher than 30*3-5
		if(val>30*3-5)
			val=30*3-5;
	if(val<1)
		val=1;	// no potion can handle a value lower than 1

	return val;
}

void DrinkPotion(byte type,byte lvl)
{
	player.potionClock=(word)PotionDuration(type,lvl);
	player.potionType=type;
	player.potionPower=lvl;
	player.potionsDrunk++;
	TalentPoint(TLT_MIXOLOGY,1+lvl/2);
	MakeNormalSound(SND_DRINKPOTION);
	PlayerCalcStats();
}

word PotionCost(equip_t *e)
{
	// chart is base cost, plus how much extra per level
	word cost[20*2]={
		30,20, // Brain Juice
		30,20, // Mudder's Milk
		30,20, // Stone Soup
		30,22, // Life-Ade
		40,24, // Red Cow
		50,42, // Soula Cola
		50,43, // Dr. Deflector
		33,26, // Alien Blood
		24,27, // Fear Beer
		20,39, // Jolting Soda
		44,47, // Sparkling Water
		63,51, // Smart Sauce
		71,53, // Luckenbrau
		20,21, // Eau De Monster
		10,12, // Healenbrau
		10,11, // Magicola
		10,7, // Stamino
		37,31, // Rocket Sauce
		43,42, // Explosivo
		39,41, // Cool-Ade
	};

	return cost[(e->type-IT_POT1)*2]+cost[(e->type-IT_POT1)*2+1]*e->value;
}

void MixPotion(byte ing1,byte ing2,byte ing3)
{
	byte mixTab[20][3]={
		{IT_HERBC,IT_HERBE,IT_HERBF}, // Brain Juice
		{IT_HERBA,IT_HERBB,IT_HERBD}, // Mudder's Milk
		{IT_HERBA,IT_HERBE,IT_HERBF}, // Stone Soup
		{IT_HERBB,IT_HERBE,IT_HERBF}, // Life-Ade
		{IT_HERBA,IT_HERBB,IT_HERBE}, // Red Cow
		{IT_HERBD,IT_HERBE,IT_HERBF}, // Soula Cola
		{IT_HERBA,IT_HERBC,IT_HERBE}, // Dr. Deflector
		{IT_HERBB,IT_HERBD,IT_HERBE}, // Alien Blood
		{IT_HERBC,IT_HERBD,IT_HERBF}, // Fear Beer
		{IT_HERBB,IT_HERBC,IT_HERBD}, // Jolting Soda
		{IT_HERBA,IT_HERBB,IT_HERBC}, // Sparkling Water
		{IT_HERBB,IT_HERBD,IT_HERBF}, // Smart Sauce
		{IT_HERBA,IT_HERBD,IT_HERBF}, // Luckenbrau
		{IT_HERBC,IT_HERBD,IT_HERBE}, // Eau De Monster
		{IT_HERBB,IT_HERBC,IT_HERBF}, // Healenbrau
		{IT_HERBB,IT_HERBC,IT_HERBE}, // Magicola
		{IT_HERBA,IT_HERBC,IT_HERBF}, // Stamino
		{IT_HERBA,IT_HERBC,IT_HERBD}, // Rocket Sauce
		{IT_HERBA,IT_HERBD,IT_HERBE}, // Explosivo
		{IT_HERBA,IT_HERBB,IT_HERBF}, // Cool-Ade
	};
	int i;
	byte b;

	if(ing2<ing1)
	{
		b=ing1;
		ing1=ing2;
		ing2=b;
	}
	if(ing3<ing2)
	{
		b=ing2;
		ing2=ing3;
		ing3=b;
		if(ing2<ing1)
		{
			b=ing1;
			ing1=ing2;
			ing2=b;
		}
	}

	for(i=0;i<20;i++)
	{
		if(ing1==mixTab[i][0] && ing2==mixTab[i][1] && ing3==mixTab[i][2])
		{
			int j,sum;
			// making potion #i
			// let's get a bell curve of numbers centered around your talent value, with a range of 12 either way
			// (so you can still fail at talent 10, but almost always get perfect potions instead)
			// you can also get perfect at talent 0, but it's really unlikely
			sum=0;
			for(j=0;j<20;j++)
				sum+=(-12+(rand()%25));	// -12 to +12
			sum=(sum/20);
			sum+=player.talentLevel[TLT_ALCHEMY];
			if(sum<1 && (rand()%2))
				sum=1;
			if(sum<1)
			{
				// failed
				NewMessage("The potion exploded!",0);
				MakeNormalSound(SND_BOOM);
				ExplodeParticles2(PART_FIRE,goodguy->x,goodguy->y,FIXAMT*10,50,5);
				return;
			}
			else
			{
				bullet_t *b;

				if(sum>10)
					sum=10;
				b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*10,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->speed=0;
					b->anim=IT_POT1+i;
					b->target=sum-1;
					b->owner=0;
				}
				TalentPoint(TLT_ALCHEMY,1);
				return;

			}
		}
	}
	NewMessage("IMPOSSIBLE",0);	// can't get here, this is like a low-budget assert
}
