void AI_Villager(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4 && me->hp==0)
		MakeSound(SND_VILLDIE1+(me->aiType>MONS_VILLAGERF),me->x,me->y,SND_CUTOFF,1200);
	
	if(me->action==ACTION_BUSY)
	{
		return;	// can't do nothin' right now
	}

	if(me->reload==0 && Random(64)==0 && me->mind3==0)	// scared girl doesn't yawn
	{
		if(me->aiType==MONS_VILLAGERM)
			MakeSound(SND_VILLAGERSCRATCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_VILLAGERYAWN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=(byte)(30*4+Random(136));
		return;
	}

	if(!(me->mind2--) || (me->mind1>0 && me->mind3==0))	// time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind2=(byte)Random(40)+20;
		me->mind=(byte)Random(4);
		if(me->mind==1)
			me->mind=2;
		if(me->mind3 && me->mind>0)
			me->mind=(byte)Random(4)+3;

	}

	if(me->mind3==0 && RangeToTarget(me,goodguy)<FIXAMT*60)
	{
		me->mind=0;
		FaceGoodguy(me,goodguy);
	}

	me->dx=(Cosine(me->facing*32)*me->mind*FIXAMT)/FIXAMT;
	me->dy=(Sine(me->facing*32)*me->mind*FIXAMT)/FIXAMT;
	if(me->mind==0)
	{
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
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=me->mind*32;
		}
	}
	me->mind1=0;

	if(me->mind3)
	{
		// follow obnoxiously
		FaceGoodguy(me,goodguy);
	}
	
	// villagers have lanterns in dark places
	if(map->flags&MAP_TORCHLIT)
	map->DimTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,10);
}

void AI_Mummy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,z;
	Guy *g;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(me->aiType==MONS_KINGTOOT && me->seq==ANIM_DIE && Random(5)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(me,x,y,0,BLT_EVILFACE,me->friendly);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0 && me->aiType!=MONS_PHARAOHF)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			if(me->aiType!=MONS_KINGTOOT)
			{
				FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
				if(me->aiType==MONS_PHARAOHG)
				{
					FireBullet(me,x,y,(me->facing*32+240)&255,BLT_ENERGY,me->friendly);
					FireBullet(me,x,y,(me->facing*32+16)&255,BLT_ENERGY,me->friendly);
				}
			}
			else
				FireBullet(me,x,y,(me->facing)&255,BLT_BEAM,me->friendly);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm>=3 && me->frm<=5 && me->reload==0 && me->aiType==MONS_PHARAOHF)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			z=me->facing*32-12+Random(25)+256;
			FireBullet(me,x,y,z&255,BLT_ENERGY,me->friendly);
		}
		if(me->seq==ANIM_A2 && me->frm==4 && me->reload==0)
		{
			g=AddGuy(me->x-FIXAMT*50,me->y+FIXAMT*10,0,MONS_RAGMAN+2*(me->aiType==MONS_KINGTOOT),me->friendly);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			g=AddGuy(me->x+FIXAMT*50,me->y+FIXAMT*10,0,MONS_RAGMAN+2*(me->aiType==MONS_KINGTOOT),me->friendly);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				g->type=MONS_NONE;	// vanish if this spot is occupied
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind1==0)
		me->mind1=70;
	else
	{
		me->mind1--;
		if(me->mind1==0)
		{
			if((me->aiType==MONS_PHARAOHG || me->aiType==MONS_KINGTOOT) && Random(4)==0)
			{
				MakeSound(SND_MUMMYSUMMON,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_A2;
				me->frm=0;
				me->frmTimer=0;
				if (me->aiType!=MONS_KINGTOOT)
					me->frmAdvance=128;
				else
					me->frmAdvance=96;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
			}
		}
	}

	if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(8)==0 &&
		map->CheckLOS(me->mapx,me->mapy,4,goodguy->mapx,goodguy->mapy))
	{
		// get him!
		MakeSound(SND_MUMBLECRUSH,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		if(me->aiType==MONS_RAGMAN)
			me->frmAdvance=256;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		return;
	}

	x=64;
	if(me->aiType==MONS_PHARAOHF)
		x=32;
	if(me->aiType!=MONS_RAGMAN && RangeToTarget(me,goodguy)<(360*FIXAMT) && Random(x)==0 &&
		map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// shoot
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

	if(me->aiType==MONS_RAGMAN && me->seq==ANIM_MOVE)
		me->frmAdvance=256;

	WanderAI(me,64,40,3,map,world,goodguy,FIXAMT*3);
}

void AI_EvilTree(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_TREEOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_TREEDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if (me->reload)
	me->reload--;
	
	// initially adjust position
	if(me->mind3==0)
	{
		me->x-=FIXAMT*3;
		me->y+=FIXAMT;
		me->mind3=1;
	}

	if(me->aiType==MONS_TORTREE && me->seq==ANIM_DIE && Random(10)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(me,x,y,0,BLT_EVILFACE,me->friendly);
	}

	if(me->seq==ANIM_DIE)
	{
		for(i=0;i<12;i++)
		{
			BlowSmoke(me->x,me->y,FIXAMT*10,Random(12)*FIXAMT);
		}
	}
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			if(me->aiType==MONS_SWAMPSTUMP)
			{
				// gas attack
				MakeSound(SND_SKELGAS,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,64,BLT_FART,me->friendly);
				FireBullet(me,me->x,me->y,64+16,BLT_FART,me->friendly);
				FireBullet(me,me->x,me->y,64-16,BLT_FART,me->friendly);
				me->reload=30*2;
			}
			else if(me->aiType==MONS_TERRORTREE)
			{
				// summon frog
				MakeSound(SND_TREEFROG,me->x,me->y,SND_CUTOFF,1200);
				x=me->x;
				y=me->y+40*FIXAMT;
				g=AddGuy(x,y,FIXAMT*10,MONS_FROG3,me->friendly);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				me->reload=30*3;
			}
			else
			{
				// summon frog
				MakeSound(SND_TREEFROG,me->x,me->y,SND_CUTOFF,1200);
				x=me->x;
				y=me->y+40*FIXAMT;
				g=AddGuy(x,y,FIXAMT*10,MONS_FROGURT,me->friendly);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				me->reload=30*3;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->aiType==MONS_TERRORTREE)
	{
		// launch frogs when player is very near
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && me->reload==0 && Random(16)==0)
		{
			// shoot
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}
	else if(me->aiType==MONS_TORTREE)
	{
		// launch frogs when player is near
		if(RangeToTarget(me,goodguy)<(400*FIXAMT) && me->reload==0 && Random(16)==0)
		{
			// shoot
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}
	else
	{
		// launch gas at a standard rate
		if(me->reload==0)
		{
			// shoot
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->action=ACTION_BUSY;
			return;
		}
	}
}

void AI_Larry(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;
	
	if(me->mind)
		me->mind--;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LARRYOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_SWAMPDIE,me->x,me->y,SND_CUTOFF,2000);
	}
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,8,map,world);
			}
			if(me->frm==8 && me->frmTimer<128)
			{
				x=me->x+Cosine(me->facing*32)*48;
				y=me->y+Sine(me->facing*32)*48;
				//MakeRingParticle(x,y,0,128,64);
				if(me->AttackCheck(128,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,8,map,world);
				}
				FireBullet(me,me->x,me->y-FIXAMT,0,BLT_SHOCKWAVE,me->friendly);
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1500);
				for(i=0;i<5;i++)
				{
					x=me->x+((256-Random(513))<<FIXSHIFT);
					y=me->y+((256-Random(513))<<FIXSHIFT);
					FireExactBullet(me,x,y,FIXAMT*80+(Random(40)<<FIXSHIFT),
									0,0,0,0,30,2,BLT_ROCK,me->friendly);
				}
				ShakeScreen(15);
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A1 && (me->frm==6 || me->frm==7) && me->frmTimer<128)
		{
			x=me->x+Cosine((me->facing+7+2*(7-me->frm))*32)*20;
			y=me->y+Sine((me->facing+7+2*(7-me->frm))*32)*20;
			MakeSound(SND_LARRYDARTS,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<10;i++)
			{
				FireBullet(me,x,y,(me->facing*32+(me->frm-6)*8+256-48+i*(96/10))&255,BLT_CLAWS,me->friendly);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			}
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-20*FIXAMT+Random(40*FIXAMT);
			y=me->y-20*FIXAMT+Random(40*FIXAMT);
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
	{
		// if goodguy is in sight
		if(RangeToTarget(me,goodguy)<(256*FIXAMT) && Random(12)==0)
		{
			// get him!
			MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
			DoMove2(me,ANIM_ATTACK,128,1,0,0);
			FaceGoodguy2(me,goodguy);
			return;
		}
		if(Random(32)==0)	// fire at any range
		{
			// shoot darts
			MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
			DoMove2(me,ANIM_A1,200,1,0,0);
			FaceGoodguy2(me,goodguy);
			return;
		}
		if(me->mapx==me->mind2 && me->mapy==me->mind3)
		{
			me->mind2=goodguy->mapx;
			me->mind3=goodguy->mapy;
			SelectDestination(me,goodguy,map,0);
		}
	}
	else
	{
		if(me->mapx==me->mind2 && me->mapy==me->mind3)
			SelectDestination(me,goodguy,map,0);
	}

	/*
	if(me->mind1 && (abs(me->dx)>FIXAMT*3 || abs(me->dy)>FIXAMT*3))
	{
		me->mind1=0;
		me->dx=0;
		me->dy=0;
		SelectDestination(me,goodguy,map,1);
	}
	*/

	if(me->mapx>me->mind2)
		me->dx-=FIXAMT;
	else if(me->mapx<me->mind2)
		me->dx+=FIXAMT;
	if(me->mapy>me->mind3)
		me->dy-=FIXAMT;
	else if(me->mapy<me->mind3)
		me->dy+=FIXAMT;
	Dampen(&me->dx,FIXAMT/4);
	Dampen(&me->dy,FIXAMT/4);
	Clamp(&me->dx,FIXAMT*6);
	Clamp(&me->dy,FIXAMT*6);
	if(me->dx<-FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=5;
		else if(me->dy>FIXAMT)
			me->facing=3;
		else
			me->facing=4;
	}
	else if(me->dx>FIXAMT)
	{
		if(me->dy<-FIXAMT)
			me->facing=7;
		else if(me->dy>FIXAMT)
			me->facing=1;
		else
			me->facing=0;
	}
	else
	{
		if(me->dy<-FIXAMT)
			me->facing=6;
		else
			me->facing=2;
	}

	if(me->seq!=ANIM_MOVE)
		DoMove2(me,ANIM_MOVE,180,0,me->dx,me->dy);
}

