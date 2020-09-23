void AI_Minitilda(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MATTIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_LOOKEYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==5)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
			me->reload=100;
			me->facing=0;
			return;
		}
		if(me->seq==ANIM_DIE && me->frmTimer<128)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(1);
		}
		if(me->seq==ANIM_ATTACK && me->frm>3 && me->frm<11 && me->reload==0)
		{
			// shoot lil lasers
			x=me->x;
			y=me->y+FIXAMT*8;
			MakeSound(SND_BEAM2,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me,x,y,1,BLT_BEAM,me->friendly);
			FireBullet(me,x,y,2,BLT_BEAM,me->friendly);
			FireBullet(me,x,y,3,BLT_BEAM,me->friendly);
			me->reload=0;
			me->mind=0;
			me->reload=100;
		}
		me->facing=0;
		return;
	}

	if((!me->reload) && Random(100)==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;	// very very slow animation
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
	}

	// do everything the brain does
	AI_MattieBrain(me,map,world,goodguy);
}

void AI_PatrolZoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;
	if(me->mind3)
		me->mind3--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(me,x,y,me->facing*32,BLT_MISSILE,me->friendly);
			FireBullet(me,x,y,(me->facing*32+4),BLT_MISSILE,me->friendly);
			FireBullet(me,x,y,(me->facing*32+8+4),BLT_MISSILE,me->friendly);
			me->reload=20;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && me->reload==0&&
		TargetInSight(me,map,goodguy))
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			// spit on him!
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			if(goodguy->aiType==MONS_BOUAPHA)
				player.spotted=30*8;
			return;
		}
	}
	
	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->mind2=(me->mind2+2)&7;
		me->mind=Random(120)+8;
	}
	
	if (me->facing != me->mind2 && !me->mind3)
	{
		me->facing=(me->facing+1)&7;
		me->mind3 = 3;
	}

	me->dx=Cosine(me->facing*32)*3;
	me->dy=Sine(me->facing*32)*3;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_XenoEmpressHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;
	
	if (me->mind2)
		me->mind2--;

	if(me->reload)
		me->reload--;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOOKEYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			{MakeSound(SND_LOOKEYDIE,me->x,me->y,SND_CUTOFF,1200);
			me->parent->mind4=1;}
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					(y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					me->z,FIXAMT);
		}
		if(me->seq==ANIM_ATTACK && me->frm>4 && me->frm<8)
		{
			if(goodguy && RangeToTarget(me,goodguy)<700*FIXAMT)
				{
					for(i=0;i<3;i++)
					{
						// spit acid glob
						x=me->x+Cosine(me->facing*32)*20-FIXAMT*4+(Random(9)<<FIXSHIFT);
						y=me->y+Sine(me->facing*32)*20-FIXAMT*4+(Random(9)<<FIXSHIFT);
						f=me->facing*32-32+Random(65);

						FireBullet(me,x,y,f,BLT_ACID,me->friendly);
					}
				}
		}
		return;
	}
	else if(goodguy)
	{
		if(Random(32)==0 && RangeToTarget(me,goodguy)<800*FIXAMT && me->mind2==0)
		{
			// get him!
			me->mind2=60;
			MakeSound(SND_ALIENSPIT,me->x,me->y,SND_CUTOFF,1200);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->reload=0;
			return;
		}
	}
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_XenoEmpressBody(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,h;
	if(me->reload)
		me->reload--;
	
	if (me->mind2)
		me->mind2--;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LOOKEYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			{MakeSound(SND_LOOKEYDIE,me->x,me->y,SND_CUTOFF,1200);
			KillKids(me);}
	}
	
	if (me->mind2==0 && me->mind4>0)
	{
		x=me->x+Cosine(me->facing*32)*16;
		y=me->y+Sine(me->facing*32)*16;
		h = Random(64)+32;
		FireBullet(me,x,y,h,BLT_ACID,me->friendly);
		me->reload=2;
	}
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					(y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					me->z,FIXAMT);
		}
		if(me->seq==ANIM_ATTACK && me->frm>4 && me->frm<8)
		{
			if(goodguy && RangeToTarget(me,goodguy)<700*FIXAMT)
				{
					x=me->x;
					y=me->y+FIXAMT*8;
					LaunchMegabeam(me,x,y,me->ID);
				}
		}
		return;
	}
	else if(goodguy && me->mind4>0)
	{
		if(Random(32)==0 && RangeToTarget(me,goodguy)<800*FIXAMT && me->mind2==0)
		{
			// get him!
			me->mind2=60;
			MakeSound(SND_ALIENSPIT,me->x,me->y,SND_CUTOFF,1200);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->reload=0;
			return;
		}
	}
}

void AI_XenoEmpressTail(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,h;
	Guy *g;
	
	if(me->reload)
		me->reload--;
	else
	{
		if (me->mind2!=0)
		me->mind2=0;
	}
	
	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_MARKMOVIE,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					(y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					me->z,FIXAMT);
		}
		else if(me->seq==ANIM_ATTACK && me->frm==6 && me->mind2==0 && me->reload==0)
		{
			me->mind2=1;
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			h = Random(64)+32;
			FireExactBullet(me,me->x,me->y,me->z+FIXAMT*40,Cosine(h)*10,Sine(h)*10,FIXAMT*10,0,60,h,BLT_EGG,me->friendly);
			me->reload=2;
		}
	}
	else if(goodguy)
	{	
		if(Random(32)==0 && RangeToTarget(me,goodguy)<800*FIXAMT && me->reload==0 && me->seq!=ANIM_ATTACK)
		{
			// get him!
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->reload=0;
			me->mind2 = 0;
			return;
		}
	}
}

void AI_ScreamQueen(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->parent)
	{
		// sitting on The Thing's tongue
		switch(me->parent->frm)
		{
			case 4:
			case 5:
				me->y+=10*FIXAMT;
				break;
			case 6:
			case 7:
				me->y+=15*FIXAMT;
				break;
			case 8:
				me->parent=NULL;	// I am free!
				if(me->hp>0)
				{
					me->seq=ANIM_MOVE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=200;
					me->mind=1;	// pop out in hunt mode
				}
				if(!me->CanWalk(me,me->x,me->y,map,world))
					me->aiType=MONS_NONE;	// vanish if this spot is occupied
				break;
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->reload==0 && me->frm==5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(me,x,y,me->facing,BLT_BEAM,me->friendly);
			MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,300);
			me->reload=4;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
		if(me->mind1<31)
			me->mind1++;

		if(me->mind1>30 && Random(50)==0)
		{
			switch(Random(16))
			{
				case 0:
					MakeSound(SND_BABYSNORE,me->x,me->y,SND_CUTOFF,800);
					me->seq=ANIM_A1;	// take a nap
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=32;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
				case 2:
				case 3:

					me->seq=ANIM_A3;	// look right
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 4:
				case 5:
				case 6:
					me->seq=ANIM_A4;	// look left
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 7:
				case 8:
					me->mind=2;	// go into wander mode
					me->mind1=Random(40)+1;
					break;
				default:	// blink is very common
					me->seq=ANIM_A2;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
			}
		}
		if(RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			me->mind1=255;

			FaceGoodguy(me,goodguy);
		}
		me->dx=0;
		me->dy=0;
	}
	else if(me->mind==1)	// hunting mode
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(8)==0 && me->reload==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		if(Random(80)==0)
		{
			me->mind1=20;
			me->mind=2;	// wander
		}
		me->dx=Cosine(me->facing*32)*5;
		me->dy=Sine(me->facing*32)*5;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
	}
	else	// random wander mode
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			switch(Random(3))
			{
				case 0:
					if(goodguy)
						me->mind=1;	// hunt him
					break;
				case 1:
					me->mind=0;	// stop here and rest
					me->mind1=0;
					break;
				case 2:	// just wander a different way
					me->facing=(byte)Random(8);
					me->mind1=Random(20)+1;
					break;
			}
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=96;
		}
	}
	// this is needed because his idle frame doesn't fit into his walking animation, so this skips it
	if(me->seq==ANIM_MOVE && me->frm==10 && me->frmTimer>127)
	{
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=96;
	}
}

