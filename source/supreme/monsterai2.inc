void AI_MineCart(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte ok[4];
	int diff,dir;
	static byte noiseLoop=0;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
		return;

	me->facing=me->mind2/32;	// mind2 holds the 'true' visual facing 0-255

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
		else if(RangeToTarget(me,goodguy)<32*FIXAMT && player.vehicle==0)
		{
			MakeSound(SND_MINECART,goodguy->x,goodguy->y,SND_CUTOFF,1200);
			noiseLoop=0;
			me->mind=1;
			player.vehicle=VE_MINECART;
			goodguy->parent=me;
			goodguy->x=me->x;
			goodguy->y=me->y+1;
			goodguy->z=FIXAMT*8;
			me->mind1=0;	// acceleration
		}
	}
	else	// being ridden
	{
		if(!editing && !player.cheated)
			profile.progress.cartDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);
		noiseLoop++;
		if(noiseLoop>=28)
		{
			noiseLoop=0;
			MakeSound(SND_MINECART,goodguy->x,goodguy->y,SND_CUTOFF,1200);
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
				goodguy->dx=0;
				goodguy->dy=0;
				player.vehicle=0;
				goodguy->parent=NULL;
				goodguy->x=me->x;
				goodguy->y=me->y;
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
						x=goodguy->facing/2;
						if(ok[x])	// the direction you're facing is ok
							me->mind3=x*64;
						else if(ok[(x+1)&3])	// direction to your left
							me->mind3=((x+1)&3)*64;
						else if(ok[(x+3)&3])	// direction to your right (same as x-1)
							me->mind3=((x+3)&3)*64;
						else
							me->mind3=((x+2)&3)*64;	// behind you as a last resort
					}
				}
			}
		}

		// move forward at that pace
		me->dx=Cosine(me->mind3)*(me->mind1/8);
		me->dy=Sine(me->mind3)*(me->mind1/8);

		// confine the player to the cart
		goodguy->x=me->x;
		goodguy->y=me->y+1;
		goodguy->z=FIXAMT*8;
		goodguy->dx=0;
		goodguy->dy=0;
		goodguy->dz=0;

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
}

void AI_Raft(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte tries;

	goodguy=GetGuyOfAIType(MONS_BOUAPHA);
	if(!goodguy)
		return;

	if(me->mind==0)	// waiting to be ridden
	{
		// chill
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;
	}
	else	// being ridden
	{
		if(!editing && !player.cheated)
			profile.progress.raftDistance+=abs(me->dx/FIXAMT)+abs(me->dy/FIXAMT);
		// if you've reached the center of a tile, time to decide where to go
		x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
		y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
		if((me->x<=x && me->oldx>x) || (me->x>=x && me->oldx<x) ||
		   (me->y<=y && me->oldy>y) || (me->y>=y && me->oldy<y) || me->mind1==0)
		{
			tries=0;
			do
			{
				x=me->mapx;
				y=me->mapy;
				switch(me->facing)
				{
					case 0:
						x++;
						break;
					case 1:
						y++;
						break;
					case 2:
						x--;
						break;
					case 3:
						y--;
						break;
				}
				if(x<0 || x>=map->width || y<0 || y>=map->height ||
					map->map[x+y*map->width].wall>0 ||
					(GetItem(map->map[x+y*map->width].item)->flags&IF_SOLID))
				{
					me->x-=me->dx;
					me->y-=me->dy;
					me->facing=(me->facing+2)&3;
					tries++;
				}
				else
					tries=10;	// we're okay!
			}
			while(tries<3);
			if(tries<10)	// it failed and would've been in an infinite loop
			{
				for(tries=0;tries<4;tries++)
				{
					x=me->mapx;
					y=me->mapy;
					switch(tries)
					{
						case 0:
							x++;
							break;
						case 1:
							y++;
							break;
						case 2:
							x--;
							break;
						case 3:
							y--;
							break;
					}
					if(x>=0 && x<map->width && y>=0 && y<map->height &&
						map->map[x+y*map->width].wall==0 &&
						!(GetItem(map->map[x+y*map->width].item)->flags&IF_SOLID) &&
						!(GetTerrain(world,map->map[x+y*map->width].floor)->flags&(TF_SOLID|TF_WATER|TF_LAVA)))
					{
						me->facing=tries;
						me->x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
						me->y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
						me->mind1=0;
						me->mind=0;
						goodguy->dx=0;
						goodguy->dy=0;
						me->dx=0;
						me->dy=0;
						goodguy->x=me->x+Cosine(me->facing*64)*TILE_WIDTH;
						goodguy->y=me->y+Sine(me->facing*64)*TILE_HEIGHT;
						player.vehicle=0;
						return;

					}
				}
				if(tries==4)	// there was no spot to put the player
				{
					NewMessage("Your raft broke!",60,0);
					me->type=MONS_NONE;
					player.vehicle=0;
					return;
				}

			}
			// otherwise if it's not water, we're done
			if(!(GetTerrain(world,map->map[x+y*map->width].floor)->flags&(TF_WATER|TF_LAVA)))
			{
				me->x=(me->mapx*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
				me->y=(me->mapy*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
				me->mind1=0;
				me->mind=0;
				goodguy->dx=0;
				goodguy->dy=0;
				me->dx=0;
				me->dy=0;
				goodguy->x=me->x+Cosine(me->facing*64)*TILE_WIDTH;
				goodguy->y=me->y+Sine(me->facing*64)*TILE_HEIGHT;
				player.vehicle=0;
				return;
			}
		}
		// move forward at the right speed otherwise
		me->dx=Cosine(me->facing*64)*(me->mind1/4);
		me->dy=Sine(me->facing*64)*(me->mind1/4);

		// confine the player to the raft
		goodguy->x=me->x+me->dx;
		goodguy->y=me->y+me->dy+1;
		goodguy->z=FIXAMT*8;
		goodguy->dx=0;
		goodguy->dy=0;
		goodguy->dz=0;

		if(me->mind1<16)
			me->mind1++;

		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
		}
		me->frmAdvance=32;
	}
}

void AI_Vampire(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	dword d;

	if(me->reload)
		me->reload--;

	if(me->aiType==MONS_DARKVAMP)
	{
		// get burned by the light!
		if(map->GetTile(me->mapx,me->mapy)->light>0 && me->ouch==0 && me->hp>0)
		{
			d=monsType[me->type].flags;
			monsType[me->type].flags=0;
			me->GetShot(me,me,0,0,map->GetTile(me->mapx,me->mapy)->light,map,world);
			monsType[me->type].flags=d;
			for(i=0;i<6;i++)
			{
				BlowSmoke(me->x,me->y,FIXAMT*10,Random(6)*FIXAMT);
			}
		}
	}

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_VAMPOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_VAMPDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,12,map,world);
				if(me->aiType==MONS_DARKVAMP)
					PoisonVictim(goodguy,20);
			}
			me->reload=5;
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly drift towards Bouapha
	{
		if(player.garlic && goodguy && RangeToTarget(me,goodguy)<500*FIXAMT)
		{
			MakeSound(SND_VAMPFEAR,me->x,me->y,SND_CUTOFF,1200);
			me->mind=2;	// enter fear mode
			return;
		}

		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(72*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_VAMPATTK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=320;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
				return;
			}
			FaceGoodguy(me,goodguy);

			me->dx=Cosine(me->facing*32)*9;
			me->dy=Sine(me->facing*32)*9;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
			}
			if(me->mind1>0 && RangeToTarget(me,goodguy)>(64*FIXAMT))
			{
				me->mind=1;	// bumped a wall, so randomly maneuver
				me->facing=(byte)Random(8);
				me->mind2=10;
			}
		}
		else
		{
			me->mind=1;	// if there's no goodguy, get random
			me->facing=(byte)Random(8);
		}
	}
	else if(me->mind==1)	// random wandering
	{
		if(!(me->mind2--))	// time to get a new direction
		{
			me->mind=0;	// get back on track
		}

		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
		}
	}
	else
	{
		if(player.garlic && goodguy)
		{
			// run from the garlic
			FaceGoodguy(me,goodguy);
			me->facing=(me->facing+4)&7;	// face opposite direction
			me->dx=Cosine(me->facing*32)*8;
			me->dy=Sine(me->facing*32)*8;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
			}
		}
		else
		{
			me->mind=0;
		}
		return;
	}

	me->mind1=0;
}

