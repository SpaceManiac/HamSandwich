int pickupX,pickupY;

void AI_Bonehead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;
	
	if(me->aiType==MONS_GANGRENE)
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,12,16);

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SKELOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SKELDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		
		if(me->seq==ANIM_A2 && me->aiType==MONS_NUMBSKULL && me->reload==0 && me->frm>=2)
		{
			i=Random(TILE_WIDTH)+TILE_WIDTH*(me->frm-1);
			x=me->x+Cosine(me->facing*48)*i;
			y=me->y+Sine(me->facing*48)*i;
			FireBullet(me,x,y,me->facing*32,BLT_FLAMEWALL,me->friendly);
			me->reload=3;
		}
		if (me->seq == ANIM_ATTACK && me->frm == 3 && me->reload == 0 && goodguy)
		{
			x = me->x + Cosine(me->facing * 32)*16;
			y = me->y + Sine(me->facing * 32)*16;
			if (me->AttackCheck(16, x >> FIXSHIFT, y >> FIXSHIFT, goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing * 32)*4, Sine(me->facing * 32)*4, 4, map, world);
			me->reload = 5;
		}
		if(me->seq==ANIM_ATTACK && me->aiType==MONS_SKULLY)
		{
			me->dx+=Cosine(me->facing*32)*2;
			me->dy+=Sine(me->facing*32)*2;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
		}
		if(me->seq==ANIM_A4 && me->frm==4 && me->reload==0)
		{
			// raise the dying
			me->reload=5;
			RaiseSkellies(me);
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->aiType==MONS_REDBONE)
				FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
			else if(me->aiType==MONS_GANGRENE)
				FireBullet(me,x,y,me->facing*32,BLT_FART,me->friendly);
			else if(me->aiType==MONS_SKULLY)
			{
				FireBullet(me,x,y,(byte)((me->facing*32-16+Random(16))&255),BLT_FLAME5,me->friendly);
				FireBullet(me,x,y,(byte)((me->facing*32-16+Random(16))&255),BLT_FLAME5,me->friendly);
				FireBullet(me,x,y,(byte)((me->facing*32-16+Random(16))&255),BLT_FLAME5,me->friendly);
				FireBullet(me,x,y,(byte)((me->facing*32-16+Random(16))&255),BLT_FLAME5,me->friendly);
			}
			else if(me->aiType==MONS_GLASSJAW)
			{
				FireBullet(me,x,y,me->facing*32+256-12,BLT_FREEZE2,me->friendly);
				FireBullet(me,x,y,me->facing*32+12,BLT_FREEZE2,me->friendly);
				FireBullet(me,x,y,me->facing*32,BLT_FREEZE2,me->friendly);
			}
			else if(me->aiType==MONS_MARSHBONE)
			{
				FireBullet(me,x,y,me->facing*32+256-12,BLT_BADGREEN,me->friendly);
				FireBullet(me,x,y,me->facing*32+12,BLT_BADGREEN,me->friendly);
				FireBullet(me,x,y,me->facing*32,BLT_BADGREEN,me->friendly);
			}
			else
			FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
			me->reload=5;
			me->mind1=1;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to point at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if((!Random(100)) && me->mind==0 && me->aiType!=MONS_SKULLY)
	{
		me->seq=ANIM_A2;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}
	if(me->mind==0 && me->aiType==MONS_SKULLY)
	{
		if(RangeToTarget(me,goodguy)<(200*FIXAMT) && Random(8)==0 &&
			map->CheckLOS(me->mapx,me->mapy,10,goodguy->mapx,goodguy->mapy))
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
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(8)==0  && (me->type==MONS_BONEHEAD || me->type==MONS_NUMBSKULL))
			{
				// get him!
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			if(me->aiType==MONS_GANGRENE)
			{me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;}
			else
			{me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;}
				
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				if(me->aiType==MONS_GANGRENE)
				me->frmAdvance=192;
				else if(me->aiType==MONS_GLASSJAW)
				me->frmAdvance=96;
				else
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
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 && (me->type!=MONS_NUMBSKULL))
			{
				// spit at him
				MakeSound(SND_SKELSHOOT,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				if(me->aiType==MONS_GANGRENE)
					me->frmAdvance=96;
				else if(me->aiType==MONS_GLASSJAW)
					me->frmAdvance=96;
				else
					me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				FaceGoodguy(me,goodguy);
				return;
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		if(me->aiType==MONS_GANGRENE)
			{me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;}
		else if(me->aiType==MONS_GLASSJAW)
			{me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;}
		else
			{me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			if(me->aiType==MONS_GANGRENE)
				me->frmAdvance=192;
			else if(me->aiType==MONS_GLASSJAW)
				me->frmAdvance=96;
			else
				me->frmAdvance=128;
		}
	}
	
	if(me->type==MONS_NUMBSKULL && !me->reload)
	{
		// resurrect any dying skellies
		if(Random(8)==0 && DyingSkellies(me))
		{
			// resurrect dying skellies
			MakeSound(SND_SKELSUMMON,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A4;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			FaceGoodguy2(me,goodguy);
			return;
		}
		if(Random(32)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// point to cause flame geyser
			FaceGoodguy(me,goodguy);
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			return;
		}
	}
}

void AI_Bat(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->type==MONS_FIREBAT)
		map->DimTorch(me->mapx,me->mapy,6);

	if(me->ouch==4)
	{
		if(me->hp>0)
		MakeSound(SND_BATOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
		MakeSound(SND_BATDIE,me->x,me->y,SND_CUTOFF,1200);
	}
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world);
			me->reload=2;

		}
		if(me->seq==ANIM_A1 && me->type==MONS_FIREBAT && Random(4)==0)
		{
			FireExactBullet(me,me->x,me->y,me->z,Random(FIXAMT),Random(FIXAMT),FIXAMT,0,90+Random(60),0,BLT_BADSITFLAME,me->friendly);
			MakeSound(SND_FLAMEGO,me->x,me->y,SND_CUTOFF,1100);
		}
		if(me->seq==ANIM_A1 && me->frm>1 && goodguy && me->hp>0)
		{
			// diving attack hit check
			x=me->x;
			y=me->y;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,4,map,world);
				me->seq=ANIM_A2;	// bounce off
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->dx=-me->dx/4;
				me->dy=-me->dy/4;
			}
		}
		if(me->seq==ANIM_DIE && me->frmTimer<128 && me->type==MONS_FIREBAT)
			FireExactBullet(me,me->x,me->y,me->z,Random(FIXAMT),Random(FIXAMT),FIXAMT,0,30*8+Random(30*4),0,BLT_BADSITFLAME,me->friendly);

		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly zip towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(4)==0)
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
				return;
			}
			if(RangeToTarget(me,goodguy)>(64*FIXAMT) && Random(32)==0)
			{
				MakeSound(SND_BATDIVE,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A1;	// diving attack move
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				return;
			}

			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(16)==0)
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

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=110;
		}
	}
}

void AI_Spider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_SPD1DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*2,Sine(me->facing*32)*2,1,map,world);
			me->reload=2;

		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(32*FIXAMT) && Random(4)==0)
		{
			FaceGoodguy(me,goodguy);
			// get him!
			MakeSound(SND_SPD1GNAW,me->x,me->y,SND_CUTOFF,1200);
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
	}
	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(40)+1;
	}

	me->dx=Cosine(me->facing*32)*8;
	me->dy=Sine(me->facing*32)*8;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_BigSpider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

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
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0)
		{
			// spit acid glob
			
			MakeSound(SND_SPD2SPIT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(me->aiType==MONS_FIREBUG)
				FireBullet(me,x,y,me->facing*32-10+Random(21),BLT_ENERGY,me->friendly);
			else if(me->aiType==MONS_SPIDER2)
			{
				FireBullet(me,x,y,me->facing*32+256-12,BLT_ACID,me->friendly);
				FireBullet(me,x,y,me->facing*32+12,BLT_ACID,me->friendly);
				FireBullet(me,x,y,me->facing*32,BLT_ACID,me->friendly);
			}
			else
			FireBullet(me,x,y,me->facing*32,BLT_ACID,me->friendly);
			me->reload=40;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0 && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			// spit on him!
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
	}
	
	if(me->aiType==MONS_FIREBUG && Random(20)==0 && RangeToTarget(me,goodguy)<FIXAMT*500)
		FireExactBullet(me,me->x,me->y,FIXAMT*5,Random(FIXAMT),Random(FIXAMT),0,0,24-Random(4),Random(8),BLT_FLAME2,me->friendly);
	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(120)+1;
	}

	if(me->aiType==MONS_SPIDER2)
	{
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
	}
	else
	{
		me->dx=Cosine(me->facing*32)*3;
		me->dy=Sine(me->facing*32)*3;
	}
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Zombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if (me->aiType==MONS_PEABRAIN)
		{
			if(me->hp>0)
				MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
		}
		else
		{
			if(me->hp>0)
				MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		
		if(me->seq==ANIM_DIE && me->aiType==MONS_BOMBIE && me->reload%5==0)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			}
			
		if(me->seq==ANIM_DIE && me->aiType==MONS_BOMBIELORD)
		{
			if(Random(5)==0)
			{
				FireBullet(me,me->x-32*FIXAMT+Random(65)*FIXAMT,me->y-32*FIXAMT+Random(65)*FIXAMT,0,BLT_BOOM,me->friendly);
			}
		}
			
		if(me->seq==ANIM_DIE && me->aiType==MONS_BOMBIE && me->reload==0)
			{
				FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
				me->reload=15;
			}
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			if (me->aiType==MONS_BOMBIE)
			{me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;}
			else if (me->aiType==MONS_PEABRAIN)
			{me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;}
			{me->dx=Cosine(me->facing*32)*12;
			me->dy=Sine(me->facing*32)*12;}
		}
		if(me->seq==ANIM_ATTACK && me->frm>6)
		{
			Dampen(&me->dx,FIXAMT/2);
			Dampen(&me->dy,FIXAMT/2);
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && goodguy)
		{
			if(me->aiType==MONS_ZOMBIELORD)
			{
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				FireBullet(me,x,y,me->facing-1,BLT_DIRTSPIKE,me->friendly);
				FireBullet(me,x,y,me->facing,BLT_DIRTSPIKE,me->friendly);
				FireBullet(me,x,y,me->facing+1,BLT_DIRTSPIKE,me->friendly);
				me->reload=5;
				me->mind1=1;
			}
			else
			{
				for(i=0;i<3;i++)
				{
				x=me->x-48+Random(97)*FIXAMT;
				y=me->y-48+Random(97)*FIXAMT;
				g=AddBaby(x,y,0,MONS_BOMBIE,me);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					RemoveGuy(g);
				else if(g)
					{
						g->seq=ANIM_A3;	// arising from the ground
						g->frm=0;
						g->frmTimer=0;
						g->frmAdvance=128;
						g->action=ACTION_BUSY;
					}
				}
			}
			me->reload=5;
		}
		if(me->seq==ANIM_ATTACK && me->frm==11 && me->reload==0 && goodguy && me->aiType==MONS_FROZOMBIE)
		{
			me->reload = 24;
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			FireBullet(me,x,y,me->facing*32,BLT_BIGSNOW,me->friendly);
		}
		if(me->seq==ANIM_ATTACK && me->frm>=7 && me->frm<13 && me->reload==0 && goodguy)
		{
			if(me->aiType==MONS_BOMBIE)
				{x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
				me->z,FIXAMT);}
			if(me->frm==11 && me->reload==0 && me->aiType==MONS_BOMBIE)	// bombie
			{
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->dx=0;
				me->dy=0;
				me->hp=1;
				me->GetShot(me,me,0,0,1,map,world);
			}
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(8,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,1,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}
	if((me->aiType==MONS_ZOMBIELORD || me->aiType==MONS_BOMBIELORD) && RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(64)==0)
	{
		// stomp earth at him
		MakeSound(SND_MATTIESTOMP,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
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
	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if((RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(32)==0)||(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(24)==0&&me->aiType==MONS_PEABRAIN))
			{
				// get him!
				MakeSound(SND_ZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				if(me->aiType==MONS_PEABRAIN||me->aiType==MONS_ZOMBIELORD)
				me->frmAdvance=128;
				else
				me->frmAdvance=64;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}

			FaceGoodguy(me,goodguy);
			if (me->aiType==MONS_PEABRAIN || me->aiType==MONS_BOMBIE)
			{me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;}
			else if (me->aiType==MONS_BOMBIELORD || me->aiType==MONS_ZOMBIELORD)
			{me->dx=Cosine(me->facing*32)*1;
			me->dy=Sine(me->facing*32)*1;}
			else
			{me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;}
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				switch(me->aiType)
				{
					case MONS_PEABRAIN:	// bombie
					case MONS_BOMBIE:	// bombie
						me->frmAdvance=128;
						break;
					case MONS_ZOMBIELORD:	// zombie lord
					case MONS_BOMBIELORD:
						me->frmAdvance=48;
						break;
					default:
						me->frmAdvance=64;
						break;
				}
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
		if(me->aiType==MONS_BOMBIE)	// bombie
		{
			me->dx=me->dx/2;
			me->dy=me->dy/2;
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}
		
		if (me->aiType==MONS_PEABRAIN || me->aiType==MONS_BOMBIE)
		{me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;}
		else if (me->aiType==MONS_BOMBIELORD || me->aiType==MONS_ZOMBIELORD)
		{me->dx=Cosine(me->facing*32)*1;
		me->dy=Sine(me->facing*32)*1;}
		else
		{me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;}
		
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			switch(me->aiType)
			{
				case MONS_BOMBIE:	// bombie
				case MONS_PEABRAIN:	// peabrain
					me->frmAdvance=128;
					break;
				case MONS_ZOMBIELORD:	// zombie lord
				case MONS_BOMBIELORD:
					me->frmAdvance=48;
					break;
				default:
					me->frmAdvance=64;
					break;
			}
		}
	}
}