void AI_GrueJr(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;

	if(!goodguy)
		return;

	// heal in darkness
	if(map->map[me->mapx+me->mapy*map->width].templight<-3)
	{
		if(me->hp<me->maxHP && me->seq!=ANIM_DIE)
		{
			i=me->hp;
			me->hp+=2;
			if(me->hp>me->maxHP)
				me->hp=me->maxHP;
			ShowEnemyLife(me->name,i*128/me->maxHP,me->hp*128/me->maxHP,1);
		}
	}

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD2OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD2DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(Random(2))
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0 && goodguy)
		{
			FaceGoodguy(me,goodguy);
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,10,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1)
		me->mind1--;

	if(map->map[goodguy->mapx+goodguy->mapy*map->width].templight<-8)	// if you are in the dark, you are good eats
	{
		if (me->mind!=2)
			MakeColdRingParticle(me->x,me->y,0,2*15);
		me->mind=2;	// dash at you
		me->mind1=3;
	}

	switch(me->mind)
	{
		case 0:	// normal heading towards you
			FaceGoodguy(me,goodguy);
			if(me->mind1==0)
			{
				me->mind=1;
				me->facing=Random(8);
				me->mind1=10+Random(40);
			}
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->frmAdvance=128;
			break;
		case 1:	// normal wandering
			if(me->mind1==0)
			{
				me->mind=0;
				me->mind1=10+Random(90);
			}
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->frmAdvance=128;
			break;
		case 2:	// dashing at you
			if(me->mind1==0)
			{
				me->mind=1;
				me->mind1=10+Random(40);
			}
			f=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
			me->facing=f/32;
			me->dx=Cosine(f)*8;
			me->dy=Sine(f)*8;
			me->frmAdvance=200;
			break;
	}
	
	if((me->mind==0 || me->mind==2)&&Random(4)==0)
	{
		SuckParticle(me->x,me->y,FIXAMT*20); //used to show where he is
	}
	
	if(goodguy)
	{

		x=RangeToTarget(me,goodguy);
		if(x<(128*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SPD3CHOMP,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// bite him
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=200;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}
	}

	if(me->mind!=2)	// when dashing, he ignores light, but otherwise wants to avoid it
	{
		x=(me->x+me->dx)/(TILE_WIDTH*FIXAMT);
		y=(me->y+me->dy)/(TILE_HEIGHT*FIXAMT);
		if(map->map[me->mapx+me->mapy*map->width].templight<-5 &&
			map->map[x+y*map->width].templight>=-5)	// you are in darkness now, but wouldn't be on moving
		{
			me->dx=0;
			me->dy=0;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			return;
		}
		if(map->map[me->mapx+me->mapy*map->width].templight>=-5)	// if you are in light at any time, run
		{
			me->dx=(me->dx*3)/2;
			me->dy=(me->dy*3)/2;
			me->frmAdvance=160;
		}
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_PygmyBoomer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*10;
			y=me->y+Sine(me->facing*32)*10;
			// throw spear
			FireBullet(me,x,y,me->facing*32,BLT_BOOMERANG,me->friendly);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && Random(200)==0)
		{
			switch(Random(2))
			{
				case 0:
					MakeSound(SND_UNGADANCE,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A1;	// unga dance
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
					me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
					me->mind1=0;
					break;
			}
		}
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<320*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				if(me->ouch==0)
					me->mind1=60;	// for 2 seconds minimum
				else
					me->mind1=120;	// 4 seconds, because they HATE getting shot
				FaceGoodguy(me,goodguy);

				// do angry animation
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;	// very fast
				MakeSound(SND_PYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
			}
		}
		me->dx=0;
		me->dy=0;
	}
	else
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0 && me->reload==0)
		{
			// spear him!
			MakeSound(SND_PYGMYSTAB,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>1024*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=30;	// stay on trail a little longer
			return;
		}
		if(goodguy)
		{
			
			x=goodguy->x;
			y=goodguy->y;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT*1;
			if(me->y<y)
				me->dy+=FIXAMT*1;
			if(me->x>x)
				me->dx-=FIXAMT*1;
			if(me->y>y)
				me->dy-=FIXAMT*1;

			Clamp(&me->dx,FIXAMT*5);
			Clamp(&me->dy,FIXAMT*5);
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

//LUNACHICK
void AI_Lunachick(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind3)
		me->mind3--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LUNAOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_LUNADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireExactBullet(me,x,y,FIXAMT*10,Cosine(me->facing*32+Random(16)-8)*6,Sine(me->facing*32+Random(16)-8)*6,0,0,30,me->facing,BLT_LUNAX2,me->friendly);
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_A5 && me->frm > 7 && me->frm < 19 && me->frm % 2 == 0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireExactBullet(me,x,y,FIXAMT*10,Cosine(me->facing*32+Random(16)-8)*(8+Random(4)),Sine(me->facing*32+Random(16)-8)*(8+Random(4)),0,0,40,me->facing,BLT_LUNAX1,me->friendly);
			me->reload=7;
			me->mind1=1;
		}
		if(me->seq==ANIM_A4) //ah, the leap
		{
			if (me->frm==2)
			{me->dx=Cosine(me->facing*32)*14;
			me->dy=Sine(me->facing*32)*14;
			me->dz=16*FIXAMT;}
			else if(me->frm<7)
			{
				// stay the course
				Dampen(&me->dx,FIXAMT/8);
				Dampen(&me->dy,FIXAMT/8);
			}
			else if(me->frm==18 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
				MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,600);
				map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
				me->x+=-FIXAMT*320+Random(FIXAMT*640);
				me->y+=-FIXAMT*240+Random(FIXAMT*480);
				if(me->x<TILE_WIDTH*2*FIXAMT)
					me->x=TILE_WIDTH*2*FIXAMT;
				else if(me->x>(map->width-2)*TILE_WIDTH*FIXAMT)
					me->x=(map->width-2)*TILE_WIDTH*FIXAMT;
				if(me->y<TILE_HEIGHT*2*FIXAMT)
					me->y=TILE_HEIGHT*2*FIXAMT;
				else if(me->y>(map->height-2)*TILE_HEIGHT*FIXAMT)
					me->y=(map->height-2)*TILE_HEIGHT*FIXAMT;
				me->dz=8*FIXAMT;
			}
		}
		if(me->seq==ANIM_DIE)
			me->facing=(me->facing+1)&7;
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(24)==0)
		{
			// spit at him
			MakeSound(SND_LUNASHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		else if(RangeToTarget(me,goodguy)<(1024*FIXAMT) && RangeToTarget(me,goodguy)>(256*FIXAMT) && Random(36)==0)
		{
			// spit at him
			MakeSound(SND_LUNABORED,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A5;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		else if(Random(48)==0)
		{
			// spit at him
			MakeSound(SND_LUNABRAINS,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A4;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		FaceGoodguy(me,goodguy);
		
		me->dx=Cosine(me->facing*32)*5;
		me->dy=Sine(me->facing*32)*5;
		
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		if(Random(128)==0)
		{
			me->mind=1;		// occasionally wander
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(24)==0)
		{
			// spit at him
			MakeSound(SND_LUNASHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		else if(RangeToTarget(me,goodguy)<(1024*FIXAMT) && Random(36)==0)
		{
			// spit at him
			MakeSound(SND_LUNABORED,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A5;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}
		else if(Random(48)==0)
		{
			// spit at him
			MakeSound(SND_LUNABRAINS,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A4;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*5;
		me->dy=Sine(me->facing*32)*5;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_NightTerror(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;

	if(!goodguy)
		return;
	
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_VAMPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_VAMPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{

		if(me->seq==ANIM_DIE)
		{
			if(Random(2))
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
					(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
					me->z, FIXAMT);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			FaceGoodguy(me,goodguy);
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,24,map,world);
			}
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1)
		me->mind1--;
	
	if (me->mind1 % 2 == 1 && me->mind !=2)
	{
		if(me->hp<me->maxHP && me->seq!=ANIM_DIE)
		{
			i=me->hp;
			me->hp+=1;
			if(me->hp>me->maxHP)
			me->hp=me->maxHP;
		}
	}

	if(map->map[me->mapx+me->mapy*map->width].templight>-8)	// if you can see him, RUN!
	{
		me->mind=2;	// dash at you
		me->mind1=3;
	}

	switch(me->mind)
	{
		case 0:	// normal heading towards you
			FaceGoodguy(me,goodguy);
			if(me->mind1==0)
			{
				me->mind=1;
				me->facing=Random(8);
				me->mind1=10+Random(40);
			}
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
			me->frmAdvance=128;
			if(goodguy->x>me->x-FIXAMT*64 && goodguy->x<me->x+FIXAMT*48 && goodguy->y>me->y-FIXAMT*64 && goodguy->y<me->y+FIXAMT*48 && goodguy->ouch==0)
			goodguy->GetShot(me,goodguy,0,0,1,map,world);
			break;
		case 1:	// normal wandering
			if(me->mind1==0)
			{
				me->mind=0;
				me->mind1=10+Random(90);
			}
			me->dx=Cosine(me->facing*32)*1;
			me->dy=Sine(me->facing*32)*1;
			me->frmAdvance=128;
			if(goodguy->x>me->x-FIXAMT*64 && goodguy->x<me->x+FIXAMT*48 && goodguy->y>me->y-FIXAMT*64 && goodguy->y<me->y+FIXAMT*48 && goodguy->ouch==0)
			goodguy->GetShot(me,goodguy,0,0,2,map,world);
			break;
		case 2:	// dashing at you
			if(me->mind1==0)
			{
				me->mind=1;
				me->mind1=10+Random(40);
			}
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->frmAdvance=200;
			if(goodguy->x>me->x-FIXAMT*64 && goodguy->x<me->x+FIXAMT*48 && goodguy->y>me->y-FIXAMT*64 && goodguy->y<me->y+FIXAMT*48 && goodguy->ouch==0)
			goodguy->GetShot(me,goodguy,0,0,1,map,world);
			break;
	}

	if(goodguy)
	{

		x=RangeToTarget(me,goodguy);
		if(x<(128*FIXAMT) && Random(16)==0 && me->reload==0 && me->mind == 2)
		{
			MakeSound(SND_VAMPATTK,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// bite him
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=100;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}
	}

	if(me->mind!=2)	// when dashing, he ignores light, but otherwise wants to avoid it
	{
		x=(me->x+me->dx)/(TILE_WIDTH*FIXAMT);
		y=(me->y+me->dy)/(TILE_HEIGHT*FIXAMT);
		if(map->map[me->mapx+me->mapy*map->width].templight<-5 &&
			map->map[x+y*map->width].templight>=-5)	// you are in darkness now, but wouldn't be on moving
		{
			me->dx=0;
			me->dy=0;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			return;
		}
		if(map->map[me->mapx+me->mapy*map->width].templight<=-5)	// if you are in light at any time, run
		{
			me->dx=(me->dx*3)/4;
			me->dy=(me->dy*3)/4;
			me->frmAdvance=160;
		}
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_CloneArmor(Guy *me, Map *map, world_t *world, Guy *goodguy)
{
	int x, y, x2, y2, f;

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_ROBOBOUAPHAOUCH, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if (me->seq == ANIM_ATTACK && me->frm > 3 && me->frm < 12 && me->frm % 2 == 1 && me->reload == 0 && goodguy)
		{
			MakeSound(SND_ARMORSHOOT, me->x, me->y, SND_CUTOFF, 1200);
			f = me->facing * 32 - 64;
			x = me->x + Cosine(me->facing * 32)*20;
			y = me->y + Sine(me->facing * 32)*20;
			x2 = x + Cosine(f)*32;
			y2 = y + Sine(f)*32;
			FireBullet(me,x2, y2, me->facing * 32, BLT_BIGSHELL, me->friendly);
			x2 = x - Cosine(f)*32;
			y2 = y - Sine(f)*32;
			FireBullet(me,x2, y2, me->facing * 32, BLT_BIGSHELL, me->friendly);
			me->mind1 = 1;
		}
		if (me->seq == ANIM_A1 && me->frm == 2 && me->reload == 0 && goodguy)
		{
			QuadMissile(me,me->x, me->y, me->facing*32, me->friendly);
			me->reload = 5;
			me->mind1 = 1;
		}
		if (me->frm >= 3 && me->seq == ANIM_DIE)
			{
				x = me->x >> FIXSHIFT;
				y = me->y >> FIXSHIFT;
				BlowUpGuy(x + me->rectx, y + me->recty, x + me->rectx2, y + me->recty2, me->z, 2);
				BlowSmoke((x + me->rectx + Random(me->rectx2 - me->rectx)) << FIXSHIFT,
						(y + me->recty + Random(me->recty2 - me->recty)) << FIXSHIFT,
						me->z, FIXAMT);
			}
		if (me->frm == 4 && me->seq == ANIM_DIE)
			{
				// become an evil clone...
				FireBullet(me,me->x, me->y, 0, BLT_BOOM, me->friendly);
				me->type=MONS_EVILCLONE;	// become a lesser clone
				me->aiType=MONS_EVILCLONE;
				strcpy(me->name,monsType[me->aiType].name);
				me->hp=32;
				me->maxHP=32;
				me->mind=0;
				me->mind1=0;
				me->facing=2;
			}
		return; // can't do nothin' right now
	}

	if (me->mind == 0) // when mind=0, singlemindedly lumber towards Bouapha
	{
		if (RangeToTarget(me, goodguy)<(512 * FIXAMT) && Random(32) == 0)
		{
			// spit at him
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			FaceGoodguy(me, goodguy);
			return;
		}
		else if (RangeToTarget(me, goodguy)<(512 * FIXAMT) && Random(128) == 0)
		{
			// spit at him
			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			FaceGoodguy(me, goodguy);
			return;
		}
		FaceGoodguy(me, goodguy);

		me->dx = Cosine(me->facing * 32)*4;
		me->dy = Sine(me->facing * 32)*4;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
		if (Random(64) == 0)
		{
			me->mind = 1; // occasionally wander
			me->mind1 = 1;
		}
	}
	else if (me->mind == 1) // random wandering
	{
		if (RangeToTarget(me, goodguy)<(512 * FIXAMT) && Random(32) == 0)
		{
			// spit at him
			MakeSound(SND_HAMMERTOSS, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_ATTACK;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			FaceGoodguy(me, goodguy);
			return;
		}
		else if (RangeToTarget(me, goodguy)<(512 * FIXAMT) && Random(128) == 0)
		{
			// spit at him
			me->seq = ANIM_A1;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 256;
			me->action = ACTION_BUSY;
			me->dx = 0;
			me->dy = 0;
			me->reload = 0;
			FaceGoodguy(me, goodguy);
			return;
		}

		if (!(me->mind1--)) // time to get a new direction
		{
			if ((goodguy) && Random(3) == 0)
				me->mind = 0; // get back on track
			else
				me->facing = (byte) Random(8);
			me->mind1 = Random(40) + 1;
		}

		me->dx = Cosine(me->facing * 32)*5;
		me->dy = Sine(me->facing * 32)*5;
		if (me->seq != ANIM_MOVE)
		{
			me->seq = ANIM_MOVE;
			me->frm = 0;
			me->frmTimer = 0;
			me->frmAdvance = 128;
		}
	}
}


void AI_Decoy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp<=0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
		if(me->hp>0)
		{
			me->seq=ANIM_A2;	//sad
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
	if(me->action==ACTION_BUSY)
	{
		FaceGoodguy(me,goodguy);
		if(me->seq==ANIM_ATTACK && me->frm==1 && me->reload==0 && goodguy)
		{
			if(RangeToTarget(me,goodguy)<60*FIXAMT)
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*16,Sine(me->facing*32)*16,10,map,world);
			me->reload=4;
		}
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->action=ACTION_IDLE;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// waiting for Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch)
			{
				me->action=ACTION_BUSY;
				me->seq=ANIM_MOVE;	// muscles
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->mind=1;
			}
		}
	}
	else if(me->mind==1)	// chase down Bouapha
	{
		if(me->z==0)
		{
			me->dz=12*FIXAMT;
			me->dx=me->dx*2.50;
			me->dy=me->dy*2.50;
		}
		if(goodguy)
		{
			i=RangeToTarget(me,goodguy);
			if(i<(60*FIXAMT) && Random(4)==0 && me->reload==0)
			{
				// get him! (chomp)
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*2;
				return;
			}
			FaceGoodguy2(me,goodguy);

			if(me->seq!=ANIM_ATTACK)
			{
				me->dx=Cosine(me->facing*32)*8;
				me->dy=Sine(me->facing*32)*8;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*2;
			}
			if(me->seq!=ANIM_MOVE && me->seq!=ANIM_ATTACK)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
		else
			me->mind=0;
	}
}

void AI_Hologram(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HOLOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HOLODIE,me->x,me->y,SND_CUTOFF,1200);
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->mind==2)
		{
			if(me->frm>5 && me->frm<14)
			{
				if(me->frm%2==1)
				{
				MakeSound(SND_BALLLIGHTNING,me->x,me->y,SND_CUTOFF,60);
				FireBullet(me,me->x,me->y,me->facing+7+me->frm%3,BLT_BALLLIGHTNING,me->friendly);
				}
				if (me->frm==10 && me->mind3==0)
				{
					me->mind3 = 1;
					for(i=0;i<8;i++)
					{
						FireExactBullet(me,me->x,me->y,FIXAMT*10,Cosine(me->facing*32+i)*9,Sine(me->facing*32+i)*9,0,0,128,i,BLT_ENERGY_BOUNCE,me->friendly);
					}
				}
				if(me->frm==13)
				me->mind=1;
			}
		}
		if (me->seq==ANIM_DIE)
		{if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
	}

	if(me->mind==0)	// if Bouapha gets near, laugh and then run
	{
		if(goodguy && RangeToTarget(me,goodguy)<160*FIXAMT)
		{
			MakeNormalSound(SND_HOLOLAUGH);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=192;
			me->action=ACTION_BUSY;
			me->mind=1;
			me->reload=72;
			return;
		}
	}
	if(me->mind==1)	// running away
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->mind3=Random(3);
			if (me->mind3==1)
			{
				me->mind=2;
				me->mind3=0;
				me->action=ACTION_BUSY;
				me->seq=ANIM_ATTACK;
				MakeNormalSound(SND_HOLOSHOOT);
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->reload=0;
				FaceGoodguy2(me,goodguy);
				me->dx=0;
				me->dy=0;
			}
			else
			{
				me->facing=(me->facing+1-(byte)Random(3))&7;
				me->reload=Random(16)+1;
			}
		}

		if(me->mind1 && me->seq==ANIM_MOVE)
		{
			me->facing=(byte)Random(8);
			me->mind1=0;
			me->reload=16;
		}

		if(me->seq!=ANIM_MOVE&&me->seq!=ANIM_ATTACK)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		
		if(me->seq==ANIM_MOVE)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
	}
}

void AI_Bat2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,b;

	if (me->reload)
		me->reload--;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
		MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
		MakeSound(SND_BATDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->type==MONS_GASBAT && Random(16)==0)
	{
		b = (byte)Random(256);
		FireBullet(me,me->x,me->y,b,BLT_FART,me->friendly);
	}
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				if(goodguy)
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,5,map,world);
					if(me->AttackCheck(12,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
					{
						if(goodguy->aiType==MONS_BOUAPHA && me->aiType==MONS_GRABBYBAT && me->mind3==0)
						{
							me->mind3=StealWeapon();
						}
						else if(me->aiType==MONS_VAMPBAT)
						{
							me->hp+=5;
							
						}
					}
				}
			}
			me->reload=2;

		}
		if(me->seq==ANIM_A1 && me->frm>1 && goodguy && me->hp>0)
		{
			// diving attack hit check
			x=me->x;
			y=me->y;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,5,map,world);
				me->seq=ANIM_A2;	// bounce off
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->dx=-me->dx/8;
				me->dy=-me->dy/8;
			}
		}
		if(me->seq==ANIM_A3 && me->frm==3 && me->reload==0)
		{
			FireBullet(me,me->x,me->y,me->facing*32,BLT_BIGSHELL,me->friendly);
			me->reload=5;
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;

		return;	// can't do nothin' right now
	}

	if(me->aiType!=MONS_GASBAT)
	{
		if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(4)==0 &&
			map->CheckLOS(me->mapx,me->mapy,6,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			MakeSound(SND_BATEYES,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy(me,goodguy);
			return;
		}

		if(RangeToTarget(me,goodguy)>(128*FIXAMT) && Random(32)==0 && me->aiType==MONS_DINGBAT &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			FaceGoodguy(me,goodguy);
			MakeSound(SND_BATDIVE,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;	// diving attack move
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			return;
		}
		
		if(RangeToTarget(me,goodguy)>(240*FIXAMT) && Random(32)==0 && me->aiType==MONS_GARGOYLE &&
			map->CheckLOS(me->mapx,me->mapy,25,goodguy->mapx,goodguy->mapy))
		{
			MakeSound(SND_BATEYES,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A3;	// shoot
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			FaceGoodguy(me,goodguy);
			return;
		}

		if(RangeToTarget(me,goodguy)>(160*FIXAMT) && Random(48)==0 && me->aiType==MONS_VAMPBAT &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			MakeSound(SND_BATEYES,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A3;	// shoot
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			FaceGoodguy(me,goodguy);
			return;
		}
	}
	if(me->mind3==0)
	{
		if(me->aiType==MONS_VAMPBAT)
		WanderAI(me,64,40,2,map,world,goodguy,FIXAMT*8);
		if(me->aiType==MONS_GARGOYLE)
		WanderAI(me,64,40,1,map,world,goodguy,FIXAMT*6);
		else
		WanderAI(me,64,40,3,map,world,goodguy,FIXAMT*10);
	}
	else
	{
		// have stolen something, must run away!
		FaceGoodguy(me,goodguy);
		me->facing=(me->facing+4)&7;
		me->dx=Cosine(me->facing*32)*10;
		me->dy=Sine(me->facing*32)*10;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		return;
	}
}

void AI_RollerKiller(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,f;

	if(me->ouch==4)
	{
		//NO NOISE!
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			FireBullet(me,me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(8),BLT_ROCK,me->friendly);
			FireBullet(me,me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(8),BLT_ROCK,me->friendly);
			FireBullet(me,me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(8),BLT_ROCK,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not activated yet
	{
		if(RangeToTarget(me,goodguy)<512*FIXAMT || me->ouch)
		{
			me->mind=1;
			me->mind2=3;
			MakeNormalSound(SND_CARSTART);
		}
	}
	else if(me->mind==1)	// active, zooming after Bouapha
	{
		if(me->mind2)
			me->mind2--;
		else
		{
			me->mind2=3;
			FaceGoodguy2(me,goodguy);
		}

		Dampen(&me->dx,FIXAMT/8);
		Dampen(&me->dy,FIXAMT/8);

		me->dx+=Cosine(me->facing*32)/2;
		me->dy+=Sine(me->facing*32)/2;

		if(RangeToTarget(me,goodguy)<64*FIXAMT)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(35,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,255,map,world);
				me->mind1=0;
				MakeSound(SND_ROLYPOLYWALL,me->x,me->y,SND_CUTOFF,1200);
			}
			else
			{
				me->dx=-me->dx;
				me->dy=-me->dy;	
			}
		}
		if(me->mind1)	// hit a wall
		{
			if(me->mind1&1)
				me->dx=-me->dx;
			if(me->mind1&2)
				me->dy=-me->dy;
			MakeSound(SND_ROLYPOLYWALL,me->x,me->y,SND_CUTOFF,1200);
			me->mind1=0;
		}
	}

	Clamp(&me->dx,FIXAMT*10);
	Clamp(&me->dy,FIXAMT*10);
	
	if(me->facing%2==0)
	{
		switch(me->facing)
		{
			case 0:
				f=ANIM_A1;	// roll to the right
				break;
			case 2:
				f=ANIM_MOVE; // roll down
				break;
			case 4:
				f=ANIM_A2;	// roll to the left
				break;
			case 6:
				f=ANIM_ATTACK;	// roll up
				break;
		}
		if(me->seq!=f)
		{
			me->seq=f;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	
	if(me->mind!=0)
	{
		if(me->seq==ANIM_IDLE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		if(me->seq==ANIM_MOVE)
			me->frmAdvance=(abs(me->dx)+abs(me->dy))*512/(FIXAMT*20);
	}
}

void AI_FlyingPatch(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PATCHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==18)
		{
			me->seq=ANIM_A1;
			me->frm=0;
		}
		if(me->seq==ANIM_A1 && me->frm==7 && me->reload==0)
		{
			for (i = 0; i < 8; i++)
				{
				x = me->x + Cosine(i*32)*16;
				y = me->y + Sine(i*32)*16;
				FireBullet(me,x, y, i*32, BLT_FLAME5, me->friendly);
			}
			g=AddBaby(me->x,me->y+FIXAMT*2,0,MONS_PUMPKINFLY,me);
			if(g)
			{
				g->mind=1;
				g->mind1=30;
				g->facing=Random(8);
			}
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			me->reload=45;
		}
		return;
	}

	if((!me->reload) && Random(50)==0)
	{
		MakeSound(SND_EGGOPEN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
	}
}

void AI_WinterGourd(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			for(i=0;i<8;i++)
			SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT), me->y+((-16+Random(33))<<FIXSHIFT));
			if(me->frm==2 && me->reload==0)
			{
				me->reload=10;
				for(i=0;i<8;i++)
				{
					x=me->x+Cosine(i*32)*8;
					y=me->y+Sine(i*32)*8;
					FireBullet(me,x,y,i*32,BLT_FREEZE2,me->friendly);
				}
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>=2 && me->frm<=4 && me->reload==0)
		{
			FireBullet(me,me->x+Cosine(me->facing*32)*16,me->y+Sine(me->facing*32)*16,me->facing*32,
				BLT_FREEZE2,me->friendly);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT), me->y+((-16+Random(33))<<FIXSHIFT));
		me->mind3=(byte)Random(10);
	}
	else
		me->mind3--;

	if(me->mind==0)	// not currently aware of goodguy
	{
		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}

		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<512*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				if(me->ouch==0)
					me->mind1=240;
				FaceGoodguy(me,goodguy);
			}
		}
		me->dx=0;
		me->dy=0;
	}
	else
	{
		FaceGoodguy(me,goodguy);
		if(!goodguy)
		{
			me->mind=0;	// go back to being bored, nothing to see here
			me->mind1=0;
			return;
		}

		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>=512*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}

		if(RangeToTarget(me,goodguy)<40*FIXAMT)
		{
			// shoot at him
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
		}
	}
}

void AI_StickSnowman(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT), me->y+((-16+Random(33))<<FIXSHIFT));
	SpecialSnow(me->x+((-16+Random(33))<<FIXSHIFT), me->y+((-16+Random(33))<<FIXSHIFT));
	
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			i=(me->facing*32-8+Random(17))&255;
			if(i<0)
				i+=256;
			x=me->x+Cosine(i)*4;
			y=me->y+Sine(i)*4;
			FireExactBullet(me,x,y,FIXAMT*50,Cosine(i)*8,Sine(i)*8,-FIXAMT,0,50,i,BLT_SNOWBALL,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, charge towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(16)==0)
			{
				// get him!
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(10)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=(byte)Random(100);
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, sit around
			me->mind1=(byte)Random(100);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(goodguy && RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(16)==0)
		{
			// get him!
			FaceGoodguy2(me,goodguy);
			MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}

		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(40)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Gingersnap(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GINGEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GINGERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->reload)
		me->reload--;
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm%3==0) && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,12,map,world);
			FireBullet(me,me->x,me->y,0,BLT_SHOCKWAVE,me->friendly);
			if(me->frm==11)
			{
				me->frmAdvance=64;
				me->dx=0;
				me->dy=0;
			}
			me->reload=4;
		}
		return;	// can't do nothin' right now
	}
	else
	{
		if(me->mind1>0)
			me->mind1--;
		else
		{
			me->mind1=6;
			me->mind2+=8;
		}
		
		if(me->z<FIXAMT)
		{
			me->dz=8*FIXAMT;
		}
	}
	
	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(64*FIXAMT) && Random(4)==0)
		{
			// get him!
			MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
	}

	me->facing=floor(me->mind2/32);
	me->dx=Cosine(me->mind2)*3;
	me->dy=Sine(me->mind2)*3;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
	}
}