void AI_Coffin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(me->ouch==4 && me->hp>0)
	{
		MakeSound(SND_VAMPOUCH,me->x,me->y,SND_CUTOFF,1200);
		if(me->seq!=ANIM_ATTACK)
		{
			MakeSound(SND_COFFIN,me->x,me->y,SND_CUTOFF,1200);
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=127;
			me->action=ACTION_BUSY;
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==15 && me->frmTimer>64)
		{
			if(me->aiType==MONS_DARKCOFFIN)
				AddBaby(me->x+FIXAMT*20,me->y+FIXAMT*10,0,MONS_DARKVAMP,me);
			else
				AddBaby(me->x+FIXAMT*20,me->y+FIXAMT*10,0,MONS_VAMPIRE,me);
			// then die
			me->hp=1;
			me->GetShot(me,me,0,0,1,map,world);
			me->ouch=0;
		}
		return;	// can't do nothin' right now
	}

	if(goodguy && RangeToTarget(me,goodguy)<180*FIXAMT && !player.garlic)
	{
		// goodguy got close, pop out
		MakeSound(SND_COFFIN,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=127;
		me->action=ACTION_BUSY;
	}
}

void AI_Ghost(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			MakeSound(SND_GHOSTOUCH,me->x,me->y,SND_CUTOFF,1200);
			if(me->action==ACTION_IDLE)
			{
				// teleport - ghosts don't like being hit
				me->action=ACTION_BUSY;
				me->seq=ANIM_A1;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
				me->dy=0;
				return;
			}
		}
		else
		{
			MakeSound(SND_GHOSTDIE,me->x,me->y,SND_CUTOFF,1200);
			for(i=0;i<16;i++)
				BlowSmoke(me->x-32*FIXAMT+Random(64*FIXAMT),me->y-32*FIXAMT+Random(64*FIXAMT),Random(64)<<FIXSHIFT,Random(FIXAMT*4));
		}
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK)
		{
			if (me->aiType!=MONS_WETGHOST && me->frm==2 && me->reload==0)
			{
				me->reload=10;
				if(goodguy && RangeToTarget(me,goodguy)<120*FIXAMT)
				{
					goodguy->GetShot(me,goodguy,0,0,6,map,world);
				}
			}
			if (me->aiType==MONS_WETGHOST && me->frm>3 && me->reload==0)
			{
				me->reload=2;
					MakeSound(SND_CRAZYSTAB,me->x,me->y,SND_CUTOFF,1200);
				x=me->x+Cosine(me->facing*32)*16;
				y=me->y+Sine(me->facing*32)*16;
				FireBulletZ(me,x,y,FIXAMT*12,me->facing*32-16+(byte)Random(33),BLT_SHARK,me->friendly);
			}
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->frmTimer>64)
		{
			for(i=0;i<8;i++)
				BlowSmoke(me->x-32*FIXAMT+Random(64*FIXAMT),me->y-32*FIXAMT+Random(64*FIXAMT),Random(64)<<FIXSHIFT,Random(FIXAMT*4));
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
			for(i=0;i<8;i++)
				BlowSmoke(me->x-32*FIXAMT+Random(64*FIXAMT),me->y-32*FIXAMT+Random(64*FIXAMT),Random(64)<<FIXSHIFT,Random(FIXAMT*4));
		}

		return;	// can't do nothin' right now
	}

	if(me->mind==0 && me->hp>0)	// hasn't spotted Bouapha yet
	{
		if((goodguy && RangeToTarget(me,goodguy)<300*FIXAMT) || (me->ouch>0))
		{
			MakeSound(SND_GHOSTTPORT,me->x,me->y,SND_CUTOFF,1200);
			me->mind=1;
			me->action=ACTION_BUSY;
			me->seq=ANIM_A3;	// materialize
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=64;
			me->dx=0;
			me->dy=0;
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
	else if(me->mind==1)	// has spotted him, give chase
	{
		if(Random(500)==1)
		{
			// teleport
			me->action=ACTION_BUSY;
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			return;
		}
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			if (me->aiType==MONS_WETGHOST)
			{
				me->dx=Cosine(me->facing*32)*5;
				me->dy=Sine(me->facing*32)*5;
			}
			else
			{
				me->dx=Cosine(me->facing*32)*3;
				me->dy=Sine(me->facing*32)*3;
			}
			if(RangeToTarget(me,goodguy)<140*FIXAMT && Random(20)==1)
			{
				// scream
				MakeSound(SND_GHOSTYELL,me->x,me->y,SND_CUTOFF,1200);
				me->action=ACTION_BUSY;
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=72;
				me->dx=0;
				me->dy=0;
				return;
			}
		}
	}
}