void AI_EggSac(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_EGGSACDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==18 && me->reload==0)
		{
			g=AddBaby(me->x,me->y+FIXAMT*2,0,MONS_SPIDER,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			me->reload=200;
		}
		return;
	}

	if((!me->reload) && Random(500)==0)
	{
		MakeSound(SND_EGGSACBIRTH,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
	}
}

void AI_MamaSpider(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPD3OUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SPD3DIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(Random(2))
			{
				AddBaby(me->x+((-8+Random(17))<<FIXSHIFT),
						me->y+((-8+Random(17))<<FIXSHIFT),0,MONS_SPIDER,me);
			}
		}
		if(me->seq==ANIM_ATTACK && me->frm>4 && me->frm<8 && me->reload==0)
		{
			// only actually fire if the range is reasonable to avoid creating excess bullets
			if(goodguy && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				for(i=0;i<3;i++)
				{
					// spit acid glob
					x=me->x+Cosine(me->facing*32)*20-FIXAMT*4+(Random(9)<<FIXSHIFT);
					y=me->y+Sine(me->facing*32)*20-FIXAMT*4+(Random(9)<<FIXSHIFT);
					f=me->facing*32-16+Random(33);

					FireBullet(me,x,y,f,BLT_ACID,me->friendly);
				}
			}
			me->reload=0;
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,20,map,world);
				me->reload=2;
			}
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		x=RangeToTarget(me,goodguy);
		if(x<(512*FIXAMT) && x>128*FIXAMT && Random(32)==0 && me->reload==0)
		{
			MakeSound(SND_SPD3SPEW,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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
		else if(x<(128*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SPD3CHOMP,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// bite him
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			return;
		}
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(120)+1;
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

void AI_Pygmy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

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
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*6,Sine(me->facing*32)*6,8,map,world);
				me->reload=10;
			}
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
			switch(Random(3))
			{
				case 0:
					MakeSound(SND_PYGMYSPIN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A1;	// spin the spear
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
					MakeSound(SND_PYGMYYAWN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A2;	// yawn
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 2:
					me->facing=(me->facing+(-1+Random(3)))&7;	//change facing
					me->mind1=0;
					break;
			}
		}
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0)
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
				me->seq=ANIM_A3;
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
		if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(16)==0 && me->reload==0)
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
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
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

void AI_Serpent(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

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
			if (me->aiType==MONS_KELVOZOID)
			{
				FireBullet(me,x,y,me->facing*32,BLT_FREEZE2,me->friendly);
				FireBullet(me,x,y,(me->facing*32+4)&255,BLT_FREEZE2,me->friendly);
				FireBullet(me,x,y,(me->facing*32-4)&255,BLT_FREEZE2,me->friendly);
			}
			else if (me->aiType==MONS_MISSOZOID)
			{
				FireBullet(me,x,y,me->facing*32,BLT_MISSILE,me->friendly);
			}
			else
			{
				FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
				FireBullet(me,x,y,(me->facing*32+4)&255,BLT_ENERGY,me->friendly);
				FireBullet(me,x,y,(me->facing*32-4)&255,BLT_ENERGY,me->friendly);
			}
			me->reload=15;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}
	
	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*Random(32));

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_MattieBrain(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte tempface;
	byte faceTable[8]={5,4,0,1,2,3,0,6};
	int x,y,q;

	if(me->ouch==4 && me->aiType==MONS_MATBRAIN)	// skull and head have their own ouch noises
	{
		if(me->hp>0)
			MakeSound(SND_MATTIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_LOOKEYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	// use facing variable to determine which way for eyes to look
	if(me->action!=ACTION_BUSY)
	{
		tempface=me->facing;
		FaceGoodguy(me,goodguy);

		if(me->facing>=5)	// can't look straight back, so deal with it
		{
			if(tempface<2)
				me->facing=0;
			else
				me->facing=4;
		}
		me->facing=faceTable[me->facing];
		if(me->aiType!=MONS_MATBRAIN && me->facing!=0)
			me->facing+=4;
	}
	else
		me->facing=0;	// always look ahead when doing things

	if(me->seq==ANIM_DIE)
	{
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		if(me->aiType==MONS_MATBRAIN)
		{
			BlowUpGuy(x+me->rectx-40,y+me->recty-200,x+me->rectx2+40,y+me->recty2+10,me->z,2);
			BlowSmoke((x+me->rectx-40+Random(me->rectx2-me->rectx+80))<<FIXSHIFT,
					  (y+me->recty-200+Random(me->recty2-me->recty+210))<<FIXSHIFT,
					  me->z,FIXAMT);
			BlowSmoke((x+me->rectx-40+Random(me->rectx2-me->rectx+80))<<FIXSHIFT,
					  (y+me->recty-200+Random(me->recty2-me->recty+210))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(2);
		}
	}
	// use the claws to cover your face when scared
	if(me->ouch && me->mind1<64)
		me->mind1+=8;	// mind1 is fear, when it gets high, the claws cover her face

	if(me->mind1)
		me->mind1--;
}

void AI_MattieSkullOrHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,q;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MATTIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MATTIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==5)
		{
			if(me->aiType==MONS_MATHEAD)
			{
				me->type=MONS_MATSKULL;
				me->aiType=MONS_MATSKULL;
				strcpy(me->name,MonsterName(me->type));
				if(!me->friendly)
					player.enemiesSlain--;
				me->hp=monsType[MONS_MATSKULL].hp;
				me->maxHP=monsType[MONS_MATSKULL].hp;
			}
			else
			{
				me->type=MONS_MATBRAIN;
				me->aiType=MONS_MATBRAIN;
				strcpy(me->name,MonsterName(me->type));
				if(!me->friendly)
					player.enemiesSlain--;
				me->hp=monsType[MONS_MATBRAIN].hp;
				me->maxHP=monsType[MONS_MATBRAIN].hp;
			}
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
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			// shoot laser
			x=me->x;
			y=me->y+FIXAMT*8;
			LaunchMegabeam(me,x,y,me->ID);
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
		me->frmAdvance=32;	// very very slow animation
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
	}

	// do everything the brain does
	AI_MattieBrain(me,map,world,goodguy);
}

void AI_MattieClaw(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload>0)
		me->reload--;

	if(!me->parent || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;	// can't exist alone
		return;
	}

	if(me->aiType==MONS_MATCLAW1)
	{
		me->x=me->parent->x-45*FIXAMT;
		me->y=me->parent->y+12*FIXAMT;
		me->z=me->parent->z-12*FIXAMT;
	}
	else if(me->aiType==MONS_MATCLAW2)
	{
		me->x=me->parent->x+45*FIXAMT;
		me->y=me->parent->y+12*FIXAMT;
		me->z=me->parent->z-12*FIXAMT;
	}
	else if(me->aiType==MONS_MINIMATCLAW1)
	{
		me->x=me->parent->x-26*FIXAMT;
		me->y=me->parent->y+6*FIXAMT;
		me->z=me->parent->z-6*FIXAMT;
	}
	else if(me->aiType==MONS_MINIMATCLAW2)
	{
		me->x=me->parent->x+26*FIXAMT;
		me->y=me->parent->y+6*FIXAMT;
		me->z=me->parent->z-6*FIXAMT;
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
		if(me->seq==ANIM_A1 && me->frm==2 && me->parent->mind1>32)
			me->frmTimer=0;	// hold them up until she calms down
		if(me->seq==ANIM_ATTACK && me->frm>1)
		{
			if(me->frm<5)
			{
				if(me->aiType==MONS_MATCLAW1)
					x=me->x-FIXAMT*118;
				else if(me->aiType==MONS_MATCLAW2)
					x=me->x+FIXAMT*118;
				else if(me->aiType==MONS_MINIMATCLAW1)
					x=me->x-FIXAMT*59;
				else if(me->aiType==MONS_MINIMATCLAW2)
					x=me->x+FIXAMT*59;
				
				if(me->aiType==MONS_MINIMATCLAW1 or me->aiType==MONS_MINIMATCLAW2)
				y=me->y+FIXAMT*89;
				else
				y=me->y+FIXAMT*45;
			}
			else
			{
				if(me->aiType==MONS_MATCLAW1)
					x=me->x-FIXAMT*69;
				else if(me->aiType==MONS_MATCLAW2)
					x=me->x+FIXAMT*69;
				else if(me->aiType==MONS_MINIMATCLAW1)
					x=me->x-FIXAMT*35;
				else if(me->aiType==MONS_MINIMATCLAW2)
					x=me->x+FIXAMT*35;
				
				if(me->aiType==MONS_MINIMATCLAW1 or me->aiType==MONS_MINIMATCLAW2)
				y=me->y+FIXAMT*80;
				else
				y=me->y+FIXAMT*141;
			}

			FindVictim(me,x>>FIXSHIFT,y>>FIXSHIFT,50,0,FIXAMT*8,8,map,world,me->friendly);
		}
		return;
	}

	if(me->reload==0)
	{
		MakeSound(SND_MATTIECLAW,me->x,me->y,SND_CUTOFF,1200);
		me->reload=Random(180)+20;
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		return;
	}

	// make their idle weird and wiggly
	me->frmAdvance=Random(128)+1;

	if(me->parent->mind1>32)	// she is scared, block!!
	{
		me->action=ACTION_BUSY;
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=48;
	}
}