void AI_StickFish(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
}

void AI_Yerfboss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_YERFOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_YERFLEAVE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<32)
		{
			x = me->x + Cosine(me->facing*32)*20;
			y = me->y + Sine(me->facing*32)*20;
			if (me->frm%4==0)
			FireBullet(me,x,y,me->facing*32,BLT_MISSILE,me->friendly);
		}
		if(me->seq==ANIM_DIE && me->frm>=4)
		{
			me->frmAdvance=64;
		}
		return;	// can't do nothin' right now
	}
	
	if(me->mind==0)
	{
		if(goodguy && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch>0))
		{
			me->mind=1;
		}
	}
	if(me->mind==1)	// active
	{
		if(goodguy)
		{
			// calculate desired location (want to be above Bouapha)
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/2;
			if(me->y<y)
				me->dy+=FIXAMT/2;
			if(me->x>x)
				me->dx-=FIXAMT/2;
			if(me->y>y)
				me->dy-=FIXAMT/2;

			Clamp(&me->dx,FIXAMT*6);
			Clamp(&me->dy,FIXAMT*6);
			
			if (Random(8))
			{
				FireExactBullet(me,me->x,me->y,me->z,Cosine(me->facing*32+256-8+Random(16))*8,Sine(me->facing*32+256-8+Random(16))*8,0,0,30,me->facing*32,BLT_ENERGY,me->friendly);
			}
			
			if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,36,0,0,1,map,world,me->friendly))
			{
				WeakenVictim(GetLastGuyHit(),255);
			}
		}
	}

	me->dz+=FIXAMT*3/2;
	if(me->z==0)
	{
		if(goodguy && RangeToTarget(me,goodguy)<512*FIXAMT && me->mind1==1)
		{
			FaceGoodguy(me,goodguy);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->dz=8*FIXAMT;
			return;
		}
		else
		{
		me->mind1 = Random(8);
		me->dz=FIXAMT*3+Random(FIXAMT*7);
			for(i=0;i<16;i++)
			{
				FireExactBullet(me,me->x,me->y,me->z,Cosine(i*16)*8,Sine(i*16)*8,0,0,30,i*16,BLT_ENERGY,me->friendly);
			}
		}
	}
	
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_MechaLuna(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int x,y,i;
}

void AI_ChefLiem(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WACKOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WACKODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->reload)
		me->reload--;
	
	if(me->dx==0 && me->dy==0)
	{
		me->dx=FIXAMT*2;
		me->dy=FIXAMT*4;
	}
	
	if(me->mind1&1)
	{
		if (Random(2))
			me->dx=me->dx/-3;
		else
			me->dx=-me->dx;
	}
	if(me->mind1&2)
	{
		if (Random(2))
			me->dy=me->dy/-3;
		else
			me->dy=-me->dy;
	}
	me->mind1=0;
}