void AI_Burner(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->mind==0)
	{
		if(me->reload)
			me->reload--;
		else
		{
			me->mind=1;
			me->mind1=10;
		}
	}
	else
	{
		if(me->reload)
			me->reload--;

		if(me->mind1)
		{
			me->mind1--;
			if(me->aiType==MONS_BURNER)
			{
				if((me->mind1&1) && RangeToTarget(me,goodguy)<500*FIXAMT)
					switch(me->mind2)
					{
						case 0:
							if(me->mapx<map->width-1 && !map->map[me->mapx+1+me->mapy*map->width].wall)
								FireBullet(me,me->x+FIXAMT*TILE_WIDTH/2,me->y,0,BLT_FLAME2,me->friendly);
							break;
						case 1:
							if(me->mapy<map->height-1 && !map->map[me->mapx+0+(me->mapy+1)*map->width].wall)
								FireBullet(me,me->x,me->y+FIXAMT*TILE_HEIGHT/2,2,BLT_FLAME2,me->friendly);
							break;
						case 2:
							if(me->mapx>0 && !map->map[me->mapx-1+(me->mapy+0)*map->width].wall)
								FireBullet(me,me->x-FIXAMT*TILE_WIDTH/2,me->y,4,BLT_FLAME2,me->friendly);
							break;
						case 3:
							if(me->mapy>0 && !map->map[me->mapx+(me->mapy-1)*map->width].wall)
								FireBullet(me,me->x,me->y-FIXAMT*TILE_HEIGHT/2,6,BLT_FLAME2,me->friendly);
							break;
					}
			}
			else if(me->aiType==MONS_MEGABURNER)	// megaburner
			{
				if((me->reload==0) && (me->mind1&1) && RangeToTarget(me,goodguy)<500*FIXAMT)
					switch(me->mind2)
					{
						case 0:
							if(me->mapx<map->width-1 && !map->map[me->mapx+1+me->mapy*map->width].wall)
							{
								FireBullet(me,me->x+FIXAMT*TILE_WIDTH/2,me->y-FIXAMT*20,0,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x+FIXAMT*TILE_WIDTH/2,me->y,0,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x+FIXAMT*TILE_WIDTH/2,me->y+FIXAMT*20,0,BLT_FLAME2,me->friendly);
							}
							break;
						case 1:
							if(me->mapy<map->height-1 && !map->map[me->mapx+0+(me->mapy+1)*map->width].wall)
							{
								FireBullet(me,me->x-FIXAMT*20,me->y+FIXAMT*TILE_WIDTH/2,2,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x,me->y+FIXAMT*TILE_WIDTH/2,2,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x+FIXAMT*20,me->y+FIXAMT*TILE_WIDTH/2,2,BLT_FLAME2,me->friendly);
							}
							break;
						case 2:
							if(me->mapx>0 && !map->map[me->mapx-1+(me->mapy+0)*map->width].wall)
							{
								FireBullet(me,me->x-FIXAMT*TILE_WIDTH/2,me->y-FIXAMT*20,4,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x-FIXAMT*TILE_WIDTH/2,me->y,4,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x-FIXAMT*TILE_WIDTH/2,me->y+FIXAMT*20,4,BLT_FLAME2,me->friendly);
							}
							break;
						case 3:
							if(me->mapy>0 && !map->map[me->mapx+(me->mapy-1)*map->width].wall)
							{
								FireBullet(me,me->x-FIXAMT*20,me->y-FIXAMT*TILE_WIDTH/2,6,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x,me->y-FIXAMT*TILE_WIDTH/2,6,BLT_FLAME2,me->friendly);
								FireBullet(me,me->x+FIXAMT*20,me->y-FIXAMT*TILE_WIDTH/2,6,BLT_FLAME2,me->friendly);
							}
							break;
					}
			}
			else if(me->aiType==MONS_DEATHBURNER)
			{
				FireBullet(me,me->x,me->y,me->mind2,BLT_FLAME5,me->friendly);
			}
		}
		else
		{
			if(me->aiType==MONS_DEATHBURNER)
			{
				me->mind2+=8;
				if(me->mind2>255)
					me->mind2=0;
				me->mind=0;
				me->reload=1;
			}
			else 
			{
				me->mind2++;
				if(me->mind2>3)
					me->mind2=0;
				me->mind=0;
				me->reload=15;
			}
		}
	}
}