void AI_MattieTail(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte b;
	int x,y;
	short shootXTable[9]={-134,-124,-101,-57,0,67,113,134,143};
	short shootYTable[9]={-51,-43,-24,-7,-11,-15,-29,-41,-46};

	if(!me->parent || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;	// can't exist alone
		return;
	}

	if (me->aiType==MONS_MINIMATTAIL)
	{me->x=me->parent->x;
	me->y=me->parent->y-52*FIXAMT;
	me->z=me->parent->z;}
	else
	{me->x=me->parent->x;
	me->y=me->parent->y-104*FIXAMT;
	me->z=me->parent->z;}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			b=(32-me->facing)*4;
			// shoot some bullets
			if(me->parent->aiType==MONS_MINIMATTIE)
			{
				x=me->x+((shootXTable[(me->facing>>2)]<<FIXSHIFT)/2);
				y=me->y+((shootYTable[(me->facing>>2)]<<FIXSHIFT)/2);
			}
			else
			{
				x=me->x+(shootXTable[(me->facing>>2)]<<FIXSHIFT);
				y=me->y+(shootYTable[(me->facing>>2)]<<FIXSHIFT);
			}
			
			if(me->parent->aiType==MONS_MINIMATTIE)
			{b+=12-Random(25);
			if (Random(2))
			FireExactBullet(me,x,y,me->z+16*FIXAMT,Cosine(b)*4,Sine(b)*4,-FIXAMT/0.5,
			0,60,b,BLT_ENERGY,me->friendly);}
			else
			{b+=8-Random(17);
			FireExactBullet(me,x,y,me->z+32*FIXAMT,Cosine(b)*6,Sine(b)*6,-FIXAMT/2,
			0,60,b,BLT_ENERGY,me->friendly);}
			if(me->parent->aiType==MONS_MATBRAIN)
			{
				b+=16-Random(33);
				FireExactBullet(me,x,y,me->z+32*FIXAMT,Cosine(b)*6,Sine(b)*6,-FIXAMT/2,
					0,120,b,BLT_ENERGY,me->friendly);
			}
		}
		return;
	}

	if(me->mind)
		me->mind--;

	if(me->mind<10)
	{
		MakeSound(SND_MATTIESHOOT,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=255;
		me->action=ACTION_BUSY;
		if(me->mind==0)
		{
			if(me->parent->aiType==MONS_MATBRAIN or me->parent->aiType==MONS_MATSKULL)
				me->mind=80;
			else if(me->parent->aiType!=MONS_MATBRAIN)
				me->mind=50;
			else
				me->mind=30;
		}
	}

	if(goodguy)
	{
		// point at Bouapha
		if(goodguy->x<me->x-FIXAMT*128)
		{
			if(goodguy->y<me->y+FIXAMT*64)
				b=0;
			else
				b=8;
		}
		else if(goodguy->x>me->x+FIXAMT*128)
		{
			if(goodguy->y<me->y+FIXAMT*64)
				b=32;
			else
				b=24;
		}
		else if(goodguy->y>me->y+FIXAMT*64)
		{
			b=16;
		}
		else
		{
			// can't point at him when he is behind, but you can try
			if(goodguy->x<me->x)
				b=0;
			else
				b=32;
		}
	}
	else
		b=16;

	// b contains the desired facing
	if(me->mind1<b)
		me->mind1++;
	else if(me->mind1>b)
		me->mind1--;
	me->facing=me->mind1&(~3);	// facing is locked in multiples of 4
}