void AI_Crabshell(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	if(me->mind2)
		me->mind2--;
	else
	{
		me->mind2=10;
		me->facing=((Random(8)*32)&255);
		me->dx=Cosine(me->facing)*4;
		me->dy=Sine(me->facing)*4;
	}
	
	if(me->seq==ANIM_MOVE && me->frm%2==0 && goodguy)	// hits on this frame
	{
		FindVictims2(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,0,0,1,map,world,me->friendly);
	}
	
	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;
	me->mind1=0;
	
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Noodlenode(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;

	if(me->reload)
		me->reload--;
	else if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<300*FIXAMT && (map->CheckLOS(me->mapx,me->mapy,300,goodguy->mapx,goodguy->mapy)))
		{	
			me->reload=10;
			f=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
			FireBullet(me,me->x,me->y,f,BLT_ENERGY,me->friendly);
		}
	}
	
	if(me->mind2)
		me->mind2--;
	else
	{
		me->mind2=5;
		HealOthers(me->x,me->y,me->friendly,5);
	}
	
	if(me->dx==0 && me->dy==0)
	{
		me->dx=FIXAMT*4;
		me->dy=FIXAMT*4;
	}
	
	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;
	me->mind1=0;
	
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Galvigourd(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->mind3)
		me->mind3--;

	if(me->ouch==4)
	{
		// make noise?
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->frmTimer<64)
		{
			// fire a hammer
			FireBullet(me,me->x,me->y,me->facing,BLT_MISSILE,me->friendly);
			me->reload=20+Random(50);
		}
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->frmTimer<64 && Random(4)==0)
			FaceGoodguy(me,goodguy);

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		if(me->seq==ANIM_A1 && me->frm==7)
		{
			FaceGoodguy(me,goodguy);
		}
		return;
	}

	if(me->mind==0)	// unaware of Bouapha
	{
		if(goodguy && (RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0))
		{
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->reload=5+Random(30);
			me->mind=1;	// activate
		}
		else
		{
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}
	else if(me->mind==1)	// normal mode
	{
		if(me->reload==0 && Random(4)!=1)	// time to run!
		{
			me->mind=2;	// running mode
			me->mind1 = 16;
			me->seq=ANIM_A1;	// melt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
		else if(me->reload==0)	// time to fire
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			if(goodguy)
				FaceGoodguy(me,goodguy);
			me->dx=0;
			me->dy=0;
			return;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(goodguy)
				FaceGoodguy(me,goodguy);
		}
	}
	else // running away mode
	{
		if (me->mind3==0)
			me->mind3=16;
		if (me->mind3%4==0)
			FireBullet(me,me->x,me->y,0,BLT_SLIME,me->friendly);
		if (me->mind3%8==0 && goodguy)
			FaceGoodguy(me,goodguy);
		me->seq=ANIM_A2;	// stay melted
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=16;
		if(me->mind1<=0)
		{
			if (Random(3)==1)
			{
				me->seq=ANIM_A3;	// unmelt
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->reload=8+Random(30);
				me->mind=1;	// activate
				return;
			}
			else
			{
				me->mind1 = 16;
			}
		}
		// accelerate
		me->dx+=Cosine(me->facing*32);
		me->dy+=Sine(me->facing*32);
		Clamp(&me->dx,FIXAMT*8);
		Clamp(&me->dy,FIXAMT*8);
	}
	
	if((me->seq==ANIM_A2 && me->mind1%2 == 0 && goodguy) || (me->frm%2==0 && me->seq==ANIM_MOVE && goodguy))	// hits on this frame
	{
		FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world,me->friendly);
	}
	if(me->seq==ANIM_A3)
	{
		me->dx*=0.95;
		me->dy*=0.95;
	}
}

void AI_CountEshkah(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,f,i,a;
	Guy *g;
	int summonTab[]={MONS_PUMPKIN,MONS_BIGSPDR,MONS_BOOMKIN,MONS_SPIDER2};

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ELDEROUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_ELDERDIE,me->x,me->y,SND_CUTOFF,2000);
	}
	
	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*6;
	
	if(me->reload)
	me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm==4 && me->frmTimer<32)
		{
			for(i=0;i<2;i++)
			{
				//summon: summons a spider or a pumpkin
				x=me->x+Cosine(me->facing*32)*48;
				y=me->y+Sine(me->facing*32)*48;
				x=x+Cosine((me->facing*32+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);
				y=y+Sine((me->facing*32+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);

				g=AddBaby(x,y,0,summonTab[Random(4)],me);
				if(g && !g->CanWalk(g,x,y,map,world))
					RemoveGuy(g);
				else if(g)
				{
					g->mind=1;
				}
				map->BrightTorch(g->x/(TILE_WIDTH*FIXAMT),g->y/(TILE_HEIGHT*FIXAMT),20,10);
				for(i=0;i<15;i++)
					BlowSmoke(g->x-40*FIXAMT+Random(80*FIXAMT),g->y-30*FIXAMT+Random(60*FIXAMT),0,Random(8)*FIXAMT+FIXAMT);
				x=me->x+Cosine(me->facing*32)*48;
				y=me->y+Sine(me->facing*32)*48;
				x=x+Cosine((me->facing*32+128+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);
				y=y+Sine((me->facing*32+128+64)&255)*32-FIXAMT*16+Random(FIXAMT*33);

				g=AddBaby(x,y,0,summonTab[Random(2)],me);
				if(g && !g->CanWalk(g,x,y,map,world))
					RemoveGuy(g);
				else if(g)
				{
					g->mind=1;
				}
				map->BrightTorch(g->x/(TILE_WIDTH*FIXAMT),g->y/(TILE_HEIGHT*FIXAMT),20,10);
				for(i=0;i<15;i++)
					BlowSmoke(g->x-40*FIXAMT+Random(80*FIXAMT),g->y-30*FIXAMT+Random(60*FIXAMT),0,Random(8)*FIXAMT+FIXAMT);
			}
		}
		if(me->seq==ANIM_A2)
		{
			if(me->facing>7)
				me->facing=0;
			else
				me->facing++;
			//fire: circle of fire
			map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,16,8);
			if(Random(2)==1)
			{
				a=Random(256);
				
			}
			if (!me->reload)
			{
				for(i=0;i<8;i++)
				{
					x = me->x + Cosine(i*32)*16;
					y = me->y + Sine(i*32)*16;
					if (Random(2)==1)
					FireBullet(me,me->x,me->y,0,BLT_BADSITFLAME,me->friendly);
					else
					FireExactBullet(me,me->x,me->y,FIXAMT*0,Cosine(i*32+me->mind3+8)*4,Sine(i*32+me->mind3+8)*4,FIXAMT*-1,0,30*6,0,BLT_FLAME5,me->friendly);
				}
					me->reload = 4;
			}
		}
		if(me->seq==ANIM_A3)
		{
			if(me->frm>2 && me->frm<13 && me->reload==0)
			{
				me->reload=2;
				me->mind3+=4;	// rotate the angle of fire
				for(i=0;i<5;i++)
				{
					a=(me->mind3+(i*256)/5)&255;
					x=me->x+Cosine(a)*16;
					y=me->y+Sine(a)*16;
					FireBullet(me,x,y,a,BLT_SHARK,me->friendly);
				}
				MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1200);
				if(me->facing>7)
				me->facing=0;
				else
				me->facing++;
			}
		}
		if(me->seq==ANIM_A4)
		{
			//air:
			if(goodguy->aiType==MONS_BOUAPHA)
			{
				me->mind1=255;

				if(goodguy->x<me->x)
				{
					goodguy->dx+=(me->mind1*FIXAMT/16);
					goodguy->dy+=(me->mind1*FIXAMT/128);
				}
				else
				{
					goodguy->dx-=(me->mind1*FIXAMT/16);
					goodguy->dy-=(me->mind1*FIXAMT/128);
				}
				if(goodguy->y<me->y)
				{
					goodguy->dy+=(me->mind1*FIXAMT/16);
					goodguy->dx-=(me->mind1*FIXAMT/128);
				}
				else
				{
					goodguy->dy-=(me->mind1*FIXAMT/16);
					goodguy->dx+=(me->mind1*FIXAMT/128);
				}
				Clamp(&goodguy->dx,FIXAMT*6);
				Clamp(&goodguy->dy,FIXAMT*6);
			}
			if(me->seq==ANIM_ATTACK && me->frm>=4 && me->frm<11 && goodguy)
			{
				x=me->x+Cosine(me->facing*32)*8;
				y=me->y+Sine(me->facing*32)*8;
				MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
				FireBulletZ(me,x,y,FIXAMT*12,(me->facing*32-16+(byte)Random(33))&255,BLT_ACID,me->friendly);
			}
		}
		if(me->seq==ANIM_A5)
		{
			if(me->frm<2)
			{
				me->dx=0;
				me->dy=0;
				me->dz=0;
			}
			else if(me->frm==2)
			{
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				me->dz=10*FIXAMT;
			}
			else if(me->frm==8)
			{
				me->dz=-16*FIXAMT;
			}
			else if(me->frm==10 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
				ShakeScreen(20);
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				for (i = 0; i < 8; i++)
				{
					x = me->x + Cosine(i*32)*16;
					y = me->y + Sine(i*32)*16;
					FireBullet(me,x,y,i,BLT_DIRTSPIKE,me->friendly);
				}
			}
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			//explosion
		}
		FaceGoodguy2(me,goodguy);
		me->dx+=Cosine(me->facing*32)*4/2;
		me->dy+=Sine(me->facing*32)*4/2;
		Clamp(&me->dx,FIXAMT*12);
		Clamp(&me->dy,FIXAMT*9);
		return;	// can't do nothin' right now
	}
	else if(Random(40)==0)
	{
		i = Random(5);
		//i = 5;
		if (i==1)
		{
			// summon spiders & pumpkins)
			MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx-=Cosine(me->facing*32)*4/2;
			me->dy-=Sine(me->facing*32)*4/2;
		}
		else if (i==2)
		{
			// firey stuff (fire)
			MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx-=Cosine(me->facing*32)*4/4;
			me->dy-=Sine(me->facing*32)*4/4;
		}
		else if (i==3)
		{
			// fire liquid - acid/water/ice (water)
			MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A3;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx-=Cosine(me->facing*32)*4/4;
			me->dy-=Sine(me->facing*32)*4/4;
		}
		else if (i==4)
		{
			// spikes & wind (air)
			MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A4;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx-=Cosine(me->facing*32)*4/2;
			me->dy-=Sine(me->facing*32)*4/2;
		}
		else
		{
			// jump & ground pound (earth)
			MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A5;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
		}
	}
	else
	{
		if(me->z<16*FIXAMT)
			me->dz+=FIXAMT*3;
	}
	// calculate desired location (want to be above Bouapha)
	x=goodguy->x;
	y=goodguy->y-128*FIXAMT;

	FaceGoodguy3(me,goodguy);
	if(me->x<x)
		me->dx+=FIXAMT/4;
	if(me->y<y)
		me->dy+=FIXAMT/4;
	if(me->x>x)
		me->dx-=FIXAMT/4;
	if(me->y>y)
		me->dy-=FIXAMT/4;

	Clamp(&me->dx,FIXAMT*8);
	Clamp(&me->dy,FIXAMT*6);
}

