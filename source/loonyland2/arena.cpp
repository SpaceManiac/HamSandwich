#include "arena.h"
#include "leveldef.h"
#include "monster.h"
#include "player.h"
#include "quest.h"
#include "achieve.h"
#include "gallery.h"

arenaMatch_t arenaMatch[MAX_MATCHES]=
{
	// Rank 0 matches
	{"To The Pain!",
		{"Simple fight - Beat all monsters, you win.  Die, you",
		 "lose.  Also, run out of time, you lose."},
	 0,		// rank
	 0,	 	// var
	 10,	// cost
	 15,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 0,		// flags
	 30*60,	// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{2,MONS_PUPPET},{2,MONS_MOUSE},{2,MONS_MOUSE2},{0,0},{0,0}},
		},
	},
	{"Don't Get Shot",
		{"Title speaks for itself.  Remain alive for 30 seconds",
		 "for big prize."},
	 0,		// rank
	 0,		// var
	 10,	// cost
	 15,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 30*30,	// time limit
	 30,	// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{4,MONS_MOUSE2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{1,MONS_MOUSE2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"The Hunt",
		{"The monsters are easy... but can you find them all",
		 "before time runs out?"},
	 0,		// rank
	 0,		// var
	 15,	// cost
	 20,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 30*60,	// time limit
	 0,	// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{16,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Toy Trouble",
		{"Stand strong against wave after wave of angry toys!",
		 "5 waves of enemies in this match."},
	 0,		// rank
	 0,		// var
	 20,	// cost
	 30,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 0,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{8,MONS_MOUSE},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{8,MONS_MOUSE2},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{4,MONS_TEDDY2},{4,MONS_MOUSE},{0,0},{0,0},{0,0}},
			// wave 5
			{{6,MONS_PUPPET},{2,MONS_MONKEY},{0,0},{0,0},{0,0}},
		},
	},
	{"Sibling Rivalry",
		{"Klonk's brother Klunk calls out a blood oath of",
		 "revenge upon you! Are you chicken?"},
	 0,		// rank
	 VAR_KLONKDEAD,	// var
	 30,	// cost
	 40,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_KLUNK},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	// Rank 1 matches
	{"Burly Brawl",
		{"The puppets just keep on coming.  Can you take",
		 "them all?"},
	 1,		// rank
	 0,	 	// var
	 20,	// cost
	 30,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*5,		// spawn rate
	 7,		// wave count
		{
			// wave 1
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 6
			{{8,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 7
			{{1,MONS_PUPPET3},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Fancy Footwork",
		{"We have flooded the arena floor to add to the",
		 "excitement.  Destroy them all!"},
	 1,		// rank
	 0,	 	// var
	 35,	// cost
	 50,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA4,	// level
	 AF_RANDOM,		// flags
	 30*45,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{4,MONS_MOUSE2},{4,MONS_PUPPET},{0,0},{0,0},{0,0}},
		},
	},
	{"The Maze Of Death",
		{"At least you know what you're getting into.  Collect",
		 "all the Souvenir Swords in time."},
	 1,		// rank
	 0,	 	// var
	 40,	// cost
	 55,	// prize
	 AR_COLLECT,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 30*60*2,		// time limit
	 30*15,		// spawn rate
	 3,		// wave count
		{
			// wave 1
			{{5,MONS_PUPPET},{5,MONS_MOUSE},{5,MONS_MOUSE2},{0,0},{0,0}},
			// wave 2
			{{1,MONS_MONKEY},{1,MONS_TINGUY},{0,0},{0,0},{0,0}},
			// wave 3
			{{3,MONS_PUPPET},{3,MONS_MOUSE2},{0,0},{0,0},{0,0}},
			// wave 4 (empty, signal to wrap around)
			{{0,0},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Infested",
		{"The arena has a bit of a rat problem. So we are",
		 "making it an event! Survive for 60 seconds!"},
	 1,		// rank
	 0,	 	// var
	 40,	// cost
	 55,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 30*2,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{60,MONS_MOUSE},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{10,MONS_MOUSE},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Stronghold",
		{"Guard the base as wave after wave of toys strike",
		 "from every direction."},
	 1,		// rank
	 0,	 	// var
	 50,	// cost
	 80,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{20,MONS_MOUSE},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{20,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{15,MONS_MOUSE2},{10,MONS_MOUSE},{0,0},{0,0},{0,0}},
			// wave 4
			{{10,MONS_PUPPET},{5,MONS_MONKEY},{0,0},{0,0},{0,0}},
			// wave 5
			{{5,MONS_TINGUY},{5,MONS_MONKEY},{0,0},{0,0},{0,0}},
		},
	},
	// Rank 2
	{"Burlier Brawl",
		{"WAY more puppets than before.",
		 ""},
	 2,		// rank
	 0,	 	// var
	 50,	// cost
	 80,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*10,		// spawn rate
	 7,		// wave count
		{
			// wave 1
			{{30,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{30,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{30,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{30,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{30,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
			// wave 6
			{{5,MONS_PUPPET2},{0,0},{0,0},{0,0},{0,0}},
			// wave 7
			{{5,MONS_PUPPET3},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Blunderdome",
		{"You have only 20 seconds to demolish all who oppose",
		 "you.  Good luck!"},
	 2,		// rank
	 0,	 	// var
	 60,	// cost
	 100,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 30*20,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{4,MONS_MONKEY},{4,MONS_TEDDY2},{0,0},{0,0},{0,0}},
			// wave 2 (empty)
			{{0,0},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Cold War",
		{"The arena's been flooded again... but someone left",
		 "the windows open all night!"},
	 2,		// rank
	 0,	// var
	 60,	// cost
	 100,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA3,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 4,		// wave count
		{
			// wave 1
			{{4,MONS_ICEMICE},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{8,MONS_TINGUY},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{2,MONS_ICEMICE},{4,MONS_TINGUY},{0,0},{0,0},{0,0}},
			// wave 4
			{{3,MONS_ICEMICE},{3,MONS_MONKEY},{3,MONS_TINGUY},{3,MONS_TEDDY2},{0,0}},
		},
	},
	{"It's Not Easy",
		{"This would be no problem at all... if we didn't pump",
		 "you full of deadly poison before the match."},
	 2,		// rank
	 0,	 	// var
	 70,	// cost
	 120,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM|AF_POISON,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{5,MONS_PUPPET},{5,MONS_MOUSE2},{5,MONS_MONKEY},{5,MONS_TINGUY},{5,MONS_TEDDY2}},
		},
	},
	{"Sock It To Ya",
		{"Are there monkeys?  Terrible sock monkeys?",
		 "Yes, there are."},
	 2,		// rank
	 0,	 	// var
	 70,	// cost
	 120,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*10,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{10,MONS_MONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{10,MONS_MONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{10,MONS_MONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{10,MONS_MONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{1,MONS_MONKEY2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	// Rank 3
	{"Klones",
		{"For added fun, we have cloned Klunk.  Sadly, the",
		 "clones have mutant healing powers."},
	 3,		// rank
	 VAR_KLONKDEAD,	 	// var
	 70,	// cost
	 120,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM|AF_HEALING,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{2,MONS_KLUNK},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Bear Room Brawl",
		{"The room is filled with bears.  It's a necessity that",
		 "you slay them."},
	 3,		// rank
	 VAR_QUESTDONE+QUEST_BIGBEAR,	 	// var
	 80,	// cost
	 140,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,			// time limit
	 0,			// spawn rate
	 4,		// wave count
		{
			// wave 1
			{{20,MONS_TEDDY2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{4,MONS_TEDDY},{8,MONS_TEDDY2},{0,0},{0,0},{0,0}},
			// wave 3
			{{6,MONS_TEDDY},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{3,MONS_TEDDY3},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Garden Of Pain",
		{"Looks like the crops are coming up nicely this year.",
		 "Can you put them back in the ground?"},
	 3,		// rank
	 0,	 	// var
	 80,	// cost
	 140,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{4,MONS_ROSE2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{2,MONS_ROSE},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"In The Army Now",
		{"Guard the base, soldier!  The evil tin army is",
		 "invading!"},
	 3,		// rank
	 0,	 	// var
	 90,	// cost
	 160,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*10,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{10,MONS_TINGUY},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{10,MONS_TINGUY},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{10,MONS_TINGUY},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{10,MONS_TINGUY},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{4,MONS_TINGUY2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Beyond Blunderdome",
		{"It's like Blunderdome, only beyond.  30 seconds is the",
		 "limit this time."},
	 3,		// rank
	 0,	 	// var
	 100,	// cost
	 200,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA4,	// level
	 AF_RANDOM,		// flags
	 30*30,		// time limit
	 30*2,		// spawn rate
	 3,		// wave count
		{
			// wave 1
			{{2,MONS_PUPPET2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{4,MONS_PUPPET3},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{15,MONS_PUPPET},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"12 Monkeys",
		{"You face the dreaded army of the 12 monkeys!",
		 "Beware their deadly virus."},
	 4,		// rank
	 0,	 	// var
	 120,	// cost
	 180,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA4,	// level
	 AF_RANDOM,		// flags
	 30*30,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{12,MONS_MONKEY2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"The Cliffs Of Insanity",
		{"Relive the horrors of the Frosty Cliffs.",
		 "This time on level ground."},
	 4,		// rank
	 0,	 	// var
	 120,	// cost
	 190,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*10,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{5,MONS_TEDDY},{5,MONS_ROSE2},{5,MONS_PUPPET3},{0,0},{0,0}},
			// wave 2
			{{4,MONS_PUPPET3},{4,MONS_TEDDY},{4,MONS_ROSE2},{0,0},{0,0}},
			// wave 3
			{{3,MONS_PUPPET3},{3,MONS_TEDDY},{3,MONS_TEDDY},{0,0},{0,0}},
			// wave 4
			{{2,MONS_PUPPET3},{2,MONS_TEDDY},{2,MONS_TEDDY},{0,0},{0,0}},
			// wave 5
			{{1,MONS_PUPPET3},{1,MONS_TEDDY},{1,MONS_TEDDY},{0,0},{0,0}},
		},
	},
	{"Thuggin' Love",
		{"Don't you just love guys with giant spiked maces?",
		 "Don't let a single one hit you!"},
	 4,		// rank
	 VAR_ONIONCHAT,	 	// var
	 130,	// cost
	 200,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_PERFECT|AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 3,		// wave count
		{
			// wave 1
			{{3,MONS_THUG},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{3,MONS_THUG},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{1,MONS_BRUISER},{0,0},{0,0},{0,0},{0,0}},

		},
	},
	{"They're All Ears",
		{"Can you defeat an entire Snuggly Bunny horde",
		 "without ever getting hit?"},
	 4,		// rank
	 VAR_ONIONCHAT,	 	// var
	 130,	// cost
	 200,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_PERFECT|AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 3,		// wave count
		{
			// wave 1
			{{3,MONS_BUNNYTROOP},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{3,MONS_BUNNYTROOP},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{1,MONS_EVILBOT},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Tag Team",
		{"Team up with both Onion Thugs and Order Knights",
		 "to hold off Bruisers and Clockbots."},
	 4,		// rank
	 VAR_ONIONCHAT,	 	// var
	 150,	// cost
	 250,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA1,	// level
	 AF_TEAM|AF_RANDOM,		// flags
	 30*60,		// time limit
	 30*5,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{1,MONS_BRUISER},{1,MONS_EVILBOT},{0,0},{4,MONS_BUNNYTROOP},{4,MONS_THUG}},
			// wave 2
			{{1,MONS_BRUISER},{1,MONS_EVILBOT},{0,0},{0,0},{0,0}},
		},
	},
	{"Factory Redux",
		{"Did you miss the Toy Factory?  It's back, and its",
		 "pollution is slowly killing you!"},
	 5,		// rank
	 VAR_QUESTDONE+QUEST_FACTORY,	 	// var
	 150,	// cost
	 220,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_POISON,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{8,MONS_TOYMAKER},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Monkey Business",
		{"Protect the Arena Crystal from waves of rabid",
		 "monkeys."},
	 5,		// rank
	 0,	 	// var
	 160,	// cost
	 240,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM|AF_PROTECT,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{20,MONS_MONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{5,MONS_MONKEY2},{10,MONS_MONKEY},{0,0},{0,0},{0,0}},
			// wave 3
			{{40,MONS_MONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{10,MONS_GHOSTMONKEY},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{10,MONS_MONKEY3},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Feeling Crabby",
		{"We're miles from the ocean, yet still the crabs keep",
		 "on coming."},
	 5,		// rank
	 0,	 	// var
	 170,	// cost
	 260,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*2+15,		// spawn rate
	 10,		// wave count
		{
			// wave 1
			{{6,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{3,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{3,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{3,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{2,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 6
			{{2,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 7
			{{2,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 8
			{{2,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 9
			{{2,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 10
			{{2,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Red Claws",
		{"Everyone loves Bodzhas!  They're so cuddly, they'll",
		 "cuddle your face right off."},
	 5,		// rank
	 0,	 	// var
	 180,	// cost
	 270,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 30*5,		// spawn rate
	 4,		// wave count
		{
			// wave 1
			{{10,MONS_BADGER},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{5,MONS_BADGER},{5,MONS_LION},{0,0},{0,0},{0,0}},
			// wave 3
			{{5,MONS_BADGER},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{5,MONS_BADGER},{5,MONS_LION},{0,0},{0,0},{0,0}},
		},
	},
	{"Knives & Stampedes",
		{"Go mano a mano with the fiercest gun in the west",
		 "(ern side of Winter Woods)."},
	 5,		// rank
	 0,	 	// var
	 200,	// cost
	 300,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 0,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_TRIGUN},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Ghostbusting",
		{"The maze has become haunted.  Cleanse it!",
		 ""},
	 6,		// rank
	 0,	 	// var
	 200,	// cost
	 300,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{10,MONS_GHOSTDOG},{10,MONS_GHOSTMONKEY},{10,MONS_BADGHOST},{5,MONS_BADGHOST2},{0,0}},
		},
	},
	{"Ganging Up",
		{"Sadly, the Onion Ring and The Order Of The",
		 "Snuggly Bunny have teamed up to squish you."},
	 6,		// rank
	 VAR_ONIONCHAT,	 	// var
	 200,	// cost
	 300,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 4,		// wave count
		{
			// wave 1
			{{5,MONS_BUNNYTROOP},{5,MONS_THUG},{0,0},{0,0},{0,0}},
			// wave 2
			{{2,MONS_EVILBOT},{2,MONS_BRUISER},{0,0},{0,0},{0,0}},
			// wave 3
			{{10,MONS_BUNNYTROOP},{10,MONS_THUG},{0,0},{0,0},{0,0}},
			// wave 1
			{{5,MONS_BUNNYTROOP},{5,MONS_THUG},{2,MONS_BRUISER},{2,MONS_EVILBOT},{0,0}},
		},
	},
	{"The Crowd",
		{"You have risen to great heights... now those you trod",
		 "upon want their revenge!"},
	 6,		// rank
	 0,	 	// var
	 200,	// cost
	 300,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 30*90,		// time limit
	 30*10,		// spawn rate
	 6,		// wave count
		{
			// wave 1
			{{20,MONS_PUPPET},{50,MONS_MOUSE},{20,MONS_MOUSE2},{0,0},{0,0}},
			// wave 2
			{{20,MONS_TINGUY},{20,MONS_MONKEY},{0,0},{0,0},{0,0}},
			// wave 3
			{{20,MONS_TEDDY2},{20,MONS_MOUSE2},{0,0},{0,0},{0,0}},
			// wave 4
			{{20,MONS_TEDDY},{20,MONS_HOUND},{0,0},{0,0},{0,0}},
			// wave 5
			{{10,MONS_TINGUY},{10,MONS_MONKEY},{10,MONS_MOUSE4},{10,MONS_TEDDY2},{10,MONS_TEDDY}},
			// wave 6
			{{10,MONS_KLUNK},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Junkyard Wars",
		{"A group of local Junksmiths has entered the",
		 "tournament.  Trash them!"},
	 6,		// rank
	 VAR_ONIONCHAT,	 	// var
	 220,	// cost
	 330,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{5,MONS_JUNKSMITH},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Caught Between",
		{"Rock, meet hard place.  You get in middle!",
		 ""},
	 6,		// rank
	 0,	 	// var
	 230,	// cost
	 340,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 3,		// wave count
		{
			// wave 1
			{{15,MONS_CRAB},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{4,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{2,MONS_STONEWALL},{10,MONS_CRAB},{0,0},{0,0},{0,0}},
		},
	},
	{"Quake Deathmatch",
		{"Grab onto something, because the arena's shaking!",
		 ""},
	 7,		// rank
	 0,	 	// var
	 240,	// cost
	 360,	// prize
	 AR_PRESSURE,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 30*10,		// spawn rate
	 5,		// wave count
		{
			// wave 1
			{{5,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{5,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{5,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{5,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
			// wave 5
			{{5,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Reinfested",
		{"It's not rats this time.",
		 ""},
	 7,		// rank
	 0,	 	// var
	 250,	// cost
	 370,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 30*2,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{60,MONS_TINGUY2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{10,MONS_TINGUY2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Holy Floor",
		{"Battle beastly Bodzhas burrowing briskly beneath",
		 "your boots!"},
	 7,		// rank
	 0,	 	// var
	 260,	// cost
	 380,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 30*90,		// time limit
	 0,		// spawn rate
	 4,		// wave count
		{
			// wave 1
			{{5,MONS_BADGER2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{5,MONS_BADGER2},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{10,MONS_BADGER2},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{20,MONS_BADGER2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Cold As Ice",
		{"These foes are precisely as cold as ice.  The floor",
		 "is too."},
	 7,		// rank
	 VAR_QUESTASSIGN+QUEST_FURNACE,	 	// var
	 270,	// cost
	 400,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA3,	// level
	 AF_HEALING,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{8,MONS_TITAN},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Titanica",
		{"Titans big.  You small.  Titans win!",
		 ""},
	 7,		// rank
	 VAR_QUESTDONE+QUEST_FURNACE,	 	// var
	 300,	// cost
	 450,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{1,MONS_TITAN2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{2,MONS_TITAN2},{4,MONS_TITAN},{0,0},{0,0},{0,0}},
		},
	},
	{"Save Tokyo!",
		{"This is a re-enactment of a terrible battle that",
		 "leveled a distant city."},
	 8,		// rank
	 VAR_QUESTDONE+QUEST_PLANTS,	 	// var
	 320,	// cost
	 470,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_HEALING,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_BIGPLANT},{7,MONS_LION2},{0,0},{0,0},{0,0}},
		},
	},
	{"We Don't Need 'Em",
		{"Stinkin' Bodzhas!  Keep the Arena Crystal safe from",
		 "them.  They seem to be attracted to crystals."},
	 8,		// rank
	 VAR_QUESTDONE+QUEST_TITANS2,	 	// var
	 340,	// cost
	 500,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM|AF_PROTECT,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 4,		// wave count
		{
			// wave 1
			{{1,MONS_BADGER3},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{2,MONS_BADGER3},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{3,MONS_BADGER3},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{6,MONS_BADGER3},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Could Be Dandier",
		{"I hope you don't have allergies.",
		 "Defeat 6 waves of polleny fiends!"},
	 8,		// rank
	 0,	 	// var
	 350,	// cost
	 520,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 6,		// wave count
		{
			// wave 1
			{{20,MONS_LION},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{6,MONS_LION2},{0,0},{0,0},{0,0},{0,0}},
			// wave 3
			{{4,MONS_BURNINGBUSH},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{10,MONS_LION},{5,MONS_LION2},{0,0},{0,0},{0,0}},
			// wave 5
			{{10,MONS_LION2},{0,0},{0,0},{0,0},{0,0}},
			// wave 6
			{{10,MONS_LION},{4,MONS_LION2},{2,MONS_BURNINGBUSH},{0,0},{0,0}},
		},
	},
	{"Obstacle Course",
		{"Explore our vast arena maze, seeking out the",
		 "Souvenir Swords while enduring... obstacles."},
	 8,		// rank
	 0,	 	// var
	 360,	// cost
	 540,	// prize
	 AR_COLLECT,	// rules
	 LVL_ARENA2,	// level
	 AF_HEALING|AF_RANDOM,		// flags
	 30*90,		// time limit
	 30*30,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{10,MONS_TURRET},{0,0},{0,0},{0,0},{0,0}},
			// wave 1
			{{0,0},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"My, What Big Ears",
		{"A new challenger has joined our proud arena, and it",
		 "seems he is seeking revenge..."},
	 8,		// rank
	 VAR_QUESTDONE+QUEST_FURNACE,	 	// var
	 380,	// cost
	 580,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_HEALING,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_BUNNYBOSS2},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Big Bodzha Battle",
		{"No waves, no timer, no tricks.  Just every Bodzha",
		 "from within 100 miles, all at once."},
	 9,		// rank
	 0,	 	// var
	 400,	// cost
	 600,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{50,MONS_BADGER},{50,MONS_BADGER2},{50,MONS_BADGER3},{0,0},{0,0}},
		},
	},
	{"System Malfunction",
		{"The prototype toys have malfunctioned and are",
		 "running amok.  Defend yourself!"},
	 9,		// rank
	 VAR_QUESTDONE+QUEST_TOYS2,	 	// var
	 430,	// cost
	 640,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA1,	// level
	 AF_HEALING,		// flags
	 30*60,		// time limit
	 30*1,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{8,MONS_PROTOTYPE},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{1,MONS_PROTOTYPE},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"On Thin Ice",
		{"The enemies aren't so tough.  But you lose if they",
		 "scratch you!"},
	 9,		// rank
	 0,	 	// var
	 450,	// cost
	 680,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA3,	// level
	 AF_PERFECT,		// flags
	 30*30,		// time limit
	 30*5,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{3,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
			// wave 1
			{{2,MONS_STONEWALL},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Really Don't Get Shot",
		{"You won't survive this.  Save your money.",
		 ""},
	 9,		// rank
	 0,	 	// var
	 480,	// cost
	 700,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_HEALING,		// flags
	 30*60,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{8,MONS_TURRET},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Pot, Meet Kettle",
		{"All you have to do is clear 10 waves.  But",
		 "somebody seems to be getting in the way!"},
	 9,		// rank
	 0,	 	// var
	 1000,	// cost
	 2000,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 10,		// wave count
		{
			// wave 1
			{{1,MONS_TINPOT},{10,MONS_TEDDY4},{0,0},{0,0},{0,0}},
			// wave 2
			{{1,MONS_TINPOT},{10,MONS_MONKEY4},{0,0},{0,0},{0,0}},
			// wave 3
			{{1,MONS_TINPOT},{10,MONS_MOUSE5},{0,0},{0,0},{0,0}},
			// wave 4
			{{1,MONS_TINPOT},{10,MONS_TRIGUN},{0,0},{0,0},{0,0}},
			// wave 5
			{{1,MONS_TINPOT},{10,MONS_BADGER3},{0,0},{0,0},{0,0}},
			// wave 6
			{{2,MONS_TINPOT},{10,MONS_TEDDY4},{0,0},{0,0},{0,0}},
			// wave 7
			{{2,MONS_TINPOT},{10,MONS_MONKEY4},{0,0},{0,0},{0,0}},
			// wave 8
			{{2,MONS_TINPOT},{10,MONS_MOUSE5},{0,0},{0,0},{0,0}},
			// wave 9
			{{2,MONS_TINPOT},{10,MONS_TRIGUN},{0,0},{0,0},{0,0}},
			// wave 10
			{{4,MONS_TINPOT},{3,MONS_BADGER3},{3,MONS_TEDDY4},{3,MONS_MONKEY4},{3,MONS_MOUSE5}},
		},
	},
	{"Not Just A Job",
		{"One on one match against one of the top warriors",
		 "in the arena!  Hoo-ah!"},
	 10,		// rank
	 0,	 	// var
	 2000,	// cost
	 2500,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_ARMYONE},{0,0},{0,0},{0,0},{0,0}},

		},
	},
	{"The Three Bears",
		{"Watch out for these bears - they have an extremely",
		 "vengeful nature!"},
	 10,		// rank
	 0,	 	// var
	 2100,	// cost
	 2600,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{3,MONS_TEDDY5},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{100,MONS_TEDDY},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Victory Garden",
		{"In Winter Woods, garden has victory over you!",
		 "Unless you win."},
	 10,		// rank
	 0,	 	// var
	 2200,	// cost
	 2700,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 2,		// wave count
		{
			// wave 1
			{{16,MONS_ROSE4},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{1,MONS_ROSE5},{0,0},{0,0},{0,0},{0,0}},
		},
	},
	{"Gorka Park",
		{"This one's just a walk in the park!",
		 "A poisonous park full of mutants."},
	 10,		// rank
	 0,	 	// var
	 3000,	// cost
	 3600,	// prize
	 AR_SURVIVE,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM|AF_POISON|AF_HEALING,		// flags
	 30*60,		// time limit
	 30*7,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{10,MONS_GORKA2},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{3,MONS_GORKA2},{0,0},{0,0},{0,0},{0,0}},

		},
	},
	{"The Championship",
		{"You have proven yourself against all other foes.",
		 "Now face the champion himself!"},
	 10,		// rank
	 0,	 	// var
	 5000,	// cost
	 6000,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_KILLBOR},{0,0},{0,0},{0,0},{0,0}},

		},
	},
#ifdef DIRECTORS
	{"Hush Puppies",
		{"You have 1 minute to slay as many Hot Dogs as you",
		 "can.  Prize money is paid per Dog."},
	 11,		// rank
	 VAR_MADCAP,	 	// var
	 4000,	// cost
	 100,	// prize
	 AR_SLAY,	// rules
	 LVL_ARENA2,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 30*1,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{20,MONS_HOTDOG},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{1,MONS_HOTDOG},{0,0},{0,0},{0,0},{0,0}},

		},
	},
	{"Frigid Air",
		{"Slay as many Iceboxes as you can in 1 minute.",
		 "You earn the prize money for each one."},
	 11,		// rank
	 VAR_MADCAP,	 	// var
	 5000,	// cost
	 800,	// prize
	 AR_SLAY,	// rules
	 LVL_ARENA3,	// level
	 AF_RANDOM,		// flags
	 30*60,		// time limit
	 30*10,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{2,MONS_ICEBOX},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{2,MONS_ICEBOX},{0,0},{0,0},{0,0},{0,0}},

		},
	},
	{"Garden Of Bleedin'",
		{"Can you weed the garden in just one and a",
		 "half minutes?"},
	 11,		// rank
	 VAR_MADCAP,	 	// var
	 6000,	// cost
	 7000,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_HEALING,		// flags
	 30*60+30*30,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_DEATHBERRY},{3,MONS_ROSE2},{1,MONS_DEATHBERRY},{3,MONS_ROSE2},{0,0}},
		},
	},
	{"The Red Baron",
		{"A dogfight with an ace necromancer.",
		 "No holds barred, mano a mano."},
	 11,		// rank
	 VAR_MADCAP,	 	// var
	 7000,	// cost
	 8500,	// prize
	 AR_BRAWL,	// rules
	 LVL_ARENA1,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 1,		// wave count
		{
			// wave 1
			{{1,MONS_REDBARON},{0,0},{0,0},{0,0},{0,0}},

		},
	},
	{"Ultimate Cage Match",
		{"The title says it all (although there is no cage,",
		 "so ignore that part)."},
	 11,		// rank
	 VAR_MADCAP,	 	// var
	 10000,	// cost
	 20000,	// prize
	 AR_WAVES,	// rules
	 LVL_ARENA5,	// level
	 AF_RANDOM,		// flags
	 0,		// time limit
	 0,		// spawn rate
	 10,		// wave count
		{
			// wave 1
			{{10,MONS_KLUNK},{0,0},{0,0},{0,0},{0,0}},
			// wave 2
			{{5,MONS_MOUSE3},{5,MONS_MONKEY2},{5,MONS_ROSE},{0,0},{0,0}},
			// wave 3
			{{10,MONS_TEDDY3},{0,0},{0,0},{0,0},{0,0}},
			// wave 4
			{{10,MONS_TRIGUN},{5,MONS_JUNKSMITH},{0,0},{0,0},{0,0}},
			// wave 5
			{{2,MONS_BIGPLANT},{3,MONS_BUNNYBOSS2},{0,0},{0,0},{0,0}},
			// wave 6
			{{4,MONS_TITAN2},{20,MONS_TITAN},{0,0},{0,0},{0,0}},
			// wave 7
			{{5,MONS_TURRET},{20,MONS_MOUSE5},{4,MONS_TINPOT},{5,MONS_MONKEY4},{5,MONS_TEDDY4}},
			// wave 8
			{{20,MONS_GORKA2},{3,MONS_ARMYONE},{0,0},{0,0},{0,0}},
			// wave 9
			{{10,MONS_BADGHOST5},{2,MONS_KILLBOR},{0,0},{0,0},{0,0}},
			// wave 10
			{{4,MONS_BIRD},{0,0},{0,0},{0,0},{0,0}},
		},
	},
#endif
	// name ideas
	// amber waves of pain
	// death capades (on an ice map)
};

int FindRandomSpawn(Map *map)
{
	int pos;

	pos=rand()%(map->width*map->height);
	while(map->map[pos].tag!=1)
		pos=rand()%(map->width*map->height);

	return pos;
}

byte ArenaAddWave(Map *map)
{
	int set,which;
	int mapPos,count;
	byte emptyWave;

	if(player.arenaWave>=MAX_WAVES)
		return 0;

	if(arenaMatch[player.arenaLvl].rules==AR_WAVES)
		PurgeMonsterSprites();

	if(!(arenaMatch[player.arenaLvl].flags&AF_RANDOM))
	{
		mapPos=0;
		// cycle around until you find a spawn spot
		while(map->map[mapPos].tag!=1)
			mapPos++;
	}
	else
		mapPos=FindRandomSpawn(map);

	emptyWave=1;
	for(set=0;set<5;set++)
	{
		count=arenaMatch[player.arenaLvl].waves[player.arenaWave][set].count;
		if(ModifierOn(MOD_CROWDED))
			count*=2;

		for(which=0;which<count;which++)
		{
			Guy *g;

			emptyWave=0;
			g=AddGuy(((mapPos%map->width)*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,((mapPos/map->width)*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,
				arenaMatch[player.arenaLvl].waves[player.arenaWave][set].type);

			if(g)
			{
				map->map[g->mapx+g->mapy*map->width].templight=31;
				g->bright=32;
				g->team=EVIL;	// no friends in the arena!  usually.
				if((arenaMatch[player.arenaLvl].flags&AF_TEAM) && set>=3)
					g->team=GOOD;
				g->hp=MonsterHP(g->type,g->team);
				FXRing(0,g->x,g->y,g->z,16,1);
			}
			if(!(arenaMatch[player.arenaLvl].flags&AF_RANDOM))
			{
				mapPos++;
				while(map->map[mapPos].tag!=1)
				{
					mapPos++;
					if(mapPos>=map->width*map->height)
						mapPos=0;
				}
			}
			else
				mapPos=FindRandomSpawn(map);
		}

	}
	if(emptyWave)
		return 0;
	else
	{
		return 1;
	}
}

byte ArenaCheckWave(Map *map)
{
	int set;

	if(player.arenaWave>=MAX_WAVES)
		return 1;

	for(set=0;set<5;set++)
	{
		if(arenaMatch[player.arenaLvl].waves[player.arenaWave][set].count)
			return 0;
	}
	return 1;
}

void ScatterSwords(Map *map)
{
	int i,n;

	n=10;
	while(n>0)
	{
		i=rand()%(map->width*(map->height-1));	// can't be in bottommost row
		if(map->map[i].wall==0 && map->map[i].item==0 && (map->map[i+map->width].wall==0))
		{
			map->map[i].item=IT_SWORD;
			n--;
		}
	}
}

void SetupArena(Map *map)
{
	if(levelDef[player.levelNum].flags&LF_ARENA)
	{
		player.arenaHearts=player.hearts;
		player.arenaMagic=player.magic;
		player.arenaStamina=player.stamina;

		player.arenaSpawn=0;
		player.arenaTime=0;
		player.arenaWave=0;
		HealGoodguy(9999);
		player.stamina=player.maxStamina;
		player.magic=player.maxMagic;
		ArenaAddWave(map);
		if(arenaMatch[player.arenaLvl].rules==AR_COLLECT)
		{
			player.var[VAR_SWORDS]=0;
			ScatterSwords(map);
		}
		if(arenaMatch[player.arenaLvl].flags&AF_PROTECT)
		{
			AddGuy(goodguy->x,goodguy->y-TILE_HEIGHT,0,MONS_ARENACRYSTAL);
		}
	}
}

void ResetPlayer(void)
{
	player.hearts=player.arenaHearts;
	player.magic=player.arenaMagic;
	player.stamina=player.arenaStamina;
	PlayerCalcStats();
}

void WinArena(void)
{
	dword prize;

	MakeNormalSound(SND_ARENAWIN);
	NewBigMessage("VICTORY!",60,2);
	prize=arenaMatch[player.arenaLvl].prize;
	if(ModifierOn(MOD_ARENA))
		prize*=5;
	if(player.var[VAR_MADCAP])
		prize*=2;
	if(arenaMatch[player.arenaLvl].rules==AR_SLAY)
	{
		char s[48];
		sprintf(s,"You won $%d!",prize*player.var[VAR_TEMP]);
		NewMessage(s,0);
		PlayerGetMoney(prize*player.var[VAR_TEMP]);
	}
	else
		PlayerGetMoney(prize);
	player.destx=12;
	player.desty=4;
	if(player.var[VAR_ARENAWINS]<255)
		player.var[VAR_ARENAWINS]++;
	if(player.var[VAR_ARENAWINS]>=100)
		AchieveNow(9);
	SendMessageToGame(MSG_GOTOMAP,LVL_ARENALOBBY);
	TalentPoint(TLT_GLADIATOR,arenaMatch[player.arenaLvl].rank+1);
	goodguy->poison=0;
	ResetPlayer();
#ifdef DIRECTORS
	if(player.arenaLvl>=57 && player.arenaLvl<=59)
		picToDrop=player.arenaLvl-57+10;
	else if(player.arenaLvl==56 && player.var[VAR_TEMP]>=10)
		picToDrop=9;
	else if(player.arenaLvl==55 && player.var[VAR_TEMP]>=50)
		picToDrop=8;
#endif
}

void LoseArena(void)
{
	MakeNormalSound(SND_ARENALOSE);
	NewBigMessage("DEFEAT!",60,2);
	player.destx=12;
	player.desty=4;
	SendMessageToGame(MSG_GOTOMAP,LVL_ARENALOBBY);
	ResetPlayer();
}

byte SwordsExist(Map *map)
{
	int i;

	for(i=0;i<map->width*map->height;i++)
		if(map->map[i].item==IT_SWORD)
			return 1;

	return 0;
}

void UpdateArena(void)
{
	if(!(levelDef[player.levelNum].flags&LF_ARENA))
		return;

	player.arenaTime++;
	player.arenaSpawn++;

	if((arenaMatch[player.arenaLvl].flags&AF_HEALING) && (player.arenaTime%10)==0)
	{
		word w;

		w=GuyList_First();
		while(w!=65535)
		{
			if(GetGuy(w)->type && GetGuy(w)->team==EVIL && GetGuy(w)->hp)
			{
				if(player.var[VAR_MADCAP])
					HealGuy(GetGuy(w),10);
				else
					HealGuy(GetGuy(w),1);
			}
			w=GuyList_Next();
		}
	}

	if((arenaMatch[player.arenaLvl].flags&AF_POISON) && (goodguy->poison<5))
		goodguy->GetPoisoned(60);

	if((arenaMatch[player.arenaLvl].flags&AF_PERFECT) && goodguy->ouch)
		LoseArena();

	if(arenaMatch[player.arenaLvl].flags&AF_PROTECT)
	{
		if(!MonsterExists(MONS_ARENACRYSTAL))
			LoseArena();
	}
	switch(arenaMatch[player.arenaLvl].rules)
	{
		case AR_BRAWL:
			if(!AnyMonsterExists())
			{
				WinArena();
			}
			if(arenaMatch[player.arenaLvl].timeLimit!=0 && player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				LoseArena();
			break;
		case AR_WAVES:
			if(!AnyMonsterExists())
			{
				player.arenaWave++;
				if(player.arenaWave>9)
				{
					player.arenaWave=9;
					WinArena();
					return;
				}
				if(!ArenaAddWave(curMap))
				{
					player.arenaWave--;
					WinArena();
				}
			}
			if(arenaMatch[player.arenaLvl].timeLimit!=0 && player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				LoseArena();
			break;
		case AR_SURVIVE:
			if(player.arenaSpawn>=arenaMatch[player.arenaLvl].spawnRate)
			{
				player.arenaSpawn=0;
				player.arenaWave=1;
				ArenaAddWave(curMap);
			}
			if(arenaMatch[player.arenaLvl].timeLimit!=0 && player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				WinArena();
			break;
		case AR_PRESSURE:
			if(player.arenaSpawn>=arenaMatch[player.arenaLvl].spawnRate)
			{
				player.arenaSpawn=0;
				player.arenaWave++;
				ArenaAddWave(curMap);
			}
			if(!AnyMonsterExists() && ArenaCheckWave(curMap))	// make sure you are past all the waves
				WinArena();
			if(arenaMatch[player.arenaLvl].timeLimit!=0 && player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				LoseArena();
			break;
		case AR_COLLECT:
			if(player.arenaSpawn>=arenaMatch[player.arenaLvl].spawnRate)
			{
				player.arenaSpawn=0;
				player.arenaWave++;
				if(!ArenaAddWave(curMap))
				{
					player.arenaWave=0;
					ArenaAddWave(curMap);
				}
			}
			if(!SwordsExist(curMap))
				WinArena();
			if(arenaMatch[player.arenaLvl].timeLimit!=0 && player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				LoseArena();
			break;
		case AR_SLAY:
			if(player.arenaSpawn>=arenaMatch[player.arenaLvl].spawnRate)
			{
				player.arenaSpawn=0;
				player.arenaWave=1;
				ArenaAddWave(curMap);
			}
			if(arenaMatch[player.arenaLvl].timeLimit!=0 && player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				WinArena();
			break;
	}
}

void PrintArenaInfo(int y)
{
	char s[16];
	word w,wv;

	CenterPrintDark(319,-29-1+y,arenaMatch[player.arenaLvl].name,0);
	CenterPrintDark(321,-29+1+y,arenaMatch[player.arenaLvl].name,0);
	CenterPrint(320,-29+y,arenaMatch[player.arenaLvl].name,0,0);

	s[0]='\0';
	switch(arenaMatch[player.arenaLvl].rules)
	{
		case AR_BRAWL:
			// time if need
			if(arenaMatch[player.arenaLvl].timeLimit!=0)
			{
				if(player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
					w=0;
				else
					w=(arenaMatch[player.arenaLvl].timeLimit-player.arenaTime)/30;
				sprintf(s,"%d:%02d",w/60,w%60);
			}
			break;
		case AR_WAVES:
			// wave #? and time if needed
			if(arenaMatch[player.arenaLvl].timeLimit!=0)
			{
				if(player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
					w=0;
				else
					w=(arenaMatch[player.arenaLvl].timeLimit-player.arenaTime)/30;
				sprintf(s,"Wave %d/%d   %d:%02d",player.arenaWave+1,arenaMatch[player.arenaLvl].waveCnt,w/60,w%60);
			}
			else
				sprintf(s,"Wave %d/%d",player.arenaWave+1,arenaMatch[player.arenaLvl].waveCnt);
			break;
		case AR_SURVIVE:
			// time
			if(player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				w=0;
			else
				w=(arenaMatch[player.arenaLvl].timeLimit-player.arenaTime)/30;
			sprintf(s,"%d:%02d",w/60,w%60);
			break;
		case AR_PRESSURE:
			// time
			wv=player.arenaWave+1;
			if(wv>arenaMatch[player.arenaLvl].waveCnt)
				wv=arenaMatch[player.arenaLvl].waveCnt;
			if(arenaMatch[player.arenaLvl].timeLimit)
			{
				if(player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
					w=0;
				else
					w=(arenaMatch[player.arenaLvl].timeLimit-player.arenaTime)/30;
				if(arenaMatch[player.arenaLvl].waveCnt>1)
					sprintf(s,"Wave %d/%d   %d:%02d",wv,arenaMatch[player.arenaLvl].waveCnt,w/60,w%60);
				else
					sprintf(s,"%d:%02d",w/60,w%60);
			}
			else
			{
				if(arenaMatch[player.arenaLvl].waveCnt>1)
					sprintf(s,"Wave %d/%d",wv,arenaMatch[player.arenaLvl].waveCnt);
			}
			break;
		case AR_COLLECT:
			w=(arenaMatch[player.arenaLvl].timeLimit-player.arenaTime)/30;
			sprintf(s,"%d/10 %d:%02d",player.var[VAR_SWORDS],w/60,w%60);
			break;
		case AR_SLAY:
			// time
			if(player.arenaTime>arenaMatch[player.arenaLvl].timeLimit)
				w=0;
			else
				w=(arenaMatch[player.arenaLvl].timeLimit-player.arenaTime)/30;
			sprintf(s,"%d Slain %d:%02d",player.var[VAR_TEMP],w/60,w%60);
			break;
	}
	CenterPrintDark(319,-29+25-1+y,s,0);
	CenterPrintDark(321,-29+25+1+y,s,0);
	CenterPrint(320,-29+25+y,s,0,0);
}

void ArenaKill(byte type,int x,int y)
{
	if(arenaMatch[player.arenaLvl].rules==AR_SLAY)
	{
		if(type==arenaMatch[player.arenaLvl].waves[0][0].type)
		{
			if(player.var[VAR_TEMP]<255)
				player.var[VAR_TEMP]++;
		}
	}
}