void AI_MattieBody(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte b;

	if(!me->parent || me->parent->type==MONS_NONE)
	{
		me->type=MONS_NONE;	// can't exist alone
		return;
	}

	// unlike other components, this one tells the head itself where to be
	if(me->parent->aiType==MONS_MINIMATBODY)
	{
		me->parent->x=me->x;
		me->parent->y=me->y+20*FIXAMT;
		me->parent->z=me->z+16*FIXAMT;
	}
	else
	{
		me->parent->x=me->x;
		me->parent->y=me->y+40*FIXAMT;
		me->parent->z=me->z+32*FIXAMT;
	}

	if(goodguy)
	{
		if(me->parent->aiType==MONS_MINIMATBODY)
		{
			if(goodguy->x<me->x)
				me->dx-=FIXAMT/8;	// slow acceleration
			if(goodguy->x>me->x)
				me->dx+=FIXAMT/8;
			if(goodguy->y<me->y)	//CRUSH him if he gets behind you
				me->dy-=FIXAMT;
			else
			{
				if(me->mind>128)
				{
					me->mind--;
					me->dy+=FIXAMT/8;
				}
				else if(me->mind<128)
				{
					me->mind++;
					me->dy-=FIXAMT/8;
				}
			}
		}
		else
		{
			if(goodguy->x<me->x)
				me->dx-=FIXAMT/16;	// slow acceleration
			if(goodguy->x>me->x)
				me->dx+=FIXAMT/16;
			if(goodguy->y<me->y)	//CRUSH him if he gets behind you
				me->dy-=FIXAMT/4;
			if(goodguy->y>me->y)	//CRUSH him if he gets behind you
				me->dy+=FIXAMT/4;
			else
			{
				if(me->mind>128)
				{
					me->mind--;
					me->dy+=FIXAMT/16;
				}
				else if(me->mind<128)
				{
					me->mind++;
					me->dy-=FIXAMT/16;
				}
			}
		}
			
	}
	b=(byte)Random(128);
	if(b<10)
		me->mind=128-Random(64);
	else if(b<40)
		me->mind=128+Random(64);
	else if(b<50)
		me->mind=128;
	
	if(me->parent->aiType==MONS_MINIMATBODY)
	{
		Dampen(&me->dx,FIXAMT/32);
		Dampen(&me->dy,FIXAMT/32);
		Clamp(&me->dx,FIXAMT*1);
		Clamp(&me->dy,FIXAMT*1);
	}
	else
	{
		Dampen(&me->dx,FIXAMT/24);
		Dampen(&me->dy,FIXAMT/24);
		Clamp(&me->dx,FIXAMT*2);
		Clamp(&me->dy,FIXAMT*2);
	}
	
	if(me->seq!=ANIM_MOVE)
	{
		MakeSound(SND_MATTIESTOMP,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
	}
}

void AI_Ginger(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GINGEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GINGERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==2 || me->frm==4 || me->frm==11) && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,6,map,world);
			if(me->frm==11)
			{
				me->frmAdvance=64;
				me->dx=0;
				me->dy=0;
			}
			me->reload=3;
		}
		if(me->seq==ANIM_A1 && goodguy)
		{
			if(me->frm<2)
			{
				me->dx=0;
				me->dy=0;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*10;
				me->dy=Sine(me->facing*32)*10;
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				if(FindVictim(me,x>>FIXSHIFT,y>>FIXSHIFT,16,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,12,map,world,me->friendly))
				{
					me->seq=ANIM_A2;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=32;
					me->dx=-Cosine(me->facing*32)*4;
					me->dy=-Sine(me->facing*32)*4;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly stalk Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(64*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_GINGERCOMBO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=Cosine(me->facing*32);
				me->dy=Sine(me->facing*32);
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
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
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(128)==0)
		{
			// jumpkick him
			MakeSound(SND_GINGERJKICK,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			FaceGoodguy(me,goodguy);
			me->dz=FIXAMT*6;
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

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
}

void AI_Pumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;
	
	if(me->aiType==MONS_MUTANTPUMKIN)
	{
		if(Random((me->hp/me->maxHP)*150)==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
		}
	}

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PUMPKDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if (me->reload==0)
			{
				FireExactBullet(me,me->x,me->y,me->z,Cosine(me->facing*32+Random(16)-8)*3,
					Sine(me->facing*32+Random(16)-8)*3,0,0,30,me->facing,BLT_ENERGY_BOUNCE,me->friendly);
				me->reload=2;
				me->facing++;
				if(me->facing>7)
				me->facing=0;
			}
		}
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			if(me->aiType==MONS_BOOMKIN && me->frm==0)
				FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
			me->reload=2;
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->frm==2 && goodguy)	// hits on this frame
	{
		if(me->aiType!=MONS_BOOMKIN)
			FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,24,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,2,map,world,me->friendly);
		else
			if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,0,0,0,map,world,me->friendly))
			{
				me->hp=1;
				me->GetShot(me,me,0,0,1,map,world);
				return;
			}
	}

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
			if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				if(me->ouch==0)
					me->mind1=60;	// for 2 seconds minimum
				else
					me->mind1=120;	// 4 seconds, because they HATE getting shot
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
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
			return;
		}
		if(me->aiType==MONS_BOOMKIN)
		{
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
		}
		else
		{
			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
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

void AI_BabyThing(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
					me->type=MONS_NONE;	// vanish if this spot is occupied
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
			FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
			me->reload=5;
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
			me->frmAdvance=32;
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
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0)
			{
				// if the goodguy is near, or he shot me
				me->mind=1;	// start hunting
				me->mind1=255;

				FaceGoodguy(me,goodguy);
			}
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
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 && me->reload==0)
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
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
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
			me->frmAdvance=128;
		}
	}
	// this is needed because his idle frame doesn't fit into his walking animation, so this skips it
	if(me->seq==ANIM_MOVE && me->frm==10 && me->frmTimer>127)
	{
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Moss(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *baby;
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->reload)
		me->reload--;

	if(me->mind)
		me->mind--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq!=ANIM_DIE && me->frm==4)
		{
			x=((me->x>>FIXSHIFT)/TILE_WIDTH);
			y=((me->y>>FIXSHIFT)/TILE_HEIGHT);
			me->x=(x*TILE_WIDTH+TILE_WIDTH/2)<<FIXSHIFT;
			me->y=(y*TILE_HEIGHT+TILE_HEIGHT/2)<<FIXSHIFT;
			me->dx=0;
			me->dy=0;	// you are at your destination
			me->reload=Random(200)+55;

			// this prevents overlapping ones, in cases of ones getting born near-simultaneously
			me->aiType=MONS_NONE;	// so it doesn't get counted in the moss check
			if(MossCheck(x,y))
			{
				if(!me->friendly)
					player.totalEnemies--;
				return;	// with type=MONS_NONE, so it is dead
			}
			me->aiType=MONS_MOSS;
		}
		return;
	}

	// if goodguy steps on you, hurt him
	if(goodguy && (!me->mind) && RangeToTarget(me,goodguy)<20*FIXAMT)
	{
		goodguy->GetShot(me,goodguy,0,0,4,map,world);
		me->mind=5;	// so as not to hurt him too often
	}
	if(me->reload==0)
	{
		me->reload=Random(200)+55;
		// spawn
		x=(me->x>>FIXSHIFT)/TILE_WIDTH;
		y=(me->y>>FIXSHIFT)/TILE_HEIGHT;
		switch(Random(4))
		{
			case 0:
				// left
				if(x>0 && map->GetTile(x-1,y)->wall==0 &&
					(GetTerrain(world,map->GetTile(x-1,y)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
					!(GetItem(map->GetTile(x-1,y)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
					&& (!MossCheck(x-1,y)))
				{
					baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_ATTACK;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=-(32*FIXAMT)/20;
						baby->dy=0;
					}
				}
				break;
			case 1:
				// right
				if(x<map->width-1 && map->GetTile(x+1,y)->wall==0 &&
					(GetTerrain(world,map->GetTile(x+1,y)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
					!(GetItem(map->GetTile(x+1,y)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
					&& (!MossCheck(x+1,y)))
				{
					baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_ATTACK;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=(32*FIXAMT)/20;
						baby->dy=0;
					}
				}
				break;
			case 2:
				// up
				if(y>0 && map->GetTile(x,y-1)->wall==0 &&
					(GetTerrain(world,map->GetTile(x,y-1)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
					!(GetItem(map->GetTile(x,y-1)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
					&& (!MossCheck(x,y-1)))
				{
					baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_MOVE;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=0;
						baby->dy=-(24*FIXAMT)/20;
					}
				}
				break;
			case 3:
				// down
				if(y<map->height-1 && map->GetTile(x,y+1)->wall==0 &&
					(GetTerrain(world,map->GetTile(x,y+1)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
					!(GetItem(map->GetTile(x,y+1)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
					&& (!MossCheck(x,y+1)))
				{
					baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
					if(baby)
					{
						baby->action=ACTION_BUSY;
						baby->seq=ANIM_MOVE;
						baby->frm=0;
						baby->frmTimer=0;
						baby->frmAdvance=64;
						baby->dx=0;
						baby->dy=(24*FIXAMT)/20;
					}
				}
				break;
		}
	}
}

void AI_MossGrande(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *baby;
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==11 && me->mind1==0)
		{
			for(x=0;x<16;x++)
				FireBullet(me,me->x,me->y,x*16,BLT_ENERGY,me->friendly);	// ring of fire
			me->mind1=60;
		}
		return;
	}

	if(me->seq==ANIM_IDLE)
		me->frmAdvance=Random(200)+1;	// animate idle at random speeds

	if(!me->mind1 && Random(100)==0)
	{
		me->mind1=0;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
	}
	if((me->reload==0) && Random(200)==0)
	{
		me->reload=255;
		// spawn in all four directions at once
		x=(me->x>>FIXSHIFT)/TILE_WIDTH;
		y=(me->y>>FIXSHIFT)/TILE_HEIGHT;

		// left
		if(x>0 && map->GetTile(x-1,y)->wall==0 &&
			(GetTerrain(world,map->GetTile(x-1,y)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
			!(GetItem(map->GetTile(x-1,y)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
			&& (!MossCheck(x-1,y)))
		{
			baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_ATTACK;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=-(32*FIXAMT)/20;
				baby->dy=0;
			}
		}
		// right
		if(x<map->width-1 && map->GetTile(x+1,y)->wall==0 &&
			(GetTerrain(world,map->GetTile(x+1,y)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
			!(GetItem(map->GetTile(x+1,y)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
			&& (!MossCheck(x+1,y)))
		{
			baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_ATTACK;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=(32*FIXAMT)/20;
				baby->dy=0;
			}
		}
		// up
		if(y>0 && map->GetTile(x,y-1)->wall==0 &&
			(GetTerrain(world,map->GetTile(x,y-1)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
			!(GetItem(map->GetTile(x,y-1)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
			&& (!MossCheck(x,y-1)))
		{
			baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_MOVE;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=0;
				baby->dy=-(24*FIXAMT)/20;
			}
		}
		// down
		if(y<map->height-1 && map->GetTile(x,y+1)->wall==0 &&
			(GetTerrain(world,map->GetTile(x,y+1)->floor)->flags&(TF_WATER|TF_LAVA|TF_SOLID))==0 &&
			!(GetItem(map->GetTile(x,y+1)->item)->flags&(IF_SOLID|IF_BULLETPROOF))
			&& (!MossCheck(x,y+1)))
		{
			baby=AddBaby(me->x,me->y,0,MONS_MOSS,me);
			if(baby)
			{
				baby->action=ACTION_BUSY;
				baby->seq=ANIM_MOVE;
				baby->frm=0;
				baby->frmTimer=0;
				baby->frmAdvance=64;
				baby->dx=0;
				baby->dy=(24*FIXAMT)/20;
			}
		}
	}
}

void AI_Magmazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=12 && me->reload==0 && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			FireBullet(me,x,y,me->facing,BLT_FLAME2,me->friendly);
			me->reload=2;
			me->mind=0;
		}
		if(me->seq==ANIM_ATTACK && me->frm==13 && me->reload==0)
			me->reload=20;
		return;	// can't do nothin' right now
	}
	
	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*Random(32));

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			FaceGoodguy(me,goodguy);
			// roast him!
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
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Shroom(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

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

void AI_Mush(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_MUSHOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_MUSHDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm>=1 && me->frm<=15 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*72;
			y=me->y+Sine(me->facing*32)*64;
			i=(me->facing*32-64)&255;
			x+=Cosine(i)*48;
			y+=Sine(i)*32;
			i=(me->facing*32-16+Random(33))&255;
			FireExactBullet(me,x,y,FIXAMT*64,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE,me->friendly);
			MakeSound(SND_MUSHSPORES,me->x,me->y,SND_CUTOFF,600);
			me->reload=1;
		}
		if(me->seq==ANIM_A1 && me->frm==16)
			me->frmAdvance=64;	// slow the animation for the recovery part
		if(me->seq==ANIM_ATTACK && me->frm==1 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*72;
			y=me->y+Sine(me->facing*32)*64;
			i=(me->facing*32+64)&255;
			x+=Cosine(i)*48;
			y+=Sine(i)*32;
			FireBullet(me,x,y,me->facing,BLT_SHROOM,me->friendly);
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1000);
			me->reload=5;
		}
		if(me->seq==ANIM_DIE)
		{
			me->facing=2;	// die only works from forward (memory saver)
			if(me->frm>=3)
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

	if(me->mind==0)	// this mode is only used prior to him seeing Bouapha the first time
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0)
			{
				// get mad!
				MakeSound(SND_MUSHMAD,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				me->mind=2;		// destroy bouapha
				me->facing=2;	// angry animation ONLY works from forward (memory saver)
			}
		}
	}
	else if(me->mind==2)		// when mind=2, hold still and go off on Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(32)==0)
			{
				// get him! (fire shroom cannon)
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
			if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(28)==0)
			{
				// get him! (fire sporegun)
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
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
			if(RangeToTarget(me,goodguy)>(256*FIXAMT))
			{
				// chase him down!
				me->mind=1;
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
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)<200*FIXAMT)
				me->mind=2;	// in range, start killin'
		}
		else
			me->mind=0;
	}
}

void AI_TheThing(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;
	byte f;

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

	if(me->action==ACTION_BUSY)
	{
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
			g=AddBaby(me->x,me->y,0,MONS_BABYTHING,me);
			if(g)
			{
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
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
	if(me->reload==0 && Random(16)==0)
	{
		MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
		FireBullet(me,me->x-48*FIXAMT,me->y-10*FIXAMT,Random(88)+20,BLT_GRENADE,me->friendly);
		FireBullet(me,me->x+48*FIXAMT,me->y-10*FIXAMT,Random(88)+20,BLT_GRENADE,me->friendly);
		me->reload=30;
	}
	if(Random(128)==0)	// tongue
	{
		MakeSound(SND_THINGVOMIT,me->x,me->y,SND_CUTOFF,1500);
		me->action=ACTION_BUSY;
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_ThingTentacle(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->mind1<128)
		me->mind1++;
	if(me->mind1>128)
		me->mind1--;

	if((!me->parent) || me->parent->type==MONS_NONE)
	{
		if(me->hp>0 && !me->friendly)
			player.enemiesSlain++;
		me->type=MONS_NONE;
		return;
	}

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
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			me->x+=((1-Random(3))<<FIXSHIFT);
			me->y+=((1-Random(3))<<FIXSHIFT);
			me->facing=(me->facing+1-Random(3))&15;
			FlailLock(me);
			if(me->frm==4 && !me->mind3 && me->parent && (me->parent->aiType!=MONS_THINGTENT&&me->parent->aiType!=MONS_THING2TENT) && me->aiType==MONS_THING2TENT)
			{
				me->parent->mind3++;
				me->mind3 = 1;
				MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
			}
			if(me->frm==4 && me->parent && (me->parent->aiType==MONS_THINGTENT||me->parent->aiType==MONS_THING2TENT) && me->parent->hp>0)	// kill parent (unless it's The Thing)
			{
				me->parent->hp=1;
				me->parent->GetShot(0,me,0,0,10,map,world);
			}
			g=me->parent;
			while(g && (g->aiType==MONS_THINGTENT||g->aiType==MONS_THING2TENT))
			{
				// make the whole thing flail
				g->facing=(g->facing+1-Random(3))&15;
				FlailLock(g);
				g=g->parent;
			}
			if(me->parent->aiType==MONS_THINGTENT||me->parent->aiType==MONS_THING2TENT)
			{
				me->x=me->parent->x+Cosine(me->parent->facing*16)*64;
				me->y=me->parent->y+Sine(me->parent->facing*16)*44;
			}
			if(me->frm==4 && me->mind3 == 1 && me->parent->aiType==MONS_THING2TENT)
			{
				me->mind3++;
				for(int i = 0; i < 8; i++)
				{
					FireBullet(me,me->x+Cosine(i*32)*32,me->y+Sine(i*32)*32,32*i,BLT_ENERGY,me->friendly);
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->parent->aiType==MONS_THINGTENT||me->parent->aiType==MONS_THING2TENT)
	{
		// send damage up and down the list
		if(me->hp>me->parent->hp)
		{
			me->hp=me->parent->hp;
			me->ouch=4;
		}
		else if(me->hp<me->parent->hp)
		{
			me->parent->hp=me->hp;
			me->parent->ouch=4;
		}
		if(me->parent->hp==0 && me->seq!=ANIM_DIE)
		{
			me->hp=1;
			me->GetShot(0,me,0,0,10,map,world);
		}
	}

	FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,32,0,0,2,map,world,me->friendly);

	if(me->parent->aiType==MONS_THINGTENT||me->parent->aiType==MONS_THING2TENT)
		me->mind1=me->parent->mind1;
	else
	{
		if(me->mind1==128)
		{
			me->mind1=128-20+Random(41);
		}
	}
	if(Random(64)==0)
	{
		me->mind1=128-20+Random(41);
	}
	else
	{
		if(me->mind1<128)
			me->facing=(me->facing+1)&15;
		else if(me->mind1>128)
			me->facing=(me->facing-1)&15;
	}

	// keep tentacles within allowable angles
	FlailLock(me);

	if(me->parent->aiType==MONS_THINGTENT||me->parent->aiType==MONS_THING2TENT)
	{
		me->x=me->parent->x+Cosine(me->parent->facing*16)*64;
		me->y=me->parent->y+Sine(me->parent->facing*16)*44;
	}
	else
	{
		switch(me->mind)
		{
			case 0:
				me->x=me->parent->x-40*FIXAMT;
				me->y=me->parent->y-1*FIXAMT;
				break;
			case 1:
				me->x=me->parent->x+40*FIXAMT;
				me->y=me->parent->y-1*FIXAMT;
				break;
			case 2:
				me->x=me->parent->x+40*FIXAMT;
				me->y=me->parent->y-16*FIXAMT;
				break;
			case 3:
				me->x=me->parent->x-40*FIXAMT;
				me->y=me->parent->y-16*FIXAMT;
				break;
		}
	}
}

void AI_SuperZombie(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
			else if(me->frm==3)
			{
				MakeSound(SND_SZOMBIELEAP,me->x,me->y,SND_CUTOFF,1200);
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				me->dz=10*FIXAMT;
			}
			else if(me->frm<7)
			{
				// stay the course
				Dampen(&me->dx,FIXAMT/8);
				Dampen(&me->dy,FIXAMT/8);
			}
			else if(me->frm==12 && me->frmTimer<32)
			{
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
				ShakeScreen(10);
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
			if(RangeToTarget(me,goodguy)<300*FIXAMT)
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

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(RangeToTarget(me,goodguy)<128*FIXAMT && me->mind1==0)
			{
				me->mind=2;	// in range, start killin'
				me->mind1=8;
			}
			else if(Random(64)==0)
			{
				//leap!
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
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

void AI_StickMan(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_HAPPYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_HAPPYDIE,me->x,me->y,SND_CUTOFF,1200);
		if(me->hp>0)
		{
			me->seq=ANIM_A2;	//sad
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}

	if(me->seq==ANIM_A2 && me->frm==1)
	{
		me->frm=0;
		me->seq=ANIM_IDLE;
	}
	if(me->action==ACTION_BUSY)
	{
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
				MakeSound(SND_HAPPYFLEX,me->x,me->y,SND_CUTOFF,1500);
				me->action=ACTION_BUSY;
				me->seq=ANIM_A1;	// muscles
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->mind=1;
			}
		}
	}
	else if(me->mind==1)	// chase down Bouapha
	{
		if(goodguy)
		{
			i=RangeToTarget(me,goodguy);
			if(i<(60*FIXAMT) && Random(4)==0 && me->reload==0)
			{
				// get him! (chomp)
				MakeSound(SND_HAPPYCHOMP,me->x,me->y,SND_CUTOFF,1500);
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

			if(me->seq!=ANIM_A2)
			{
				me->dx=Cosine(me->facing*32)*8;
				me->dy=Sine(me->facing*32)*8;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*2;
			}
			if(me->seq!=ANIM_MOVE && me->seq!=ANIM_A2)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(256)==0)
			{
				// show off
				MakeSound(SND_HAPPYFLEX,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
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

void AI_BabySeal(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SEALOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SEALDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<32)
		{
			f=me->facing*32;
			switch(me->frm)
			{
				case 3:
					f-=20;
					break;
				case 5:
					f-=10;
					break;
				case 7:
					f-=4;
					break;
				case 9:
					f+=4;
					break;
				case 11:
					f+=10;
					break;
				case 13:
					f+=20;
					break;
				default:
					return;	// don't fire on this frame
			}
			f=f-8+Random(17);
			x=me->x+Cosine(f)*16;
			y=me->y+Sine(f)*16;
			FireBullet(me,x,y,f,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,x,y,SND_CUTOFF,1050);
			me->reload=20;
		}
		if(me->seq==ANIM_DIE && me->frm>=4)
		{
			for(f=0;f<8;f++)
				SpecialSnow(me->x+((-32+Random(65))<<FIXSHIFT),
							me->y+((-32+Random(65))<<FIXSHIFT));
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(Random(64)==0 && me->reload==0 && RangeToTarget(me,goodguy)<700*FIXAMT)
			{
				// get him!
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
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

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Isozoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->mind==0 && me->hp>0)	// hasn't spotted Bouapha yet
	{
		if((goodguy && RangeToTarget(me,goodguy)<200*FIXAMT) || (me->ouch>0))
		{
			me->mind=1;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;	// rise out of snow
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=72;
			ExplodeParticles2(PART_SNOW2,me->x,me->y,0,12,6);
		}
		else if(me->seq!=ANIM_A2)
		{
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=1;
		}
		else if(me->frmTimer>20)
			me->frmTimer=0;	// keep him on that frame
		FaceGoodguy3(me,goodguy);
		return;
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// spit snowball
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(me,x,y,(me->facing*32-16+Random(33))&255,BLT_SNOWBALL,me->friendly);
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(8)==0 && me->reload==0)
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
			return;
		}
	}

	FaceGoodguy3(me,goodguy);

	// Isozoids don't move
	me->dx=0;
	me->dy=0;
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_Snowguy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SNOWOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SNOWDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==11 && me->reload==0 && goodguy)
		{
			// only fire if the range is good
			if(RangeToTarget(me,goodguy)<800*FIXAMT)
			{
				x=me->x+Cosine(me->facing*32)*32;
				y=me->y+Sine(me->facing*32)*32;
				FireBullet(me,x,y,me->facing*32,BLT_BIGSNOW,me->friendly);
			}
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=128;
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(64*FIXAMT) && Random(8)==0)
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

			me->dx=Cosine(me->facing*32)*2;
			me->dy=Sine(me->facing*32)*2;
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
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				// throw a snowball
				MakeSound(SND_SNOWTHROW,me->x,me->y,SND_CUTOFF,1100);
				me->seq=ANIM_A1;
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
		}
		if(!(me->mind1--))	// time to get a new direction
		{
			if((goodguy) && Random(3)==0)
				me->mind=0;	// get back on track
			else
				me->facing=(byte)Random(8);
			me->mind1=Random(40)+1;
		}

		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
}

void AI_Penguin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PENGDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frm<4)
			{
				me->dx=Cosine(me->facing*32)*7;
				me->dy=Sine(me->facing*32)*7;
				x=me->x+Cosine(me->facing*32)*10;
				y=me->y+Sine(me->facing*32)*10;
				if(me->AttackCheck(10,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
					me->frm=4;	// bounce back
					me->frmTimer=0;
				}
			}
			else
			{
				me->dx=-Cosine(me->facing*32)*3;
				me->dy=-Sine(me->facing*32)*3;
			}
			me->reload=10;
		}

		if(me->seq==ANIM_DIE)
		{
			me->frmAdvance=128;
			me->facing=(me->facing+1)&7;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==2)		// when mind=2, pursue relentlessly
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT))
			{
				me->mind=1;	// start circling
			}
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=256;
			}
		}
		else
		{
			me->mind=0;	// if there's no goodguy, get random
			me->mind1=1;
		}
	}
	else if(me->mind==0)	// random wandering
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(128*FIXAMT))
			{
				me->mind=1;	// circling mode
			}
		}
		if(!(me->mind1--))	// time to get a new direction
		{
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
			me->frmAdvance=256;
		}
	}
	else if(me->mind==1)	// circling
	{
		FaceGoodguy2(me,goodguy);

		// move sideways
		me->dx=Cosine((me->facing*32+64)&255)*3;
		me->dy=Sine((me->facing*32+64)&255)*3;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}

		if(goodguy && RangeToTarget(me,goodguy)>128*FIXAMT)
		{
			me->mind=2;	// chase him down
		}

		if((!me->reload) && Random(32)==0)
		{
			MakeSound(SND_PENGPECK,me->x,me->y,SND_CUTOFF,1000);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
		}
	}
}

void AI_Zomboni(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			if(me->frm>=4)
			{
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
			if(me->frm==8)
			{
				// become a zombie
				player.enemiesSlain--;
				me->type=MONS_ZOMBIE;
				me->aiType=MONS_ZOMBIE;
				strcpy(me->name,MonsterName(me->type));
				me->mind=0;
				me->mind1=0;
				me->hp=20;
				me->maxHP=20;
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=32;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->seq==ANIM_MOVE && me->frm==1 && me->frmTimer<32)
		MakeSound(SND_ZOMBONIZOOM,me->x,me->y,SND_CUTOFF,1500);

	if(me->mind==0)
	{
		if((goodguy && RangeToTarget(me,goodguy)<(400*FIXAMT)) || (me->ouch))
		{
			me->mind=1;	// start chasing him down
		}
	}
	else
	{
		if(!me->reload)
		{
			FaceGoodguy2(me,goodguy);
			me->reload=8;
		}
		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		if(me->frm==0)
		{
			BlowSmoke(me->x-Cosine(me->facing*32)*32,me->y-Sine(me->facing*32)*32,me->z+FIXAMT*40,FIXAMT);
			if(goodguy && RangeToTarget(me,goodguy)<90*FIXAMT)
			{
				// smash into him
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,10,map,world);
			}
		}
	}
}

void AI_Yeti(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_YETIOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_YETIDIE,me->x,me->y,SND_CUTOFF,1200);
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
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->frmTimer<32)
		{
			x=me->x+Cosine(me->facing*32)*64;
			y=me->y+Sine(me->facing*32)*64;
			x+=Cosine(((me->facing+2)*32)&255)*32;
			y+=Sine(((me->facing+2)*32)&255)*32;
			FireExactBullet(me,x,y,40*FIXAMT,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,4*FIXAMT,0,100,
								me->facing*32,BLT_BIGSNOW,me->friendly);
			me->reload=10;
		}
		if(me->seq==ANIM_A2 && me->frm>3)
		{
			ShakeScreen(3);
			// snowballs fall all over
			x=me->x+((320-Random(641))<<FIXSHIFT);
			y=me->y+((240-Random(481))<<FIXSHIFT);
			if(Random(3)==0)
				FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_BIGSNOW,me->friendly);
			else
				FireExactBullet(me,x,y,400*FIXAMT,0,0,0,0,100,me->facing*32,BLT_SNOWBALL,me->friendly);
		}
		if(me->seq==ANIM_A1 && (me->frm==5 || me->frm==6))
		{
			//Lunatic difficulty changes
			if(profile.supremeMode)
			{
				x=me->x+Cosine(me->facing*32-16)*64;
				y=me->y+Sine(me->facing*32)*64;
				FireBullet(me,x,y,me->facing-1,BLT_ICESPIKE,me->friendly);
				x=me->x+Cosine(me->facing*32+16)*64;
				FireBullet(me,x,y,me->facing+1,BLT_ICESPIKE,me->friendly);
			}
			else
			{
				x=me->x+Cosine(me->facing*32)*64;
				y=me->y+Sine(me->facing*32)*64;
				FireBullet(me,x,y,me->facing,BLT_ICESPIKE,me->friendly);
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

			me->seq=ANIM_A3;	// wave hello first
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->facing=2;
			me->action=ACTION_BUSY;

			if(me->aiType==MONS_SVEN) // say "Hello, I am Sven"
				MakeSound(SND_SVENHI,me->x,me->y,SND_CUTOFF,1200);
			else	// say "Hello, I am Bjorn"
				MakeSound(SND_BJORNHI,me->x,me->y,SND_CUTOFF,1200);
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
				// Sven throws snowballs
				if(me->aiType==MONS_SVEN||(me->aiType==MONS_IMITATER&&me->type==MONS_SVEN))
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
					// Bjorn causes stalag-quakes
					me->seq=ANIM_A1;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					FaceGoodguy(me,goodguy);
					me->action=ACTION_BUSY;
				}
			}
			else	// 25% chance of yodel
			{
				MakeSound(SND_YETIYODEL,me->x,me->y,SND_CUTOFF,1500);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->facing=2;
				me->action=ACTION_BUSY;
			}
			me->dx=0;
			me->dy=0;
			return;
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
}

void AI_Geozoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

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
			// spit rock
			FireBullet(me,me->x,me->y,me->facing,BLT_ROCK,me->friendly);
			me->reload=20;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(8)==0 && me->reload==0)
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
			return;
		}
	}

	FaceGoodguy3(me,goodguy);

	if(me->mind1)
	{
		if(!(--me->mind1))
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			// VERY slow inching along
			me->dx=Cosine(me->facing*32)/8;
			me->dy=Sine(me->facing*32)/8;
		}
		else
		{
			me->dx=0;
			me->dy=0;
		}
	}
	else
	{
		if(me->seq==ANIM_IDLE)
		{
			me->dx=0;
			me->dy=0;
			me->mind1=10+Random(50);
		}
	}
}