void AI_Shapeshifter(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,j,t;
	Guy *g,*g2;
	int bossTab[]={MONS_IMITATER,MONS_MATHEAD,MONS_THING,MONS_SVEN,MONS_BJORN,MONS_SPHINX,MONS_SDZL,MONS_COUNTESS,MONS_KONGOR,MONS_PATTY,MONS_DOZER,MONS_LOOKEYLOO,MONS_BOBBY,MONS_GOAT1,MONS_GOAT2,MONS_GOAT3,MONS_SPATULA,MONS_SKULL,MONS_RADISH,MONS_SPEEDY,MONS_GRUE,MONS_LARRY,MONS_FRANK,MONS_BIGGHOST,MONS_ELDER,MONS_ELDER2,MONS_ELDER3,MONS_ELDER4,MONS_BONKULA,MONS_YERFBOSS};//~30

	//Boss list: Imitater, Matilda, The Thing, Yeti Brothers, Sphinxter, SDZL, Countess, Kongor, Loonybot 5000,
	//			Crabby Patty, Dozer the Dozerian, The Lookey-Loo, Bobby Khan, Optimum Octon, Three Gruffs,
	//			Madame Spatula, Headless Horseradish, Speedy Hermandez, Iron Skull, The Grue, Spookley?,
	//			Frankenjulie, Larry, Polterguy, The Elders, Bonkula, Happy Stick Witch, Yerfdog
	
	if(me->action==ACTION_BUSY)
	{
		if(Random(64) == 0 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(me,x,y,me->facing-8+256+Random(16),BLT_ENERGY,me->friendly);
			me->reload=2;
			return;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->type!=MONS_IMITATER)
			me->type=MONS_IMITATER; //turn back into your original form!
			else
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z-Random(FIXAMT*60),FIXAMT);
			}
		return;
		}
	}
	
	if(Random(256)==1 && me->type!=MONS_SPHINX)
	{
		// teleport
		MakeSound(SND_GHOSTTPORT,me->x,me->y,SND_CUTOFF,1200);
		me->x+=-FIXAMT*320+Random(FIXAMT*640);
		me->y+=-FIXAMT*240+Random(FIXAMT*480);
		if(me->x<TILE_WIDTH*2*FIXAMT)
			me->x=TILE_WIDTH*2*FIXAMT;
		else if(me->x>(map->width-2)*TILE_WIDTH*FIXAMT)
			me->x=(map->width-2)*TILE_WIDTH*FIXAMT;
		if(me->y<TILE_HEIGHT*2*FIXAMT)
			me->y=TILE_HEIGHT*2*FIXAMT;
		else if(me->y>(map->height-2)*TILE_HEIGHT*FIXAMT)
			me->y=(map->height-2)*TILE_HEIGHT*FIXAMT;
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
		MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,1200);
	}
	
	if (me->reload)
	me->reload--;

	if (me->mind5)
	{
		me->mind5--;
		if (me->mind5 < 30 && me->seq!=ANIM_ATTACK)
		{
			me->seq=ANIM_ATTACK;	// breathe stuff
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=256;
		}
	}
	else if (me->mind5==0)
	{
	x = me->x;
	y = me->y;
	me->dx = 0*FIXAMT;
	me->dy = 0*FIXAMT;
		//the effects!
		RemoveKids(me);
		TeamChangeRing(7,me->x-24*FIXAMT,me->y-24*FIXAMT,0,16,4);
		TeamChangeRing(7,me->x-24*FIXAMT,me->y+24*FIXAMT,0,16,4);
		TeamChangeRing(7,me->x+24*FIXAMT,me->y-24*FIXAMT,0,16,4);
		TeamChangeRing(7,me->x+24*FIXAMT,me->y+24*FIXAMT,0,16,4);
		TeamChangeRing(7,me->x,me->y,0,16,4);
		for(i=0;i<30;i++)
			BlowSmoke(me->x-32*FIXAMT+Random(64*FIXAMT),me->y-32*FIXAMT+Random(64*FIXAMT),Random(64)<<FIXSHIFT,Random(FIXAMT*4));
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,6,12);
		//the actual changing
		if (Random(5)!=1)
			me->type=bossTab[Random(30)];
		else
			me->type=me->aiType;
		me->special=1;
		//sounds
		//sounds
		if(me->friendly)
		MakeSound(SND_TURNGOOD,me->x,me->y,SND_CUTOFF,10);
		else
		MakeSound(SND_TURNEVIL,me->x,me->y,SND_CUTOFF,10);
	
	if(me->type==MONS_MATHEAD)	// Matilda, need to add all the parts
	{
		me->z=32*FIXAMT;
		g=AddGuy(x,y-32*FIXAMT,0,MONS_MATBODY,me->friendly);
		if(g)
			{g->parent=me;
			g->special=1;}
		g=AddGuy(x-40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_MATCLAW1,me->friendly);
		if(g)
			{g->parent=me;
			g->special=1;}
		g=AddGuy(x+40*FIXAMT,y-1*FIXAMT,32*FIXAMT,MONS_MATCLAW2,me->friendly);
		if(g)
			{g->parent=me;
			g->special=1;}
		g=AddGuy(x,y-96*FIXAMT,32*FIXAMT,MONS_MATTAIL,me->friendly);
		if(g)
			{g->parent=me;
			g->special=1;}
	}
	if(me->type==MONS_THING)	// The Thing needs to add tentacles
		{
			// lower left tentacle
			g=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
			g->parent=me;
			g->special=1;
			g->facing=6;
			g->mind=0;	// mind tells them which tentacle they are (to constrain rotation)
			g->mind1=128;
			for(j=0;j<3;j++)
			{
				g2=g;
				// adding them all overlapping, they'll move into place when updated
				g=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
				g->parent=g2;
				g->facing=6;
				g->mind=0;
				g->special=1;
			}
			g2=AddGuy(x-64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,me->friendly);
			g2->parent=g;
			g2->special=1;
			g2->facing=6;
			g2->mind=0;
			// lower right tentacle
			g=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
			g->parent=me;
			g->special=1;
			g->facing=2;
			g->mind=1;
			g->mind1=128;
			for(j=0;j<3;j++)
			{
				g2=g;
				// adding them all overlapping, they'll move into place when updated
				g=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
				g->parent=g2;
				g->special=1;
				g->facing=2;
				g->mind=1;
			}
			g2=AddGuy(x+64*FIXAMT,y+48*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,me->friendly);
			g2->parent=g;
			g2->special=1;
			g2->facing=2;
			g2->mind=1;
			// upper right tentacle
			g=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
			g->parent=me;
			g->special=1;
			g->facing=14;
			g->mind=2;
			g->mind1=128;
			for(j=0;j<3;j++)
			{
				g2=g;
				// adding them all overlapping, they'll move into place when updated
				g=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
				g->parent=g2;
				g->special=1;
				g->facing=14;
				g->mind=2;
			}
			g2=AddGuy(x+64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,me->friendly);
			g2->parent=g;
			g2->special=1;
			g2->facing=14;
			g2->mind=2;
			// upper left (and last!) tentacle
			g=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
			g->parent=me;
			g->special=1;
			g->facing=10;
			g->mind=3;
			g->mind1=128;
			for(j=0;j<3;j++)
			{
				g2=g;
				// adding them all overlapping, they'll move into place when updated
					g=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENT,me->friendly);
				g->parent=g2;
				g->special=1;
				g->facing=10;
				g->mind=3;
			}
			g2=AddGuy(x-64*FIXAMT,y-20*FIXAMT,12*FIXAMT,MONS_THINGTENTTIP,me->friendly);
			g2->parent=g;
			g2->special=1;
			g2->facing=10;
			g2->mind=3;
		}
		if(me->type==MONS_SPHINX) //sphinxter gets its arms
		{
			// left arm
			g=AddGuy(x-108*FIXAMT,y+22*FIXAMT,0,MONS_SPHXARM1,me->friendly);
			g->parent=me;
			g->special=1;
			// right arm
			g=AddGuy(x+108*FIXAMT,y+22*FIXAMT,0,MONS_SPHXARM2,me->friendly);
			g->parent=me;
			g->special=1;
		}
		if(me->type==MONS_SVEN||me->type==MONS_BJORN||me->type==MONS_COUNTESS||me->type==MONS_KONGOR||me->type==MONS_SKULL||me->aiType==MONS_FRANK||me->aiType==MONS_BIGGHOST)
		{
			me->mind=1;
		}
		me->mind5=30*(Random(5)+5);
		me->seq=ANIM_MOVE;
	}
	
	//imitater
	if (me->type==MONS_IMITATER)
	{
		if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				// spit at him
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else if(me->mind==1)	// random wandering
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				// spit at him
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}

			if(!(me->mind1--))	// time to get a new direction
			{
				if((goodguy) && Random(3)==0)
					me->mind=0;	// get back on track
				else
					me->facing=(byte)Random(8);
				me->mind1=Random(40)+1;
			}

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
	}
	//matilda
	if(me->type==MONS_MATHEAD)
	{
		AI_MattieSkullOrHead(me,map,world,goodguy);
	}
	//the thing
	if(me->type==MONS_THING)
	{
		AI_TheThing(me,map,world,goodguy);
	}
	//sven & bjorn
	if(me->type==MONS_SVEN||me->type==MONS_BJORN)
	{
		AI_Yeti(me,map,world,goodguy);
	}
	//sphinxter
	if(me->type==MONS_SPHINX)
	{
		AI_Sphinx(me,map,world,goodguy);
	}
	//sdzl
	if(me->type==MONS_SDZL)
	{
		AI_SDZL(me,map,world,goodguy);
	}
	//countess
	if(me->type==MONS_COUNTESS)
	{
		AI_Countess(me,map,world,goodguy);
	}
	//kongor
	if(me->type==MONS_KONGOR)
	{
		AI_Kongor(me,map,world,goodguy);
	}
	//patty
	if(me->type==MONS_PATTY)
	{
		AI_Patty(me,map,world,goodguy);
	}
	//dozer
	if(me->type==MONS_DOZER)
	{
		AI_Dozer(me,map,world,goodguy);
	}
	//lookey loo
	if(me->type==MONS_LOOKEYLOO)
	{
		AI_Lookeyloo(me,map,world,goodguy);
	}
	//booby khan
	if(me->type==MONS_BOBBY)
	{
		AI_BobbyKhan(me,map,world,goodguy);
	}
	//rammy
	if(me->type==MONS_GOAT1)
	{
		AI_Rammy(me,map,world,goodguy);
	}
	//hoppy
	if(me->type==MONS_GOAT2)
	{
		AI_Hoppy(me,map,world,goodguy);
	}
	//gruffy
	if(me->type==MONS_GOAT3)
	{
		AI_Gruffy(me,map,world,goodguy);
	}
	//madam spatula
	if(me->type==MONS_SPATULA)
	{
		AI_Spatula(me,map,world,goodguy);
	}
	//iron skull
	if(me->type==MONS_SKULL)
	{
		AI_IronSkull(me,map,world,goodguy);
	}
	//headless horseradish
	if(me->type==MONS_RADISH)
	{
		AI_Horseradish(me,map,world,goodguy);
	}
	//speedy hermandez
	if(me->type==MONS_SPEEDY)
	{
		AI_Speedy(me,map,world,goodguy);
	}
	//teh grue!
	if(me->type==MONS_GRUE)
	{
		AI_Grue(me,map,world,goodguy);
	}
	//larry
	if(me->type==MONS_LARRY)
	{
		AI_Larry(me,map,world,goodguy);
	}
	//frankenjulie
	if(me->type==MONS_FRANK)
	{
		AI_Frankenjulie(me,map,world,goodguy);
	}
	//polterguy
	if(me->type==MONS_BIGGHOST)
	{
		AI_Polterguy(me,map,world,goodguy);
	}
	//ELDER
	if(me->type==MONS_ELDER)
	{
		AI_WindElder(me,map,world,goodguy);
	}
	//elder2
	if(me->type==MONS_ELDER2)
	{
		AI_EarthElder(me,map,world,goodguy);
	}
	//elder3
	if(me->type==MONS_ELDER3)
	{
		AI_WaterElder(me,map,world,goodguy);
	}
	//elder4
	if(me->type==MONS_ELDER4)
	{
		AI_FireElder(me,map,world,goodguy);
	}
	//bonkula
	if(me->type==MONS_BONKULA)
	{
		AI_Bonkula(me,map,world,goodguy);
	}
	//yerfdog
	if(me->type==MONS_YERFBOSS)
	{
		AI_Yerfboss(me,map,world,goodguy);
	}
}

void AI_Hotfoot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	map->DimTorch(me->mapx,me->mapy,2);
	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			for(i=0;i<4;i++)
				FireBullet(me,me->x,me->y,0,BLT_BADSITFLAME,me->friendly);
			me->reload=100;
		}
		return;	// can't do nothin' right now
	}
	
	if(Random(4)==0)
		FireBullet(me,me->x,me->y,Random(256),BLT_FLAME3,me->friendly);

	FaceGoodguy(me,goodguy);
	if(!goodguy)
	{
		return;
	}

	me->dx=Cosine(me->facing*32)*6;
	me->dy=Sine(me->facing*32)*6;
	
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Yeti3(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_YETIOUCH,me->x,me->y,SND_CUTOFF,1200);
		else if(me->mind5>0)
			MakeSound(SND_YETIDIE,me->x,me->y,SND_CUTOFF,1200);
		else
		{
			MakeSound(SND_TURNEVIL,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<2;i++)
			{
				int tries;
				tries=0;
				while((tries++)<200)
				{//Find a place for the split ones
				x=me->x-FIXAMT*100+(rand()%(FIXAMT*200+1));
				y=me->y-FIXAMT*100+(rand()%(FIXAMT*200+1));
					if(me->CanWalk(me,x,y,map,world))
					{
						g=AddGuy(x,y,0,me->aiType,me->friendly);
						if(g)
						{
							g->hp=me->maxHP/2;
							g->maxHP=me->maxHP/2;
							g->mind5=i+1;
							g->aiType=me->aiType;
							g->seq=ANIM_MOVE;
							tries = 200;
						}
					}
				}
			}
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;	// can only die forward
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm<8 && me->frmTimer<32)
		{
			me->facing=(me->facing+1)&7;
			if(me->frm%2==0)
			{
				x=me->x+Cosine(me->facing*32)*64;
				y=me->y+Sine(me->facing*32)*64;
				x+=Cosine(((me->facing*32)*32)&255)*32;
				y+=Sine(((me->facing*32)*32)&255)*32;
				FireExactBullet(me,x,y,40*FIXAMT,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,4*FIXAMT,0,100,
								me->facing*32,BLT_BIGSNOW,me->friendly);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>7 && me->frm<15 && me->frmTimer<32)
		{
			me->mind4=me->facing*32+256-8+Random(16);
			x=me->x+Cosine(me->mind4)*64;
			y=me->y+Sine(me->mind4)*64;
			x+=Cosine(((me->mind4)*32)&255)*32;
			y+=Sine(((me->mind4)*32)&255)*32;
			FireExactBullet(me,x,y,40*FIXAMT,Cosine(me->mind4)*8,Sine(me->mind4)*8,4*FIXAMT,0,100,
							me->mind4,BLT_SNOWBALL,me->friendly);
			if(me->mind5 && me->frm % 2 == 0)
			{
				FireExactBullet(me,x,y,40*FIXAMT,Cosine((me->mind4+64+Random(8)-16)&255)*8,Sine((me->mind4+64)&255)*8,4*FIXAMT,0,100,
								(me->mind4+64)&255,BLT_SNOWBALL,me->friendly);
				FireExactBullet(me,x,y,40*FIXAMT,Cosine((me->mind4-64+Random(8)-16)&255)*8,Sine((me->mind4-64)&255)*8,4*FIXAMT,0,100,
								(me->mind4+64)&255,BLT_SNOWBALL,me->friendly);
			}
			me->reload=10;
		}
		if(me->seq==ANIM_A2 && me->frm>3)
		{
			ShakeScreen(5);
			// snowballs fall all over
			x=me->x+((320-Random(641))<<FIXSHIFT);
			y=me->y+((240-Random(481))<<FIXSHIFT);
			if(me->mind5 <= 0)
			{
				if(Random(3)==0)
					FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_BIGSNOW,me->friendly);
				else
					FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_SNOWBALL,me->friendly);
			}
			else
			{
				if(Random(2)==0)
					FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_BIGSNOW2,me->friendly);
				else
					FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_BIGSNOW,me->friendly);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==5 && me->frmTimer<64)
		{
			if(me->mind5 == 0)
				for(i=0;i<4;i++)
				{
					x=me->x+Cosine(i*2)*64;
					y=me->y+Sine(i*2)*64;
					FireBullet(me,x,y,i*2,BLT_ICESPIKE,me->friendly);
				}
			else
			{
				int j = rand()%2;
				for(i=0;i<4;i++)
				{
					x=me->x+Cosine(i*2+j)*64;
					y=me->y+Sine(i*2+j)*64;
					FireBullet(me,x,y,i*2+j,BLT_BEAM,me->friendly);
				}
			}
		}
		return;	// can't do nothin' right now
	}
	if(me->mind==0)
	{
		if((goodguy && RangeToTarget(me,goodguy)<(300*FIXAMT)) || (me->ouch))
		{
			me->mind=1;	// start into action
			me->mind1=0;
		}
	}
	else
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			// rotate
			me->facing=(me->facing-1+Random(3))&7;
			me->mind1=Random(30)+1;
		}
		if((!me->reload) && goodguy && Random(20)==0)
		{
			// select an attack method
			x=Random(16);
			if(x<12) // 75% chance of primary weapon
			{
				if(me->mind5 == 0)
				{
					if(x<7)
					{
						me->seq=ANIM_ATTACK;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=192;
						FaceGoodguy(me,goodguy);
						me->action=ACTION_BUSY;
					}
					else
					{
						me->seq=ANIM_A1;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=192;
						FaceGoodguy(me,goodguy);
						me->action=ACTION_BUSY;
					}
				}
				else
				{
					if(me->mind5 == 1)
					{
						me->seq=ANIM_ATTACK;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
						FaceGoodguy(me,goodguy);
						me->action=ACTION_BUSY;
					}
					else
					{
						me->seq=ANIM_A1;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
						FaceGoodguy(me,goodguy);
						me->action=ACTION_BUSY;
					}
				}
			}
			else	// 25% chance of yodel
			{
				MakeSound(SND_YETIYODEL,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				if(me->mind5)
					me->frmAdvance=64;
				else
					me->frmAdvance=96;
				me->facing=2;
				me->action=ACTION_BUSY;
			}
			me->dx=0;
			me->dy=0;
			return;
		}

		if(me->mind5)
		{
			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
		}
		else
		{
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			if(me->mind5)
				me->frmAdvance=128;
			else
				me->frmAdvance=192;
		}

	}
}