void AI_Swampdog(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,z,i;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SWAMPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SWAMPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->mind)
		me->mind--;

	if(me->aiType==MONS_SWAMPDOG4 && me->seq==ANIM_DIE && Random(10)==0)
	{
		y=me->y-FIXAMT*20+Random(FIXAMT*41);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(me,x,y,0,BLT_EVILFACE,me->friendly);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A3 && me->frm>=4 && me->reload%2==0)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			
			if (me->aiType==MONS_HOTDOG)
				for(i=0;i<3;i++)
				{
					FireBullet(me,x,y,me->facing*32+(i-1)*8,BLT_FLAME5,me->friendly);
				}
			else if (me->aiType==MONS_SWAMPDOG4)
				for(i=0;i<3;i++)
				{
					FireBullet(me,x,y,me->facing*32+(i-1)*8,BLT_ENERGY,me->friendly);
				}
		}
		if(me->seq==ANIM_A3 && me->frm==6 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;

			if(me->aiType==MONS_SWAMPDOG3 || me->aiType==MONS_SWAMPDOG4)
			{
				if(me->mind3&1)
				{
					z=(me->facing+8)*32-32;
					z+=(me->mind3-1)*8;
					z=z&255;
				}
				else
				{
					z=(me->facing+8)*32+32;
					z-=(me->mind3-1)*8;
					z=z&255;
				}
				if(me->aiType==MONS_SWAMPDOG3)
				{
					FireBullet(me,x,y,me->facing*32,BLT_BIGSHELL,me->friendly);
					me->reload=4;
				}
				else if(me->aiType!=MONS_SWAMPDOG4)
				{
					FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
					me->reload=4;
				}
				if(me->mind3>1)
				{
					// repeat fire
					me->mind3-=2;
					me->frm=4;
				}
			}
			else
			{
				FireBullet(me,x,y,me->facing*32,BLT_ENERGY,me->friendly);
				me->reload=5;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind2==0)	// unalert
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch>0)
		{
			// seen goodguy/got shot - ALERT!!
			me->mind2=1;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
			me->mind=30*5;
			MakeSound(SND_SWAMPALERT,me->x,me->y,SND_CUTOFF,1200);
		}
		else
		{
			if(Random(64)==0)
				me->facing=(me->facing+7+(byte)Random(3))&7;

			if(me->mind1==0)	// standing
			{
				me->dx=0;
				me->dy=0;
				if(me->seq!=ANIM_IDLE)
				{
					me->seq=ANIM_IDLE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=64;
				}
				if(me->mind==0)
				{
					// plod around a bit
					me->mind=(byte)Random(30*4);
					me->mind1=1;
				}
			}
			else	// plodding
			{
				if (me->aiType==MONS_SWAMPDOG4 || me->aiType==MONS_HOTDOG)
				{
					me->dx=Cosine(me->facing*32)*4;
					me->dy=Sine(me->facing*32)*4;
				}
				else if (me->aiType==MONS_SWAMPDOG3 || me->aiType==MONS_SWAMPDOG2)
				{
					me->dx=Cosine(me->facing*32)*3;
					me->dy=Sine(me->facing*32)*3;
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
					me->frmAdvance=64;
				}
				if(me->mind==0)
				{
					// stand around a bit
					me->mind=(byte)Random(30*2);
					me->mind1=0;
				}
			}
		}
	}
	else // ALERT
	{
		if(me->ouch)
			me->mind=30*5;	// force alertness when injured

		if(me->mind==0)
		{
			// time to be bored again
			DoMove2(me,ANIM_A4,64,1,0,0);
			me->mind2=0;
		}
		if(me->seq==ANIM_A5 && me->frm>1)
			me->frm=0;

		if(me->seq==ANIM_IDLE)
		{
			// can't stand still, must charge
			FaceGoodguy(me,goodguy);
			if(me->aiType!=MONS_SWAMPDOG3)
			{
				MakeSound(SND_SWAMPCHARGE,me->x,me->y,SND_CUTOFF,1200);
				DoMove2(me,ANIM_A2,128,0,Cosine(me->facing*32)*5,Sine(me->facing*32)*5);
			}
			else	// Shambling Thing does stand still
				DoMove2(me,ANIM_A5,128,0,0,0);
		}
		if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(8)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			FaceGoodguy(me,goodguy);
			MakeSound(SND_SWAMPJAB,me->x,me->y,SND_CUTOFF,1200);
			DoMove2(me,ANIM_ATTACK,128,1,0,0);
			me->reload=0;
			return;
		}
		if(me->aiType==MONS_SWAMPDOG3)
			x=8;
		else
			x=32;
		if(RangeToTarget(me,goodguy)<(380*FIXAMT) && Random(x)==0 &&
			map->CheckLOS(me->mapx,me->mapy,15,goodguy->mapx,goodguy->mapy))
		{
			// get him!
			FaceGoodguy(me,goodguy);
			MakeSound(SND_SWAMPSHOOT,me->x,me->y,SND_CUTOFF,1200);
			DoMove2(me,ANIM_A3,128,1,0,0);
			me->reload=0;
			me->mind3=4*2+(byte)Random(2);

			return;
		}
	}
	if(me->seq==ANIM_A2)
	{
		// charging
		if(me->AttackCheck(32,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
		{
			goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
		}
	}
}

void AI_LLGhost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_GHOSTOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_GHOSTDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	// banshees glow
	if(me->aiType==MONS_GHOST3)
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
						 (me->y/TILE_HEIGHT)>>FIXSHIFT,8,9);
	// shades make darkness
	else if(me->aiType==MONS_GHOST2)
		map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
						 (me->y/TILE_HEIGHT)>>FIXSHIFT,-8,9);

	if(me->mind3>0)
		me->mind3--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*2,Sine(me->facing*32)*2,5,map,world);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==4 && me->reload==0)
		{
			// banshees teleport randomly
			if(me->aiType==MONS_GHOST3)
			{
				me->x=(me->x-320*FIXAMT)+Random(640*FIXAMT);
				me->y=(me->y-240*FIXAMT)+Random(480*FIXAMT);
				if(me->x<32*FIXAMT)
					me->x=32*FIXAMT;
				if(me->y<32*FIXAMT)
					me->y=32*FIXAMT;
				if(me->x>map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT)
					me->x=map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT;
				if(me->y>map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT)
					me->y=map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT;
			}
			else	// shades teleport to player
			{
				me->x=Random(80*FIXAMT)+40*FIXAMT;
				if(Random(2)==0)
					me->x=-me->x;
				me->x+=goodguy->x;
				me->y=Random(60*FIXAMT)+30*FIXAMT;
				if(Random(2)==0)
					me->y=-me->y;
				me->y+=goodguy->y;
				if(me->x<32*FIXAMT)
					me->x=32*FIXAMT;
				if(me->y<32*FIXAMT)
					me->y=32*FIXAMT;
				if(me->x>map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT)
					me->x=map->width*TILE_WIDTH*FIXAMT-TILE_WIDTH*FIXAMT;
				if(me->y>map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT)
					me->y=map->height*TILE_HEIGHT*FIXAMT-TILE_HEIGHT*FIXAMT;
			}
			for(x=0;x<4;x++)
			{
				map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,16,9);
			}
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && (me->frm==2 || me->frm==8 || me->frm==14))
		{
			MakeSound(SND_GHOSTSPIN,me->x,me->y,SND_CUTOFF,1200);
			if(me->frm==14)
			{
				me->dx=Cosine(me->facing*32)*6;
				me->dy=Sine(me->facing*32)*6;
			}
		}
		if(me->seq==ANIM_A2 && me->frm==18)
			MakeSound(SND_GHOSTPUNCH,me->x,me->y,SND_CUTOFF,1200);
		if(me->seq==ANIM_A2 && me->frm>=17 && me->frm<=25)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(24,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,10,map,world);
			}
		}
		if(me->seq==ANIM_A2 && me->frm>21)
		{
			Dampen(&me->dx,FIXAMT/4);
			Dampen(&me->dy,FIXAMT/4);
		}
		if(me->seq==ANIM_A3 && me->frm==2 && me->reload==0)
		{
			// scream
			me->reload=10;

			//MakeColdRingParticle(me->x,me->y,0,180);
			if(RangeToTarget(me,goodguy)<200*FIXAMT)
			{
				goodguy->dx=0;
				goodguy->dy=0;
				MakeSound(SND_FREEZE,x*FIXAMT,y*FIXAMT,SND_CUTOFF,1200);
			}
		}
		return;	// can't do nothin' right now
	}

	// punch
	// Shades only use super punches if player is stone
	if(RangeToTarget(me,goodguy)<(88*FIXAMT) && Random(6)==0 &&
		(me->aiType!=MONS_GHOST2))
	{
		// get him!
		MakeSound(SND_GHOSTPUNCH,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_ATTACK,164,1,0,0);
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	// Super punch!!
	if(me->aiType==MONS_GHOST2 && RangeToTarget(me,goodguy)<(160*FIXAMT) &&
		Random(32)==0)
	{
		DoMove2(me,ANIM_A2,256,1,0,0);
		me->reload=0;
		FaceGoodguy(me,goodguy);
		return;
	}
	// teleport if non-Ghost
	if(RangeToTarget(me,goodguy)>(320*FIXAMT) && Random(32)==0)
	{
		MakeSound(SND_GHOSTTPORT,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A1,128,1,0,0);
		me->reload=0;
		return;
	}
	// Banshees scream
	if(me->aiType==MONS_GHOST3 && RangeToTarget(me,goodguy)<256*FIXAMT && me->mind3==0 && Random(128)==0)
	{
		// scream
		me->mind3=30*5;
		MakeSound(SND_GHOSTYELL,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A3,128,1,0,0);
		me->reload=0;
		return;
	}
	WanderAI(me,256,20,3,map,world,goodguy,FIXAMT*3);
}