void AI_Mumble(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// crush skull
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*10,Sine(me->facing*32)*10,12,map,world);
			}
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// not yet aware
	{
		if(me->aiType==MONS_MUMBLE2)
			me->mind=1;

		if(goodguy && RangeToTarget(me,goodguy)<200*FIXAMT)
		{
			me->mind=1;	// begin chasing.  Slowly.
		}
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
	}
	else
	{
		if(RangeToTarget(me,goodguy)<(60*FIXAMT) && Random(8)==0 && me->reload==0)
		{
			// crush his skull
			MakeSound(SND_MUMBLECRUSH,me->x,me->y,SND_CUTOFF,1000);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			if(me->aiType==MONS_MUMBLE2 || me->aiType==MONS_MUMBLE3)
				me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=0;
			return;
		}
		if(!me->mind1)
		{
			// turns only once a second
			FaceGoodguy3(me,goodguy);
			if(me->aiType==MONS_MUMBLE2)
				me->mind1=0;
			else if(me->aiType==MONS_MUMBLE3)
				me->mind1=20;
			else
				me->mind1=30;
		}
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			if(me->aiType==MONS_MUMBLE2)
				me->frmAdvance=256;
			else if(me->aiType==MONS_MUMBLE2)
				me->frmAdvance=256;
		}
		else
		{
			if(me->aiType==MONS_MUMBLE2)
			{
				me->dx=Cosine(me->facing*32)*4;
				me->dy=Sine(me->facing*32)*4;
			}
			else
			{
				if(!((me->frm>=3 && me->frm<=5) || (me->frm>=9 && me->frm<=11)))
				{
					if (me->aiType==MONS_MUMBLE3)
					{
						me->dx=Cosine(me->facing*32)*4;
						me->dy=Sine(me->facing*32)*4;
					}
					else
					{
						me->dx=Cosine(me->facing*32);
						me->dy=Sine(me->facing*32);
					}
				}
				else
				{
					me->dx=0;
					me->dy=0;
				}
			}
		}
	}
}