void AI_Thing2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,f,f2;
	Guy *g;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}
	
	if(me->mind1&1)
		me->dx=-me->dx*0.75;
	if(me->mind1&2)
		me->dy=-me->dy*0.75;
	me->mind1=0;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A2)
		{
			if(me->frm>4&&me->frm<13&&me->reload==0)
			{
				MakeSound(SND_MUSHSPORES,me->x,me->y,SND_CUTOFF,1500);
				f=Random(88)+20;
				f2=Random(88)+20;
				FireExactBullet(me,me->x-48*FIXAMT,me->y-10*FIXAMT,40*FIXAMT,Cosine(f)*8,Sine(f)*8,-0.5*FIXAMT,0,100,
								f,BLT_BIGSHELL,me->friendly);
				FireExactBullet(me,me->x+48*FIXAMT,me->y-10*FIXAMT,40*FIXAMT,Cosine(f2)*8,Sine(f2)*8,-0.5*FIXAMT,0,100,
								f2,BLT_BIGSHELL,me->friendly);
				if (!me->mind3)
					me->reload=4;
				else if (me->mind3 <= 2)
					me->reload=3;
				else
					me->reload=2;
				
			}
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0)
				KillKids(me);	// destroy the tentacles
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			me->x+=((1-Random(3))<<FIXSHIFT);
			me->y+=((1-Random(3))<<FIXSHIFT);
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->frmTimer<32)
		{
			g=AddBaby(me->x,me->y,0,MONS_WHATSIT,me);
			if(g)
			{
				if(!g->CanWalk(g,g->x,g->y,map,world))
					RemoveGuy(g);
				g->parent=me;
				g->seq=ANIM_A1;	// take a nap
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=32;
				g->action=ACTION_BUSY;
			}
		}
		return;	// can't do nothin' right now
	}
	if(Random(128)==0)	// blink
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
	}
	if(me->reload==0 && Random(10 - me->mind3)==0)
	{
		MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
		FireBullet(me,me->x-48*FIXAMT,me->y-10*FIXAMT,Random(88)+20,BLT_REDNADE,me->friendly);
		FireBullet(me,me->x+48*FIXAMT,me->y-10*FIXAMT,Random(88)+20,BLT_REDNADE,me->friendly);
		me->reload=30;
	}
	if(Random(128 - me->mind3*5)==0)	// tongue
	{
		MakeSound(SND_THINGVOMIT,me->x,me->y,SND_CUTOFF,1500);
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
	if(goodguy)
	{
		x=goodguy->x;
		y=goodguy->y-50*FIXAMT;

		FaceGoodguy2(me,goodguy);
		if(me->x<x)
			me->dx+=FIXAMT/12;
		if(me->y<y)
			me->dy+=FIXAMT/12;
		if(me->x>x)
			me->dx-=FIXAMT/12;
		if(me->y>y)
			me->dy-=FIXAMT/12;
		
		Clamp(&me->dx,FIXAMT*8);
		Clamp(&me->dy,FIXAMT*8);
	}
}