void AI_StickWitch(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i, x, y;
	
	if(me->mind3)
	me->mind3--;
	else
	{
		me->mind2=(byte)Random(5);
		me->mind3=30*2+(byte)Random(30*5);
		FaceGoodguy2(me,goodguy);
		DoMove2(me,ANIM_ATTACK,128,1,me->dx,me->dy);
	}
	
	if(me->mind1)
	me->mind1--;
	else // time to get a new direction
	{
		me->facing=(byte)Random(8);
		me->mind1=(byte)Random(30*3)+1;
	}

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_STICKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_STICKDIE,me->x,me->y,SND_CUTOFF,1200);
		if(me->hp>0)
		{
			me->seq=ANIM_A1;	//sad
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
		if(me->seq==ANIM_ATTACK && me->reload==0)
		{
			// shoot
			switch(me->mind2)
			{
				case 0:	// blast spell
					//MakeSound(SND_CATKICK,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;;
					FireBullet(me,x,y,me->mind2,BLT_BIGYELLOW,me->friendly);
					me->reload=2;
					break;
				case 1:	// gas blast
					x=me->x+Cosine(me->facing*32)*72;
					y=me->y+Sine(me->facing*32)*64;
					i=(me->facing*32-64)&255;
					x+=Cosine(i)*48;
					y+=Sine(i)*32;
					i=(me->facing*32-16+Random(33))&255;
					FireExactBullet(me,x,y,FIXAMT*64,Cosine(i)*12,Sine(i)*12,0,0,16,i,BLT_SPORE,me->friendly);
					MakeSound(SND_MUSHSPORES,me->x,me->y,SND_CUTOFF,600);
					me->reload=1;
					break;
				case 2:	// flames
					//MakeSound(SND_STEAM,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					me->reload=1;
					FireBullet(me,x,y,me->mind2,BLT_FLAME5,me->friendly);
					map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,16,8);
					break;
				case 3:	// wind missiles
					MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					me->reload=3;
					FireBullet(me,me->x,me->y,me->facing,BLT_WIND,me->friendly);
					break;
				case 4: // earth spikes
					//MakeSound(SND_ZOMBIESTOMP,me->x,me->y,SND_CUTOFF,1200);
					x=me->x+Cosine(me->mind*32)*16;
					y=me->y+Sine(me->mind*32)*16;
					FireBullet(me,me->x,me->y,(me->facing),BLT_DIRTSPIKE,me->friendly);
					me->reload=3;
					break;
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)	// waiting for Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch)
			{
				MakeSound(SND_STICKTAUNT,me->x,me->y,SND_CUTOFF,1500);
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;	// muscles
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

			if(me->seq!=ANIM_ATTACK)
			{
				me->dx=Cosine(me->facing*32)*6;
				me->dy=Sine(me->facing*32)*6;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*3;
				me->dy=Sine(me->facing*32)*3;
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

void AI_Frankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			if(me->mind<2)
				MakeSound(SND_FRANKOUCH,me->x,me->y,SND_CUTOFF,1200);
			else
				MakeSound(SND_FRANKOUCH2,me->x,me->y,SND_CUTOFF,1200);

			if(me->hp<me->maxHP*0.5 && me->mind==0)
			{
				me->mind=1;
				FireBullet(me,me->x,me->y,0,BLT_MEGABOOM,me->friendly);
				DoMove2(me,ANIM_A1,128,0,0,0);
				ShakeScreen(5);
			}
			if(me->hp<me->maxHP*0.25 && me->mind==1)
			{
				me->mind=2;
				FireBullet(me,me->x,me->y,0,BLT_MEGABOOM,me->friendly);
				DoMove2(me,ANIM_A1,128,0,0,0);
				ShakeScreen(5);
				g=AddGuy(me->x-FIXAMT*250,me->y,0,MONS_HAND,me->friendly);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
				g=AddGuy(me->x+FIXAMT*250,me->y,0,MONS_HAND,me->friendly);
				if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
					g->type=MONS_NONE;	// vanish if this spot is occupied
			}
		}
		else
		{
			MakeSound(SND_FRANKDIE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	if(me->mind2==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			me->mind2=1;
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ShakeScreen(120);
			FireBullet(me,me->x-60*FIXAMT+Random(120*FIXAMT),me->y-60*FIXAMT+Random(120*FIXAMT),0,BLT_MEGABOOM,me->friendly);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frmTimer<128)
				MakeSound(SND_FRANKHEAD,me->x,me->y,SND_CUTOFF,400);
			if(me->frm<16 && me->frmTimer<128)
			{
				x=(me->x+Cosine(me->frm*16)*80);
				y=(me->y+Sine(me->frm*16)*60);
				FireExactBullet(me,x,y,FIXAMT*70,Cosine(me->frm*16)*8,Sine(me->frm*16)*8,
					-FIXAMT*2,0,60,me->frm*16,BLT_ENERGY,me->friendly);
				x=(me->x+Cosine((me->frm+8)*16)*80);
				y=(me->y+Sine((me->frm+8)*16)*60);
				FireExactBullet(me,x,y,FIXAMT*70,Cosine((me->frm+8)*16)*8,Sine((me->frm+8)*16)*8,
					-FIXAMT*2,0,60,(me->frm+8)*16,BLT_ENERGY,me->friendly);

			}
		}
		if(me->seq==ANIM_A3)
		{
			if(me->frm>4 && me->frm<20)
			{
				me->dx+=Cosine(me->facing*32);
				me->dy+=Sine(me->facing*32);
				Clamp(&me->dx,6*FIXAMT);
				Clamp(&me->dy,6*FIXAMT);

				x=(me->x+Cosine((me->frm-4)*16)*200)/FIXAMT;
				y=(me->y+Sine((me->frm-4)*16)*160)/FIXAMT;
				if(me->AttackCheck(50,x,y,goodguy))
					goodguy->GetShot(me,goodguy,Cosine((me->frm-4)*16)*8,Sine((me->frm-4)*16)*8,3,map,world);
				x=(me->x+Cosine((me->frm+4)*16)*200)/FIXAMT;
				y=(me->y+Sine((me->frm+4)*16)*160)/FIXAMT;
				if(me->AttackCheck(50,x,y,goodguy))
					goodguy->GetShot(me,goodguy,Cosine((me->frm+4)*16)*8,Sine((me->frm+4)*16)*8,3,map,world);
			}
			else if(me->frm>=20)
			{
				me->dx=(me->dx*3)/4;
				me->dy=(me->dy*3)/4;
				if(me->frm==21)
				DoMove2(me,ANIM_A1,128,0,0,0);
			}
		}
		if(me->seq==ANIM_A4 && me->frm==4 && me->frmTimer<128)
		{
			x=me->x-120*FIXAMT;
			y=me->y-20*FIXAMT;
			FireBullet(me,x,y,128,BLT_MISSILE,me->friendly);
			FireBullet(me,x,y-30*FIXAMT,148,BLT_MISSILE,me->friendly);
			x=me->x+120*FIXAMT;
			y=me->y-20*FIXAMT;
			FireBullet(me,x,y,0,BLT_MISSILE,me->friendly);
			FireBullet(me,x,y-30*FIXAMT,236,BLT_MISSILE,me->friendly);
			//MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
		}
		if(me->seq==ANIM_A4 && me->frm==10)
		{
			DoMove2(me,ANIM_A2,128,0,0,0);
		}
		return;
	}
	if(RangeToTarget(me,goodguy)<96*FIXAMT)
	{
		// 'acidic'
		goodguy->GetShot(me,goodguy,0,0,5,map,world);
	}

	FaceGoodguy2(me,goodguy);
	me->dx+=Cosine(me->facing*32)/2;
	me->dy+=Sine(me->facing*32)/2;
	
	Clamp(&me->dx,FIXAMT*5);
	Clamp(&me->dy,FIXAMT*5);

	if(me->mind==0)	// have head
	{
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove2(me,ANIM_ATTACK,128,1,me->dx,me->dy);
			}
		}
	}
	if(me->mind==1)	// lost head already
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A1 && me->frm>1))
		{
			DoMove2(me,ANIM_A1,128,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				MakeSound(SND_FRANKARMS,me->x,me->y,SND_CUTOFF,1200);
				DoMove2(me,ANIM_A3,256,1,me->dx,me->dy);
				FaceGoodguy(me,goodguy);
			}
			if(me->mind1%15==0)
			{
				MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,(byte)Random(256),BLT_GRENADE,me->friendly);
			}
		}
	}
	if(me->mind==2)	// lost body too
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A2 && me->frm>1))
		{
			DoMove2(me,ANIM_A2,128,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*2;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove2(me,ANIM_A4,128,1,me->dx,me->dy);
			}
		}
	}
}