void AI_Djinni(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_DJINNIOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_DJINNIDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			// punch him
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*10,Sine(me->facing*32)*10,6,map,world);
			}
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(54*FIXAMT) && Random(8)==0 && me->reload==0)
	{
		// punch him
		MakeSound(SND_DJINNIPUNCH,me->x,me->y,SND_CUTOFF,600);
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
	if(Random(256)==0)
	{
		// scare him
		MakeSound(SND_DJINNIBOO,me->x,me->y,SND_CUTOFF|SND_ONE,1300);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}

	if(!me->mind1)
	{
		// turns only every so often
		FaceGoodguy3(me,goodguy);
		me->mind1=10;
	}
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->dx=Cosine(me->facing*32)*2;
		me->dy=Sine(me->facing*32)*2;
	}
}

void AI_MagicLamp(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
		{
			MakeSound(SND_LAMPDIE,me->x,me->y,SND_CUTOFF,1200);
			KillKids(me);	// kill off all my children!
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==18 && me->reload==0)
		{
			g=AddBaby(me->x+FIXAMT*20,me->y+FIXAMT*12,10*FIXAMT,MONS_DJINNI,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->seq=ANIM_A2;	// materialize
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=64;
				g->action=ACTION_BUSY;
				g->parent=me;
			}
			me->reload=200;
		}
		else if(me->seq==ANIM_ATTACK && me->frm>4)
		{
			BlowSmoke(me->x+FIXAMT*20,me->y+FIXAMT*12,FIXAMT*10,3*FIXAMT);
		}
		if(me->seq==ANIM_DIE)
			me->frmAdvance=256;
		return;
	}

	if((!me->reload) && Random(200)==0)
	{
		MakeSound(SND_LAMPPOOF,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
	}
}

void AI_Cactus(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;
	byte b;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_CACTONDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>6)
		{
			me->frmAdvance=256;
		}
		if(me->seq==ANIM_ATTACK && me->frm==3)
		{
			// fire lots of spines
			MakeSound(SND_CACTONSHOOT,me->x,me->y,SND_CUTOFF,600);
			// but only fire if within a few hundred miles
			if(goodguy && RangeToTarget(me,goodguy)<1024*FIXAMT)
			{
				b=(me->facing*32-60);
				if(me->frmTimer>64)
					b+=5;

				for(i=0;i<12;i++)
				{
					x=me->x+Cosine(b)*32;
					y=me->y+Sine(b)*32;
					FireBullet(me,me->x,me->y,b,BLT_SPINE,me->friendly);
					b+=10;
				}
			}

		}
		if(me->seq==ANIM_A1)
		{
			// accelerate
			if(abs(me->dx)<abs(Cosine(me->facing*32)*6))
				me->dx+=Cosine(me->facing*32)/2;
			if(abs(me->dy)<abs(Sine(me->facing*32)*6))
				me->dy+=Sine(me->facing*32)/2;
		}
		if(me->seq==ANIM_A2)
		{
			// decelerate
			if(abs(me->dx)>0)
				me->dx-=Cosine(me->facing*32);
			if(abs(me->dy)>0)
				me->dy-=Sine(me->facing*32);
		}
		return;
	}

	if(me->mind==0)	//idling
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			me->mind=1;	// start rolling somewhere
			me->facing=(byte)Random(8);
			me->mind1=Random(30)+30;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;	// get into rolling animation
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			return;
		}
		if(me->reload==0)
		{
			me->reload=120;
			if(goodguy)
				FaceGoodguy(me,goodguy);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;	// shoot some spines
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			return;
		}
		me->dx=0;
		me->dy=0;
		if(me->seq!=ANIM_IDLE)
		{
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;

	}
	else if(me->mind==1)
	{
		if(me->mind1)
			me->mind1--;
		else
		{
			me->mind=0;	// got to the destination
			me->mind1=Random(70)+30;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A2;	// get out of rolling animation
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			// drift to a stop
			me->dx=Cosine(me->facing*32);
			me->dy=Sine(me->facing*32);
			return;
		}
		if(abs(me->dx)<abs(Cosine(me->facing*32)*6))
			me->dx+=Cosine(me->facing*32)/2;
		if(abs(me->dy)<abs(Sine(me->facing*32)*6))
			me->dy+=Sine(me->facing*32)/2;

		if((me->seq!=ANIM_MOVE) || (me->seq==ANIM_MOVE && me->frm==4 && me->frmTimer>128))
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=128;
	}
}

void AI_Roller(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me,me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(8),BLT_ROCK,me->friendly);
			FireBullet(me,me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(8),BLT_ROCK,me->friendly);
			FireBullet(me,me->x+((32-Random(65))<<FIXSHIFT),me->y+((32-Random(65))<<FIXSHIFT),
				(byte)Random(8),BLT_ROCK,me->friendly);
		}
		return;
	}

	if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,40,0,0,255,map,world,me->friendly))
	{
		MakeSound(SND_ROLYPOLYWALL,me->x,me->y,SND_CUTOFF,1000);
	}
	if(me->mind1)	// bonked a wall
	{
		me->mind1=0;
		if(me->aiType==MONS_ROLLER || me->aiType==MONS_ROLLER3)
			me->facing=(me->facing+2)&7;
		else
			me->facing=(me->facing+6)&7;

		MakeSound(SND_ROLYPOLYWALL,me->x,me->y,SND_CUTOFF,800);
	}

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
		default:
			f=ANIM_ATTACK;	// roll up
			break;
	}
	if(me->seq!=f)
	{
		me->seq=f;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
	}
}

void AI_Lich(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			// still chase, but slowly
			x=goodguy->x;
			y=goodguy->y-72*FIXAMT;

			FaceGoodguy3(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/8;
			if(me->y<y)
				me->dy+=FIXAMT/8;
			if(me->x>x)
				me->dx-=FIXAMT/8;
			if(me->y>y)
				me->dy-=FIXAMT/8;

			Clamp(&me->dx,FIXAMT*1);
			Clamp(&me->dy,FIXAMT*1);

			if(me->frm==1 && me->frmTimer<64)
			{
				// shoot laser
				x=me->x;
				y=me->y+FIXAMT*8;
				LaunchMegabeam(me,x,y,me->ID);
			}
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
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
		if(goodguy)
		{
			if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,55,0,0,1,map,world,me->friendly))
			{
				PoisonVictim(GetLastGuyHit(),60);
			}

			// calculate desired location (want to be above Bouapha)
			x=goodguy->x;
			y=goodguy->y-96*FIXAMT;

			FaceGoodguy2(me,goodguy);
			if(me->x<x)
				me->dx+=FIXAMT/8;
			if(me->y<y)
				me->dy+=FIXAMT/8;
			if(me->x>x)
				me->dx-=FIXAMT/8;
			if(me->y>y)
				me->dy-=FIXAMT/8;

			Clamp(&me->dx,FIXAMT*3);
			Clamp(&me->dy,FIXAMT*4);
		}

		if(me->reload==8)
		{
			// left eye fires
			f=(me->facing*32-32+me->mind1)&255;
			// can't fire backwards
			if(f>128)
			{
				if(f<128+64)
					f=128;
				else
					f=0;
			}
			FireExactBullet(me,me->x-32*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
						Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY,me->friendly);
		}
		if(!me->reload)
		{
			// right eye fires
			f=(me->facing*32+32-me->mind1)&255;
			// can't fire backwards
			if(f>128)
			{
				if(f<128+64)
					f=128;
				else
					f=0;
			}
			FireExactBullet(me,me->x+32*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
						Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY,me->friendly);
			me->reload=16;
			me->mind1+=8;
			if(me->mind1>=64)
				me->mind1=0;
		}

		if(Random(128)==0)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			return;
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

void AI_DustDevil(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}

	if(me->mind1<255)
		me->mind1++;

	me->frmAdvance=128;	// always frame speed=128
	if(goodguy)
	{
		if(me->mind==0)
		{
			if(goodguy->x<me->x)
			{
				goodguy->dx+=(me->mind1*FIXAMT/256);
				goodguy->dy+=(me->mind1*FIXAMT/1024);
			}
			else
			{
				goodguy->dx-=(me->mind1*FIXAMT/256);
				goodguy->dy-=(me->mind1*FIXAMT/1024);
			}
			if(goodguy->y<me->y)
			{
				goodguy->dy+=(me->mind1*FIXAMT/256);
				goodguy->dx-=(me->mind1*FIXAMT/1024);
			}
			else
			{
				goodguy->dy-=(me->mind1*FIXAMT/256);
				goodguy->dx+=(me->mind1*FIXAMT/1024);
			}
		}
		else
		{
			// immediately after hitting him, shove him away
			if(goodguy->x<me->x)
				goodguy->dx-=FIXAMT*2;
			else
				goodguy->dx+=FIXAMT*2;
			if(goodguy->y<me->y)
				goodguy->dy-=FIXAMT*2;
			else
				goodguy->dy+=FIXAMT*2;
			if(me->mind1>10)
				me->mind=0;	// go back to sucking
		}
		Clamp(&goodguy->dx,FIXAMT*6);
		Clamp(&goodguy->dy,FIXAMT*6);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->frmTimer<64 && (goodguy))
		{
			if(me->AttackCheck(80,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				FaceGoodguy(me,goodguy);
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*12,
								Sine(me->facing*32)*12,16,map,world);
				me->mind1=0;
				me->mind=1;	// push him away
			}
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<80*FIXAMT)
		{
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->action=ACTION_BUSY;
			return;
		}
	}
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
	}
}