void AI_Sphinx2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,f2;
	Guy *g;
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->reload)
		me->reload--;
	if(me->mind2 && me->action!=ACTION_BUSY)
		me->mind2--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPHINXOUCH,me->x,me->y,SND_CUTOFF,600);
		else
			MakeSound(SND_SPHINXDIE,me->x,me->y,SND_CUTOFF,600);
	}
	
	if(me->z==0 && me->mind==1)
	{
		if(me->action!=ACTION_BUSY)
		{
			me->dz = 20*FIXAMT;
		}
		if (!me->mind2)
		{
			me->mind2 = 2;
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
			ShakeScreen(10);
			FireBullet(me,me->x,me->y,0,BLT_SHOCKWAVE,me->friendly);
		}
	}
	
	if (me->mind3)
	{
		if(me->z<40*FIXAMT)	// go up if you need to
			me->dz+=4*FIXAMT;
		Clamp(&me->dz,FIXAMT*12);
	}
	
	
	if(me->mind==1)
	{
		if(goodguy)
		{
			// calculate desired location (want to be above Bouapha)
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/4;
			if(me->y<y)
				me->dy+=FIXAMT/4;
			if(me->x>x)
				me->dx-=FIXAMT/4;
			if(me->y>y)
				me->dy-=FIXAMT/4;

			Clamp(&me->dx,FIXAMT*6);
			Clamp(&me->dy,FIXAMT*6);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		Dampen(&me->dx,FIXAMT*0.95);
		Dampen(&me->dy,FIXAMT*0.95);
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm>4&&me->frm<18&&me->reload==0)
			{
				f2=Random(88)+20;
				FireExactBullet(me,me->x,me->y-10*FIXAMT,56*FIXAMT,Cosine(f2)*8,Sine(f2)*8,12*FIXAMT,0,100, f2,BLT_GRENADE,me->friendly);
				MakeSound(SND_MUSHMISSILE,x,y,SND_CUTOFF,1050);
				me->reload=4;
			}
			else if(me->frm==18)
			me->reload=120 + rand()%10*10;
		}
		if(me->seq==ANIM_A1 && me->frm==12 && me->frmTimer<32)
		{
			// sneeze some rocks out
			for(x=0;x<6;x++)
			{
				FireExactBullet(me,me->x,me->y+FIXAMT*40,60*FIXAMT,(8*FIXAMT-Random(17*FIXAMT)),
								Random(8*FIXAMT),0,0,60,2,BLT_BIGYELLOW,me->friendly);
			}
			// special rocks to deal with people trying to hide in "safe" spots
			FireExactBullet(me,me->x,me->y+FIXAMT*40,60*FIXAMT,8*FIXAMT,0,0,0,60,2,BLT_BIGYELLOW,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*40,60*FIXAMT,-8*FIXAMT,0,0,0,60,2,BLT_BIGYELLOW,me->friendly);
			me->reload=120 + rand()%10*10;
		}

		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0 && me->frmTimer<32)
				KillKids(me);
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(60);
		}
		return;
	}

	if(me->mind==0)
	{
		if(goodguy && (RangeToTarget(me,goodguy)<400*FIXAMT || me->ouch>0))
		{
			me->mind=1;
		}
	}
	if(me->mind==1)	// active
	{

		if(RangeToTarget(me,goodguy)<600*FIXAMT && goodguy->y>me->y && me->reload==0)
		{
			me->mind3 = me->mind3*-1+1;
			if(Random(3)==0)
			{
				MakeSound(SND_SPHINXSNEEZE,me->x,me->y,SND_CUTOFF,600);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				return;
			}
			else	
			{
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				return;
			}
		}
	}

	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_SDZL2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i, x, y, w;
	
	w = (me->hp/me->maxHP)*-1+2; //multiplier

	if (!goodguy)
		return; // no point in doing anything without an opponent

	if (me->reload)
		me->reload--;

	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_SDZLOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
			MakeSound(SND_SDZLDIE, me->x, me->y, SND_CUTOFF, 1200);
	}

	if (me->action == ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=8)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}
		
		if (me->seq == ANIM_ATTACK && me->frm >= 9 && me->frm <= 13 && me->mind1 == 0)
		{
			// slash for pain
			x = (me->x >> FIXSHIFT) + 50;
			y = (me->y >> FIXSHIFT) + 10;

			if (FindVictim(me,x, y, 100, -FIXAMT * 12, FIXAMT * 4, 25, map, world, me->friendly))
			{
				i=floor(Random(6+18));
				goodguy->dx = -FIXAMT * i-6;
				goodguy->dy = FIXAMT * i-18;
				me->mind1 = 1; // don't hit him again
			}
		}
		if (me->seq == ANIM_ATTACK && me->frm == 6 && me->mind1 == 0)
		{
			//idk
		}
		if (me->seq == ANIM_A5 && me->frm == 1 && me->frmTimer < 64)
		{
			for (i = 0; i < 5; i++)
			{
			x = me->x + Cosine(i*32)*16;
			y = me->y + Sine(i*32)*16;
			FireBullet(me,x, y, i*32, BLT_GRENADE, me->friendly);
			}
		}
		if (me->seq == ANIM_A4 && me->frm == 1 && me->frmTimer < 64)
		{
			int j = rand()%12;
			for (i = 0; i < 12; i += 1)
				FireExactBullet(me,me->x+Cosine(i*256/12)*32,me->y+Sine(i*256/12)*32,FIXAMT*30,Cosine(i*256/12)*(4+((i+j)%12)/2), Sine(i*256/12)*(4+((i+j)%12)/2),0,0,150,i*256/12,BLT_BIGYELLOW,me->friendly);
			me->reload = 5;
			me->dx = Cosine(me->facing)*12*w;
			me->dy = Sine(me->facing)*12*w;
			MonsterGlow(me->x,me->y,me->aiType);
		}
		if (me->seq == ANIM_A4)
		{
			FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,80,me->dx*4,me->dy*4,10,map,world,me->friendly);
		}
		if (me->seq == ANIM_A2 && me->frm == 12 && me->frmTimer < 64)
		{
			// spew projectiles
			for (i = 0; i < 8 ; i++)
			{
				x = Cosine((i*32+16)&255);
				y = Sine((i*32+16)&255);
				FireExactBullet(me,me->x+x*32,me->y+y*32,FIXAMT*60,x*10,y*10,FIXAMT,0,40,2,BLT_REDNADE,me->friendly);
			}
		}
		if (me->seq == ANIM_A3 && (me->frm == 7 && me->frmTimer<32))
		{
			FireBullet(me,me->x - FIXAMT*32, me->y + FIXAMT*96, 0, BLT_SHOCKWAVE, me->friendly);
		}
		if (me->seq == ANIM_A3 && (me->frm == 7 || me->frm == 8))
		{
			ShakeScreen(60);
			MakeSound(SND_SDZLPOUND, me->x, me->y, SND_CUTOFF, 1200);
			for (i = 0; i < 12; i++)
			{
				x = me->x + ((256 - Random(1025)) << FIXSHIFT);
				y = me->y + ((256 - Random(1025)) << FIXSHIFT);
				if (rand()%4==1)
					FireExactBullet(me,x, y, FIXAMT * 80 + (Random(40) << FIXSHIFT), 0, 0, 0, 0, 30, 2, BLT_ROCK, me->friendly);
				else
					FireExactBullet(me,x, y, FIXAMT * 80 + (Random(40) << FIXSHIFT), 0, 0, 0, 0, 30, 2, BLT_ROCKBIG, me->friendly);
			}
		}

		if (me->seq == ANIM_DIE && me->frm == 14 && me->frmTimer < 32)
		{
			for(i=0;i<8;i++)
			FireBullet(me,me->x-32*FIXAMT+Random(65)*FIXAMT,me->y-32*FIXAMT+Random(65)*FIXAMT,0,BLT_BOOM,me->friendly);
		}
		return; // can't do nothin' right now
	}

	me->mind1 = 0;

	FaceGoodguy2(me, goodguy);

	if (me->reload == 0 && RangeToTarget(me, goodguy) < 256 * FIXAMT)
	{
		i = Random(1024);
		if (i < 40)
		{
			MakeSound(SND_SDZLSLASH, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_A3; // ground pound!
			me->frmTimer = 0;
			me->frm = 0;
			me->frmAdvance = 128;
			me->dx = 0;
			me->dy = 0;
			me->action = ACTION_BUSY;
			me->reload = 60;
			return;
		}
		else if (i < 100)
		{
			MakeSound(SND_SDZLSPIT, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_A2; // breathe stuff
			me->frmTimer = 0;
			me->frm = 0;
			me->frmAdvance = 128;
			me->dx = 0;
			me->dy = 0;
			me->action = ACTION_BUSY;
			me->reload = 60;
			return;
		}
		else if (i < 180)
		{
			MakeSound(SND_SDZLSLASH, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_ATTACK; // slash
			me->frmTimer = 0;
			me->frm = 0;
			me->frmAdvance = 128;
			me->dx = 0;
			me->dy = 0;
			me->action = ACTION_BUSY;
			me->reload = 30;
			return;
		}
		else if (i < 260)
		{
			MakeSound(SND_BOBBYSLASH, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_A4; // slash
			me->facing = ((me->facing * 32) - 16 + Random(33));
			me->frmTimer = 0;
			me->frm = 0;
			me->frmAdvance = 128;
			me->dx = 0;
			me->dy = 0;
			me->action = ACTION_BUSY;
			me->reload = 60;
			return;
		}
		else if (i < 340)
		{
			MakeSound(SND_BOBBYSTOMP, me->x, me->y, SND_CUTOFF, 1200);
			me->seq = ANIM_A5; // slash
			me->facing = ((me->facing * 32) - 16 + Random(33));
			me->frmTimer = 0;
			me->frm = 0;
			me->frmAdvance = 128;
			me->dx = 0;
			me->dy = 0;
			me->action = ACTION_BUSY;
			me->reload = 30;
			return;
		}
	}

	if (me->facing == 7 || me->facing < 3)
		i = ANIM_MOVE; // move to the right
	else
		i = ANIM_A1; // move to the left
	if (me->seq == ANIM_MOVE || me->seq == ANIM_A1)
	{
		if (me->facing == 0 || me->facing == 1 || me->facing == 7)
		me->dx += FIXAMT*w;
		else if (me->facing == 3 || me->facing == 4 || me->facing == 5)
			me->dx -= FIXAMT*w;
		if (me->facing == 1 || me->facing == 2 || me->facing == 3)
			me->dy += FIXAMT*w;
		else if (me->facing == 5 || me->facing == 6 || me->facing == 7)
			me->dy -= FIXAMT*w;

		Dampen(&me->dx, FIXAMT / 2);
		Dampen(&me->dy, FIXAMT / 2);
		Clamp(&me->dx, FIXAMT * 5);
		Clamp(&me->dy, FIXAMT * 5);
	}
	if (me->seq != i)
	{
		me->seq = i;
		me->frmTimer = 0;
		me->frm = 0;
		me->frmAdvance = 128;
		me->dx = Cosine(me->facing * 32)*6;
		me->dy = Sine(me->facing * 32)*4;
	}
}

void AI_Firedancer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A4 && me->frm>=4 && me->z==0 && goodguy)
		{
			if (me->mind4==0)
			{
				me->mind4=1;
				MakeSound(SND_ACIDSPLAT, me->x, me->y, SND_CUTOFF, 1500);
				for (i=0; i<16; i++)
				{
					x = me->x + Cosine(i*16)*16;
					y = me->y + Sine(i*16)*16;
					FireBullet(me,x, y, i*16, BLT_FLAME5, me->friendly);
				}
			}
			else
			{
				if(goodguy->x<me->x)
				{
					goodguy->dx-=(me->mind1*FIXAMT/16);
					goodguy->dy-=(me->mind1*FIXAMT/128);
				}
				else
				{
					goodguy->dx+=(me->mind1*FIXAMT/16);
					goodguy->dy+=(me->mind1*FIXAMT/128);
				}
				if(goodguy->y<me->y)
				{
					goodguy->dy-=(me->mind1*FIXAMT/16);
					goodguy->dx+=(me->mind1*FIXAMT/128);
				}
				else
				{
					goodguy->dy+=(me->mind1*FIXAMT/16);
					goodguy->dx-=(me->mind1*FIXAMT/128);
				}
				Clamp(&goodguy->dx,FIXAMT*8);
				Clamp(&goodguy->dy,FIXAMT*8);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>=4 && me->frm<11 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			for(i=0;i<2;i++)
			FireBulletZ(me,x,y,FIXAMT*12,(me->facing*32-16+(byte)Random(33))&255,BLT_FLAME5,me->friendly);
		}
		if(me->seq==ANIM_A1 && goodguy->aiType==MONS_BOUAPHA)	// unga bunga!
		{
			me->mind1=255;
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(Random(2)==0)
			FireBulletZ(me,x,y,FIXAMT*12,(me->facing*32-16+(byte)Random(33))&255,BLT_FLAME5,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not currently aware of goodguy
	{
		me->frmAdvance=32;	// overcome the auto-128 frmAdvance of guy.cpp

		if(me->seq==ANIM_MOVE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
		if(me->mind1<101)
			me->mind1++;

		if(me->mind1>100 && Random(200)==0)
		{
			me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
			me->mind1=0;
		}
		if((RangeToTarget(me,goodguy)<320*FIXAMT || me->ouch>0)&&me->z==0)
		{
			// if the goodguy is near, or he shot me
			me->mind=1;	// start hunting
			me->mind1=255;	// for a long time
			FaceGoodguy(me,goodguy);

			// do angry animation
			me->action=ACTION_BUSY;
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;	// very fast
			me->mind5=0;
			MakeSound(SND_PYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
		}
		me->dx=0;
		me->dy=0;
	}
	else
	{
		if (!me->mind5 && me->z == 0)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(24)==0 && me->reload==0)
			{
				// fire!!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=255;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=1;
				FaceGoodguy2(me,goodguy);
				return;
			}
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 && me->reload==0)
			{
				// blow him away!!
				MakeSound(SND_UNGADANCE,me->x,me->y,SND_CUTOFF,1200);
				MakeSound(SND_PYGMYSPIN,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=96;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=1;
				FaceGoodguy2(me,goodguy);
				return;
			}
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(24)==0 && me->reload==0)
			{
				// jump
				MakeSound(SND_PYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A4;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=96;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32)*(Random(6)+3);
				me->dy=Sine(me->facing*32)*(Random(6)+3);
				me->dz=20*FIXAMT;
				me->reload=1;
				me->mind4=0;
				return;
			}
		}

		if(me->ouch && me->mind1<240)
			me->mind1+=10;
		if(me->mind1)
			me->mind1--;
		else
		{
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		else
		{
			if (me->mind5)
				me->mind5--;
			else if (!me->mind5 && me->z == 0)
			{
				FaceGoodguy2(me,goodguy);
				me->dx=Cosine(me->facing*32)*3+Random(3);
				me->dy=Sine(me->facing*32)*3+Random(3);
				me->dz=FIXAMT*10;
				me->mind5+=10;
			}
		}
	}
}

void AI_Kongor2(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_KONGOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_KONGDIE,me->x,me->y,SND_CUTOFF,1200);
		me->mind2++;
	}

	if(me->action==ACTION_BUSY)
	{
		me->dx*=0.85;
		me->dy*=0.85;
		// left pound
		if(me->seq==ANIM_ATTACK && me->frm==7)
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<2;i++)
			{FireBullet(me,me->x-FIXAMT*96,me->y+FIXAMT*64,(64+256-16+(byte)Random(32)),BLT_WIND,me->friendly);}
			FireBullet(me,me->x-FIXAMT*96,me->y+FIXAMT*64,0,BLT_SHOCKWAVE,me->friendly);
			ShakeScreen(5);
		}
		// right pound
		if(me->seq==ANIM_A2 && me->frm==7)
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<2;i++)
			{FireBullet(me,me->x+FIXAMT*96,me->y+FIXAMT*64,(64+256-16+(byte)Random(32)),BLT_WIND,me->friendly);}
			FireBullet(me,me->x+FIXAMT*96,me->y+FIXAMT*64,0,BLT_SHOCKWAVE,me->friendly);
			ShakeScreen(5);
		}
		// chest pound (go to chest pound second half)
		if(me->seq==ANIM_A3)
		{
			if(me->frm>=5 && (me->frm-5)%5==0)
			{
				ShakeScreen(6);
			}

			if(me->frm==17 && me->frmTimer>64)
			{
				me->seq=ANIM_A4;
				me->frm=0;
				me->frmTimer=0;
			}
		}
		if(me->seq==ANIM_A4 && me->frm==2)
		{
			ShakeScreen(3);
		}

		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	// waiting for goodguy to arrive
	if(me->mind==0)
	{
		if(me->ouch>0 || RangeToTarget(me,goodguy)<350*FIXAMT)
		{
			MakeSound(SND_KONGHELLO,me->x,me->y,SND_CUTOFF,1200);
			me->mind=1;
			// pound chest
			me->seq=ANIM_A3;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
		}
	}
	else
	{
		if(goodguy->x>me->x-FIXAMT*225 && goodguy->x<me->x+FIXAMT*225 && goodguy->y<me->y+FIXAMT*190 && Random(16)==0)
		{
			if(goodguy->x<me->x)
			{
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				return;
			}
			else
			{
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				return;
			}
		}
		else
		{
			if(goodguy->x<me->x-FIXAMT*10)
			{
				if(me->seq!=ANIM_MOVE)
				{
					me->seq=ANIM_MOVE;
				}
			}
			else if(goodguy->x>me->x+FIXAMT*10)
			{
				if(me->seq!=ANIM_A1)
				{
					me->seq=ANIM_A1;
				}
			}
			if(goodguy)
			{
				// calculate desired location (want to be above Bouapha)
				x=goodguy->x;
				y=goodguy->y-100*FIXAMT;

				FaceGoodguy2(me,goodguy);
				if(me->x<x)
					me->dx+=FIXAMT/8;
				if(me->y<y)
					me->dy+=FIXAMT/8;
				if(me->x>x)
					me->dx-=FIXAMT/8;
				if(me->y>y)
					me->dy-=FIXAMT/8;

				Clamp(&me->dx,FIXAMT*12);
				Clamp(&me->dy,FIXAMT*12);
			}
		}
		if(Random(70)==0 && me->mind2>5)
		{
			me->mind2=0;
			// pound chest
			me->dx=0;
			me->seq=ANIM_A3;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
		}
	}
}