void AI_Hand(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	if(me->reload)
	me->reload--;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_PUMPKOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_PUMPKDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==7 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*64;
			y=me->y+Sine(me->facing*32)*64;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,10,map,world);
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<(128*FIXAMT) && Random(20)==0)
	{
		// get him!
		MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
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

	WanderAI(me,512,10,10,map,world,goodguy,FIXAMT*3);
}

void AI_WindElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;
	
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
		if(me->seq==ANIM_ATTACK && me->frm<6)
		{
			me->dx=Cosine(me->facing*32)*(6+me->frm*2);
			me->dy=Sine(me->facing*32)*(6+me->frm*2);
		}
		else if(me->seq==ANIM_ATTACK)
		{
			me->dx/=2;
			me->dy/=2;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=6)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*16,Sine(me->facing*32)*16,8,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frmTimer<128)
		{
			x=me->x-400*FIXAMT+Random(800*FIXAMT);
			y=me->y-300*FIXAMT+Random(600*FIXAMT);
			g=AddGuy(x,y,0,MONS_GHOST,me->friendly);
			if(g && !g->CanWalk(g,g->x,g->y,map,world))
				g->type=MONS_NONE;
		}
		if(me->seq==ANIM_A2 && me->frm==2 && me->reload==0)
		{
			FireBullet(me,me->x,me->y,me->facing-1+8,BLT_WIND,me->friendly);
			FireBullet(me,me->x,me->y,me->facing,BLT_WIND,me->friendly);
			FireBullet(me,me->x,me->y,me->facing+1,BLT_WIND,me->friendly);
			me->reload=4;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			//explode
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		// summon hordes of ghosts
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A1,128,1,0,0);
		return;
	}

	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(20)==0)
	{
		// charge at him
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_ATTACK,128,1,0,0);
		return;
	}
	if(Random(50)==0)
	{
		// fire wind missiles
		MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A2,128,1,0,0);
		return;
	}

	WanderAI(me,1024,2,1,map,world,goodguy,FIXAMT*6);
}

void AI_EarthElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ELDEROUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_ELDERDIE,me->x,me->y,SND_CUTOFF,2000);
	}
	
	if(me->reload)
	me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{	//dirt spike
			FireBullet(me,me->x,me->y,(me->facing),BLT_DIRTSPIKE,me->friendly);
			FireBullet(me,me->x,me->y,(me->facing-1+8),BLT_DIRTSPIKE,me->friendly);
			FireBullet(me,me->x,me->y,(me->facing+1),BLT_DIRTSPIKE,me->friendly);
			me->reload=5;
		}

		if(me->seq==ANIM_A1 && me->frm==3 && me->reload==0)
		{
			for(x=0;x<6;x++)
				FireBullet(me,me->x,me->y,(byte)Random(256),BLT_BIGYELLOW,me->friendly);
			me->reload=5;
		}
		if(me->seq==ANIM_A2 && me->frm<9)
		{
			x=me->x+Cosine(me->facing*32)*me->frm*48;
			y=me->y+Sine(me->facing*32)*me->frm*32;
			if(me->frm==8 && me->reload==0)
			{
				me->reload=3;
				FireBullet(me,x,y,0,BLT_MEGABOOM,me->friendly);
			}
			else if(me->frmTimer<128)
			FireBullet(me,x,y,0,BLT_BOOM,me->friendly);
		}
		if(me->seq==ANIM_DIE)
		{
			MakeSound(SND_ELDERDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
			// doesn't explode
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3)
		me->mind3-=2;
	else
		me->mind3=30*8;

	if(me->mind3==30*5 || me->mind3==30*3)
	{
		// launch grenades
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A1,128,1,0,0);
		return;
	}

	if(me->mind3==30*8 || me->mind3==30*6 || me->mind3==30*4)
	{
		// shoot earth spikes
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_ATTACK,128,1,0,0);
		return;
	}

	if(me->mind3==30*7)
	{
		// boom!
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A2,128,1,0,0);
		return;
	}


	FaceGoodguy(me,goodguy);

	WanderAI(me,1024,2,1,map,world,goodguy,FIXAMT*2);
}