void AI_MechaBouapha(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;

	if(me->ouch==4)
	{
		// make noise?
		if(me->mind1<64 && me->mind==1)
			me->mind1+=16;	// ouch
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
			FireBullet(me,me->x,me->y,me->facing,BLT_EVILHAMMER,me->friendly);
			me->reload=20+Random(50);
		}
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->frmTimer<64 && Random(4)==0)
			me->facing=(byte)Random(8);

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
			me->facing=(byte)Random(8);
		}
		return;
	}

	if(me->mind==0)	// unaware of Bouapha
	{
		if(goodguy && (RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0))
		{
			me->seq=ANIM_A3;	// unmelt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->reload=5+Random(30);
			me->mind=1;	// activate
		}
		else
		{
			me->seq=ANIM_A2;	// sit as a ball
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=32;
		}
	}
	else if(me->mind==1)	// normal mode
	{
		if(me->reload==0)	// time to fire
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
		if(me->mind1>32)	// time to run!
		{
			me->mind=2;	// running mode
			me->seq=ANIM_A1;	// melt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
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
		}
	}
	else // running away mode
	{
		if(Random(32)==0)
			me->facing=(byte)Random(8);
		me->seq=ANIM_A2;	// stay melted
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=16;
		if(me->mind1==0)
		{
			me->seq=ANIM_A3;	// unmelt
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->action=ACTION_BUSY;
			me->reload=5+Random(30);
			me->mind=1;	// activate
			return;
		}
		// accelerate
		me->dx+=Cosine(me->facing*32)*2;
		me->dy+=Sine(me->facing*32)*2;
		if(me->z<16*FIXAMT)
			me->dz+=FIXAMT*3;
		Clamp(&me->dx,FIXAMT*12);
		Clamp(&me->dy,FIXAMT*12);
	}
}

void AI_SphinxArm(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// make noise?
	}
	
	if(me->parent && me->parent->type!=MONS_NONE)
	{
		if(me->aiType==MONS_SPHXARM1||me->aiType==MONS_ELDARM1)
			{
				me->x=me->parent->x-108*FIXAMT;
				me->y=me->parent->y+22*FIXAMT;
				me->z=me->parent->z;
			}
		else if(me->aiType==MONS_SPHXARM2||me->aiType==MONS_ELDARM2)
			{
				me->x=me->parent->x+108*FIXAMT;
				me->y=me->parent->y+22*FIXAMT;
				me->z=me->parent->z;
			}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->frmTimer<64 && (goodguy))
		{
			// hurt guy
			MakeSound(SND_SPHINXSTOMP,me->x,me->y,SND_CUTOFF,600);
			x=me->x>>FIXSHIFT;
			y=(me->y>>FIXSHIFT)+115;
			if(me->aiType==MONS_ELDARM1||me->aiType==MONS_ELDARM2)
			{
				int f=AngleFrom(me->x,me->y,goodguy->x,goodguy->y);
				me->reload=10;
				FireBullet(me,me->x,me->y,(f-32)&255,BLT_BIGSHELL,me->friendly);
				FireBullet(me,me->x,me->y,f,BLT_BIGSHELL,me->friendly);
				FireBullet(me,me->x,me->y,(f+32)&255,BLT_BIGSHELL,me->friendly);
			}
			
			if(me->aiType==MONS_SPHXARM1)
				x-=20;
			else
				x+=20;

			if(FindVictim(me,x,y,60,Cosine(me->facing*32)*12,
								Sine(me->facing*32)*12,30,map,world,me->friendly))
			{
				GetLastGuyHit()->dx=Cosine(me->facing*32)*6;
				GetLastGuyHit()->dx=Sine(me->facing*32)*6;
			}

			if(RangeToTarget(me,goodguy)<1000*FIXAMT)
				ShakeScreen(6);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2+1,y+me->recty2+1,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx+1))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty+1))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;
	}

	if(me->reload==0)
	{
		// swipe at him
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->reload=30+Random(60);
		return;
	}
	// just sit there when not swiping
	me->seq=ANIM_IDLE;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=128;
}

void AI_Sphinx(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SPHINXOUCH,me->x,me->y,SND_CUTOFF,600);
		else
			MakeSound(SND_SPHINXDIE,me->x,me->y,SND_CUTOFF,600);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->frmTimer<32)
		{
			g=AddBaby(me->x,me->y+FIXAMT*90,0,MONS_MUMBLE,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->mind=1;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==12 && me->frmTimer<32)
		{
			// sneeze some rocks out
			for(x=0;x<8;x++)
			{
				FireExactBullet(me,me->x,me->y+FIXAMT*40,60*FIXAMT,(8*FIXAMT-Random(17*FIXAMT)),
								Random(8*FIXAMT),0,0,60,2,BLT_ROCK,me->friendly);
			}
			// special rocks to deal with people trying to hide in "safe" spots
			FireExactBullet(me,me->x,me->y+FIXAMT*40,60*FIXAMT,8*FIXAMT,0,0,0,60,2,BLT_ROCK,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*40,60*FIXAMT,-8*FIXAMT,0,0,0,60,2,BLT_ROCK,me->friendly);
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

	if(me->reload==0)
	{
		// call upon a mummy
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
		me->reload=30+Random(120);
		return;
	}
	if(Random(128)==0)	// sneeze
	{
		MakeSound(SND_SPHINXSNEEZE,me->x,me->y,SND_CUTOFF,600);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		return;
	}
	// just sit there when not making mummies
	me->seq=ANIM_IDLE;
	me->frm=0;
	me->frmTimer=0;
	me->frmAdvance=128;
}

void AI_Freakazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm>=5 && me->frm<=8) && goodguy)
		{
			// spit stuff
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			FireBullet(me,x,y,me->facing*32-16+Random(33),BLT_SPINE,me->friendly);
			me->reload=15;
			me->mind=0;
		}
		return;	// can't do nothin' right now
	}
	
	if(goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		WaterRipple((me->x+me->dx*2)/FIXAMT,(me->y+me->dy*2)/FIXAMT,8*Random(32));

	if(goodguy)
	{
		if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			// spit on him!
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
	}

	if(me->mind)
		me->mind--;
	if(!me->mind)	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind=Random(60)+1;
	}

	me->dx=Cosine(me->facing*32)*2;
	me->dy=Sine(me->facing*32)*2;
	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_CentiBody(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}
	
	if(me->seq==ANIM_DIE && me->reload==0)
	{
		if(me->aiType==MONS_CENTIBBODY && me->frm==0)
			FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
		ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
		me->reload=2;
	}

	if((!me->parent || (me->parent->aiType!=MONS_CENTIBODY && me->parent->aiType!=MONS_CENTIHEAD))&&me->aiType==MONS_CENTIBODY)
	{
		me->type=MONS_CENTIHEAD;	// if you have no parent, time to get mean
		me->aiType=MONS_CENTIHEAD;
		strcpy(me->name,MonsterName(me->type));
		me->hp=40;	// with full HP
		me->maxHP=40;
		return;
	}
	
	if((!me->parent || (me->parent->aiType!=MONS_CENTIBBODY && me->parent->aiType!=MONS_CENTIBHEAD))&&me->aiType==MONS_CENTIBBODY)
	{
		me->type=MONS_CENTIBHEAD;	// if you have no parent, time to get mean
		me->aiType=MONS_CENTIBHEAD;
		strcpy(me->name,MonsterName(me->type));
		me->hp=20;	// with full HP
		me->maxHP=20;
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,3);

		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<44*FIXAMT && me->reload==0)
	{
		goodguy->GetShot(me,goodguy,me->dy,-me->dx,4,map,world);
		me->reload=5;
	}

	if(me->aiType==MONS_CENTIBBODY)
	{
		// head towards parent
		if(me->parent->x>me->x)
			me->dx+=FIXAMT*2;
		else
			me->dx-=FIXAMT*2;
		if(me->parent->y>me->y)
			me->dy+=FIXAMT*2;
		else
			me->dy-=FIXAMT*2;
	}
	else
	{
		// head towards parent
		if(me->parent->x>me->x)
			me->dx+=FIXAMT*4;
		else
			me->dx-=FIXAMT*4;
		if(me->parent->y>me->y)
			me->dy+=FIXAMT*4;
		else
			me->dy-=FIXAMT*4;
	}

	if(RangeToTarget(me,me->parent)<FIXAMT*15)
	{
		me->dx=0;
		me->dy=0;
	}
	
	if(me->aiType==MONS_CENTIBBODY)
	{
		Clamp(&me->dx,FIXAMT*6);
		Clamp(&me->dy,FIXAMT*6);
	}
	else
	{
		Clamp(&me->dx,FIXAMT*6);
		Clamp(&me->dy,FIXAMT*6);
	}

	// decide on a facing
	if(me->dx>FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=7;
		else if(me->dy>FIXAMT)
			me->facing=1;
		else
			me->facing=0;
	}
	else if(me->dx<-FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=5;
		else if(me->dy>FIXAMT)
			me->facing=3;
		else
			me->facing=4;
	}
	else
	{
		if(me->dy<0)
			me->facing=6;
		else
			me->facing=2;
	}

	if(me->seq!=ANIM_MOVE)
	{
		me->seq=ANIM_MOVE;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
	}
}

void AI_CentiHead(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}
	
	if(me->seq==ANIM_DIE && me->reload==0)
	{
		if(me->aiType==MONS_CENTIBHEAD && me->frm==0)
			FireBullet(me,me->x,me->y,0,BLT_BOOM,me->friendly);
		ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,8);
		me->reload=2;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			ExplodeParticles(PART_HAMMER,me->x,me->y,me->z,3);

		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<44*FIXAMT && me->reload==0)
	{
		goodguy->GetShot(me,goodguy,me->dx,me->dy,6,map,world);
		me->reload=5;
	}

	if(me->mind1)	// bonked a wall or guy
	{
		me->facing=(byte)Random(8);
		me->mind1=0;
		me->dx=0;
		me->dy=0;
	}

	if(Random(8)==0)
	{
		if(goodguy)
			FaceGoodguy2(me,goodguy);
	}
	if(me->facing==0 || me->facing==1 || me->facing==7)
		me->dx+=FIXAMT;
	else if(me->facing==3 || me->facing==4 || me->facing==5)
		me->dx-=FIXAMT;
	if(me->facing==1 || me->facing==2 || me->facing==3)
		me->dy+=FIXAMT;
	else if(me->facing==5 || me->facing==6 || me->facing==7)
		me->dy-=FIXAMT;

	if(me->aiType==MONS_CENTIBHEAD)
	{
		Dampen(&me->dx,FIXAMT/4);
		Dampen(&me->dy,FIXAMT/4);
		Clamp(&me->dx,FIXAMT*5);
		Clamp(&me->dy,FIXAMT*5);
	}
	else
	{
		Dampen(&me->dx,FIXAMT/2);
		Dampen(&me->dy,FIXAMT/2);
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

void AI_Wacko(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_WACKOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_WACKODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frmTimer<196 && goodguy && (me->frm==5 || me->frm==11 || me->frm==17))
		{
			MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=10;
		}

		if(me->seq==ANIM_DIE && me->frm==5)
			me->frmAdvance=128;
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(8)==0)
			{
				// get him!
				//MakeSound(SND_WACKOSTAB,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=196;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
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
}