void AI_Lefty(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_LEFTYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_LEFTYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE)
			me->frmAdvance=72;
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(16,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
			me->reload=10;
		}
		return;	// can't do nothin' right now
	}

	// randomly decide to wave at Bouapha to unnerve him
	// (but only if in pursuit mode, because otherwise you'd point the wrong way)
	if((!Random(100)) && me->mind==0)
	{
		//MakeSound(SND_LEFTYWAVE,me->x,me->y,SND_CUTOFF,200);
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		return;
	}

	if(me->mind==0)		// when mind=0, singlemindedly skitter towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(48*FIXAMT) && Random(8)==0)
			{
				// get him!
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=140;
				me->action=ACTION_BUSY;
				me->dx=0;
				me->dy=0;
				me->reload=0;
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
				me->frmAdvance=200;
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

void AI_Pygmy2(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
		if(me->seq==ANIM_ATTACK && me->frm==4 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			// throw spear
			FireBullet(me,x,y,me->facing,BLT_BADSPEAR,me->friendly);
			me->reload=10;
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
			if(RangeToTarget(me,goodguy)>256*FIXAMT)
				me->mind=0;	// get bored again
			else
				me->mind1=20;	// stay on trail a little longer
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

void AI_Pygmy3(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_SHAMANOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_SHAMANDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			// poison zap
			if(RangeToTarget(me,goodguy)<200*FIXAMT)
			{
				LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
				LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
				LightningBolt(x,y-FIXAMT*20,goodguy->x,goodguy->y-FIXAMT*20);
				goodguy->GetShot(me,goodguy,0,0,4,map,world);
				PoisonVictim(goodguy,60);
			}
			me->reload=10;
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
					MakeSound(SND_SHAMANDANCE,me->x,me->y,SND_CUTOFF,1200);
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
				MakeSound(SND_SHAMANANGRY,me->x,me->y,SND_CUTOFF,1200);
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
		if(RangeToTarget(me,goodguy)<(200*FIXAMT) && Random(64)==0 && me->reload==0)
		{
			// zap him!
			MakeSound(SND_SHAMANZAP,me->x,me->y,SND_CUTOFF,1200);
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

void AI_Pumpkinstein(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ZOMBIEOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ZOMBIEDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>4)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);

		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		// walking
		if(goodguy && RangeToTarget(me,goodguy)<64*FIXAMT)
		{
			goodguy->GetShot(me,goodguy,-4*FIXAMT+Random(8*FIXAMT),-4*FIXAMT+Random(8*FIXAMT),10,map,world);
			me->mind1=1;
		}
		if(me->mind1)
		{
			// bumped a wall
			MakeSound(SND_PKSTEINBUMP,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}
			me->mind1=0;
			me->dx=0;
			me->dy=0;
			me->mind=1;	// switch to turning mode
			me->reload=2;

			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		else
		{
			if(me->dx==0 && me->dy==0)
			{
				x=Random(6)+4;
				me->dx=Cosine(me->facing*32)*x;
				me->dy=Sine(me->facing*32)*x;
			}
			if(me->seq!=ANIM_MOVE && me->seq!=ANIM_A2)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
	}
	else
	{
		// turning
		if(!me->reload)
		{
			me->facing=(me->facing+me->mind3-1)&7;
			if(me->facing==me->mind2)	// done turning
			{
				me->mind=0;
			}
			me->reload=2;
		}
	}
}

void AI_Knight(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
		{
			if(me->action!=ACTION_BUSY && Random(2)==0)
			{
				me->action=ACTION_BUSY;
				me->seq=ANIM_A2;	// block
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			MakeSound(SND_GLASSHIT,me->x,me->y,SND_CUTOFF,1200);
		}
		else
			MakeSound(SND_GLASSDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==5 && me->reload==0)
		{
			// hack at him with axe
			x=me->x-FIXAMT*20;
			y=me->y+FIXAMT*60;

			FindVictim(me,x>>FIXSHIFT,y>>FIXSHIFT,60,0,FIXAMT*8,20,map,world,me->friendly);

			me->reload=10;

		}
		if(me->seq==ANIM_A1 && me->frm==13 && me->reload==0)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			GlassShatter(x-40,y-100,x+40,y+20,me->z,50);
			MakeSound(SND_GLASSBREAK,me->x,me->y,SND_CUTOFF,1200);
			me->reload=3;
		}
		if(me->seq==ANIM_DIE && me->frm==12 && me->reload==0)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			GlassShatter(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,100);
			MakeSound(SND_GLASSBREAK,me->x,me->y,SND_CUTOFF,1200);
			me->reload=3;
		}
		return;	// can't do nothin' right now
	}

	if(me->hp>0 && me->hp<me->maxHP && me->aiType==MONS_KNIGHT2)
	{
		me->hp+=150;
		if(me->hp>me->maxHP)
			me->hp=me->maxHP;
	}
	
	if(me->mind==0 && me->aiType==MONS_KNIGHT)
	{
		// unaware, sitting in glass
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=64;

		if(goodguy && (RangeToTarget(me,goodguy)<100*FIXAMT || me->ouch>0))
		{
			me->mind=1;
			me->action=ACTION_BUSY;	// continue the arrival animation
		}
	}
	else if(me->aiType==MONS_KNIGHT2)
	{
		me->seq=ANIM_A1;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=2;
	}
	else
	{
		if(goodguy)
		{
			me->dx=0;
			me->dy=0;
			if(goodguy->y<me->y+FIXAMT*30)
				me->dy=-FIXAMT*6;
			else if(goodguy->y>me->y+FIXAMT*128)
				me->dy=FIXAMT*4;
			if(goodguy->x<me->x-FIXAMT*40)
				me->dx=-FIXAMT*4;
			if(goodguy->x>me->x+FIXAMT*40)
				me->dx=FIXAMT*4;

			if(me->dx==0 && me->dy==0)
			{
				if(RangeToTarget(me,goodguy)>72*FIXAMT)
				{
					if(goodguy->x>me->x+FIXAMT*2)
						me->dx=FIXAMT*2;
					else if(goodguy->x<me->x-FIXAMT*2)
						me->dx=-FIXAMT*2;
					if(goodguy->y>me->y+FIXAMT*2)
						me->dy=FIXAMT*2;
					else if(goodguy->y<me->y-FIXAMT*2)
						me->dy=-FIXAMT*2;
				}
			}
			if(RangeToTarget(me,goodguy)<128*FIXAMT && goodguy->y>me->y && Random(10)==0)
			{
				MakeSound(SND_SKELKICK,me->x,me->y,SND_CUTOFF,1200);
				me->dx=0;
				me->dy=0;
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
				return;
			}
		}
		if((me->dx!=0 || me->dy!=0)&&me->aiType==MONS_KNIGHT)
		{
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
		else
		{
			if(me->seq!=ANIM_IDLE)
			{
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
		}
	}
}

void AI_Triceroid(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_TRICEROUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_TRICERDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm>5)
		{
			me->facing=(me->facing+1)&7;
		}
		return;	// can't do nothin' right now
	}

	x=me->maxHP-me->hp+5;	// more damaged=faster

	if(me->mind==0)
	{
		// walking
		if(goodguy && RangeToTarget(me,goodguy)<64*FIXAMT)
		{
			goodguy->GetShot(me,goodguy,-4*FIXAMT+Random(8*FIXAMT),-4*FIXAMT+Random(8*FIXAMT),10,map,world);
			me->dx=0;
			me->dy=0;
			me->mind=1;	// switch to turning mode
			me->reload=2;
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			MakeSound(SND_TRICERAROAR,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}

			return;
		}
		if(me->mind1)
		{
			// bumped a wall
			MakeSound(SND_TRICERBUMP,me->x,me->y,SND_CUTOFF,1200);
			// pick a new direction
			if(Random(2)==0)
			{
				me->mind2=(me->facing+4+Random(3))&7;
				me->mind3=0;
			}
			else
			{
				me->mind2=(me->facing+4-Random(3))&7;
				me->mind3=2;
			}
			me->mind1=0;
			me->dx=0;
			me->dy=0;
			me->mind=1;	// switch to turning mode
			me->reload=2;

			me->seq=ANIM_IDLE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		else
		{
			if((me->dx==0 && me->dy==0) || (me->ouch))
			{
				me->dx=Cosine(me->facing*32)*x/5;
				me->dy=Sine(me->facing*32)*x/5;
			}
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=(x/5)*30+40;
			}
		}
	}
	else
	{
		// turning
		if(!me->reload)
		{
			me->facing=(me->facing+me->mind3-1)&7;
			if(me->facing==me->mind2)	// done turning
			{
				me->mind=0;
			}
			me->reload=2;
		}
	}
}

void AI_Countess(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	Guy *g;
	int x,y,i;
	int xpos[]={-FIXAMT*50,0,FIXAMT*50};
	int ypos[]={0,FIXAMT*50,0};
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
			MakeSound(SND_COUNTOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_COUNTDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>5 && me->frm<9 && me->reload==0)
		{
			me->reload=2;
			g=AddBaby(me->x,me->y+FIXAMT*40,FIXAMT*20,MONS_BAT,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				MakeSound(SND_BATDIVE,g->x,g->y,SND_CUTOFF,1200);
				g->seq=ANIM_A1;	// diving attack move
				g->frm=0;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->action=ACTION_BUSY;
				g->dx=Cosine(g->facing*32)*12-FIXAMT*2+Random(FIXAMT*4);
				g->dy=Sine(g->facing*32)*12;
			}
		}
		if(me->seq==ANIM_A1 && me->frm==2 && me->reload==0)
		{
			me->reload=5;
			for(i=0;i<3;i++)
			{
				g=AddBaby(me->x+xpos[i],me->y+ypos[i],0,MONS_BONEHEAD,me);
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

		if(me->seq==ANIM_DIE && me->frm>3)
		{
			me->frmAdvance=128;
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z-Random(FIXAMT*60),FIXAMT);
		}
		if(me->seq==ANIM_A2 && me->frm==2)
		{
			me->dx=Cosine(me->facing)*14;
			me->dy=Sine(me->facing)*14;
		}

		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(me->mind==0)	// waiting for Bouapha
	{
		if(RangeToTarget(me,goodguy)<300*FIXAMT || me->ouch>0)
		{
			MakeSound(SND_COUNTHELLO,me->x,me->y,SND_CUTOFF,1200);
			me->mind=1;
		}
		me->mind1=60;
	}
	if(me->mind==1)	// wandering mode
	{
		// calculate desired location (want to be above Bouapha)
		x=goodguy->x;
		y=goodguy->y-96*FIXAMT;

		FaceGoodguy3(me,goodguy);
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

		if(RangeToTarget(me,goodguy)>400*FIXAMT && Random(10)==1)
		{
			MakeSound(SND_COUNTCHARGE,me->x,me->y,SND_CUTOFF,1200);
			me->mind=2;
			me->mind1=10+Random(10)+1;
			FaceGoodguy(me,goodguy);
			me->facing=((me->facing*32)-16+Random(33));
			me->action=ACTION_BUSY;
			me->seq=ANIM_A2;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
			me->dx=0;
			me->dy=0;
		}
		if(me->mind1)
			me->mind1--;
		else
		{
			x=Random(4);
			switch(x)
			{
				case 0:	// shoot bats
					if(me->y>goodguy->y)
						me->mind1=10;	// don't fire bats if below player
					else
					{
						MakeSound(SND_COUNTSHOOT,me->x,me->y,SND_CUTOFF,1200);
						me->action=ACTION_BUSY;
						me->seq=ANIM_ATTACK;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
						me->mind1=60;
						me->dx=0;
						me->dy=0;
					}
					break;
				case 1:	// summon boneheads
					if(RangeToTarget(me,goodguy)>160*FIXAMT || me->y>goodguy->y)
					{
						MakeSound(SND_COUNTSUMMON,me->x,me->y,SND_CUTOFF,1200);
						me->action=ACTION_BUSY;
						me->seq=ANIM_A1;
						me->frm=0;
						me->frmTimer=0;
						me->frmAdvance=128;
						me->mind1=60;
						me->dx=0;
						me->dy=0;
					}
					else
						me->mind1=10;	// don't summon if close, unless under goodguy
					break;
				case 2: // hyper-dash
					MakeSound(SND_COUNTCHARGE,me->x,me->y,SND_CUTOFF,1200);
					me->mind=2;
					me->mind1=10+Random(10);
					FaceGoodguy(me,goodguy);
					me->facing=((me->facing*32)-16+Random(33));
					me->action=ACTION_BUSY;
					me->seq=ANIM_A2;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=255;
					me->dx=0;
					me->dy=0;
					break;
				case 3:
					me->mind1=10;
					break;
			}
		}
	}
	else if(me->mind==2)
	{
		if(RangeToTarget(me,goodguy)<72*FIXAMT)
		{
			goodguy->GetShot(me,goodguy,0,0,4,map,world);
		}
		MonsterGlow(me->x,me->y,me->type);

		if(me->mind1)
			me->mind1--;
		else
		{
			me->mind=1;
			me->mind1=10;
		}
	}
}

void AI_AlienEgg(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
		if(me->hp>0)
			MakeSound(SND_EGGOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_EGGDIE,me->x,me->y,SND_CUTOFF,1200);
		me->mind1=1;
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==10 && me->reload==0)
		{
			me->reload=90;
			if(me->aiType==MONS_ALIENEGG2)
				me->reload=10;
			g=AddBaby(me->x,me->y,FIXAMT*20,MONS_BABYALIEN,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			else if(g)
			{
				g->facing=(byte)Random(8);
				g->seq=ANIM_ATTACK;	// diving attack move
				g->frm=2;
				g->frmTimer=0;
				g->frmAdvance=128;
				g->action=ACTION_BUSY;
				g->dx=Cosine(g->facing*32)*12;
				g->dy=Sine(g->facing*32)*12;
			}
		}
		if(me->seq==ANIM_DIE)
		{
			FireBullet(me,me->x,me->y,(byte)Random(256),BLT_ACID,me->friendly);
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,4,10);
		}
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(RangeToTarget(me,goodguy)<300*FIXAMT || me->mind1)
	{
		if(me->reload==0 && (Random(150)==0 || me->aiType==MONS_ALIENEGG2))
		{
			MakeSound(SND_EGGOPEN,me->x,me->y,SND_CUTOFF,1200);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=255;
		}
	}
}

void AI_BabyAlien(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
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
		if(me->seq==ANIM_ATTACK && me->frm>2)
		{
			me->dx=Cosine(me->facing*32)*14;
			me->dy=Sine(me->facing*32)*14;
			if(goodguy && RangeToTarget(me,goodguy)<44*FIXAMT && me->mind2)
			{
				goodguy->GetShot(me,goodguy,me->dx,me->dy,4,map,world);
				me->mind2=0;	// don't allow multiple hits
			}
		}
		if(me->seq==ANIM_DIE)
		{
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,4,10);
		}
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;

	if(me->mind1)
	{
		me->mind1--;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
		}
		if(RangeToTarget(me,goodguy)<160*FIXAMT && Random(20)==0)
		{
			MakeSound(SND_SPD2SPIT,me->x,me->y,SND_CUTOFF,1200);
			FaceGoodguy(me,goodguy);
			me->action=ACTION_BUSY;
			me->seq=ANIM_ATTACK;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->dx=0;
			me->dy=0;
			me->mind1=0;
			me->mind2=1;
		}
	}
	else
	{
		me->facing=(byte)Random(8);
		me->dx=Cosine(me->facing*32)*10;
		me->dy=Sine(me->facing*32)*10;
		me->mind1=10+Random(20);
	}
}