void AI_WaterElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,f;
	Guy *g;

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
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=6 && me->reload==0)
		{
			if(Random(3))
			FireBullet(me,me->x,me->y,me->facing*32+256-8+(byte)Random(16),BLT_FREEZE,me->friendly);
			else
			{FireBullet(me,me->x,me->y,me->facing*32+256-8+(byte)Random(16),BLT_SHARK,me->friendly);
			FireBullet(me,me->x,me->y,me->facing*32+256-8+(byte)Random(16),BLT_SHARK,me->friendly);}
			me->reload=2;
		}
		if(me->seq==ANIM_A2 && me->frm>2 && me->frm<10 && me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			f=me->facing*32-16+Random(33);
			FireBullet(me,x,y,f,BLT_BUBBLE,me->friendly);
			me->reload=2;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			//explosion
		}
		return;	// can't do nothin' right now
	}

	if(Random(40)==0)
	{
		// fire ice shards
		MakeSound(SND_ELDERCHARGE,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_ATTACK,192,1,me->dx/2,me->dx/2);
		return;
	}
	if(Random(30)==0)
	{
		// fire bubbles
		MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A2,192,1,me->dx/2,me->dx/2);
		return;
	}
	
	FaceGoodguy2(me,goodguy);
	me->dx+=Cosine(me->facing*32)*3/2;
	me->dy+=Sine(me->facing*32)*3/2;
	Clamp(&me->dx,FIXAMT*8);
	Clamp(&me->dy,FIXAMT*8);
}

void AI_FireElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ELDEROUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_ELDERDIE,me->x,me->y,SND_CUTOFF,2000);
	}
	
	if(me->reload)
	me->reload--;

	map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,16,8);

	if(me->ouch && me->action==ACTION_BUSY && me->seq!=ANIM_DIE && me->seq!=ANIM_ATTACK && Random(10)==0)
	{
		// run away from the water
		DoMove2(me,ANIM_ATTACK,128,1,0,0);
		me->facing=(byte)Random(8);
		return;
	}
	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*8;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm<6)
		{
			me->dx=Cosine(me->facing*32)*(6+me->frm*2);
			me->dy=Sine(me->facing*32)*(6+me->frm*2);
		}
		else if(me->seq==ANIM_ATTACK)
		{
			me->dx/=2;
			me->dy/=2;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=3 && me->frm<=6)
		{
			x=me->x+Cosine(me->facing*32)*30;
			y=me->y+Sine(me->facing*32)*30;
			if(me->AttackCheck(32,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*16,Sine(me->facing*32)*16,8,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==1)
			me->mind2=me->facing*32;
		if(me->seq==ANIM_A1 && me->frm>0 && me->frm<22)
		{
			me->mind2+=8;
			me->facing=me->mind2/32;
			x=me->x+20*Cosine(me->mind2);
			y=me->y+20*Sine(me->mind2);
			FireBullet(me,x,y,me->mind2,BLT_FLAME5,me->friendly);
		}

		if(me->seq==ANIM_A2 && me->frm>4 && me->frm<9 && me->frmTimer<128)
		{
			x=me->x+Cosine(me->facing*32)*(me->frm-4)*48;
			y=me->y+Sine(me->facing*32)*(me->frm-4)*32;
			if(me->frm==8 && me->reload==0)
			{
				me->reload=3;
				FireBullet(me,x,y,0,BLT_MEGABOOM,me->friendly);
			}
			else if(me->frmTimer<128)
				FireBullet(me,x,y,0,BLT_BOOM,me->friendly);
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			//explode
		}
		return;	// can't do nothin' right now
	}

	x=me->x-40*FIXAMT+Random(80*FIXAMT);
	y=me->y-30*FIXAMT+Random(60*FIXAMT);
	FireBullet(me,x,y,me->facing,BLT_FLAME,me->friendly);

	if(me->mind3==0 || me->mind3==30*4)
	{
		// unleash flame circle
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A1,128,1,0,0);
		return;
	}

	if(me->mind3==30*2 || me->mind3==30*6)
	{
		// fire explosion stream
		MakeSound(SND_ELDERSHOOT,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A2,128,1,0,0);
		return;
	}

	WanderAI(me,1024,2,1,map,world,goodguy,FIXAMT*6);
}

void AI_SummonElder(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,a,b;
	Guy *g;
	
	if(me->seq==ANIM_DIE && Random(5)==0)
	{
		y=me->y-FIXAMT*10+Random(FIXAMT*11);
		x=me->x-32*FIXAMT+Random(65*FIXAMT);
		FireBullet(me,x,y,0,BLT_EVILFACE,me->friendly);
	}
	
	if(me->reload)
	me->reload--;

	map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,(me->y/TILE_HEIGHT)>>FIXSHIFT,8,8);

	if(me->mind3)
		me->mind3--;
	else
		me->mind3=30*4;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_A1 && me->frm>0 && me->frm<22 && me->reload==0)
		{
			a=Random(256);
			b=Random(60)+20;
			x=me->x+b*Cosine(a);
			y=me->y+b*Sine(a);
			
			if (me->mind2<5)
			g=AddGuy(x,y,FIXAMT*40,MONS_VAMPLORD,me->friendly);
			else if (me->mind2<5+15)
			g=AddGuy(x,y,FIXAMT*40,MONS_SPIKEY,me->friendly);
			else if (me->mind2<5+15+25)
			g=AddGuy(x,y,FIXAMT*40,MONS_DRACULITE,me->friendly);
			else
			g=AddGuy(x,y,FIXAMT*40,MONS_VAMPIRE,me->friendly);
		
			if(g && !g->CanWalk(g,g->x,g->y,map,world))
				g->type=MONS_NONE;
			else
			{
				MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,600);
				map->BrightTorch((me->x/TILE_WIDTH)>>FIXSHIFT,
							 (me->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			}
			me->reload=(me->mind2+1)*4;
		}

		if(me->seq==ANIM_DIE)
		{
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			//explode
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0 || me->mind3==30*4)
	{
		// summon minions
		MakeSound(SND_ELDERSUMMON,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A1,128,1,0,0);
		me->mind2=(byte)Random(100);
		return;
	}

	WanderAI(me,1024,2,1,map,world,goodguy,FIXAMT*6);
}