void AI_Boiler(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BOILERHIT,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=10)
		{
			FireExactBullet(me,me->x+FIXAMT*36,me->y,FIXAMT*50,
							-FIXAMT*2,FIXAMT*12,-FIXAMT*3,
							0,24,2,BLT_FLAME2,me->friendly);
			FireExactBullet(me,me->x+FIXAMT*52,me->y,FIXAMT*50,
							0,FIXAMT*12,-FIXAMT*3,
							0,24,2,BLT_FLAME2,me->friendly);
			FireExactBullet(me,me->x+FIXAMT*68,me->y,FIXAMT*50,
							FIXAMT*2,FIXAMT*12,-FIXAMT*3,
							0,24,2,BLT_FLAME2,me->friendly);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	// lob grenades occasionally
	if(me->reload==0 && Random(16)==0)
	{
		me->mind1=25;
		me->reload=80;
	}
	if(me->mind1>0)
	{
		me->mind1--;
		if((me->mind1&7)==0)
		{
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
			FireBullet(me,me->x-48*FIXAMT,me->y+40*FIXAMT,(byte)Random(129),BLT_GRENADE,me->friendly);
		}
	}
	// shoot flame occasionally, but only when not grenading
	if(Random(64)==0 && me->mind1==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
	}
}

void AI_GreatPumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;
	byte f;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GREATPKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_GREATPKDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->frmTimer<32)
		{
			g=AddBaby(me->x-FIXAMT*32,me->y+FIXAMT*40,0,MONS_PUMPKIN,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddBaby(me->x,me->y+FIXAMT*40,0,MONS_PUMPKIN,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddBaby(me->x+FIXAMT*32,me->y+FIXAMT*40,0,MONS_PUMPKIN,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddBaby(me->x+FIXAMT*64,me->y+FIXAMT*40,0,MONS_PUMPKIN,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			g=AddBaby(me->x-FIXAMT*64,me->y+FIXAMT*40,0,MONS_PUMPKIN,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->mind=1;
				g->mind1=120;
			}
			me->reload=60;
			MakeSound(SND_GREATPKVOMIT,me->x,me->y,SND_CUTOFF,1100);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind>0)
		me->mind--;

	if(goodguy)
		FaceGoodguy(me,goodguy);

	if(me->mind==8)
	{
		// left eye fires
		f=(me->facing*32-32+me->mind1)&255;

		FireExactBullet(me,me->x-64*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
					Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY,me->friendly);
	}
	if(!me->mind)
	{
		// right eye fires
		f=(me->facing*32+32-me->mind1)&255;
		// can't fire backwards
		if(f>128)
		{
			if(f<128+64)
				f=128;
			else
				f=0;
		}
		FireExactBullet(me,me->x+64*FIXAMT,me->y+32*FIXAMT,me->z+80*FIXAMT,
					Cosine(f)*8,Sine(f)*8,-3*FIXAMT,0,30,me->facing,BLT_ENERGY,me->friendly);
		me->mind=16;
		me->mind1+=8;
		if(me->mind1>=64)
			me->mind1=0;
	}

	if(me->reload==0 && Random(20)==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;
		me->action=ACTION_BUSY;
	}
}

void AI_Ultrazoid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;

	if(me->reload)
		me->reload--;

	if(me->mind1)
		me->mind1--;
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SERPENTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SERPENTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==6 || me->frm==7) && me->frmTimer<64)
		{
			// dump a wad of zoid projectiles
				// first a pile of energy bullets
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*4,FIXAMT*6,-FIXAMT*2,0,60,2,BLT_ENERGY,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*2,FIXAMT*8,-FIXAMT*2,0,60,2,BLT_ENERGY,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,0        ,FIXAMT*9,-FIXAMT*2,0,60,2,BLT_ENERGY,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*2,FIXAMT*8,-FIXAMT*2,0,60,2,BLT_ENERGY,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*4,FIXAMT*6,-FIXAMT*2,0,60,2,BLT_ENERGY,me->friendly);
				// now some fire
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*4,FIXAMT*16,-FIXAMT*10,0,24,2,BLT_FLAME2,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,0        ,FIXAMT*18,-FIXAMT*10,0,24,2,BLT_FLAME2,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*4,FIXAMT*16,-FIXAMT*10,0,24,2,BLT_FLAME2,me->friendly);
				// and some rocks
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*8,FIXAMT*1,-FIXAMT*1,0,60,2,BLT_ROCK,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*4,FIXAMT*4,-FIXAMT*1,0,60,2,BLT_ROCK,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*4,FIXAMT*4,-FIXAMT*1,0,60,2,BLT_ROCK,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*8,FIXAMT*1,-FIXAMT*1,0,60,2,BLT_ROCK,me->friendly);
				// and big snowballs
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,-FIXAMT*5,FIXAMT*1,-FIXAMT*2,0,40,2,BLT_BIGSNOW,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,0        ,FIXAMT*6,-FIXAMT*2,0,40,2,BLT_BIGSNOW,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60, FIXAMT*5,FIXAMT*1,-FIXAMT*2,0,40,2,BLT_BIGSNOW,me->friendly);
				// and cactus spines
			for(i=108;i>19;i-=10)
				FireExactBullet(me,me->x,me->y+FIXAMT*30,FIXAMT*60,Cosine(i)*8,Sine(i)*8,-FIXAMT*2,0,40,i/16,BLT_SPINE,me->friendly);

			MakeSound(SND_SERPENTSPIT,me->x,me->y,SND_CUTOFF,1100);
			me->mind1=2;
		}

		if(me->seq==ANIM_DIE)
		{
			ShakeScreen(10);
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=72;
		me->action=ACTION_BUSY;
		me->reload=90;
	}
}

void AI_DrLunatic(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_DRLOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_DRLDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==15)
		{
			player.enemiesSlain--;
			me->type=MONS_SDZL;	// become the Super Duper Zombie Lunatic
			me->aiType=MONS_SDZL;
			strcpy(me->name,monsType[me->type].name);
			me->hp=1000;
			me->maxHP=1000;
			me->mind=0;
			me->mind1=0;
			me->facing=2;
			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_IDLE;
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// if Bouapha gets near, laugh and then run
	{
		if(goodguy && RangeToTarget(me,goodguy)<160*FIXAMT)
		{
			MakeNormalSound(SND_DRLLAUGH);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->action=ACTION_BUSY;
			me->mind=1;
			me->reload=96;
			return;
		}
	}
	if(me->mind==1)	// running away
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->mind=2;	// stop running
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->facing=6;
			me->dx=0;
			me->dy=-FIXAMT*8;
		}
	}
	if(me->mind==2)	// waiting for Bouapha again
	{
		me->facing=2;
		me->dx=0;
		me->dy=0;
		if(goodguy && (RangeToTarget(me,goodguy)<128*FIXAMT || me->ouch>0))
		{
			MakeNormalSound(SND_DRLFEAR);
			me->mind=3;	// start running in fear
			me->reload=1;
		}
	}
	if(me->mind==3)
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->facing=(me->facing+1-(byte)Random(3))&7;
			me->reload=Random(16)+1;
		}

		if(me->mind1)
		{
			me->facing=(byte)Random(8);
			me->mind1=0;
			me->reload=32;
		}

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
		me->dx=Cosine(me->facing*32)*8;
		me->dy=Sine(me->facing*32)*8;
	}
}

void AI_SDZL(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i,x,y;

	if(!goodguy)
		return;	// no point in doing anything without an opponent

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SDZLOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SDZLDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=9 && me->frm<=13 && me->mind1==0)
		{
			// slash for pain
			x=(me->x>>FIXSHIFT)+50;
			y=(me->y>>FIXSHIFT)+10;

			if(FindVictim(me,x,y,100,-FIXAMT*12,FIXAMT*4,25,map,world,me->friendly))
			{
				if(!(MonsterFlags(goodguy->type,goodguy->aiType)&MF_NOMOVE))
				{
					goodguy->dx=-FIXAMT*12;
					goodguy->dy=FIXAMT*2;
				}
				me->mind1=1;	// don't hit him again
			}
		}
		if(me->seq==ANIM_A2 && me->frm==12 && me->frmTimer<64)
		{
			// spew projectiles
			FireExactBullet(me,me->x,me->y+FIXAMT*20,FIXAMT*60,
							-FIXAMT*6,FIXAMT*2,FIXAMT,0,60,2,BLT_GRENADE,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*20,FIXAMT*60,
							-FIXAMT*4,FIXAMT*4,FIXAMT,0,60,2,BLT_GRENADE,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*20,FIXAMT*60,
							0,FIXAMT*5,FIXAMT,0,60,2,BLT_GRENADE,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*20,FIXAMT*60,
							FIXAMT*4,FIXAMT*4,FIXAMT,0,60,2,BLT_GRENADE,me->friendly);
			FireExactBullet(me,me->x,me->y+FIXAMT*20,FIXAMT*60,
							FIXAMT*6,FIXAMT*2,FIXAMT,0,60,2,BLT_GRENADE,me->friendly);
		}
		if(me->seq==ANIM_A3 && (me->frm==7 || me->frm==8))
		{
			ShakeScreen(30);
			MakeSound(SND_SDZLPOUND,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<10;i++)
			{
				x=me->x+((256-Random(513))<<FIXSHIFT);
				y=me->y+((256-Random(513))<<FIXSHIFT);
				FireExactBullet(me,x,y,FIXAMT*80+(Random(40)<<FIXSHIFT),
								0,0,0,0,30,2,BLT_ROCK,me->friendly);
			}
		}

		return;	// can't do nothin' right now
	}

	me->mind1=0;

	FaceGoodguy2(me,goodguy);

	if(me->reload==0 && RangeToTarget(me,goodguy)<256*FIXAMT)
	{
		i=Random(1024);
		if(i<40)
		{
			MakeSound(SND_SDZLSLASH,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A3;	// ground pound!
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->action=ACTION_BUSY;
			me->reload=60;
			return;
		}
		else if(i<100)
		{
			MakeSound(SND_SDZLSPIT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_A2;	// breathe stuff
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->action=ACTION_BUSY;
			me->reload=60;
			return;
		}
		else if(i<180)
		{
			MakeSound(SND_SDZLSLASH,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;	// slash
			me->frmTimer=0;
			me->frm=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->action=ACTION_BUSY;
			me->reload=30;
			return;
		}
	}

	if(me->facing==7 || me->facing<3)
		i=ANIM_MOVE;	// move to the right
	else
		i=ANIM_A1;	// move to the left

	if(me->seq!=i)
	{
		me->seq=i;
		me->frmTimer=0;
		me->frm=0;
		me->frmAdvance=128;
		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*3;
	}
}

void AI_Santa(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SANTAOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SANTADIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==8 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*6,Sine(me->facing*32)*6,16,map,world);
			}
			if(me->mind1)
			{
				me->frm=3;
				me->mind1--;
			}
			else
				me->reload=10;
		}
		if(me->seq==ANIM_DIE && me->frm>5)
			me->frmAdvance=128;

		return;	// can't do nothin' right now
	}

	if(goodguy)
	{
		FaceGoodguy3(me,goodguy);
		if(RangeToTarget(me,goodguy)<(96*FIXAMT) && Random(16)==0 && me->reload==0)
		{
			// smack him!
			MakeSound(SND_SANTAHIT,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->action=ACTION_BUSY;
			me->dx=0;
			me->dy=0;
			me->reload=1;
			me->mind1=2;	// tell him to repeat twice
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
}