void AI_MultiShroom(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y,tries;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		/*
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
			*/
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			for(i=0;i<256;i+=16)
				FireBullet(me,me->x,me->y,i,BLT_SPORE,me->friendly);

			me->reload=5;
		}
		if(me->seq==ANIM_DIE && me->frm==2 && me->reload==0)
		{
			// divide in two!
			me->reload=100;
			tries=255;
			while((tries--))
			{
				x=me->x-FIXAMT*48+Random(FIXAMT*96);
				y=me->y-FIXAMT*32+Random(FIXAMT*64);
				g=AddBaby(x,y,0,MONS_MULTISHROOM,me);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
					RemoveGuy(g);
				else if(g)
				{
					g->seq=ANIM_A1;
					break;
				}
			}
			tries=255;
			while((tries--))
			{
				x=me->x-FIXAMT*48+Random(FIXAMT*96);
				y=me->y-FIXAMT*32+Random(FIXAMT*64);
				g=AddBaby(x,y,0,MONS_MULTISHROOM,me);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
					RemoveGuy(g);
				else if(g)
				{
					g->seq=ANIM_A1;
					break;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*1;
			me->dy=Sine(me->facing*32)*1;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(64)==0)
			{
				me->mind=1;		// occasionally wander
				me->mind1=1;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Pusher(Guy *me,Map *map,world_t *world,Guy *goodguy)
{

	if(me->reload)
		me->reload--;

	if(me->mind1<255)
		me->mind1++;

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=0;
	}
	
	if (me->aiType == MONS_PUSHERH)
	PushOthers(me,true,map);
	else
	PushOthers(me,false,map);
}

void AI_MinecartBad(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte ok[4];
	int diff,dir;
	static byte noiseLoop=0;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	me->facing=me->mind2/32;	// mind2 holds the 'true' visual facing 0-255
	
	if(me->confuse)
		me->confuse=0;

	// mind2 is the visual facing, mind3 is the actual facing,
	// so rotate mind2 towards mind3
	if(me->mind3>me->mind2)
	{
		diff=me->mind3-me->mind2;
		if(diff>128)
		{
			dir=-1;
			diff=256-diff;
		}
		else
			dir=1;
	}
	else if(me->mind3<me->mind2)
	{
		diff=me->mind2-me->mind3;
		if(diff>128)
		{
			dir=1;
			diff=256-diff;
		}
		else
			dir=-1;
	}
	else
	{
		diff=0;
		dir=0;
	}

	if(me->mind==0)
	{
		// turn quickly if sitting still
		if(diff>32)
			diff=32;
	}
	else
	{
		if(diff>me->mind1/4)
			diff=me->mind1/4;
	}

	me->mind2=(me->mind2+diff*dir)&255;

	if(me->mind==0)	// waiting to be ridden
	{
		// figure out which directions are valid minecart paths
		if(me->mapx<map->width-1 && GetTerrain(world,map->map[me->mapx+1+me->mapy*map->width].floor)->flags&TF_MINECART)
			ok[0]=1;
		else
			ok[0]=0;
		if(me->mapy<map->height-1 && GetTerrain(world,map->map[me->mapx+(me->mapy+1)*map->width].floor)->flags&TF_MINECART)
			ok[1]=1;
		else
			ok[1]=0;
		if(me->mapx>0 && GetTerrain(world,map->map[me->mapx-1+me->mapy*map->width].floor)->flags&TF_MINECART)
			ok[2]=1;
		else
			ok[2]=0;
		if(me->mapy>0 && GetTerrain(world,map->map[me->mapx+(me->mapy-1)*map->width].floor)->flags&TF_MINECART)
			ok[3]=1;
		else
			ok[3]=0;

		// pick one to face
		for(x=0;x<4;x++)
			if(ok[x])
				me->mind3=x*64;

		if(me->mind1==1)	// wait for goodguy to go away before re-allowing him to board
		{
			if(RangeToTarget(me,goodguy)>32*FIXAMT)
				me->mind1=0;
		}
		else if(map->CheckLOS(me->mapx,me->mapy,12,goodguy->mapx,goodguy->mapy))
		{
			MakeSound(SND_MINECART,me->x,me->y,SND_CUTOFF,1200);
			noiseLoop=0;
			me->mind=1;
			me->mind1=0;	// acceleration
		}
	}
	else	// being ridden
	{
		FindVictims(me,(me->x+me->dx)>>FIXSHIFT,(me->y+me->dy)>>FIXSHIFT,16,-FIXAMT*10+Random(FIXAMT*21),-FIXAMT*10+Random(FIXAMT*21),20,map,world,me->friendly);
		FindVictims(me,(me->x+me->dx)>>FIXSHIFT,(me->y+me->dy)>>FIXSHIFT,16,-FIXAMT*10+Random(FIXAMT*21),-FIXAMT*10+Random(FIXAMT*21),20,map,world,me->friendly);
		
		if(me->parent)
		{
			// confine the parent to the cart
			me->parent->x=me->x;
			me->parent->y=me->y+1;
			me->parent->z=FIXAMT*8;
			me->parent->dx=0;
			me->parent->dy=0;
			me->parent->dz=0;
		}
		
		noiseLoop++;
		if(noiseLoop>=28)
		{
			noiseLoop=0;
			MakeSound(SND_MINECART,me->x,me->y,SND_CUTOFF,1200);
		}
		// if you've reached the center of a tile, time to decide where to go
		x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		if((me->x<=x && me->oldx>x) || (me->x>=x && me->oldx<x) ||
		   (me->y<=y && me->oldy>y) || (me->y>=y && me->oldy<y))
		{
			me->x=x;
			me->y=y;
			// figure out which directions are valid minecart paths
			if(me->mapx<map->width-1 && GetTerrain(world,map->map[me->mapx+1+me->mapy*map->width].floor)->flags&TF_MINECART)
				ok[0]=1;
			else
				ok[0]=0;
			if(me->mapy<map->height-1 && GetTerrain(world,map->map[me->mapx+(me->mapy+1)*map->width].floor)->flags&TF_MINECART)
				ok[1]=1;
			else
				ok[1]=0;
			if(me->mapx>0 && GetTerrain(world,map->map[me->mapx-1+me->mapy*map->width].floor)->flags&TF_MINECART)
				ok[2]=1;
			else
				ok[2]=0;
			if(me->mapy>0 && GetTerrain(world,map->map[me->mapx+(me->mapy-1)*map->width].floor)->flags&TF_MINECART)
				ok[3]=1;
			else
				ok[3]=0;

			ok[(((me->mind3/32)+4)&7)/2]=0;

			// count how many directions are available
			y=0;
			for(x=0;x<4;x++)
				if(ok[x])
					y++;

			if(y==0)	// no directions to go!
			{
				me->mind1=1;
				me->mind=0;
				me->dx=0;
				me->dy=0;
				me->frm=0;
				me->seq=ANIM_IDLE;
				me->frmTimer=0;
				me->frmAdvance=128;
				return;
			}
			else
			{
				if(y==1)
				{
					// face the only valid direction
					for(x=0;x<4;x++)
						if(ok[x])
							me->mind3=x*64;
				}
				else
				{
					// if you can go straight, then that's what you do
					if(ok[me->facing/2])
					{
						me->mind3=(me->facing/2)*64;
					}
					else
					{
						// in the case of multiple directions, we must decide on one
						// based on the player's facing
						if(ok[0]&&me->x<goodguy->x)	// the direction you're facing is ok
							me->mind3=0;
						else if(ok[1]&&me->y<goodguy->y)	// direction to your left
							me->mind3=64;
						else if(ok[2]&&me->x>goodguy->x)	// direction to your right (same as x-1)
							me->mind3=128;
						else
							me->mind3=192;	// behind you as a last resort
					}
				}
			}
		}

		// move forward at that pace
		me->dx=Cosine(me->mind3)*(me->mind1/8);
		me->dy=Sine(me->mind3)*(me->mind1/8);

		if(me->mind1<96)
			me->mind1++;

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=me->mind1;
	}
	
	if(me->parent)
	{
		me->parent->x=me->x;
		me->parent->y=me->y;
		me->parent->z=me->z+12*FIXAMT;
	}
}

void AI_SuperMutantZombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1)	// leaping attack
		{
			if(me->frm<3)
			{
				for(x=0;x<5;x++)
				FireBullet(me,me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),Random(256),BLT_SPORE,me->friendly);
				me->dx=0;
				me->dy=0;
				me->dz=0;
			}
			else if(me->frm==3)
			{
				MakeSound(SND_SZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
				me->dx=Cosine(me->facing*32)*18;
				me->dy=Sine(me->facing*32)*18;
				me->dz=(10/1.5)*FIXAMT;
			}
			else if(me->frm<7)
			{
				for(x=0;x<5;x++)
				FireBullet(me,me->x-FIXAMT*20+Random(FIXAMT*40),me->y-FIXAMT*20+Random(FIXAMT*40),Random(256),BLT_SPORE,me->friendly);
				// stay the course
				Dampen(&me->dx,FIXAMT/16);
				Dampen(&me->dy,FIXAMT/16);
			}
			else if(me->frm==12 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
				for(i=0;i<256;i+=16)
					FireExactBullet(me,me->x,me->y,FIXAMT*64,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE,me->friendly);

				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
				ShakeScreen(20);
			}
			else
			{
				Dampen(&me->dx,FIXAMT/2);
				Dampen(&me->dy,FIXAMT/2);
			}
		}
		if(me->seq==ANIM_A2 && me->frm==1 && me->reload==0 && goodguy)
		{
			// can only do the pickup move on Bouapha
			if(RangeToTarget(me,goodguy)<70*FIXAMT && goodguy->hp>0 && PlayerShield()==0 && goodguy->aiType==MONS_BOUAPHA &&
				player.weapon!=WPN_PWRARMOR && player.weapon!=WPN_MINISUB)
			{
				// got him
				MakeSound(SND_SZGRAB,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A3;	// begin lifting
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->reload=100;	// to prevent multigrabs
				goodguy->parent=me;
				goodguy->facing=(me->facing+4)&7;
				pickupX=goodguy->x;
				pickupY=goodguy->y;
				goodguy->x=me->x+Cosine(me->facing*32)*60;
				goodguy->y=me->y+Sine(me->facing*32)*60;
				goodguy->z=0;
				goodguy->dx=0;
				goodguy->dy=0;
				goodguy->dz=0;
				goodguy->action=ACTION_BUSY;
				goodguy->seq=ANIM_MOVE;	// wiggle his feet
				goodguy->frm=0;
				goodguy->frmTimer=0;
				goodguy->frmAdvance=512;
				return;
			}
			me->reload=4;
		}
		if(me->seq==ANIM_A3 && goodguy)
		{
			if(player.weapon==WPN_PWRARMOR || player.weapon==WPN_MINISUB)
			{
				// add an OOPH noise to fail to pick up, and maybe take damage
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_IDLE;
			}
			else
			{
				if((me->frm==5 || me->frm==8 || me->frm==11 || me->frm==14 || me->frm==17) && me->frmTimer<32)
				{
					MakeSound(SND_MISSILEBOOM,me->x,me->y,SND_CUTOFF,1500);
					goodguy->GetShot(me,goodguy,0,0,8,map,world);
				}

				if(me->frm==21)
				{
					goodguy->x=pickupX;
					goodguy->y=pickupY;
				}
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			x=(me->x+Cosine(me->facing*32)*40)>>FIXSHIFT;
			y=(me->y+Sine(me->facing*32)*40)>>FIXSHIFT;
			if(me->AttackCheck(30,x,y,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*16,Sine(me->facing*32)*16,15,map,world);
			me->reload=4;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=8)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<400*FIXAMT)
				me->mind=1;
		}
		if(me->ouch)
			me->mind=1;
	}
	else if(me->mind==2)		// when mind=2, hold still and go off on Bouapha
	{
		if(goodguy)
		{
			i=RangeToTarget(me,goodguy);
			if(i<(96*FIXAMT) && Random(10)==0 && me->reload==0)
			{
				// get him! (punch)
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			// won't try grab attack on non-Bouaphas
			if(i<(96*FIXAMT) && Random(16)==0 && me->reload==0 && goodguy->aiType==MONS_BOUAPHA)
			{
				// get him! (grab)
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=0;
			me->dy=0;
			if(me->seq!=ANIM_IDLE)
			{
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(i>(80*FIXAMT) && me->mind1==0)
			{
				// chase him down!
				me->mind=1;
				me->mind1=8;
			}
		}
		else
		{
			// just sit there
		}
	}
	else if(me->mind==1)	// chase down Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=192;
			}
			if(RangeToTarget(me,goodguy)<160*FIXAMT && me->mind1==0)
			{
				me->mind=2;	// in range, start killin'
				me->mind1=8;
			}
			else if(Random(48)==0)
			{
				//leap!
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=192;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
			}
		}
		else
			me->mind=0;
	}
}

void AI_SuperBombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1)	// leaping attack
		{
			if(me->frm<3)
			{
				me->dx=0;
				me->dy=0;
				me->dz=0;
			}
			else if(me->frm==3 && me->frmTimer<32)
			{
				MakeSound(SND_SZOMBIELEAP,me->x,me->y,SND_CUTOFF,600);
				me->dx=Cosine(me->facing*32)*18;
				me->dy=Sine(me->facing*32)*18;
				me->dz=(20)*FIXAMT;
			}
			else if(me->frm<7)
			{
				// stay the course
				Dampen(&me->dx,FIXAMT/4);
				Dampen(&me->dy,FIXAMT/4);
			}
			else if(me->frm==12 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE2,me->friendly);
				ShakeScreen(40);
				MakeSound(SND_SDZLPOUND,me->x,me->y,SND_CUTOFF,1200);
				for(i=0;i<32;i++)
				{
					x=me->x+((256-Random(513))<<FIXSHIFT);
					y=me->y+((256-Random(513))<<FIXSHIFT);
					FireExactBullet(me,x,y,FIXAMT*80+(Random(40)<<FIXSHIFT),
									0,0,0,0,30,2,BLT_ROCK,me->friendly);
				}
			}
			else
			{
				Dampen(&me->dx,FIXAMT/4);
				Dampen(&me->dy,FIXAMT/4);
			}
		}
		if(me->seq==ANIM_A2 && me->frm==1 && me->reload==0 && goodguy)
		{
			// can only do the pickup move on Bouapha
			if(RangeToTarget(me,goodguy)<70*FIXAMT && goodguy->hp>0 && PlayerShield()==0 && goodguy->aiType==MONS_BOUAPHA &&
				player.weapon!=WPN_PWRARMOR && player.weapon!=WPN_MINISUB)
			{
				// got him
				MakeSound(SND_SZGRAB,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A3;	// begin lifting
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->reload=100;	// to prevent multigrabs
				goodguy->parent=me;
				goodguy->facing=(me->facing+4)&7;
				pickupX=goodguy->x;
				pickupY=goodguy->y;
				goodguy->x=me->x+Cosine(me->facing*32)*60;
				goodguy->y=me->y+Sine(me->facing*32)*60;
				goodguy->z=0;
				goodguy->dx=0;
				goodguy->dy=0;
				goodguy->dz=0;
				goodguy->action=ACTION_BUSY;
				goodguy->seq=ANIM_MOVE;	// wiggle his feet
				goodguy->frm=0;
				goodguy->frmTimer=0;
				goodguy->frmAdvance=512;
				return;
			}
			me->reload=4;
		}
		if(me->seq==ANIM_A3 && goodguy)
		{
			if(player.weapon==WPN_PWRARMOR || player.weapon==WPN_MINISUB)
			{
				// add an OOPH noise to fail to pick up, and maybe take damage
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_IDLE;
			}
			else
			{
				if((me->frm==5 || me->frm==8 || me->frm==11 || me->frm==14 || me->frm==17) && me->frmTimer<32)
				{
					FireBullet(me,me->x-64*FIXAMT+Random(129)*FIXAMT,me->y-64*FIXAMT+Random(129)*FIXAMT,0,BLT_BOOM,me->friendly);
					goodguy->GetShot(me,goodguy,0,0,1,map,world);
				}

				if(me->frm==21)
				{
					goodguy->x=pickupX;
					goodguy->y=pickupY;
				}
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			x=(me->x+Cosine(me->facing*32)*40)>>FIXSHIFT;
			y=(me->y+Sine(me->facing*32)*40)>>FIXSHIFT;
			if(me->AttackCheck(30,x,y,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*16,Sine(me->facing*32)*16,15,map,world);
			me->reload=4;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=8)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<400*FIXAMT)
				me->mind=1;
		}
		if(me->ouch)
			me->mind=1;
	}
	else if(me->mind==2)		// when mind=2, hold still and go off on Bouapha
	{
		if(goodguy)
		{
			i=RangeToTarget(me,goodguy);
			if(i<(96*FIXAMT) && Random(10)==0 && me->reload==0)
			{
				// get him! (punch)
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				return;
			}
			// won't try grab attack on non-Bouaphas
			if(i<(96*FIXAMT) && Random(16)==0 && me->reload==0 && goodguy->aiType==MONS_BOUAPHA)
			{
				// get him! (grab)
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy2(me,goodguy);

			me->dx=0;
			me->dy=0;
			if(me->seq!=ANIM_IDLE)
			{
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(i>(40*FIXAMT) && me->mind1==0)
			{
				// chase him down!
				me->mind=1;
				me->mind1=8;
			}
		}
		else
		{
			// just sit there
		}
	}
	else if(me->mind==1)	// chase down Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
			}
			if(RangeToTarget(me,goodguy)<80*FIXAMT && me->mind1==0)
			{
				me->mind=2;	// in range, start killin'
				me->mind1=8;
			}
			else if(Random(80)==0)
			{
				//leap!
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=96;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
			}
		}
		else
			me->mind=0;
	}
}