//evilizer
void AI_Evilizer(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int summonTab[]={MONS_CRAZYBONE,MONS_MUTANT,MONS_CREEPAZOID,MONS_MUMBLE2,MONS_ALIEN,MONS_REDBONE,MONS_WHATSIT,MONS_VAMPIRE,MONS_BAT,MONS_NIGHTVAMP,MONS_EYEGUY,MONS_STAREYBAT,MONS_MADMAN,MONS_WACKO,MONS_GHOST,MONS_SHOCKGHOST,MONS_MUMBLE,MONS_WOLF,MONS_WOLF2,MONS_BOMBIE}; //20
	int summonTab2[]={MONS_SUPERZOMBIE,MONS_BIGBAT,MONS_TOWER,MONS_MUSH,MONS_STICKMAN,MONS_MAMASPDR,MONS_MINIFRANK,MONS_EVILCLONE,MONS_CRAZYPANTS,}; //9

	int x,y;
	Guy *g;
	
	if (me->ouch == 4)
	{
		if (me->hp > 0)
			MakeSound(SND_SDZLOUCH, me->x, me->y, SND_CUTOFF, 1200);
		else
		{MakeSound(SND_ELDERDIE, me->x, me->y, SND_CUTOFF, 1200);
		KillKids(me);}
	}
	
	if (me->mind4 > 0 && me->mind == 1)
	{
		me->mind4--;
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
	}

	// mind=whether uncovered or not
	if(me->mind==0)
		me->facing=0;
	else
		me->facing=16;
	
	if(me->mind==0 && me->hp < me->maxHP*0.5)
	{
		me->mind = 1;
		me->mind4 = 40;
	}
	
	// mind1=time to shoot an evilface off
	if(me->mind1==0)
	{
		y=me->y-FIXAMT;
		x=me->x-140*FIXAMT+Random(281*FIXAMT);
		FireBullet(me,x,y,0,BLT_EVILFACE,me->friendly);
		me->mind1=12;
		MakeSound(SND_EVILIZER,me->x,me->y,SND_CUTOFF,300);
	}
	else
	{
		me->mind1--;
		if(!(me->mind1&3))
		{
			y=me->y-FIXAMT;
			x=me->x-140*FIXAMT+Random(281*FIXAMT);
			FireBullet(me,x,y,0,BLT_EVILFACE,-me->friendly);
		}
	}
	
	if(me->reload)
		me->reload--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,2);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
		}
	}

	// mind2=grenade timer (faster as it gets hurt more)
	if(me->mind2==0)
		me->mind2=30+me->hp/80;
	else
	{
		me->mind2--;
		if(me->mind2==0)
		{
			x=180*FIXAMT;
			y=180*FIXAMT;
			FireBullet(me,me->x-x,me->y,64+(byte)Random(128),BLT_REDNADE,me->friendly);
			FireBullet(me,me->x+x,me->y,192+(byte)Random(128),BLT_REDNADE,me->friendly);
			FireBullet(me,me->x,me->y-y,128+(byte)Random(128),BLT_REDNADE,me->friendly);
			FireBullet(me,me->x,me->y+y,(byte)Random(128),BLT_REDNADE,me->friendly);
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	// mind3=missile timer (only works once exposed)
	if(me->mind==1)
	{
		if(me->mind3==0)
			me->mind3=60+me->hp/60;
		else
		{
			me->mind3--;
			if(me->mind3==0)
			{
				x=100*FIXAMT;
				y=40*FIXAMT;
				FireBullet(me,me->x-x,me->y-y,160,BLT_MISSILE,me->friendly);
				FireBullet(me,me->x+x,me->y-y,224,BLT_MISSILE,me->friendly);
				FireBullet(me,me->x-x,me->y+y,96,BLT_MISSILE,me->friendly);
				FireBullet(me,me->x+x,me->y+y,32,BLT_MISSILE,me->friendly);
				MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
			}
		}
	}
	
	// reload=summon super duper monsters, only when exposed
		if (me->reload == 30 && me->mind==1 && Random(2))
			{
				MakeSound(SND_TELEPORT,me->x,me->y,SND_CUTOFF,1200);
				x=((Random(map->width-6)+3)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				y=((Random(map->height-6)+3)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*80,summonTab[Random(20)],me->friendly);
				map->BrightTorch((g->x/TILE_WIDTH)>>FIXSHIFT,
							 (g->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
			}
		else if(me->reload==0)
			{
				x=((Random(map->width-6)+3)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				y=((Random(map->height-6)+3)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				if(Random(3) && me->mind == 1)
				{g=AddGuy(x,y,FIXAMT*40,summonTab2[Random(20)],me->friendly);
				me->reload=60*Random(6);}
				else
				{g=AddGuy(x,y,FIXAMT*40,summonTab[Random(9)],me->friendly);
				me->reload=30*Random(3);}
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
					g->type=MONS_NONE;
				else
				{map->BrightTorch((g->x/TILE_WIDTH)>>FIXSHIFT,(g->y/TILE_HEIGHT)>>FIXSHIFT,18,8);
				MakeSound(SND_TELEPORT,g->x,g->y,SND_CUTOFF,1200);}
			}
}

void AI_EvilPump(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;

	if(me->reload)
		me->reload--;

	FaceGoodguy3(me,goodguy);

	if(me->ouch==4)
		MakeSound(SND_GLASSHIT,me->x,me->y,SND_CUTOFF,1200);
	
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && (me->frm==3 || me->frm==5 || me->frm==7) && me->reload==0)
		{
			MakeSound(SND_MUMMYSHOOT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*48;
			y=me->y+Sine(me->facing*32)*48;
			FireBullet(me,x,y,(byte)(me->facing*32+240+Random(33)),BLT_BIGSHELL,me->friendly);
			me->reload=3;
			if(me->frm==7)
				me->reload=30*3;
		}
		if(me->seq==ANIM_DIE)
		{
			if(me->frm==0)
				me->reload=0;
			x=me->x-20*FIXAMT+Random(40*FIXAMT);
			y=me->y-20*FIXAMT+Random(40*FIXAMT);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			if(me->frm==4 && me->reload==0)
			{
				me->reload=10;
			}
		}
		return;
	}

	if(me->reload==0)
	{
		DoMove2(me,ANIM_ATTACK,128,1,0,0);
	}
}

void AI_Bonkula(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,a,i;
	Guy *g;
	dword d;
	
	if (me->reload)
	me->reload--;

	if(map->GetTile(me->mapx,me->mapy)->light>0 && me->ouch==0 && me->hp>0)
	{
		d=monsType[me->aiType].flags;
		monsType[me->aiType].flags=0;
		me->GetShot(me,me,0,0,map->GetTile(me->mapx,me->mapy)->light,map,world);
		monsType[me->aiType].flags=d;
		BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
		BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
		BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
		BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
		BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
	}
	
	if(me->reload)
		me->reload--;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_BONKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BONKDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
	}
	
	if(me->ouch==4)
	{
		x=me->x-40*FIXAMT+Random(80*FIXAMT);
		y=me->y-30*FIXAMT+Random(60*FIXAMT);
		FireBullet(me,x,y,0,BLT_BADSITFLAME,me->friendly);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==8 && me->reload==0)
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			ShakeScreen(5);
			x=me->x+Cosine(me->facing*32)*60;
			y=me->y+Sine(me->facing*32)*60;
			MakeRingParticle(x,y,0,6*15,10*15);
			FindVictim(me,x>>FIXSHIFT,y>>FIXSHIFT,6*15,Cosine(me->facing*32)*2,
							   Sine(me->facing*32)*2,24,map,world,me->friendly);
			me->reload=5;
		}

		if(me->seq==ANIM_DIE)
		{
			me->dx=0;
			me->dy=0;
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(me,x,y,0,BLT_BADSITFLAME,me->friendly);
		}
		if(me->seq==ANIM_A1)
		{
			me->facing++;
			if(me->facing>7)
				me->facing=0;
			if(Random(30)==0)
			{
				x=me->x-300*FIXAMT+Random(600)*FIXAMT;
				y=me->y-300*FIXAMT+Random(600)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*50,MONS_VAMPIRE,me->friendly);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		// begin spinning
		DoMove2(me,ANIM_A1,256,1,0,0);
		me->mind3=60;
	}
	else
	{
		if(me->mind3==60)
		{
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			me->dx+=-FIXAMT*2+Random(4*FIXAMT);
			me->dy+=-FIXAMT*2+Random(4*FIXAMT);
		}
		me->mind3--;
		if(RangeToTarget(me,goodguy)<200*FIXAMT)
		{
			MakeSound(SND_BONKSWING,me->x,me->y,SND_CUTOFF,2000);
			DoMove2(me,ANIM_ATTACK,256,1,0,0);
			me->mind3=0;
		}
	}
}

void AI_Polterguy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->dx==0 && me->dy==0)
	{
		me->dx=Cosine(me->facing)*6;
		me->dy=Sine(me->facing)*6;
	}

	if(me->mind1&1)
		me->dx=-me->dx;
	if(me->mind1&2)
		me->dy=-me->dy;
	me->mind1=0;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_POLTERDIE,me->x,me->y,SND_CUTOFF,2000);
		else
			MakeSound(SND_POLTEROUCH,me->x,me->y,SND_CUTOFF,2000);
	}
	
	if(me->mind==0)
	{
		DoMove2(me,ANIM_MOVE,128,1,0,0);
		me->mind=1;
		for(i=0;i<15;i++)
			LaunchJunk(map,me);
		me->reload=60;
		me->mind2=120+15;
		me->facing=(byte)(Random(4)+1)*32;
		me->dx=Cosine(me->facing)*6;
		me->dy=Sine(me->facing)*6;
	}

	if(me->mind2==0 && me->seq!=ANIM_ATTACK)
	{
		DoMove2(me,ANIM_ATTACK,128,1,0,0);
		MakeSound(SND_GHOSTYELL,me->x,me->y,SND_CUTOFF,1200);
	}
	else
		me->mind2--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			LaunchJunk(map,me);
			LaunchJunk(map,me);
			me->mind2=30*2;
			me->reload=30;

			x=me->x-300*FIXAMT+Random(600)*FIXAMT;
			y=me->y-200*FIXAMT+Random(500)*FIXAMT;
			g=AddGuy(x,y,FIXAMT*50,MONS_GHOST,me->friendly);
			if(g && !g->CanWalk(g,g->x,g->y,map,world))
			{
				g->type=MONS_NONE;
			}
			else if(g)
			{
				DoMove2(g,ANIM_A1,128,1,0,0);
				g->frm=4;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==15 && me->frmTimer<128)
		{
			Sneeze();
			MakeSound(SND_SPHINXSNEEZE,me->x,me->y,SND_CUTOFF,1200);
		}
		return;
	}

	if(FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,55,0,0,1,map,world,me->friendly))
	{
		PoisonVictim(GetLastGuyHit(),60);
	}
	
	if(me->seq!=ANIM_IDLE)
	{
		me->seq=ANIM_IDLE;
		me->frm=0;
		me->frmTimer=0;
	}
}