void AI_Alien(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ALIENOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ALIENDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*16;
			y=me->y+Sine(me->facing*32)*16;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,8,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->frm==5 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*20;
			y=me->y+Sine(me->facing*32)*20;
			SpitAcid(me,x,y,me->facing*32,BLT_ACID,me->friendly);	// same as FireBullet, just does it up a little (z axis)
													// because alien is taller than other acid-spitters
			me->reload=5;
			me->mind1=1;
		}
		if(me->seq==ANIM_DIE && me->frm==5)
		{
			for(x=0;x<8;x++)
				FireBullet(me,me->x,me->y,(byte)Random(256),BLT_ACID,me->friendly);
			ExplodeParticles2(PART_SLIME,me->x,me->y,FIXAMT*20,10,10);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(78*FIXAMT) && Random(8)==0)
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

			me->dx=Cosine(me->facing*32)*6;
			me->dy=Sine(me->facing*32)*6;
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
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0)
			{
				// spit at him
				MakeSound(SND_ALIENSPIT,me->x,me->y,SND_CUTOFF,1200);
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

void AI_Robopumpkin(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*32;
			y=me->y+Sine(me->facing*32)*32;
			if(me->AttackCheck(20,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,20,map,world);
			me->reload=5;
		}
		if(me->seq==ANIM_A1 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*40;
			x+=Cosine(((me->facing+2)&7)*32)*20;
			y=me->y+Sine(me->facing*32)*40;
			y+=Sine(((me->facing+2)&7)*32)*20;
			FireBulletZ(me,x,y,FIXAMT*50,me->facing*32-16+Random(33),BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,950);
			me->reload=1;
			me->mind1=1;
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
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly lumber towards Bouapha
	{
		if(goodguy)
		{
			if(RangeToTarget(me,goodguy)<(96*FIXAMT) && Random(8)==0)
			{
				// get him!
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
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				// shoot at him
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
			FaceGoodguy2(me,goodguy);

			me->dx=Cosine(me->facing*32)*4;
			me->dy=Sine(me->facing*32)*4;
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
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(16)==0)
			{
				// shoot at him
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

		me->dx=Cosine(me->facing*32)*4;
		me->dy=Sine(me->facing*32)*4;
	}
}

void AI_ShockTrooper(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>=3)
		{
			if(me->frm<7)
			{
				me->dx=Cosine(me->facing*32)*12;
				me->dy=Sine(me->facing*32)*12;
				if(me->mind1==0)
				{
					FindVictim(me,me->x>>FIXSHIFT,me->y>>FIXSHIFT,100,Cosine(me->facing*32)*12,Sine(me->facing*32)*12,20,map,world,me->friendly);
					me->mind1=1;
				}
			}
			else
			{
				// slow down
				Dampen(&me->dx,FIXAMT*2);
				Dampen(&me->dy,FIXAMT*2);
			}
		}
		if(me->seq==ANIM_A1 && (me->frm==3 || me->frm==4) && me->reload==0 && goodguy)
		{
			MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(((me->facing+2)&7)*32)*48;
			y=me->y+Sine(((me->facing+2)&7)*32)*32;
			FireBulletZ(me,x,y,FIXAMT*100,me->facing*32-16+Random(33),BLT_GRENADE,me->friendly);
			if(me->frm==4)
				me->reload=30;
		}
		if(me->seq==ANIM_DIE && me->frm>6)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
		}
		return;	// can't do nothin' right now
	}

	if(!goodguy)
		return;
	if(RangeToTarget(me,goodguy)<(300*FIXAMT) && Random(20)==0)
	{
		// get him!
		MakeSound(SND_INVIDCHARGE,me->x,me->y,SND_CUTOFF,1200);
		me->seq=ANIM_ATTACK;
		me->frm=0;
		me->frmTimer=0;
		me->frmAdvance=128;
		me->action=ACTION_BUSY;
		me->dx=0;
		me->dy=0;
		me->reload=0;
		me->mind1=0;
		return;
	}
	if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 && me->reload==0)
	{
		// shoot at him
		me->seq=ANIM_A1;
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
}

void AI_Minibot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frm==4 && me->frmTimer<60)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly roll towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0 && me->reload==0)
			{
				// shoot at him
				MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,me->facing*32,BLT_ENERGY,me->friendly);
				me->reload=10;
				return;
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
			if(Random(48)==0)
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
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(256)==0 && me->reload==0)
			{
				// shoot at him
				MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
				FireBullet(me,me->x,me->y,me->facing*32,BLT_ENERGY,me->friendly);
				me->reload=10;
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

void AI_Meaniebot(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm>1 && me->frm<5 && me->reload==0)
		{
			MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
			x=me->x+Cosine(((me->facing+2)&7)*32)*24;
			y=me->y+Sine(((me->facing+2)&7)*32)*20;
			FireBullet(me,x,y,me->facing*32-16+Random(33),BLT_ENERGY,me->friendly);
			x=me->x+Cosine(((me->facing-2)&7)*32)*24;
			y=me->y+Sine(((me->facing-2)&7)*32)*20;
			FireBullet(me,x,y,me->facing*32-16+Random(33),BLT_ENERGY,me->friendly);
			me->reload=2;
		}
		if(me->seq==ANIM_DIE && me->frm>3 && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			if(me->frm==5 && me->frmTimer<63)
			{
				if(me->aiType==MONS_GOODROBOT2)
				{
					g=AddBaby(me->x-FIXAMT*20,me->y,0,MONS_GOODROBOT,me);
					if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
						RemoveGuy(g);
					g=AddBaby(me->x+FIXAMT*20,me->y,0,MONS_GOODROBOT,me);
					if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
						RemoveGuy(g);
				}
				else
				{
					g=AddBaby(me->x-FIXAMT*20,me->y,0,MONS_ROBOT1,me);
					if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
						RemoveGuy(g);
					g=AddBaby(me->x+FIXAMT*20,me->y,0,MONS_ROBOT1,me);
					if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
						RemoveGuy(g);
				}
			}
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)		// when mind=0, singlemindedly roll towards Bouapha
	{
		if(goodguy)
		{
			FaceGoodguy3(me,goodguy);
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(32)==0)
			{
				// shoot at him
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
			me->dx=Cosine(me->facing*32)*3;
			me->dy=Sine(me->facing*32)*3;
			if(me->seq!=ANIM_MOVE)
			{
				me->seq=ANIM_MOVE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
			}
			if(Random(48)==0)
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
			if(RangeToTarget(me,goodguy)<(512*FIXAMT) && Random(256)==0)
			{
				// shoot at him
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
		}
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

void AI_Robofactory(Guy *me,Map *map,world_t *world,Guy *goodguy)
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
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_FACTORYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==15 && me->frmTimer>128)
		{
			// the animation was too long to fit on one sequence
			me->seq=ANIM_A1;
			me->frm=0;
			me->frmTimer=0;
		}
		if(me->seq==ANIM_A1 && me->frm==10 && me->reload==0)
		{
			g=AddBaby(me->x,me->y,0,MONS_ROBOT1,me);
			if(g && (!g->CanWalk(g,g->x,g->y,map,world)))
				RemoveGuy(g);
			me->reload=30;
			me->mind1=8;
		}
		if(me->seq==ANIM_DIE && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->mind==0)
	{
		if(goodguy && RangeToTarget(me,goodguy)<800*FIXAMT)
		{
			me->mind=1;
			me->mind1=Random(15)+1;
			me->reload=30;
		}
	}
	else
	{
		if(me->reload==0)
		{
			me->mind1--;
			if(me->mind1==0)
			{
				MakeSound(SND_FACTORYGO,me->x,me->y,SND_CUTOFF,1200);
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->action=ACTION_BUSY;
			}
			else
				me->reload=30;
		}
	}
}

void AI_Turret(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,a;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ROBOOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ROBODIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->frmTimer<63)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		}
		return;	// can't do nothin' right now
	}

	if(me->aiType<MONS_TURRETR)
	{if(me->mind==0)
	{
		if(goodguy && RangeToTarget(me,goodguy)<800*FIXAMT)
		{
			me->mind=1;
			if(me->aiType!=MONS_DEATHTURRET)
				me->reload=(byte)Random(60);
			else
				me->reload=5;
		}
	}
	else
	{
		if(me->reload==0)
		{
			x=me->x+Cosine(me->facing*32)*12;
			y=me->y+Sine(me->facing*32)*12;
			switch(me->aiType)
			{
				case MONS_TURRET:
					MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
					FireExactBullet(me,x,y,FIXAMT*4,Cosine(me->facing*32)*8,Sine(me->facing*32)*8,0,0,30,me->facing*32,BLT_ENERGY,me->friendly);
					me->reload=30;
					break;
				case MONS_MISLTURRET:
					FireBullet(me,x,y,me->facing,BLT_MISSILE,me->friendly);
					me->reload=60;
					break;
				case MONS_DEATHTURRET:
					if(RangeToTarget(me,goodguy)<400*FIXAMT)
					{
						MakeSound(SND_ROBOSHOOT,me->x,me->y,SND_CUTOFF,1200);
						a=(me->facing*32-16+Random(33))&255;
						FireExactBullet(me,x,y,FIXAMT*4,Cosine(a)*4,Sine(a)*4,0,0,60,a/16,BLT_MISSILE,me->friendly);
						me->reload=5;
					}
					break;
			}
		}
		FaceGoodguy3(me,goodguy);
	}}
	
	if (me->aiType>=MONS_TURRETR && me->aiType<=MONS_TURRETU)
	{me->facing=((me->type-MONS_TURRETR)*2);
	FireBullet(me,me->x,me->y,me->facing,BLT_LASER2,me->friendly);}
}