void AI_Ghastly(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int i;
	int x,y;
	Guy *g;
	
	if(me->reload)
	me->reload--;

	if(me->mind==0 && RangeToTarget(me,goodguy)<256*FIXAMT)
	{
		me->mind=1;
		me->reload=60;
		me->mind3=120+15;
		me->facing=(byte)Random(256);
		me->dx=Cosine(me->facing)*2;
		me->dy=Sine(me->facing)*2;
	}

	if(me->mind1)
	{
		if(me->mind1&1)
			me->dx=-me->dx;
		if(me->mind1&2)
			me->dy=-me->dy;

		if(Random(20)==0)
		{
			me->facing=(byte)Random(256);
			me->dx=Cosine(me->facing)*2;
			me->dy=Sine(me->facing)*2;
		}
		me->mind1=0;
	}

	if(Random(4)==0)
		FireBullet(me,me->x,me->y,(byte)Random(256),BLT_SPORE,me->friendly);

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0)
		{
			me->reload=30*4;

			for(i=0;i<3;i++)
			{
				x=me->x-30*FIXAMT+Random(61)*FIXAMT;
				y=me->y-20*FIXAMT+Random(51)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*30,MONS_GASBAT,me->friendly);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
			}
		}
		if(me->seq==ANIM_A1 && me->frm==15 && me->frmTimer<128)
		{
			for(i=0;i<16;i++)
				FireBullet(me,me->x,me->y,i*16,BLT_SPORE,me->friendly);
		}
		return;	// can't do nothin' right now
	}

	if(me->reload==0)
	{
		MakeSound(SND_POLTERGET,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_ATTACK,128,1,me->dx,me->dy);
	}
	if(Random(50)==0)
	{
		me->facing=(byte)Random(256);
		me->dx=Cosine(me->facing)*2;
		me->dy=Sine(me->facing)*2;
	}
}

void AI_Junk(Guy *me,Map *map,world_t *world,Guy *goodguy)
{;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles(PART_DIRT,me->x,me->y,me->z,16);
		}
		return;
	}
	if(me->parent==NULL || me->parent->hp==0)
	{
		me->hp=0;
		DoMove2(me,ANIM_DIE,128,1,0,0);
	}

	if(RangeToTarget(me,goodguy)<20*FIXAMT)
	{
		goodguy->GetShot(me,goodguy,me->dx,me->dy,2,map,world);
	}

	me->frmAdvance=180;
	FaceGoodguy(me,me->parent);
	me->dx+=Cosine(me->facing*32)/2;
	me->dy+=Sine(me->facing*32)/2;
	// circling Polterguy
	if(me->mind==0)
	{
		if(me->x<me->parent->x)
		{
			// on left side
			if(me->y<me->parent->y)
			{
				// upper left
				me->dy+=FIXAMT;
			}
			else
				// lower left
				me->dx+=FIXAMT;
		}
		else
		{
			// on right side
			if(me->y<me->parent->y)
			{
				// upper left
				me->dx-=FIXAMT;
			}
			else
				// lower left
				me->dy-=FIXAMT;
		}
		Dampen(&me->dx,FIXAMT/4);
		Dampen(&me->dy,FIXAMT/4);
		Clamp(&me->dx,FIXAMT*11);
		Clamp(&me->dy,FIXAMT*11);
	}
	else
	{
		// flying outward
		me->mind1--;
		if(me->mind1==0)
			me->mind=0;
	}
}

void AI_MiniFrankenjulie(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			MakeSound(SND_FRANKOUCH2,me->x,me->y,SND_CUTOFF,1200);

			if(me->hp < (me->maxHP*0.66) && me->mind==0)
			{
				me->mind=1;
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				DoMove2(me,ANIM_A1,256,0,0,0);
			}
			if(me->hp < (me->maxHP*0.33) && me->mind==1)
			{
				me->mind=2;
				x=me->x>>FIXSHIFT;
				y=me->y>>FIXSHIFT;
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				DoMove2(me,ANIM_A1,256,0,0,0);
			}
		}
		else
		{
			MakeSound(SND_FRANKDIE,me->x,me->y,SND_CUTOFF,1200);
		}
	}

	if(me->mind2==0)
	{
		if(RangeToTarget(me,goodguy)<256*FIXAMT || me->ouch)
		{
			me->mind2=1;
		}
		return;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		if(me->seq==ANIM_ATTACK)
		{
			if(me->frmTimer<128)
				MakeSound(SND_FRANKHEAD,me->x,me->y,SND_CUTOFF,400);
			if(me->frm<16 && me->frmTimer<128)
			{
				x=(me->x+Cosine(me->frm*16)*40);
				y=(me->y+Sine(me->frm*16)*30);
				FireExactBullet(me,x,y,FIXAMT*40,Cosine(me->frm*16)*12,Sine(me->frm*16)*12,
					-FIXAMT*2,0,60,me->frm*16,BLT_ENERGY,me->friendly);
				x=(me->x+Cosine((me->frm+8)*16)*40);
				y=(me->y+Sine((me->frm+8)*16)*30);
				FireExactBullet(me,x,y,FIXAMT*40,Cosine((me->frm+8)*16)*12,Sine((me->frm+8)*16)*12,
					-FIXAMT*2,0,60,(me->frm+8)*16,BLT_ENERGY,me->friendly);
			}
		}
		if(me->seq==ANIM_A3)
		{
			if(me->frm>4 && me->frm<20)
			{
				me->dx=Cosine(me->facing*32)*18;
				me->dy=Sine(me->facing*32)*18;

				x=(me->x+Cosine((me->frm-4)*16)*100)/FIXAMT;
				y=(me->y+Sine((me->frm-4)*16)*80)/FIXAMT;
				if(me->AttackCheck(25,x,y,goodguy))
					goodguy->GetShot(me,goodguy,Cosine((me->frm-4)*16)*8,Sine((me->frm-4)*16)*8,3,map,world);
				x=(me->x+Cosine((me->frm+4)*16)*100)/FIXAMT;
				y=(me->y+Sine((me->frm+4)*16)*80)/FIXAMT;
				if(me->AttackCheck(25,x,y,goodguy))
					goodguy->GetShot(me,goodguy,Cosine((me->frm+4)*16)*8,Sine((me->frm+4)*16)*8,3,map,world);
			}
			else if(me->frm>=20)
			{
				me->dx=(me->dx*0.95);
				me->dy=(me->dy*0.95);
				if(me->frm==21)
					DoMove2(me,ANIM_A1,256,0,me->dx,me->dy);
			}
		}
		if(me->seq==ANIM_A4 && me->frm==4 && me->frmTimer<128)
		{
			x=me->x-60*FIXAMT;
			y=me->y-10*FIXAMT;
			FireBullet(me,x,y,me->facing,BLT_MISSILE,me->friendly);
			x=me->x+60*FIXAMT;
			y=me->y-10*FIXAMT;
			FireBullet(me,x,y,me->facing,BLT_MISSILE,me->friendly);
			MakeSound(SND_FRANKMISSILE,me->x,me->y,SND_CUTOFF,1200);
		}
		if(me->seq==ANIM_A4 && me->frm==10)
		{
			DoMove2(me,ANIM_A2,256,0,me->dx,me->dy);
		}
		return;
	}
	if(RangeToTarget(me,goodguy)<73*FIXAMT)
	{
		// 'acidic'
		goodguy->GetShot(me,goodguy,0,0,3,map,world);
	}

	FaceGoodguy2(me,goodguy);
	me->dx+=Cosine(me->facing*32)/2;
	me->dy+=Sine(me->facing*32)/2;
	
	Clamp(&me->dx,FIXAMT*7);
	Clamp(&me->dy,FIXAMT*7);

	if(me->mind==0)	// have head
	{
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{//void DoMove(Guy *me,int move,int frmAdv,byte busy,int dx,int dy)
				DoMove2(me,ANIM_ATTACK,256,1,me->dx,me->dy);
			}
		}
	}
	if(me->mind==1)	// lost head already
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A1 && me->frm>1))
		{
			DoMove2(me,ANIM_A1,256,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*3;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				MakeSound(SND_FRANKARMS,me->x,me->y,SND_CUTOFF,1200);
				DoMove2(me,ANIM_A3,160,1,me->dx,me->dy);
				FaceGoodguy(me,goodguy);
			}
			if(me->mind1%15==0)
			{
				MakeSound(SND_FRANKARMS,me->x,me->y,SND_CUTOFF,1200);
				FireExactBullet(me,me->x,me->y,FIXAMT*128,Cosine(Random(255))*8,Sine(Random(255))*8,FIXAMT*4,0,120,0,BLT_GRENADE,me->friendly);
			}
		}
	}
	if(me->mind==2)	// lost body too
	{
		if(me->seq==ANIM_IDLE || me->seq==ANIM_MOVE || (me->seq==ANIM_A2 && me->frm>1))
		{
			DoMove2(me,ANIM_A2,256,0,me->dx,me->dy);
		}
		if(me->mind1==0)
			me->mind1=30*2;
		else
		{
			me->mind1--;
			if(me->mind1==0)
			{
				DoMove2(me,ANIM_A4,256,1,me->dx,me->dy);
			}
		}
	}
	
	if (me->seq==ANIM_A3)
	{if(me->mind3&1)
	{
		me->dx=-me->dx/2;
		me->mind2=10;
		me->facing=(byte)Random(8)*32;
	}
	if(me->mind3&2)
	{
		me->dy=-me->dy/2;
		me->mind2=10;
		me->facing=(byte)Random(8)*32;
	}}
	me->mind3=0;
}

void AI_Harry(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i,a,b,c;
	Guy *g;
	
	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LARRYOUCH,me->x,me->y,SND_CUTOFF,1800);
		else
			MakeSound(SND_SWAMPDIE,me->x,me->y,SND_CUTOFF,2000);
	}

	if(me->mind3==0)
		me->mind3=30*4;
	if(me->mind3>1)
		me->mind3--;

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==4)
		{
			me->dx=Cosine(me->facing*32)*10;
			me->dy=Sine(me->facing*32)*10;
		}
		if(me->seq==ANIM_ATTACK && me->frm>=6 && me->frm<=10)
		{
			Dampen(&me->dx,FIXAMT);
			Dampen(&me->dy,FIXAMT);
			if(me->AttackCheck(40,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,8,map,world);
			}
			if(me->frm==8 && me->frmTimer<128)
			{
				x=me->x+Cosine(me->facing*32)*48;
				y=me->y+Sine(me->facing*32)*48;
				if(me->AttackCheck(128,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				{
					goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,8,map,world);
				}
				ShakeScreen(50);
				MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
				for(i=0;i<4;i++)
				{
					x=me->x+((256-Random(513))<<FIXSHIFT);
					y=me->y+((256-Random(513))<<FIXSHIFT);
					FireExactBullet(me,x,y,FIXAMT*80+(Random(40)<<FIXSHIFT),
									0,0,0,0,30,2,BLT_ROCK,me->friendly);
				}
				me->reload=5;
			}
			if(me->frm==10)
			{
				me->dx=0;
				me->dy=0;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==6 && me->frmTimer<128)
		{
			x=me->x+Cosine((me->facing+7+2*(7-me->frm))*32)*20;
			y=me->y+Sine((me->facing+7+2*(7-me->frm))*32)*20;
			MakeSound(SND_LARRYDARTS,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<10;i++)
			{
				FireBullet(me,x,y,(me->facing*32+(me->frm-6)*4+256-24+i*(48/10))&255,BLT_CLAWS,me->friendly);
			}
			me->reload=5;
		}

		if(me->seq==ANIM_A2 && me->frm>=5 && me->frm<=15 && me->reload==0)
		{
			a=Random(256);
			b=Random(20)+20;
			c=Random(20)+20;
			x=me->x+b*Cosine(a);
			y=me->y+c*Sine(a);

			g=AddGuy(x,y,FIXAMT*40,MONS_DIREWOLF,me->friendly);
			if(g && !g->CanWalk(g,g->x,g->y,map,world))
				g->type=MONS_NONE;
			else
			{
				//MakeRingParticle(x,y,FIXAMT*40,20,40);
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			}
			me->reload=3;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x-20*FIXAMT+Random(40*FIXAMT);
			y=me->y-20*FIXAMT+Random(40*FIXAMT);
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			if(Random(2)==0)
			{
				y=me->y-FIXAMT*10+Random(FIXAMT*11);
				x=me->x-32*FIXAMT+Random(65*FIXAMT);
				FireBullet(me,x,y,0,BLT_EVILFACE,me->friendly);
			}
		}
		return;	// can't do nothin' right now
	}

	if(RangeToTarget(me,goodguy)<64*FIXAMT && Random(4)==0 &&
		map->CheckLOS(me->mapx,me->mapy,5,goodguy->mapx,goodguy->mapy))
	{
		// get him!
		MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_ATTACK,128,1,0,0);
		FaceGoodguy2(me,goodguy);
		return;
	}
	if(Random(32)==0)	// fire at any range
	{
		// shoot darts
		MakeSound(SND_LARRYPOUNCE,me->x,me->y,SND_CUTOFF,1200);
		DoMove2(me,ANIM_A1,200,1,0,0);
		FaceGoodguy2(me,goodguy);
		return;
	}

	if(me->mind3==1)
	{
		me->mind3=0;
		MakeSound(SND_WOLFHOWL,me->x,me->y,SND_CUTOFF,1000);
		DoMove2(me,ANIM_A2,128,1,0,0);
		return;
	}

	WanderAI(me,256,20,3,map,world,goodguy,FIXAMT*5);
}

void AI_MechaBonkula(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	Guy *g;

	if(me->reload)
		me->reload--;

	if(me->ouch==3)
	{
		if(me->hp>0)
			MakeSound(SND_BONKOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_BONKDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==8 && me->reload==0)
		{
			MakeSound(SND_BONKHIT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(me->facing*32)*60;
			y=me->y+Sine(me->facing*32)*60;

			FindVictim(me,x>>FIXSHIFT,y>>FIXSHIFT,6*15,Cosine(me->facing*32)*2,
							   Sine(me->facing*32)*2,8,map,world,me->friendly);
			MakeRingParticle(x,y,0,6*15,10*15);
			me->reload=5;
		}

		if(me->seq==ANIM_DIE)
		{
			me->dx=0;
			me->dy=0;
			x=me->x-40*FIXAMT+Random(80*FIXAMT);
			y=me->y-30*FIXAMT+Random(60*FIXAMT);
			FireBullet(me,x,y,0,BLT_FLAMEWALL,me->friendly);
			ExplodeParticles(PART_YELLOW,x,y,Random(10*FIXAMT),5);
			MakeSound(SND_BONKDIE,me->x,me->y-200*me->frm,SND_CUTOFF,2000);
		}
		if(me->seq==ANIM_A1)
		{
			me->facing++;
			if(me->facing>7)
				me->facing=0;
			if(Random(30)==0)
			{
				x=me->x-300*FIXAMT+Random(600)*FIXAMT;
				y=me->y-300*FIXAMT+Random(600)*FIXAMT;
				g=AddGuy(x,y,FIXAMT*50,MONS_VAMPIRE,me->friendly);
				if(g && !g->CanWalk(g,g->x,g->y,map,world))
				{
					g->type=MONS_NONE;
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind3==0)
	{
		// begin spinning
		DoMove2(me,ANIM_A1,256,1,0,0);
		me->mind3=60;
	}
	else
	{
		if(me->mind3==60)
		{
			FaceGoodguy(me,goodguy);
			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
			me->dx+=-FIXAMT*2+Random(4*FIXAMT);
			me->dy+=-FIXAMT*2+Random(4*FIXAMT);
		}
		me->mind3--;
		if(RangeToTarget(me,goodguy)<150*FIXAMT)
		{
			MakeSound(SND_BONKSWING,me->x,me->y,SND_CUTOFF,2000);
			DoMove2(me,ANIM_ATTACK,256,1,0,0);
			me->mind3=0;
		}
	}
}