void AI_Bunny(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y,i;
	// random bullet choices to fire (12 choices)
	byte bltTable[]={BLT_ENERGY,BLT_FLAME2,BLT_SPORE,BLT_GRENADE,BLT_SHOCKWAVE,BLT_SNOWBALL,
		BLT_BIGSNOW,BLT_ICESPIKE,BLT_ROCK,BLT_SPINE,BLT_EVILHAMMER,BLT_BADSPEAR};
	// whether each of those takes 256 facings or 8
	byte facingMode[]={1,0,1,1,0,1,1,1,0,1,0,0};

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		// go nuts shooting
		y=10+Random(40);
		for(x=0;x<y;x++)
		{
			i=Random(12);
			if(facingMode[i]==0)
				FireBullet(me,me->x,me->y,(byte)Random(8),bltTable[i],me->friendly);
			else
				FireBullet(me,me->x,me->y,(byte)Random(256),bltTable[i],me->friendly);
		}
	}

	if(me->action==ACTION_BUSY)
	{
		// go nuts shooting continuously while you die
		y=2+Random(10);
		for(x=0;x<y;x++)
		{
			i=Random(12);
			if(facingMode[i]==0)
				FireBullet(me,me->x,me->y,(byte)Random(8),bltTable[i],me->friendly);
			else
				FireBullet(me,me->x,me->y,(byte)Random(256),bltTable[i],me->friendly);
		}

		return;	// can't do nothin' right now
	}

	if(me->mind1)
	{
		me->mind=0;
		me->mind1=0;
	}
	if(!(me->mind--))	// time to get a new direction
	{
		me->facing=(me->facing+1-Random(3))&7;
		me->mind=Random(20)+2;
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

void AI_Kongor(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
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
		// left pound
		if(me->seq==ANIM_ATTACK && me->frm==7)
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me,me->x-FIXAMT*96,me->y+FIXAMT*64,0,BLT_SHOCKWAVE,me->friendly);
			ShakeScreen(5);
		}
		// right pound
		if(me->seq==ANIM_A2 && me->frm==7)
		{
			MakeSound(SND_BOMBBOOM,me->x,me->y,SND_CUTOFF,1200);
			FireBullet(me,me->x+FIXAMT*96,me->y+FIXAMT*64,0,BLT_SHOCKWAVE,me->friendly);
			ShakeScreen(5);
		}
		// chest pound (go to chest pound second half)
		if(me->seq==ANIM_A3)
		{
			if(me->frm>=5 && (me->frm-5)%5==0)
			{
				ShakeScreen(3);
				if(me->mindControl)
				{
					AddPygmy(map,world,0);
					AddPygmy(map,world,0);
				}
				else
				{
					AddPygmy(map,world,me->friendly);
					AddPygmy(map,world,me->friendly);
				}
				MakeSound(SND_CHESTPOUND,me->x,me->y,SND_CUTOFF,1200);
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
			if(me->mindControl)
			{
				AddPygmy(map,world,0);
				AddPygmy(map,world,0);
			}
			else
			{
				AddPygmy(map,world,me->friendly);
				AddPygmy(map,world,me->friendly);
			}
			MakeSound(SND_CHESTPOUND,me->x,me->y,SND_CUTOFF,1200);
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
		if(goodguy->x>me->x-FIXAMT*225 && goodguy->x<me->x+FIXAMT*225 && goodguy->y<me->y+FIXAMT*190)
		{
			me->dx=0;
			me->dy=0;
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
			if(goodguy->x<me->x-FIXAMT*100)
			{
				if(me->seq!=ANIM_MOVE)
				{
					me->seq=ANIM_MOVE;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->dx=-FIXAMT*3;
				}
			}
			else if(goodguy->x>me->x+FIXAMT*100)
			{
				if(me->seq!=ANIM_A1)
				{
					me->seq=ANIM_A1;
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
					me->dx=FIXAMT*3;
				}
			}
			else
			{
				me->seq=ANIM_IDLE;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=128;
				me->dx=0;
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

void AI_Squash(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp==0)
			MakeSound(SND_PUMPKINDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_DIE && me->reload==0)
		{
			ExplodeParticles(PART_SLIME,me->x,me->y,me->z,8);
			me->reload=2;
		}
		if(me->seq==ANIM_ATTACK && me->frm==3 && me->reload==0)
		{
			me->reload=4;
			if(me->AttackCheck(24,me->x>>FIXSHIFT,me->y>>FIXSHIFT,goodguy))
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*4,Sine(me->facing*32)*4,4,map,world);
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

void AI_UltraPygmy(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;

	if(me->reload)
		me->reload--;

	if(me->ouch==4)
	{
		if(me->hp>0)
			MakeSound(SND_ULTRAPYGMYOUCH,me->x,me->y,SND_CUTOFF,1200);
		else
			MakeSound(SND_ULTRAPYGMYDIE,me->x,me->y,SND_CUTOFF,1200);
	}

	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==6 && me->reload==0 && goodguy)
		{
			x=me->x+Cosine(me->facing*32)*8;
			y=me->y+Sine(me->facing*32)*8;
			if(me->AttackCheck(12,x>>FIXSHIFT,y>>FIXSHIFT,goodguy))
			{
				goodguy->GetShot(me,goodguy,Cosine(me->facing*32)*3,Sine(me->facing*32)*3,6,map,world);
				me->reload=5;
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
			me->frmAdvance=64;
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
					me->frmAdvance=128;
					me->mind1=0;
					me->action=ACTION_BUSY;
					break;
				case 1:
					MakeSound(SND_ULTRAPYGMYYAWN,me->x,me->y,SND_CUTOFF,1200);
					me->seq=ANIM_A2;	// yawn
					me->frm=0;
					me->frmTimer=0;
					me->frmAdvance=128;
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
				MakeSound(SND_ULTRAPYGMYANGRY,me->x,me->y,SND_CUTOFF,1200);
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
			MakeSound(SND_ULTRAPYGMYSTAB,me->x,me->y,SND_CUTOFF,1200);
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
		me->dx=Cosine(me->facing*32)*6;
		me->dy=Sine(me->facing*32)*6;
		if(me->seq!=ANIM_MOVE)
		{
			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=256;
		}
	}
}

void AI_LoonyBot(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->mindControl)
		f=2;
	else
		f=me->friendly;

	if(!goodguy)
		return;

	if(!me->parent)
	{
		me->type=MONS_NONE;
		return;
	}

	if(me->reload)
		me->reload--;

	if(me->parent->ouch>0)
		me->ouch=me->parent->ouch;

	if(me->parent->mind1==0)
	{
		me->parent->x=me->x;
		me->parent->y=me->y;
	}
	else
	{
		me->parent->x=me->x;
		me->parent->y=me->y+FIXAMT*80;
	}
	if(me->action==ACTION_BUSY)
	{
		if(me->seq==ANIM_ATTACK && me->frm==2)
			me->parent->mind1=1;
		if(me->seq==ANIM_ATTACK && me->frm==20)
		{
			me->parent->mind1=0;
			me->frmAdvance=64;
		}
		if(me->seq==ANIM_ATTACK && me->frm==10 && me->reload==0)
		{
			LaunchMegabeam(me,me->x,me->y+96*FIXAMT,me->parent->ID);
			me->frmAdvance=32;
			me->reload=10;
		}
		if(me->seq==ANIM_DIE)
		{
			x=me->x>>FIXSHIFT;
			y=me->y>>FIXSHIFT;
			BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
			BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
					  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
					  me->z,FIXAMT);
			if(me->frm>4)
			{
				BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
				BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
						  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
						  me->z,FIXAMT);
			}
			if(me->frm==7 && me->frmTimer<64)
			{
				AddBaby(me->x,me->y-FIXAMT*96,0,MONS_LOONYSHIP,me);
			}
		}
		return;
	}
	if(me->mind==0)	// waiting for player
	{
		if(RangeToTarget(me,goodguy)<400*FIXAMT || me->parent->ouch)
			me->mind=1;
	}
	else
	{
		if(me->seq!=ANIM_MOVE)
		{
			if(me->mind1==6)
			{
				// time to fire
				me->action=ACTION_BUSY;
				me->seq=ANIM_ATTACK;
				me->frm=0;
				me->frmTimer=0;
				me->frmAdvance=64;
				me->mind1=0;
				me->dx=0;
				me->dy=0;
				return;
			}
			else
			{
				// lob grenades
				MakeSound(SND_MUSHMISSILE,me->x,me->y,SND_CUTOFF,1500);
				FireBullet(me,me->x-87*FIXAMT,me->y-30*FIXAMT,Random(88)+20,BLT_GRENADE,me->friendly);
				FireBullet(me,me->x+92*FIXAMT,me->y-30*FIXAMT,Random(88)+20,BLT_GRENADE,me->friendly);

				// move towards him
				FaceGoodguy(me,goodguy);
				me->dx=Cosine(me->facing*32)*2;
				me->dy=Sine(me->facing*32)*2;
				if(goodguy->y<me->y+FIXAMT*200)
					me->dy=-3*FIXAMT;
				else if(goodguy->y>me->y+FIXAMT*300)
					me->dy=2*FIXAMT;
				else
					me->dy=0;
				if(goodguy->x<me->x-FIXAMT*100)
					me->dx=-2*FIXAMT;
				else if(goodguy->x>me->x+FIXAMT*100)
					me->dx=2*FIXAMT;
				else
					me->dx=0;
			}

			me->seq=ANIM_MOVE;
			me->frm=0;
			me->frmTimer=0;
			me->frmAdvance=128;
			me->mind1++;
			MakeSound(SND_ROBOSTOMP,me->x,me->y,SND_CUTOFF,500);
		}
		else
		{
			if(me->frm==5)
				MakeSound(SND_ROBOSTOMP,me->x,me->y,SND_CUTOFF,500);
		}
	}
}

void AI_LoonyCore(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	// take them with me
	if(me->hp==0)
		KillKids(me);
}

void AI_LoonyGun(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	byte diff;
	char dir;
	int x,y;

	if(me->action==ACTION_BUSY)
	{
		x=me->x>>FIXSHIFT;
		y=me->y>>FIXSHIFT;
		BlowUpGuy(x+me->rectx,y+me->recty,x+me->rectx2,y+me->recty2,me->z,1);
		BlowSmoke((x+me->rectx+Random(me->rectx2-me->rectx))<<FIXSHIFT,
				  (y+me->recty+Random(me->recty2-me->recty))<<FIXSHIFT,
				  me->z,FIXAMT);
		return;
	}
	if(!me->parent || me->parent->mind==0)
		return;

	me->recty+=60;
	me->recty2+=60;

	me->z=FIXAMT*48;
	me->dz=0;
	if(me->mind4==0)
	{
		me->x=me->parent->x-87*FIXAMT;
		me->y=me->parent->y;
	}
	else if(me->mind4==1)
	{
		me->x=me->parent->x+92*FIXAMT;
		me->y=me->parent->y;
	}
	else if(me->mind4==2)
	{
		me->z=me->parent->z+64*FIXAMT;
		me->x=me->parent->x;
		me->y=me->parent->y;
		if(me->facing <= me->parent->facing-32)
		me->facing+=16;
		if(me->facing >= me->parent->facing+32)
		me->facing-=16;
	
		me->brtChange=4;
		strcpy(me->name,"MechaGun");
	}

	if(!goodguy)
		return;

	if(me->mind)
		me->mind--;
	else if(me->mind1<6)
	{
		// rotate towards guy, with 16 facings
		me->mind=me->facing;
		FaceGoodguy(me,goodguy);

		me->facing*=2;

		if(me->mind3==0)	// left gun can't rotate to upper right quadrant
		{
			if(me->facing>10)
			{
				if(me->mind<6)
					me->facing=0;
				else
					me->facing=10;
			}
		}
		else if(me->mind3==1)	// right gun can't rotate to upper left quadrant
		{
			if(me->facing>8 && me->facing<14)
			{
				if(me->mind>13 || me->mind<3)
					me->facing=14;
				else
					me->facing=8;
			}
		}
		if(me->facing>me->mind)
		{
			diff=me->facing-me->mind;
			if(diff>8)
			{
				dir=-1;
				diff=16-diff;
			}
			else
				dir=1;
		}
		else if(me->facing<me->mind)
		{
			diff=me->mind-me->facing;
			if(diff>8)
			{
				dir=1;
				diff=16-diff;
			}
			else
				dir=-1;
		}
		else
		{
			dir=0;
		}
		me->facing=(me->mind+dir)&15;
		me->mind=2;	// rotate counter
	}

	switch(me->mind1)
	{
		case 0:	// waiting to unleash a burst
		case 2:
		case 4:
			if(me->mind2<60)
				me->mind2++;
			else
			{
				me->mind1++;	// next mode
				me->mind2=10;
			}
			break;
		case 1: // firing a burst
		case 3:
		case 5:
			x=me->x+Cosine(me->facing*16)*40;
			y=me->y+Sine(me->facing*16)*32;
			diff=me->facing*16-10+Random(20);
			if(me->mind3!=2)
			FireExactBullet(me,x,y,me->z+12*FIXAMT,Cosine(diff)*6,Sine(diff)*6,-FIXAMT/2,
				0,60,diff,BLT_ENERGY,me->friendly);
				else
			FireExactBullet(me,x,y,me->z+12*FIXAMT,Cosine(diff)*6,Sine(diff)*6,-FIXAMT,
				0,60,diff,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1000);
			if(me->mind2)
				me->mind2--;
			else
				me->mind1++;	// next mode
			break;
		case 6:	// waiting to unleash the doom barrage
			if(me->mind2<60)
				me->mind2++;
			else
				me->mind1=7;
			if((me->mind2&1)==0)
			{
				if(me->mind3==0)	// left gun must rotate up to 10
				{
					if(me->facing<10)
						me->facing=(me->facing+1)&15;
				}
				else if(me->mind3==1)	// right gun rotates 'down' to 14
				{
					if(me->facing!=14)
						me->facing=(me->facing-1)&15;
				}
			}
			break;
		case 7:	// the doom barrage
			x=me->x+Cosine(me->facing*16)*40;
			y=me->y+Sine(me->facing*16)*32;
			diff=me->facing*16-10+Random(20);
			FireExactBullet(me,x,y,me->z+12*FIXAMT,Cosine(diff)*6,Sine(diff)*6,-FIXAMT/2,
				0,60,diff,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1000);
			me->mind2--;
			if((me->mind2&1)==0)
			{
				if(me->mind3==0)
				{
					me->facing=(me->facing-1)&15;
					if(me->facing==15)	// overshot
					{
						me->facing=0;
						me->mind1=0;
						me->mind2=0;
					}
				}
				else if(me->mind3==1)
				{
					me->facing=(me->facing+1)&15;
					if(me->facing==9)	// overshot
					{
						me->facing=8;
						me->mind1=0;
						me->mind2=0;
					}
				}
			}
			break;
	}
}

void AI_LoonyShip(Guy *me,Map *map,world_t *world,Guy *goodguy)
{
	int x,y;
	byte f;

	if(me->reload)
		me->reload--;

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
		return;
	}

	if(!goodguy)
		return;

	FaceGoodguy2(me,goodguy);

	// countdown to turn
	if(me->mind2<10)
		me->mind2++;
	else
	{
		me->mind2=0;
		// turn randomly
		me->mind=(me->mind+1-Random(3))&7;
	}

	me->dx+=Cosine(me->mind*32);
	me->dy+=Sine(me->mind*32);

	Clamp(&me->dx,FIXAMT*7);
	Clamp(&me->dy,FIXAMT*6);

	if(me->reload==0)
	{
		me->reload=20;
		me->mind3=8;
	}
	if(me->mind3)
	{
		if((me->mind3&1)==0)
		{
			x=me->x+Cosine(me->facing*32)*40;
			y=me->y+Sine(me->facing*32)*32;
			f=me->facing*32-5+Random(10);
			FireExactBullet(me,x,y,me->z+12*FIXAMT,Cosine(f)*6,Sine(f)*6,-FIXAMT/2,
				0,60,f,BLT_ENERGY,me->friendly);
			MakeSound(SND_BULLETFIRE,me->x,me->y,SND_CUTOFF,1000);
		}
		me->mind3--;
	}

	// bounce off walls
	if(me->mind1&1)
	{
		me->dx=-me->dx/2;
		me->mind2=10;
		me->mind=(byte)Random(8);
	}
	if(me->mind1&2)
	{
		me->dy=-me->dy/2;
		me->mind2=10;
		me->mind=(byte)Random(8);
	}
	me->mind1=0;
}
