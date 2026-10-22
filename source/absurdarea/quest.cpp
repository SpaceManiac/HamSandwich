#include "quest.h"
#include "game.h"
#include "control.h"
#include "display.h"
#include "options.h"
#include "skill.h"
#include "guy.h"
#include "particle.h"
#include "pause.h"
#include "bullet.h"
#include "services.h"
#include "help.h"
#include "leveldef.h"
#include "achieve.h"

char questName[NUM_QUESTS][32]={
	"Help Yourself",
	"I've Got 99 Presents",
	"Improve Yourself",
	"Show You're Buff",
	"No, No, It's A Medicine",
	"Rock 'N Stole",
	"To Tame Them Is Your Cause",
	"Crystal Request",
	"Stock Market",
	"Offer You Won't Refuse",
	"Free Melly",
	"Talent Grinding Time",
	"Mice To Meet You",
	"Toasty Trousers",
	"It's Time For Loonymania!",
	"It's OK, They're Just Toys",
	"Reverse Fetch Quest",
	"He-Mail",
	"Snowed Out",
	"Safe To Land? Nopes!",
	"Tool Me Once",
	"Does Whittling Count?",
	"Bear With Me",
	"Big Brother Bear",
	"Magical Purposes",
	"Power Up",
	"Dismember The Titans",
	"Rad Skillz",
	"Demolition Man",
	"Core Breach",
	// these are Onion quests, the Bunny quests are in altQuestName
	"Cannon Fodder",
	"Gangster's Paradise",
	"Holey Scrap",
	"A Tough Quest",
	"Stealing Is Fun",
	"Not an Escort Mission",
	"Right In The Pocketbook",
	"Total War",
	"High Noon",
	"Furnace Despair",
	// back to regular
	"Supercalifragilistic",
	"Weed Whacker",
	"Scarred Forest",
	"Bok Back",
	"Treeslugger",
	"Under Plunder",
	"Coup De Your Grace",
	"Puppet Pals",
	"The Five Hidden Trials",
	"Happy Stick Denoument",
};

char altQuestName[10][32]={
	"Cannon Fodder",
	"Competence Test",
	"I Am Your Creator! Obey!",
	"\"Watch\" This",
	"Stealing Is Dumb",
	"Common Sense Axe Control",
	"Right In The Pocketbook",
	"Total War",
	"Mince The Onion",
	"Nothing To See Here",
};

char questDesc[NUM_QUESTS*4][64]={
	// #0 - Collect Logs
	"Earn an Objective Point and purchase a second upgrade,",
	"then return to Woody Woodsman.  He will give you one",
	"of his axes.",
	"",
	// #1 - Presents Absent
	"Collect 99 of the missing presents and return them",
	"to Bobby Joe so he can establish a gifts monopoly.",
	"Also for a reward.",
	"",
	// #2 - Prove Yourself
	"Rise to level 25, then return to Kinyova to show off.",
	"He'll give you an Objective Point!",
	"",
	"",
	// #3 - Show Your Stuff
	"Raise any Combat Talent to Level 10, then tell Kinyova",
	"to earn an excellent axe!",
	"",
	"",
	// #4 - Herbal Essences
	"Collect 3 Frostbane Herbs and bring them to Healiana in",
	"town, so she can brew some Curium.",
	"",
	"",
	// #5 - Sharpening Stone
	"Recover Woody's stolen Sharpening Stone and return it",
	"to him.  He'll be able to sharpen your axe!",
	"",
	"",
	// #6 - The Mimic Whisperer
	"Tame a Mimic for the Magicsmith.  She'll reward you with",
	"an amulet!  To tame a Mimic, feed it delicious items,",
	"and avoid hitting it or letting it bite you.",
	"",
	// #7 - Crystal Method
	"Retrieve the Crystal of Binding from \"some weird green",
	"person\" and return it to the Magicsmith so she can",
	"combine magic items for you.",
	"",
	// #8 - Stockboy
	"Collect any 3 Axes (besides the one you hold!) and bring",
	"them to any Axes R Us store so they can re-open.",
	"",
	"",
	// #9 - An Offer You Can't Refuse
	"Find Black Market Bob's wife, who was last seen in the",
	"swamp to the southeast of Einsburg.  Bob will reward you.",
	"",
	"",
	// #10 - Unchain Melody
	"Find the Snowman Key to unlock the chains holding",
	"Melody.  THEN Bob will reward you.",
	"But what was that about expecting someone else... ?",
	"",
	// #11 - Guru Hide & Seek
	"Raise any Talent to Level 10, then brag to the Guru.",
	"He will give you 5 Skill Points!",
	"",
	"",
	// #12 - Mickey's Mice
	"Clear the rats out of Mickey's cellar for a $50 reward,",
	"and get an Objective Point as a bonus!",
	"",
	"",
	// #13 - Hot Pants
	"Collect 5 Asbestos Herbs and 5 Socks and bring them to",
	"any Taylor's Knits store, so they can craft you",
	"a nice pair of Asbestos Underwear.",
	"",
	// #14 - Be A Contender
	"Demonstrate your gladiatorial skills and Tina will grant you",
	"an Objective Point!",
	"",
	"",
	// #15 - Toys
	"Mickey is slightly more fed up with the toys all over the",
	"place than he is with you.  He'll give you a reward",
	"if you smash a few of them.",
	"",
	// #16 - book
	"Return the book Betty borrowed from",
	"The Rooms of Research in the Treacherous Taiga.",
	"",
	"",
	// #17 - The Loony Express
	"Deliver Healiana's letter to her sister in Tweestad.",
	"",
	"",
	"",
	// #18 - All Tied Up
	"Find the missing Ropemaker on the Sinister Slopes.",
	"",
	"",
	"",
	// #19 - Show Him The Ropes
	"The balloonist needs you to clear the peak of",
	"Thornpaw Lions so he can safely land.",
	"",
	"",
	// #20 - Lend A Hand
	"Find Handyman Hank's toolbox in Sankyo and return it",
	"to him in the Rooms of Research.",
	"",
	"",
	// #21 - Can You Help?
	"Raise any Craft Talent to Level 10 and return to",
	"Rosetta Roper for an excellent parka!",
	"",
	"",
	// #22 - Unbearable
	"Defeat 30 Cave Bears and Teddies in",
	"the Bear Lair under the Rooms of Research.",
	"",
	"",
	// #23 - Bear With It
	"Defeat the Cave Bear leader to (hopefully)",
	"break their resolve.",
	"",
	"",
	// #24 - Medicinal Purposes
	"Purchase the highest-level upgrade in either",
	"Enchantment or Sorcery. Then, return to",
	"Curaga to show off your magical aptitude!",
	"",
	// #25 - Deliverin' The Goods
	"Raise any Magic Talent to Level 10 and return to Curaga",
	"for an excellent amulet!",
	"",
	"",
	// #26 - Remember The Titans
	"Obtain 3 Ice Cubes from the ice monsters so",
	"The Historian can study them.",
	"",
	"",
	// #27 - Mad Skillz
	"Purchase every upgrade and then go brag to",
	"The Historian for 5 Skill Points.",
	"",
	"",
	// #28 - Factory Clearance
	"Find the source of the evil toys and shut it down!",
	"",
	"",
	"",
	// #29 - Power Down
	"Descend to the Toy Factory basement and destroy the",
	"power generator there.",
	"",
	"",
	// #30 - The Buddy System
	"Find some meat shie... er, valued allies",
	"to aid you in your dangerous quest.",
	"Then return to the Historian for a prize!",
	"",
	// #31 - Loyalty Test
	"Show you have something to offer The Onion Ring",
	"by completing any 4 of the missions they offer you,",
	"including this one.  So, any 3 missions.",
	"",
	// #32 - Junksmith
	"Impress the Junksmith by Junksmithing a magic item",
	"or better, and he will let you into his junkyard.",
	"You must first raise Junksmith to Level 4.",
	"",
	// #33 - An Easy Quest
	"Break Bobby Joe's candy monopoly by seizing",
	"some of his supply.",
	"",
	"",
	// #34 - Highway Robbery
	"Collect Stolen Goods from the caravans",
	"passing by Tweestad.",
	"",
	"",
	// #35 - Shakedown
	"Rendezvous with the axe smuggler under Tweestad",
	"and protect his storage huts during the oncoming",
	"raid from the Order of the Snuggly Bunny.",
	"",
	// #36 - Supplies!
	"Destroy the Snuggly supply depot in the",
	"abandoned MacGregor farmhouse.",
	"",
	"",
	// #37 - The Rumble
	"Join the rest of the Onion Ring and push",
	"the Order of the Snuggly Bunny all the way",
	"back to their camp.",
	"",
	// #38 - Bop Em On The Head
	"Take out Captain Capitan to disrupt the Order",
	"and give the Onion Ring the opportunity to take over.",
	"",
	"",
	// #39 - Fix The Furnace
	"Find the four missing bomb pieces and",
	"destroy the Furnace to stop the Titans.",
	"",
	"",
	// #40 - Frajeelay // AA done
	"Carry a Sounding Crystal to the mystical barrier in",
	"the Crystal Caves.  Bring the Fission Chip, the first",
	"bomb part, back to Schrodinger.",
	"",
	// #41 - Plants // AA done
	"Make your way to the heart of the Loathsome Lawn",
	"to slay the fiendish Gourdzilla and recover the lost",
	"Doomsday Clock.  Then, return it to Schrodinger.",
	"",
	// #42 - Defiling
	"Clear the Bodzhas from the Fungal Forest,",
	"so Shrumford can rejuvenate and help you",
	"replace the bomb's voltage feed cables.",
	"",
	// #43 - Bok Bok // AA done
	"Rescue the ten lost Bokboks for Sharona.  She'll give you",
	"a Binding Crystal in return.",
	"",
	"",
	// #44 - Treehugger // AA done
	"Bring 20 Seeds to Sharona so she can plant friendlier",
	"vindictive shrubbery, and give you a Golden Amulet.",
	"",
	"",
	// #45 - Clash With Titans
	"Plunder the Titanic Treasury to find",
	"the long-lost Wing Boots.  They will",
	"allow you to walk on clouds.",
	"",
	// #46 - Baron
	"Delve into Sankyo Castle to find the villainous",
	"Baron von Frostburn.  Have a nice little chat",
	"with him, then administer justice.",
	"",
	// #47 - Toy Turnabout
	"Collect the Control Crystals from the Stinkin' Bodzhas",
	"hiding around Sankyo Castle.  You will first have to",
	"find a way to bring them out of hiding.",
	"",
	// #48 - Champion
	"Find and conquer the five Hidden Trials for $5000",
	"and access to a sweet treasure stash!",
	"",
	"",
	// #49 - Happy Stick
	"Put an end to the calamity by facing",
	"the menace behind all the madness.",
	"",
	"",
};

char altQuestDesc[10*4][64]={
	// #30 - The Buddy System
	"Find some meat shie... er, valued allies",
	"to aid you in your dangerous quest.",
	"Then return to the Historian for a prize!",
	"",
	// #31 - Rising Ranks
	"Prove you're worthy to join the Order of the",
	"Snuggly Bunny by completing any 4 of the missions",
	"they offer you.  Including this one.  So, any 3.",
	"",
	// #32 - Clean Their Clocks
	"Destroy the rogue Clockwork Robots so the lab",
	"can be used to build powerful bots for you.",
	"",
	"",
	// #33 - A Miner Quest
	"Bring a Clockbot with at least one of every Junk",
	"item to the Clockmaker, and he will upgrade it!",
	"You must first raise Clockwork to Level 4.",
	"",
	// #34 - Highway Patrol
	"Defend Tweestad, which is under attack by robbers",
	"from the Onion Ring.  The houses are in danger;",
	"every house you save will reward you.",
	"",
	// #35 - Shutdown
	"Crack down on the axe smugglers hiding out in",
	"the Hiemal Holes under the Rowdy River.",
	"",
	"",
	// #36 - Supplies!
	"Dig up and destroy the Onion Ring's supplies",
	"hidden all over the Precipitous Precipices.",
	"",
	"",
	// #37 - The Rumble
	"Join the Order and sally forth against a massive",
	"Onion Ring assault.  Push them all the way back",
	"to their hideout.",
	"",
	// #38 - Dice The Onion
	"Defeat the Diabolical Onion himself to throw",
	"the Onion Ring into disarray so the Order can",
	"bring... well, order.",
	"",
	// #39 - Something is Fishy
	"Ignore Captain Capitan's orders, and find the",
	"four missing bomb parts to destroy the Furnace",
	"and stop the Titans.",
	"",
};

char chatName[44][32]={
	// 0
	"Talking to Yourself",
	// 1
	"",
	// 2
	"Kinyova Sr.",
	// 3
	"Bobby Joe",
	// 4
	"Healiana",
	// 5
	"Woody Woodsman",
	// 6
	"Mrs. Smith",
	// 7
	"Geoffrey",
	// 8
	"Black Market Bob",
	// 9
	"Melody",	// in chains
	// 10
	"Melody",	// not in chains
	// 11
	"Mickey",
	// 12
	"Great Guru",
	// 13
	"Tina",
	// 14
	"Taylor",
	// 15
	"Phileas",
	// 16
	"Handyman Hank",
	// 17
	"The Historian",
	// 18
	"Guy Magic",
	// 19
	"Buried Rupert",
	// 20
	"Rupert",
	// 21
	"Rosetta",
	// 22
	"J.P. Toymonger III",
	// 23
	"Curaga",
	// 24
	"The Diabolical Onion",
	// 25
	"The Junksmith",
	// 26
	"Frankie Beans",
	// 27
	"Crush",
	// 28
	"Jimmy Two-Fingers",
	// 29
	"Smash",
	// 30
	"Captain Capitan",
	// 31
	"Major Fishbug",
	// 32
	"The Clockmaker",
	// 33
	"Lt. Hornswaggle",
	// 34
	"Pvt. Public",
	// 35
	"Betty",
	// 36
	"Schrodinger",
	// 37
	"Sharona",
	// 38
	"Shrumford",
	// 39
	"Shrumford",
	// 40
	"Shroud",
	// 41
	"Baron Von Frostburn",
	// 42
	"Titan King Melton",
	// 43
	"Happy Stick Bird",
};

char chatOfs[44][2]={
	{0,0},	// loony
	{0,0},	// unused
	{25,-10},	// kinyova
	{95,-10},	// bobby joe
	{10,-20},	// healiana
	{45,0},	// woody
	{30,-20},	// magicsmith
	{30,0},	// geoffrey
	{0,0},	// bob
	{45,0},	// melody
	{0,0},	// melody unchained
	{25,0},	// mickey
	{10,0},	// guru
	{20,0},	// tina
	{30,0},	// taylor
	{40,0},	// balloon
	{40,40},	// handyman
	{30,10},	// historian
	{30,20},	// magic guy
	{0,0},	// buried ropeguy
	{30,10},	// ropeguy
	{0,0},	// ropewife
	{30,30},	// CEO
	{30,0},	// curaga
	{10,20},	// onion
	{20,0},	// junksmith
	{20,30},	// frankie beans
	{10,20},	// crush
	{20,30},	// jimmy 2-fingers
	{10,20},	// smash
	{35,30},	// captain capitan
	{40,30},	// major fishbug
	{40,30},	// clockmaker
	{30,20},	// lt. hornswaggle
	{30,20},	// pvt. public
	{40,20},	// betty
	{30,30},		// schrodinger
	{30,80},		// sharona
	{15,50},		// shrumford old
	{50,90},		// shrumford young
	{50,30},	// shroud
	{30,40},		// baron
	{0,0},		// melton
	{40,40},		// bird
};

Convo talk[]={
	// 0
	{"It's a lovely day, eh?"
	 "",
	 0,0},
	 // KINYOVA SR.
	 // 1	first quest
	{"Wha... Hey! How'd you find this place? I thought here at least I could "
	 "admire my beard in privacy. Alas, 'twas not to be.",
	 0,2},
	 // 2
	{"So, what, you fancy yourself some sort of hero? Pah! Come back when you're "
	 "Level 25 and I'll toss you an Objective Point to get you started.",
	 0,3},
	 // 3
	{"And, hmm, you're the secret-finding sort, too? Like poking around in places you shouldn't? "
	 "Why don'cha take this... there may be a secret you can't get without it.",
	 1,0},
	 // 4 waiting to finish first quest
	{"Oh, come ON. Are you even trying? In my day, we had to slay at least three dragons "
	 "to earn the title of junior hero wannabe! Don't be such a pushover!",
	 0,0},
	 // 5 complete quest 1
	{"Now you're talking! Baby talk, but it's a start. Here's your Objective Point. ... "
	 "So, wait, is it an item? Do I hand it to you somehow?  I don't have a clue.",
	 0,6},
	 // 6
	{"What?  Oh, you want another hint about that secret? Pshaw. Kids these days. Time was, "
	 "we had to find the secrets all by ourselves! Alright, you'll need to get both R",
	 2,0},
	 // 7 quest 1 finished, offer quest 2
	{"Well, I suppose you may have some slight potential.  Still not too impressed though.  "
	 "I mean, if you could get a Combat Talent to Level 10, that'd be a start.",
	 3,0},
	 // 8 waiting on quest 2
	{"You call those Combat Talents?  Boy, when I was half your age, we had to get "
	 "Chopping to Level 15 just to mow the lawn!  Get back to exercising!",
	 0,0},
	 // 9 complete quest 2
	{"Alright, alright, you've got a portion of the right stuff.  I bestow on you the rank of "
	 "Junior Hero Wannabe... in training.  Now take this axe and scram.",
	 4,0},
	// 10 quests done
	{"Why are you still here? Can't you see I'm busy? "
	 "I've got a lot of sagely scratching my epic beard to do!",
	 0,0},
	 // BOBBY JOE
	 // 11 introduction + quest assign
	{"WAAAAHHH!!!! I'm trying to expand my candy empire to the gifts business, "
	 "but I don't have enough supply to corner the market! Can you bring me 99, pwease?",
	 5,0},
	 // 12 waiting on presents
	{"WAAHHH!!!  I still haven't acquired enough market share to establish an oppressive monopoly!  "
	 "If you help me achieve my nefarious economic goal, I'll reward you!",
	 0,0},
	 // 13 got all presents, quest complete
	{"WAAAAAHOO!! I'm gonne be rich!  These are all going RIGHT into a warehouse "
	 "so I can constrict supply!  You can have this one, and I'll buy any more you bring!",
	 6,0},
	 // 14 all done
	{"I'll pay you for any extra presents.  Hm... now what other markets should I dominate... think I could get into the toy industry?",
	 127,0},
	 // HEALIANA
	 // 15 introduction + herb quest
	{"Greetings, I am this town's healer.  Yet alas, I can heal no one!  My stores of Curium have run dry, "
	 "and I don't have enough Frostbane to mix another batch!",
	 0,16},
	 // 16
	{"If you would fetch me 3 Frostbane herbs, I'll be happy to heal you for free from now on!",
	 7,0},
	 // 17 herb quest incomplete
	{"Do you need healing?  Ah, that's a shame, since I still lack enough Frostbane Herbs "
	 "to make a batch of Curium.  If only that were remedied in some way...",
	 0,0},
	 // 18 herb quest done
	{"Oh thank you!  Let me just toss these herbs in with 22 other flavors, and we're done!  "
	 "Now, let's see to those wounds.  By which I mean just chug some, of course.",
	 8,0},
	 // 19 all done, heals you each time now
	{"Greetings.  Please enjoy this complimentary Curium sample!",
	 9,0},
	 // WOODY WOODSMAN
	 // 20 intro + wood quest
	{"Hey, you're up and moving! I found you out in the snow. I'm off to town - "
	 "when you feel ready, just follow the path. Meet me in my room in Mickey's Inn.",
	 0,0},
	 // 21
	{"Welcome! You look a bit confused about how things work, so how about I help you out. "
	 "We all need to stick together, especially at a time like this!",
	 0,22},
	 // 22
	{"Evil toys everywhere, terrible beasts roaming the taiga... it's gonna be a long winter. "
	 "OK, so. If you're going to make it out there, you'll need to upgrade yourself.",
	 0,23},
	 // 23
	{"Find yourself a powerful enemy to defeat, or someone with an important quest, and get a second upgrade. "
	 "Come to me then, and I'll give you one of my axes!",
	 10,0},
	 // 24 wood quest unfinished
	{"I heard the cranky innkeeper has a rat problem you could help with. Or, "
	 "you could look for a powerful enemy in the Frigid Forest. You got options.",
	 0,0},
	 // 25 wood quest done
	{"Good job! You seem competent enough, so I'll do you a favor. "
	 "Take one of my spare axes, and keep up the good work.",
	 11,0},
	 // 26 assign sharpening stone
	{"I'd sharpen that axe for you, but my Sharpening Stone was taken by some soldiers to the north. "
	 "If you could get it back for me, that'd be great!",
	 12,0},
	 // 27 sharpening stone not done
	{"I was seeing how bad the taiga is, when a patrol of soldiers confiscated my Sharpening Stone. "
	 "I don't know where they would have taken it.",
	 0,0},
	 // 28 finished sharpening stone
	{"Wow, you got it back! Hope you didn't run into any trouble! "
	 "I'll sharpen your axe for free, but to sharpen any others I charge $50. A man's gotta eat!",
	 13,0},
	 // 29 all done, axe sharpening shop
	{"Sharpen your axe? Adds +1 Damage to it for only $50! It's also handy for chopping "
	 "down dead trees that are too tough for a dull axe. Hint hint.",
	 14,0},
	 // MAGICSMITH
	 // 30 intro + mimic quest
	{"Hey there! I'm the local Magicsmith. I offer a service where I combine Magic items "
	 "into Golden ones. I'm sure you will find that useful!",
	 0,31},
	 // 31
	{"But can you do me a favor first? I'd really like a Mimic to store my tools in. "
	 "Heard of them? They look like treasure chests with teeth and a bad attitude.",
	 0,32},
	 // 32
	{"But if you tame one, it'll follow you and carry your stuff! Really helpful, apart from the saliva. "
	 "I'll give you something nice if you tame me one!",
	 15,0},
	 // 33 assign crystal quest
	{"Hmm, I can't find my Crystal of Binding! Let's see, the last time I saw it was... "
	 "oh no!! Right before that weird green person bumped into me!",
	 0,34},
	 // 34
	{"Argh, I've been mugged! This is EXACTLY why sane and reasonable people "
	 "carry all their stuff in borderline homicidal living furniture!",
	 0,35},
	 // 35
	{"I can't combine items for you unless you get it back.  Can you help?",
	 17,0},
	 // 36 crystal quest not done
	{"I didn't get a good look at the person. Kinda short and greenish.",
	 0,0},
	 // 37 crystal quest done
	{"Hooray! Now I can magicsmith like nobody's business! Come on back with "
	 "some Magic items, and I'll combine them for you.",
	 18,0},
	 // 38 all done, now it's a magicsmith shop
	{"Select two Magic items of the same type, and I'll quote you a price to combine them. "
	 "It costs more with more powerful items.",
	 19,0},
	 // GEOFFREY, AXES R US
	 // 39 intro, axe quest
	{"Hey, uh... I'm afraid we're not open right now. One of our suppliers ran into some... "
	 "difficulty, and we're understocked. Just a bit more and we could reopen...",
	 22,0},
	 // 40 axe quest undone
	{"Hi, I'd sell you some axes, but my suppliers haven't \"found\" any yet. "
	 "Er, made. Made, is the thing they haven't done yet.",
	 0,0},
	 // 41 axe quest done
	{"Wha... for me?! Oh thank you, thank you! No, no, it's OK, you don't need to tell me where you got them. "
	 "Well! I guess we can re-open now! Come by anytime!",
	 23,0},
	 // 42 axe shop open
	{"Congratulations, you're our 10,000th customer! To claim your prize, leave $100 on the counter "
	 "at any of our franchises, and walk out the door.",
	 0,0},
	 // BLACK MARKET BOB
	 // 43 intro, wife quest
	{"Psst, hey you!  You wanna help a guy out?  I'll give you some good stuff!  "
	 "My wife is out paying a... visit to someone tryin' to muscle in on my turf.",
	 0,44},
	{"But she ain't come back yet, and I don't take nuttin' from nobody!  "
	 "So you go see what's up, and I'll see about gettin' you set up with some goods, yeah?",
	 27,0},
	 // 45 wife quest not done
	{"I don't know exactly where she went.  Somewhere in the swamp "
	 "to the south 's all I know.  Chump's tryin' to hide from me.  Ha!",
	 0,0},
	 // 46 have finished wife quest, but not wife quest 2
	{"Where is she?  ... Wait, you say they was tryin' to ambush one of my boys?  "
	 "Bwahaha, I guessed it was somethin' like dis! Bring her back for... debriefing.",
	 0,0},
	 // 47 both wife quests done
	{"Well, well. Figured she'd try sumtin' like dis.  Can't pull a fast one on me!  "
	 "And you, I got dis for ya.  Nice,  innit? Previous owner didn't want it no more.",
	 0,48},
	 // 48
	{"And if you need some... used goods... you know where to find me.  "
	 "It's amazing, the stuff people just... drop, y'know wha I'm sayin'?",
	 28,0},
	 // 49 everything done, now black market shop
	{"Take a look, take a look! Only slightly used. I don't know what it all is, but "
	 "it's all at least magical. Ain't worth the trouble to \"acquire\" otherwise, y'know?",
	 29,0},
	 // BOB'S WIFE MELODY
	 // 50 intro, assign wife quest 2
	{"Help!  I... Hey! You're not one of his regular lackeys!  Oh phooey!  "
	 "Er, I mean.  Help, help, I've been kidnapped, won't someone unchain me.  Sigh.",
	 30,0},
	 // 51 wife quest 2 not done
	{"Get on with it already!",
	 0,0},
	 // 52 wife quest 2 done
	{"Bah! There goes my plans!  If ONLY he'd sent one of his regular lackeys "
	 "we could have interrogated for some leverage.  Sigh.",
	 31,0},
	 // SHOPKEEPER STUFF
	 // 53 already been sharpened
	{"I can't sharpen that any more! The metal wasn't folded enough times to hold that fine an edge.",
	 0,0},
	 // 54 can't afford it
	{"You can't afford that! Open a credit card and come back.",
	 0,0},
	 // 55 sharpened it
	{"There, nice and sharp! Be extra careful if you shave with it.",
	 0,0},
	 // 56 items must be magic
	{"I can only smith Magic items. Normal items can't be smithed, Golden ones already were.",
	 0,0},
	 // 57 first item chosen
	{"Great, now pick another item and I'll give you a price quote!",
	 0,0},
	 // 58 items are smithed!
	{"\"Hooey dooey alacablooey, these two items now I gluey!\" Please enjoy your Golden item responsibly.",
	 0,0},
	  // 59 trying to smith an item with itself
	{"I can't combine something with itself! Try again.",
	 0,0},
	// MICKEY
	// 60 intro + rat quest
	{"Oh, an adventurer. Perfect! As if we didn't have enough dead weight around. "
	 "I don't suppose you can cook, or you're a master carpenter? Maybe you knit a mean doily?",
	 0,61},
	 // 61
	{"Frontier town like this, you gotta pitch in! So if you can manage to be USEFUL, clean out my rat problem. "
	 "I'll spare you some cash and an Objective Point. Go on!",
	 20,0},
	 // 62 quest unfinished
	{"What, you want an advance or something? Afraid you'll catch the plague? "
	 "Try to buy a room with money, I'll STILL have a rat problem. So get moving!",
	 0,0},
	 // 63 complete quest
	{"Finally! I was thinking I'd have to send a search party. Cellar smells bad enough already. "
	 "You can stay, and here's your reward. Anything else? Then go away!",
	 21,0},
	 // 64 both quests done
	{"What do you want? Is there anything else you can do for me? No? "
	 "Then why are you wasting my valuable time?",
	 0,0},
	 // SHOPKEEPERS AGAIN
	 // 65 don't want it
	{"I'm not interested in buying that.",
	 0,0},
	 // 66 thanks for buying/selling
	{"Thank you, come again!",
	 0,0},
	 // 67 no room to hold it
	{"You'll need to free up some space first!",
	 0,0},
	 // GREAT GURU
	 // 68 intro + quest
	{"Peace and enlightenment be upon you, my child.  Speaking of enlightenment, if you get "
	 "a Talent to Level 10, I'll give you 5 Skill Points!",
	 0,69},
	 // 69
	{"And if you find yourself regretting your life choices, "
	 "I can reset all your assigned Skill Points for you.",
	 0,70},
	 // 70
	{"Just come find me, and I'll work my mojo.  "
	 "I've got places in Einsburg, Tweestad, and Sankyo.  Drop on by and I'll teleport on in!",
	 24,0},
	 // 71 found him
	{"Congratulations on earning another Talent!  Hey, did you know that there are "
	 "five Hidden Trials that only a master of something can enter?",
	 25,0},
	 // 72 alt found him
	{"Wow, you're looking tough! Keep it up!  Hey, have you found any secret areas yet?  "
	 "There are several! If you see or hear something weird, investigate!",
	 25,0},
	 // 73 alt found him
	{"Hey, you're moving on up! Had any trouble with tough enemies?  "
	 "You don't always have to beat everything.  Escaping is always an option!",
	 25,0},
	 // 74 alt found him
	{"Hey, you've been getting the correct upgrades! Ha ha! Remember, you can visit me in one of my homes "
	 "and I can reset your Skill Points.",
	 25,0},
	 // 75 alt found him
	{"If you see a solitary powerful foe, it will probably give you an Objective Point. "
	 "But if you can't beat it, don't worry! Just come back when you're stronger.",
	 25,0},
	 // 76 final found
	{"Hey, you maxed out a Talent! Huzzah! Enjoy your Skill Points! Now, I have a secret for you. "
	 "Don't tell anyone! Alright, now lean in. Closer! Ok, the secret is that if",
	 26,0},
	 // MORE MELODY
	// 77 met her before quest assigned
	{"Excuse me, I'm waiting for someone. If you want something, catch me later.",
	 0,0},
	 // 78 unchained, just relaxing at home
	{"Yeah, like I'll give you anything after you spoiled my usurpation plan.  "
	 "Scram!  Sigh... I'll probably never be a crime queen.",
	 0,0},
	 // TINA TSARINA
	// 79 intro
	{"Hey, so like, welcome to the arena! It is totally cool that you came here. "
	 "So, you know, fight or die, right?",
	 32,0},
	 // TAYLOR
	 // 80 intro + quest
	{"Eh? What?  Who's there??  Oh, a customer!  Welcome, welcome, to, errm, "
	 "welcome to... here, where our prices - Oh wait, the spiel!",
	 0,81},
	 // 81 intro 2
	{"Hello, hello! Here at Parka Knitter we have invented a new type of clothing - "
	 "Asbestos Underwear! But alas, with materials so hard to get,",
	 0,82},
	 // 82 intro 3
	{"you'll have to supply your own. Bring 5 Socks and 5 Asbestos Herbs to any franchise and we'll make some. "
	 "The leftover material is payment enough!",
	 0,83},
	 // 83 intro 4
	{"Until then, why don't you try some of our parkas? Here at... "
	 "ahrm, Scraps and Tailorings, our parkas are the best around, yes they are!",
	 33,0},
	 // 84 shop
	{"Oh, hrm, yes, welcome to, erm, Parkas and Parkas. We have only the finest clothing "
	 "at, hrm, er, Knits and Scraps, yes we do.",
	 0,0},
	 // 85 quest done
	{"What?  You've brought it! Well that's just grand, that is! "
	 "Now give me a minute, and I'll have your undies all sewn up.",
	 0,86},
	 // 86 quest done 2
	{"... annnd done. Alrighty then. These should do you just fine. "
	 "If you find any hot steam geysers, these should let you pass through. If you wanted to.",
	 0,87},
	 // 87 quest done 3
	{"Good luck! Just don't, uh. Don't let the steam touch the rest of your body."
	 "",
	 34,0},
	 // HANDYMAN HANK
	 // 88 intro+quest
	{"Help! We had a bit of a monster attack recently, and in all the commotion the ceiling caved in! "
	 "I'm a working man, so normally this wouldn't be no problem.",
	 0,89},
	 // 89 intro 2
	{"But I left my tools back home! I had a place over in Sankyo, but we evacuated in a bit of a hurry. "
	 "Some nasty monsters came down from the mountains.",
	 0,90},
	 // 90 intro 3
	{"And, well, the Baron normally takes care of that real fine, "
	 "but he shut hisself up and ain't been seen in quite a while.",
	 0,91},
	 // 91 intro 4
	{"So 'f you could go find me my tools, I could clear this area all up. "
	 "My house shouldn't be hard to find, it's the least fancy one in Sankyo.",
	 35,0},
	 // 92 quest undone
	{"Sankyo's to the east. Be real careful! There's a... thing watching the path. "
	 "Don't try to take it down unless you're REALLY confident!",
	 0,0},
	 // 93 quest done
	{"Hey, you made it! Here, gimme a sec and I'll get this cleared away. "
	 "And hey, I don't think I'll be needing this shovel anymore. Consider it your reward!",
	 36,0},
	 // 94 assign next quest, bears
	{"Phew, glad to see things put back in shape! For now, anyway. "
	 "Monsters keep wandering in and wreaking havoc. Good thing I'm handy with more than tools!",
	 0,135},
	 // PHILEAS
	 // 95 intro+quest
	{"Ahoy down there! I'm trying to land here, but unfortunately the peak has been infested "
	 "with dangerous Thornpaw Lions and I'm afraid I'd get shot down!",
	 0,96},
	 // 96
	{"If you can clear them out for me, I'd be happy to let you use my balloon "
	 "for transport! For a fee, of course.",
	 37,0},
	 // 97 quest unfinished
	{"Ahoy down there! There are still too many Thornpaw Lions for me to land here!"
	 "",
	 0,0},
	 // 98 got the ropes
	{ "Thank you so much! Hold on, I'm going to find a spot to land. How about... There!",
	 38,0},
	 // 99 ballooning shop active
	{"Okay, choose a destination! I got a pretty good look at the area from up there, "
	 "and I think I can remember where a few good landing spots are. Just $100 a ride!",
	 0,0},
	 // HISTORIAN
	 // 100 intro+quest
	{"Oh heavens! An adventurer! What a stroke of luck! No doubt you're willing to "
	 "undertake incredibly dangerous tasks for a somewhat dubious reward?",
	 0,101},
	 // 101
	{"I'm sure you've noticed the terrible ice beasts roaming the taiga.  "
	 "Let me share some history...  Thousands of years ago, the land was covered in ice.",
	 0,102},
	 // 102
	{"People lived in caves, laboring as slaves for the Ice Titans.  "
	 "You know, digging for gems, farming... whatever the Ice Titans eat. Slave stuff.",
	 0,103},
	 // 103
	{"But eventually the ice age ended through entirely natural means, and the surface became "
	 "too warm for the Ice Titans. So they moved deep underground.",
	 0,104},
	 // 104
	{"The people stayed aboveground and celebrated their new freedom, "
	 "until some of them got the bright idea that they too could lord it over people. So yeah.",
	 0,105},
	 // 105
	{"However, the Ice Titans and their pets once again walk the surface! "
	 "I know this winter may seem quite cold to you, but I assure you it is still warmer than they",
	 0,106},
	 // 106
	{"should be able to bear! We must find out how this could be happening! "
	 "Sadly, my titanic brain and immense research skills have been of no help for once.",
	 0,107},
	 // 107
	{"However, if you can collect samples from the Hounds and Titans, "
	 "I may be able to solve the riddle. Will you do it?",
	 39,0},
	 // 108 phileas shop stuff
	{"That is here. Literally here, is the spot you're referring to. So... Ta-da, we've arrived, I guess! "
	 "Such a short trip, I don't think I'll charge you. This time.",
	 0,0},
	 // 109 phileas shop stuff
	{"Sorry, but this area is under attack and it isn't safe to land. Quite inconvenient, I know!"
	 "",
	 0,0},
	 // 110 quest not done
	{"Do you need some sort of reward to motivate you into saving the world? "
	 "Sigh. Hey, aren't these ice skates shiny? Jingle jingle!",
	 0,0},
	 // 111 got enough cubes
	{"Sweet! Give me a sec to run some tests. WIZARD tests. "
	 "Here, you can have these ice skates. I don't need them at my age.",
	 40,0},
	 // 112 quests are done, results + assign next quest
	{"Oh my heavens! These ice cubes appear to be unusually resistant to heat! "
	 "Have they found a way to adapt to warmer temperatures? How is that even possible?",
	 0,113},
	 // 113
	{"We have to find out more so we can try to stop them! By \"we\" I mean you. "
	 "Go find someone with some influence and get some help. You'll need it!",
	 41,0},
	 // 114 ally quest assigned
	{"No, no, I can't help you. I have to stay and, uh, protect the valuable documents. "
	 "But if you do find some serious help, I'll give you something very special!",
	 129,0},
	 // 115 ally quest done
	{"Good work! You may find something in the volcano north of Sankyo. "
	 "But first, head into the room behind me for a gift to help you out!",
	 96,0},
	 // ROPEWIFE
	 // 116 intro+quest
	{"Oh hello, a visitor! Did you happen to see my husband on the way in? "
	 "He's tall, carries a rope, decidedly non-monster-looking.",
	 0,117},
	 // 117
	{"Well, he hasn't been back for a while, and I'm worried. This is a dangerous area these days! "
	 "If you find him, let me know, OK? OK. Stay safe out there!",
	 42,0},
	 // 118 quest unfinished
	{"He went up the slope looking for supplies for the shop he runs, "
	 "but that was hours ago. I'm sure he can handle the monsters though.",
	 0,0},
	 // 119 have found him
	{"Hey, thank you for finding my husband! You know what, you can have some of my special brews. "
	 "Supplies are limited, so don't go blabbing to everyone, OK?",
	 0,120},
	 // 120 now adding a new quest
	{"Now, you've really been doing some great work, but there's more to life than raw power, you know? "
	 "I'm a firm believer in the power of talent and cleverness.",
	 0,121},
	 // 121
	{"So if you prove you can master the subtler arts by raising a Craft Talent to Level 10, "
	 "I'll give you this special parka I knitted!",
	 43,0},
	 // 122 cans not collected
	{"Keep at it, and your inner talent will reveal itself! Or your inner lack of talent will. "
	 "Either way, it's better to find out.",
	 0,0},
	 // 123 cans collected
	{"Wow, looking great! Here's your parka. ... What, you want some more of the soup too? "
	 "Well... OK. More soup for you. But that's all you get!",
	 44,0},
	 // 124 quests all done
	{"Ah, the simple cottage life. A roaring fire... Endless housework... The ever-looming "
	 "threat of annihiliation by the uncaring elements...",
	 0,0},
	 // ROPEMAKER
	 // 125 thanks for rescue
	{"YEAHHH I'M ALIVE! Oh man, I owe you so much for digging me out! "
	 "One of those big jerks decided to try throwing snowballs. From halfway up the mountain. Yeah.",
	 45,0},
	 // 126 assign factory quest
	{"So, as I'm sure you've noticed, we've been having a spot of trouble lately. "
	 "Ice beasts roaming, evil toys swarming, and the Baron isn't keeping order.",
	 0,127},
	 // 127
	{"I'm sure it's no coincidence that all three are happening at once, but I can't puzzle it out. "
	 "But with brawn like yours, you might be able to get to the bottom of this.",
	 0,128},
	 // 128
	{"I don't know about the other two, but the toys are coming from the factory southwest of Tweestad. "
	 "If you can shut it down, we'll all sleep more soundly. A little.",
	 46,0},
	 // 129 rope shop
	{"I make all these ropes by hand. I'm also a collector of rare and expensive goods, "
	 "which I may be willing to part with - but it won't be cheap.",
	 0,0},
	 // BACK TO HISTORIAN
	 // 130 ally quest done, giving skill quest
	{"I think I could help you one more time with my wizard magic, "
	 "but this spell can only be used on an exceptionally skilled hero.",
	 0,131},
	 // 131
	{"So if you can purchase all 30 Upgrades, return to me and I'll power you up!"
	 "",
	 47,0},
	 // 132 skill quest unfinished
	{"You're not there yet. Keep a lookout for secret areas! "
	 "Many Objective Points are hidden in hard-to-find places.",
	 129,0},
	 // 133 got all skills
	{"Alright! Now, I'll need you to be quiet while I do the mystical chant. "
	 "Ahem: Alakazam, alakazoints, give this hero five shiny Skill Points!",
	 48,0},
	 // 134 quests done
	{"I can't help you any more for now. My role as an archivist transcends the mere problems of today. "
	 "So I get to sit here in safety! Woo-hoo!",
	 129,0},
	 // HANK AGAIN
	 // 135 bear quest
	{"There's a huge clan  of bears under here, though. I blocked off the entrance, but "
	 "don't know how long that'll work. Mind cutting down their numbers a bit?",
	 49,0},
	 // 136 bear quest undone
	{"I don't remember which bookshelf I blocked the entrance with. Sorry! "
	 "Good luck. I'll just... stay over here. And do... stuff. Things.",
	 0,0},
	 // 137 bear quest finished
	{"Well, that's a bit better. I guess. Still a lot more bears. "
	 "But hey, have this as a reward!",
	 50,0},
	 // 138 assign big bear
	{"Y'know, I wonder if it wouldn't help to take out some of their organizational structure, y'know? "
	 "Try bumping off officers, yeah?",
	 0,139},
	 // 139
	{"I hear Berserker Bear leads raids now and then. He's really quick, so watch yourself!",
	 51,0},
	 // 140 big bear quest undone
	{"Found him yet? Be careful down there! He's hard to miss if you run into him. "
	 "He paints himself hot pink. Pink! As if the super speed wasn't bad enough.",
	 0,0},
	 // 141 all quests done
	{"Well, looks like that dumb idea didn't work. Oh well! I'll just stay here and hope they don't "
	 "overrun the place while I'm sleeping. Good luck out there!",
	 0,0},
	 // TINA AGAIN
	 // 142 assign rank quest
	{"Wow, you are like, totally good, you know? If you can REALLY impress me, "
	 "I'll give you a totally rad Objective Point!",
	 52,0},
	 // 143 rank quest done
	{"YOOOOO WHAAAT How even is that POSSIBLE to be that good? Did you cheat? Are you cheating? "
	 "I'm just messing with you. Have your Objective Point!",
	 53,0},
	 // 144 assign champion quest
	{"Hey, so did you hear about the Five Hidden Trials? They're this sweet secret thing "
	 "and if you beat all 5, I'll give you The Ultimate Reward!",
	 54,0},
	 // 145 championship achieved
	{"OHHHH MYYYYY GOSSSHHH this is UNBELIEVEABLE! "
	 "I can't WAIT to tell everyone I met a real champ! Like, wow and stuff!",
	 55,0},
	 // HEALIANA AGAIN
	 // 146 assign letter quest
	{"Hey, since you helped me out, how about I return the favor? "
	 "I don't have much, but my sister Curaga is a powerful magician and could certainly help out.",
	 0,147},
	 // 147
	{"She lives in Tweestad, west of the Treacherous Taiga. "
	 "I'll give you this letter of introduction saying what you've done. Thanks!",
	 56,0},
	 // CEO
	 // 148 intro+quest
	{"Oh my heavens! I'm saved! Er, ahem. Greetings sir, I am J.P. Toymonger the Third, owner and proprietor "
	 "of Non-Evil Toys, Inc. Or at least I used to be!",
	 0,149},
	 // 149
	{"But then the Onion Ring discovered that Non-Evil Toys, Inc. was a front for my secret "
	 "military-grade toy empire. They took over and increased production.",
	 0, 150},
	 // 150
	{"Then they let the toys loose! I swear, they have no idea how to make a profit. "
	 "You have to ship them to unstable overseas third-world dictatorships! Honestly!",
	 0,151},
	 // 151
	{"Oh well. I suppose you'll have to destroy the Power Core to stop them for good. "
	 "I'll let you into the basement to take care of that. Watch out, it's likely guarded.",
	 0, 152},
	 // 152
	{"It'll mean the end of my days as a leader in the international violent goods industry, "
	 "but at least I invested heavily in an up-and-coming local gifts mogul.",
	 0,153},
	 // 153
	{"Also, we were working on a new Helpy the Helpfully Murderous Bodzha doll right before this all happened. "
	 "I can loan you our prototype if you like.",
	 57,0},
	 //154 quest not done
	{"If you want to rent the prototype, just go get it from the holding pen. "
	 "We'll need to hold one of your Modifiers as collateral, though.",
	 0,0},
	 // 155 quest done
	{"Oh, don't worry about me. I'll just sit here and think of ways to minimize my losses. "
	 "I think I can salvage my creepy doll side business.",
	 0,0},
	 // MICKEY AGAIN
	 // 156 toy quest
	{"Ugh, it sure is a mess out there. At least in here all we have is do-nothing adventurers, "
	 "but with all the evil toys we can't get anything done outside!",
	 0, 157},
	 // 157
	{"Tell you what, if you want to do something vaguely useful for the second time in your life, "
	 "how about you try to put a dent in the toy population?",
	 0, 158},
	 // 158
	{"I'm just a nigh-penniless innkeeper, cook, craftsman, and orator, but if you make a difference "
	 "I'll scrounge up as much as I can and hope I don't starve.",
	 58,0},
	 // 159 quest undone
	{"What am I paying you for?!",
	 0,0},
	 // 160 killed enough
	{"Alright, I guess you did what you could. Here, take this. That's all I can spare for you!",
	 59,0},
	 // CURAGA
	 // 161 intro + devil grass quest
	{"Yo, I got some real goods, but I gotta make sure it gets to someone who'll make the most of it. "
	 "Show me a real powerful spell and it's yours!",
	 60,0},
	 // 162 devil grass undone
	{"C'mon, man, don't be a square! Be a... well, a nerd, I suppose. Get your Level 5 Mage upgrade on!",
	 9,0},
	 // 163 devil grass done
	{"Dude, the power is so trippy! You can have this glowy thing, I'm sure you'll get some use out of it.",
	 61,0},
	 // 164 brought her the letter
	{"Hey man. Oh, a letter of introduction from my sister? Let's see... "
	 "wow, looks like you're quite the up-and-coming hero! That is way rad, dude!",
	 0,165},
	 // 165
	{"You know, why don'cha take some of my drank? Good for the soul! "
	 "The parts of the soul that shoot fireballs, anyway.",
	 62,0},
	 // 166 assign package quest
	{"Hey dude, I found this cool amulet in the trash. I think it's magical or something? "
	 "I'll let you have it, but you'll have to REALLY earn it!",
	 0, 167},
	 // 167
	{"Get any of your Magic Talents to Level 10, and you can have it. "
	 "I'm sure it's worth the effort. Look how shiny it is!",
	 63,0},
	 // 168 package undone
	{"Hey you got a Magic Talent to Level 10! Congrats... oh, oh, wait, I was reading my OWN Talents! "
	 "Hahahaha wowwww, dude, that's so crazy!",
	 9,0},
	 // 169 package done
	{"Lookin' sharp, bro! You can have this amulet I found. "
	 "I'm sure you'll have this grody mess cleared up in no time!",
	 64,0},
	 // 170 everything done
	{"Drop by anytime you need a heal up and I'll top you off. Stay safe out there, dude!",
	 9,0},
	 // THE ONION
	 // 171 beat power core, make choice
	{"MUAHAHAHAHA! We've got you cornered, you little rat! Pity we didn't show up soon enough to save our factory, though. "
	 "You sure do work fast for a little punk!",
	 0,172},
	 // 172
	{"But now the advantage is mine. I am the Diabolical Onion, and you've been "
	 "cheesing me off something fierce! First you throw off my expanding into the",
	 0,173},
	 // 173
	{"black market, then you stop our little Toy Factory operation - this has got to stop. "
	 "But, since you show such promise, I'm going to let you choose how.",
	 0,174},
	 // 174
	{"You can join my group and reap a share of our spoils, or... "
	 "you can leave the area. Permanently. What's your choice? MUAHAHAHAHA!!!",
	 65,0},
	 // 175 chose to join him
	{"Now that's a smart choice. We're heading back to our hideout north of the taiga. "
	 "Take this key and meet us there. Watch it, though! Things are starting to heat up.",
	 66,0},
	 // 176 chose to get beaten up
	{"Go away?  Oh, you poor naive thing! That was my way of saying \"we're going to knock you out "
	 "and chuck you in a cell so you don't join our enemies.\" Too subtle? ",
	 67,0},
	 // 177 waiting for you to get moving
	{"Go on already! Head north from the taiga, then east to our hideout. Watch out for... vermin on the way.",
	 0,0},
	 // 178 at HQ, assign quest-doing quest
	{"Welcome to my den of diabolism! MUAHAHAHAHA! Now, I know I promised a share in the perks, "
	 "but you gotta pull your weight. Complete any 3 missions first!",
	 68,0},
	 // 179 loyalty test not done
	{"Come on, nobody likes a slacker! Get out there and do something heinous!",
	 0,0},
	 // 180 loyalty test done, assign slay quest
	{"See, that wasn't so hard, was it? Doing jobs to HELP us instead of... "
	 "Sorry. Still a bit bitter. Ahem. Well, you're one of us now!",
	 0,181},
	 // 181
	{"Means you can take some lads on missions of your own. But first, I got one more job to ask of you. "
	 "I'm sure you've run across the Order of the Snuggly Bunny.",
	 0,182},
	 // 182
	{"They've been hounding us for weeks, and I'm a bit fed up! But, if you can "
	 "take down their leader, we can breathe easier. Take this key and go get 'im!",
	69,0},
	 // 183 haven't slain
	{"If you want to dress up like the rest of us, go check out an outfit in the sleeping area. "
	 "It'll take a Modifier slot though.",
	 69,0},
	 // 184 have slain
	{"Finally! We now rule uncontested! MUAHAHAHAHA! We also captured their Clockmaker. "
	 "You can go make him upgrade a Clockbot for you.",
	 0,0},
	 // JUNKSMITH
	 // 185 intro + quest
	{"What're you lookin' at, punk? Never seen a self-made outfit before? "
	 "Bet you think those parkas grow on trees. You ever made anything for yourself?",
	 0,186},
	 // 186
	{"Tell you what, if you can manage to junksmith a magic item or better, "
	 "I'll let you hang out in my junkpile. You can even touch my junk!",
	 70,0},
	 // 187 quest not done
	{"You call that junksmithing? Boy, I have 100,000 subscribers watching me make functional "
	 "power tools out of baling wire. Get back to work!",
	 0,0},
	 // 188 got enough
	{"A man after my own rusty, cobbled-together heart! Go ahead, grab some junk!",
	 71,0},
	 // 189 quest done
	{"Don't bother me now, I'm one breakthrough away from making a fusion reactor "
	 "out of used dish towels.",
	 0,0},
	 // FRANKIE BEANS
	 // 190 intro + quest1
	{"Whassup? I'm called Frankie Beans. They call me that 'acause I'm a certified accountant. "
	 "I handle a lot of the economic side of the crime woild.",
	 0,191},
	 // 191
	{"As you may know, the candy industry 'round here's dominated by the Bobby Joe monopoly. "
	 "But if you can seize enough supply to break his hold, we'll make bank!",
	 72,0},
	 // 192 candy untaken
	{"Woid has it Bobby Joe hoired some Bunny guards. Watch y'self.",
	 0,0},
	 // 193 candy taken
	{"Now this is quality stuff! I'll get this to a sweets fence roight away. "
	 "Come back when you think you can handle a somewhat more challenging task.",
	 73,0},
	 // 194 quest2
	{"You've come at just the roight time. The Order of the Snuggly Bunny is arranging a major attack against us. "
	 "So we're gettin' as many lads as we can to try and",
	 0,195},
	 // 195
	{"push 'em back. If we can push them all the way back to their camp, "
	 "we'll be free to move for a while!",
	 74,0},
	 // 196 rumble undone
	{"Prepare yourself. It's gonna be an ugly foight. "
	 "I hear they teamed up with some of those nasty toys, so watch out.",
	 0,0},
	 // 197 rumble won
	{"You done good, pal. Don't think we coulda done it without you. "
	 "Know what, how about you take an Objective Point, yeah? We also captured one of their silly getups.",
	 75,0},
	 // 198
	{"Not now, I'm trying to work out a real nice embezzlement scam. Er, to use on other people, of coise.",
	 0,0},
	 // CRUSH
	 // 199 intro+quest
	{"Graarrgh! Me Crush! Me smash! You smash? Me no crush you, you smash! "
	 "Smash caravans, get goodies! Me give money you for smash!",
	 76,0},
	 // 200 quest not done
	{"Bunnies guard caravans, smash bunnies! SMASH!",
	 0,0},
	 // 201 quest done
	{"Crush now have money, buy dictionary! You take extra.",
	 77,0},
	 // 202 all done
	{"Crush learning plenty new words! Like \"the\" and \"is!\" Crush having trouble with \"I\" though.",
	 0,0},
	 // JIMMY TWO-FINGERS
	 //203 intro+quest
	{"Yo, my name's Jimmy. Jimmy Two-Fingers. They call me that because my muscular dystrophy "
	 "keeps making me drop my club. It's a very rude nickname.",
	 0,204},
	 // 204
	{"Anyway, one of our \"associates\" in the axe-smuggling business is in a bit of trouble. "
	 "Seems he heard the local law enforcement is planning to shut him down.",
	 0, 205},
	 // 205
	{"Go meet up with him in the Hiemal Holes and fend 'em off. "
	 "We gotta protect our partners!",
	 78,0},
	 // 206 not done
	{"Our friend is keeping stores in a number of huts underground near the Rowdy River. "
	 "Try and save as many of the huts as you can.",
	 0,0},
	 // 207 done, but don't have the money
	{"Thanks a bundle! With your help, the Onion Ring will be able to continue "
	 "the illegal weapons trade! My hero, if nobody else's!",
	 0,208},
	 // 208 done
	{"Come here and help yourself to some of the stores you've been protecting.",
	 79,0},
	 // 209 all done
	{"Appreciate all your help. Let me know if I can - oops, dropped it again. "
	 "Hold on, let me - whoops! Don't laugh! This is a serious medical condition!",
	 0,0},
	 // SMASH
	 // 210 intro+quest
	{"Greetings, good sir, I am Dr. Reginald I. Smash, Ph. D. I have devoted much time to the study of our foes, "
	 "and I believe they are attempting to establish",
	 0,211},
	 // 211
	{"a backup supply depot at the abandoned MacGregor farm. "
	 "My calculations suggest that inhibiting this operation would hamper them to a great degree!",
	 80,0},
	 // 212 quest not done
	{"I have re-checked my calculations, and have confirmed that destroying the supply depot "
	 "could disrupt their supply capability by up to 17.3%!",
	 0,0},
	 // 213 quest done
	{"Congratulations, honored Sir! Your prowess has ensured that the Onion Ring "
	 "will be able to continue holding the foe at bay. Huzzah and Huzzah again!",
	 81,0},
	 // 214 all done
	{"A-ha! I have finally deduced it! There appears to be a great secret in this land, "
	 "requiring great mental acuity to discern. But the first, most crucial step is to NOT ret",
	 0,0},
	 // GUY MAGIC
	 // 215 normal shopping
	{"Welcome to this franchise of The Magic Box, offering the best prices on magical goods! "
	 "We got potions! We got amulets! We even got glasses!",
	 0,0},
	 // 216 package for curaga
	{"One of the rare convos that doesn't exist "
	 "even in the Absurd Area recompile!",
	 82,0},
	 // 217 AXES R US shakedown
	{"They're here! Defend the Storage Huts to protect my smuggling operation! "
	 "For every hut you save, I'll reward you!",
	 0,0},
	 // 218 TAYLOR shakedown
	{"Not supposed to be seeing this. Play the normal campaign with an unpatched .exe, silly!",
	 84,0},
	 // 219 MAGIC GUY shakedown
	{"Yet another invisible conversation! "
	 "Maybe I'll fix that later!",
	 85,0},
	 // 220 ROPES shakedown
	{"Well, as long as you're hacking, might as well give you a secret! Did you know that all you have to do "
	 "to get the secret reward is to make sure you do NOT, until beating the Toy F",
	 86,0},
	 // 221 take candy from Bobby
	{"WAAAHHH!! I should have figured my sweets supremacy wouldn't last. "
	 "But I still have a market share majority! Don't think I'll just roll over!",
	 87,0},
	 // CAPTAIN CAPITAN
	 // 222 rescuing you
	{"YEEHAW! Howdy, pardner! I'm Captain Capitan, leader of The Order of the Snuggly Bunny! "
	 "We've finally begun our operation to clean up this here area!",
	 0,223},
	 // 223
	{"What, you haven't heard of us? Peacekeepin' force for good? "
	 "Selflessly riskin' ourselves to take down scoundrels threatenin' law and order?",
	 0,224},
	 // 224
	{"Sigh... I knew lettin' a five-year-old princess choose the name of the King's elite knights "
	 "was a bad plan. A knight of my dignity, reduced to such a state!",
	 0,225},
	 // 225
	{"But never mind. We seen what y'all done did at the Toy Factory, "
	 "and we're mighty impressed! Take the key outside and head west to our HQ. We'll let you join up!",
	 88,0},
	 // 226 waiting for you to exit prison
	{"Note to self: Next time, dig an invasion tunnel leadin' to a room that ain't locked. "
	 "Hey, and watch it out there! Things are gettin' mighty rough.",
	 0,0},
	 // 227 assign rank quest
	{"YEEHAW! Glad y'all made it. Now, we here are an elite bunch, so y'all gonna have to prove you're worth it. "
	 "Complete any 3 missions and come back, y'hear?",
	 68,0},
	 // 228 rank quest unfinished
	{"Keep an eye out for them Onion Ring villains, y'hear? They been causin' trouble all over here for a while, "
	 "and the King sent us in to clear things up.",
	 0,229},
	 // 229
	{"So you'll probably be seein' a lot of them. Just don't listen to any rumors you might hear from them, y'hear? "
	 "This here's a standard cleanup operation, no question!",
	 0,0},
	 // 230 rank quest done
	{"Now THAT'S the sort of skill I'm lookin' for! Good on yer! You're now a full member, "
	 "givin' you the right to take some of us on missions of your own!",
	 0,231},
	 // 231
	{"Now, if you REALLY want to impress me, there's one thing I'm hopin' you can handle. "
	 "The Onion Ring's been causin' trouble in these here parts for far too long.",
	 0,232},
	 // 232
	{"Your mission, if y'all choose to accept it, is take out their leader, The Diabolical Onion. "
	 "That oughta throw them into disarray so we can clean 'em up for good!",
	 89,0},
	 // 233 leader quest undone
	{"Bein' a full member also gives you the right to dress up as one of us. "
	 "If you want to. Suit up in the Barracks. It'll cost a Modifier slot though.",
	89,0},
	// CLOCKMAKER
	// 234 assign clocks quest
	{"Hey! You there! Can you help? It's an emergency! Oh, are you new? "
	 "I make Clockwork Robots for the Order to use. But a batch of them has gone rogue!",
	 0,235},
	// 235
	{"I tried ordering them to stop, and even severe reprimands, but none of the standard cliches work. "
	 "I'm afraid the only thing left is to dismantle them. Forcibly!",
	70,0},
	// 236 quest undone
	{"I tried kicking them and everything! I even blew into the control modules!",
	0,0},
	// 237 quest done
	{"All right! Thank you so much. Come on inside. I have a few much safer experiments  "
	 "to try which you might find very helpful!",
	90,0},
	// 238 all done
	{"Drop by anytime with a Clockbot containing every Junk item, and I'll upgrade it for you. "
	 "More Junk than that and I'll upgrade it further!",
	 0,0},
	 // 239 you are his enemy
	{"Sigh... Bring me a Clockbot with one of every Junk item and I'll upgrade it for you... "
	 "If I have to. More junk items than that, and I'll upgrade it further.",
	 0,0},
	 // MAJOR FISHBUG
	 // 240 assign supplies quest
	{"Bah. Look at this ridiculous getup. You think I'd be wearing this if I had a choice? "
	 "But no, it's all \"us officers need to set an example to instill team spirit.\" Harumph.",
	 0, 241},
	// 241
	{"What was that? That chuckle just earned you a job, mister! "
	 "The Onion Ring's been caching supplies in the cliffs north of Tweestad. Go dig up and destroy them.",
	 80,0},
	 // 242 supplies undone
	{"These supplies are somewhat critical to the Onion Ring's long-term plans, "
	 "so they'll be well-guarded. Don't be taken by surprise.",
	 0,0},
	// 243 assign rumble
	{"You're just in time. That supply operation must have shown them we really mean business. "
	 "They're gathering up for a massive attack.",
	 0,244},
	 // 244
	{"I think we can take them, but we'll need your help. If we can push them all the way back "
	 "to their hideout, we can keep them there until they cry Uncle!",
	74,0},
	 // 245 Rumble unfinished
	{"Prepare for a long, nasty fight. It won't be easy. Worse, they've teamed up "
	 "with some of those evil toys. Keep on pressing ahead!",
	 0,0},
	 // 246 rumble done
	{"You got guts, kid. We all owe you one. Take this Objective Point, and be sure to call on me "
	 "whenever you're in trouble! We also captured one of their outfits.",
	75,0},
	 // 247 all done
	{"Keep up the good work. We're all in your debt. And I can tell you a rumor I heard about a great secret! "
	 "To open it, you must make careful use of certain items. First, KEE",
	 0,0},
	 // CLOCKWORK AGAIN
	 // 248 assign miner quest
	{"So, if you've been making Clockbots for a while, I'm sure you've noticed that it takes "
	 "great skill, not to mention materials, to make robots of any quality.",
	 0,249},
	 // 249
	{"Fortunately, I am an expert craftsman! If you can bring me a Clockbot with at least "
	 "one of each of the 10 types of Junk items, I'll upgrade its abilities for you!",
	 72,0},
	 // 250 not done
	{"I'd love to power up a Clockbot for you, but my experiments require a Clockbot built with at least one of each Junk item.",
	 0,0},
	 // 251 got it
	{"Thank you for the opportunity to test my skills to their limits. Come back anytime you need a bot improved!",
	 91,0},
	 // HORNSWAGGLE
	 // 252 assign highway patrol
	{"Hey, a recruit to push around! Er, I mean, a conscientious do-gooder. "
	 "The Onion Ring is planning on a bit of banditry against Tweestad! Protect the townsfolk!",
	 76,0},
	 // 253 not done
	{"Some of the houses might get damaged in the assault. "
	 "But if you save them, the townsfolk will reward you. So, save as many houses as you can!",
	 0,0},
	 // 254 done
	{"Hey, good job! Now, it's time to take credit for all your hard work! Thanks so much for this opportunity!",
	 92,0},
	 // 255 all done
	{"Tum te tum, moving up in the world through shameless exploitation "
	 "of those lower on the totem pole... it sure is great to not have scruples!",
	 0,0},
	 // PUBLIC
	 //256 assign shutdown
	{"Oh hey, another unsuspecting greenhorn to suck up to! Er, a capable and high-minded hero! "
	 "I've been assigned to clean out a band of axe smugglers, but",
	 0,257},
	 // 257
	{"I'm not sure I can handle it without one of your talent and renown. "
	 "They've been supplying the Onion Ring from their base under the Rowdy River. Let's go get 'em!",
	 78,0},
	 // 258 not done
	{"They might be scattered through quite a bit of the lower parts of the Hiemal Holes, "
	 "so make sure you, I mean \"we,\" search thoroughly.",
	 0,0},
	 // 259 did it
	{"Hey, good work, teammate! It's a shame all the work I did was just out of sight from you. "
	 "I've already reported \"our\" success, no need to bother the brass with details!",
	 93,0},
	 // 260 all done
	{"La di da, moving up in the world through spineless toadying up to those higher on the totem pole... "
	 "it sure is great to not have a sense of dignity!",
	 0,0},
	 // ONION AGAIN
	 // 261
	{"MUAHAHAHA!! I knew it was about time to be cutting ties. "
	 "Pity he made his move before I did! Eh? I can tell from your dumbstruck look he didn't clue you in.",
	 0,262},
	 // 262
	{"Well, how about I tell you what's REALLY been going on? "
	 "You didn't think this all happened at the same time by accident, did you?",
	 0, 263},
	 // 263
	{"Unleashing a horde of evil toys, building that Furnace, the local power conspicuously absent... "
	 "this was all a master plan orchestrated by...",
	 94,0},
	 // 264 (now Capitan)
	{"Take THAT! Phew! Just in time, I'm sure! Y'all sure looked like you could use some help. "
	 "I'm sure that two-faced shallot tried to confuse you, too.",
	 0, 265},
	 // 265
	{"But now it's all over. Loose ends all tied up! I'm pretty sure "
	 "everythin' is going to be completely better now with nothing to worry about from any angle.",
	 0, 266},
	 // 266
	{"Pretty sure the volcano packed with unstoppable Ice Titans poses no threat we can't handle easily. "
	 "In fact, I'm so sure of this, I'm not even goin' to tell the King!",
	 0,267},
	 // 267
	{"And don't forget: Any rumors of some sort of \"furnace\" involved "
	 "are totally unfounded. Now head on back now, y'hear?",
	95,0},
	// 268 capitan sitting around in onion camp
	{"Huh? What? You say somethin'? Sorry, I was distracted by "
	 "how thoroughly we just wrapped up this whole deal. Awful distractin', that.",
	0,0},
	// 269 onion just in case you see him before getting this quest somehow, but aren't on his side
	{"MU HU HA HA!  The Onion Ring cannot be stopped!",
	0,0},
	// CAPITAN WHEN VICTIM OF ATTACK
	// 270
	{"Ah-HA! I knew this would happen before long. So the Onion realized I ain't gonna need him no more, "
	 "and sent you to clean up? Well ain't that dandy.",
	 0, 271},
	 // 271
	{"But I ain't dumb enough to deal with a two-faced scallion "
	 "like that without bein' prepared. Draw, pardner!",
	 97,0},
	// 272 (now it's Fishbug)
	{"Stop right there! This is unforgiveable! I knew something was fishy about all this, but you? "
	 "The most successful knight in the kingdom? Why would you risk everything",
	 0,273},
	// 273
	{"on whatever this scheme is? What could you possibly hope to gain from... "
	 "but no. No point in asking questions now. Tell it to the judge! You're under arrest!",
	 0,0},
	 // 274 (Capitan has been beaten down to 50%, now escapes)
	{"Argh! You varmints! Y'all have no clue what y'all are messin' with! "
	 "With that Furnace in place, my new friends are invincible! You'll be sorry!",
	 98,0},
	 // 275 (now fishbug assigns a quest)
	{"Bah, he got away! Well, at least I have more information. "
	 "And you! Can you be trusted to help fix things around here? If you can, find a way into the volcano.",
	 0, 276},
	 // 276
	{"It all started there, somehow. Look for leads on this Furnace. "
	 "But don't think I haven't noticed who your new friends are. I'll be keeping my eye on you.",
	 99,0},
	 // BETTY
	 // 277 book quest
	{"Welcome to the super-secret Trial of the Civilian, where you will put your skills to the test... "
	 "or die! Just kidding, there isn't a Trial of the Civilian.",
	 0,278},
	 // 278
	{"But if you purchase all 5 Civilian upgrades, I'll give you some sweet loot! "
	 "Yeah! And hey, if you return this book for me, I'll give you a taste!",
	 0,279},
	 // 279
	{"Take it back to the Librarian in the Rooms of Research. "
	 "You know, out in the taiga to the east?  Then come get your reward!",
	 100,0},
	 // 280 quest not done
	{"Ah, this is the life! Drinking diet soda and eating unbuttered unsalted popcorn "
	 "while watching vicious mayhem! Did you return the book yet?",
	 0,0},
	 // 281 quest done
	{"I just realized that I have no way of telling whether you actually returned the book or not! "
	 "But, I'm going to assume you're being truthful. Enjoy the prize!",
	 101,0},
	 // 282 all done
	{"Hey hotshot. Grab a seat and watch some games! "
	 "Or do you think you can handle it in the ring? I'll be cheering you on!",
	 0,0},
	 // 283 all done, you have beaten Killbor!
	{"Wow, you beat all the Hidden Trials! Nice! "
	 "With talent like yours, you could accomplish anything!",
	 0,0},
	// 284 historian given book
	{"My word! You're returning a book! I'd thought I wouldn't see any returns until the danger had passed! Top egg!",
	 102,0},
	 // OLD SHRUMFORD
	 // 285
	{"my back... my aching back... back in my day we used to carry the elderly around... youth these days have no respect...", // AA change
	 0,0},
	 // SCHRODINGER
	 // 286 intro and first furnace quest
	 // AA change {
	{"What ho, my good chap! Salutations to you and your... heavily armed chums. "
	 "And I'm not just saying that because you have weapons and we don't!",
	 0,287},
	 // 287
	{"We Shroomulans are pacifists, so we were helpless when the Ice Titans escaped the underworld. "
	 "Those ruffians broke out when they built their ghastly Furnace.",
	 0, 288},
	 // 288
	{"By standing close to it, a little bit every day, they managed to build up a tolerance for heat! "
	 "Nothing can stop the beastly Titans if this continues!",
	 0, 289},
	 // 289
	{"So I built a giant bomb to blow the Furnace up. It's still sort of like pacifism if you do it from a distance, right? "
	 "Tally ho!",
	 0,290},
	 // 290
	{"But some hooligans sabotaged my bomb and took four vital components! I'll need them back to stop the Titans. "
	 "The first, the Fission Chip, is in the Crystal Caves.",
	 0, 291},
	 // 291
	{"It's behind a mystical barrier, so you will need to carry a Sounding Crystal to disrupt its magykal resonance. "
	 "Be careful, it's very fragile!",
	 103,0},
	 // 292 furnace 1 undone
	{"The Crystal Caves are to the west. I'd show you to the barrier, but it's a wee bit dangerous.",
	 0,0},
	 // 293 furnace 1 done, assign furnace 2
	{"You found it! Smashing! The next piece, the Doomsday Clock, was pinched by some leafy rotters to the northwest. "
	 "That wretched Gourdzilla up and swallowed it.",
	 104,0},
	 // 294 furnace 2 undone
	{"Gourdzilla set up in the Loathsome Lawn to the northwest. Good luck! Toodle pip!",
	 0,0},
	// AA change } {
	 // 295 furnace 2 done
	{"Top egg! Now for the next part - some ruffian tore out the power feed cables. "
	 "Shrumford is the only one who makes the cables, but he's in no shape to help.",
	 0,296},
	 // 296
	{"We will have to rejuvenate him first. I should explain... We Shroomulans were once people much like yourself... "
	 "until long ago, one of us discovered immortality!",
	 0, 297},
	 // 297
	{"He was a great and powerful wizard, who found how to transfer souls into mushroom bodies. "
	 "Naturally, he shared this secret with the whole tribe for free!",
	 0,298},
	 // 298
	{"His image is still proudly displayed in this village, right over... wait, which mushroom was his statue? "
	 "I quite forgot.",
	 0, 299},
	 // 299
	{"No matter. To recover our youth, we need to transfer into one of the mushrooms in the sacred Fungal Forest "
	 "to the north. Only those mushrooms have the right elan.",
	 0,300},
	 // 300
	{"Shrumford is in dire need of a rejuvenation - but it's too dangerous! Amongst all the chaos, "
	 "some nasty Bodzhas moved in. They take a leak right on the mushrooms!",
	 0, 301},
	 // 301
	{"We'd take care of the rotten things ourselves, but - did I mention? Pacifists. It's normally great for "
	 "guilting others into protecting us, but it's urgent this time!",
	 0,302},
	 // 302 finally assign the quest
	{"So we need you to do our dirty work for us one more time. After that, I'm sure Shrumford can replace "
	 "the damaged power feed cables for us.",
	 105,0},
	 // 303 quest not done
	{"Don't forget to check both sides of the Fungal Forest. "
	 "You never know where the dastardly rapscallions will be hiding out.",
	 0,0},
	 // 304 got them all!
	{"You got them all? Cracking! And I didn't even need to dirty my hands! Another win for pacifism! "
	 "We'll bring Shrumford to find a suitable mushroom. Meet us there!",
	 106,0},
	// 305 quests done, haven't visited Shrumford
	{"Last time I was there, I saw some promising candidates to the north, near the edge of the gorge.",
	 0,0},
	 // 306 in the forest
	{"Watch in awe, we've never before honored an outsider by letting you watch the transformation! "
	 "No flash photography, please.",
	 0,0},
	 // 307 forest after transform
	{"For a sacred ritual, that sure was over quickly.",
	 0,0},
	 // 308 back in town
	{"You'll have to cross the Parched Pit to reach the Furnace. But... where's the winding key? "
	 "Hey, why are you dressed that way in a volcano, what?",
	 0,0},
	 // SHARONA
	 // 309 intro+bokbok
	{"Hello! Soooo, yes I'm a mushroom person, get used to it. "
	 "Listen, I lost some of my Bokboks in the surface world. Can you bring them back to me?",
	 112,0},
	 // 310 have no bokboks in tow
	{"They must be out there somewhere!  The poor little critters, lost "
	 "in this freakishly bright land with all the weird moving air and stuff... Eugh!",
	 0,0},
	 // 311 have some
	{"Oh thank you!  I hope you can find the rest of them before they get taken by a tornado or something. "
	 "Or get hit by this \"lightning?\" The surface is weird!",
	 108,0},
	 // 312 got them all
	{"Alright!! Thank you so much! Here, you can have this. One of the Bokboks thought it was an egg "
	 "and was trying to hatch it. They, um, aren't very smart.",
	 0,313},
	 // 313
	{"They are cute though! If you'd like, you can borrow one anytime. "
	 "Step into the left-hand door to check one out and into the right to return it.",
	 109,0},
	 // 314 treehugger quest
	{"Well, I suppose you're wondering what a person of my... different appearance is doing here. "
	 "Normally we of the Shroomulan race live underground, in the",
	 0, 315},
	 // 315
	{"dormant volcano north of your Sankyo. However, we are not very skilled in the ways of self-defense, "
	 "and there are many more dangerous creatures of late.",
	 0,316},
	 // 316
	{"I'm here to study your local aggressive flora. If you bring me enough seeds, I could breed a strain "
	 "that would defend us instead of shredding everything in sight!",
	110,0},
	// 317 quest not done
	{"I think I'd need at least 20 seeds to have enough genetic diversity to begin my program. "
	 "If you can bring them, I'll give you an amulet!",
	128,0},
	// 318 got them
	{"Thank you so much! I'll get started right away! Here, take this amulet. "
	 "If you plan on using much plant-based magic, it could be quite useful!",
	111,0},
	// 319 all done
	{"I've started on my project, but it'll be a long process. "
	 "Not that I'm in a hurry to get back down there. We Shroomulans can be a bit... eccentric.",
	128,0},
	// SHROUD
	// 320 shop
	{"My heart shall rejoice nevermore... For lo, all my hopes and dreams are as extinct as this volcano... hey, no window shopping!", // AA changed
	 0,0},
	 // SHRUMFORD in forest
	 // 321
	{"you call that a mushroom? back in my day we had mushrooms twice this size! and they glowed brighter! "
	 "and we had to walk five miles uphill just to reach them!",
	107,0},
	// young SHRUMFORD
	// 322 give item
	{"Ayo fam, it was me that pulled the wires out. I was huffing that pacifism brainrot. But I got a new outlook now... "
	 "take these spare cables and go blow up that Furnace!", // change lingo?
	 113,0},
	 // 323 waiting for you to do furnace
	{"Pacifism is for boring old boomers. Blow the Furnace to rubble and do a griddy on the ashes! No cap!",
	113,0},
	// AA done }
	// 324 furnace done, so titan quest
	// AA change {
	{"Um, what the sigma? You yeeted that furnace! I bet you have the rizz to take out the Titanic Titans too! "
	 "This key will get you into the Titanic Treasury.",
	 0,325},
	 // 325
	{"They're guarding some serious drip in there... if you get the Wing Boots, you can walk on clouds! "
	 "That would be so extra!",
	 114,0},
	 // 326 quest not done
	{"Be careful in the Titanic Treasury. The Titans first came out of there, so it's a pretty sus place... "
	 "hey, why'd my bed change colors when I rejuvenated?",
	115,0},
	 // 327 did it
	{"You got the Boots! Slay! We sure don't need them down here, but I'm sure you can find a high place to use them above ground. "
	 "Stay skibidi!",
	 115,0},
	 // 328 all done
	{"What have I done? ... Now I am become Death... the destroyer of furnaces...",
	 0,0},
	 // AA done }
	 // AA change {
	 // SHARONA in forest
	 // 329 before transform
	{"I came as soon as I heard the news... it's always nostalgic to come back home.",
	 0,0},
	 // 330 after transform
	{"Annnd nostalgia satisfied! I'm good for another year.",
	 0,0},
	 // SHROUD in forest
	 // 331 before transform
	{"Sigh... once again we thwart the Reaper, binding Shrumford again to the cycle of misery...",
	 0,0},
	 // 332 after
	{"Aged suffering / Youth returns with growing pains / Cycle never ends",
	 0,0},
	 // AA done }
	 // CAPTAIN in furnace
	 // AA change {
	 // 333
	{"This volcano ain't big enough fer the two of us! Without this here windup key, your little bomb is just a pile of junk! Draw!",
	 0,0},
	 // 334 beat him
	{"Tarnation, I've been defeated! But I can't fall here... I must make my retreat! I will have to flee the whole kingdom with nothing. "
	 "Frostburn will never forgive me!",
	 0,335},
	 // 335
	{"You don't know who yer messin' with, pardner! Frostburn wanted me to guard that furnace to please his new, powerful friends. You could never defeat him!",
	 117,336},
	 // 336 (now Major Fishbug)
	{"It is always easier to serve a tyrant than to oppose him. That's why cowards tend to choose the deuce. "
	 "You're under arrest, and you can explain it to the King himself!",
	 0,337},
	 // 337
	{"I believe this key is for your bomb, Loony... take it with my thanks. You may do the honors yourself. "
	 "Then, it's about time you stopped this treasonous Baron!",
	 116,0},
	 // 338 finish furnace quest
	{"Ah, the satisfaction of a job well done. The only thing that's left to do is to pry the Baron out of his castle. "
	 "Let's put an end to this travesty!",
	 118,0},
	 // AA done }
	 // TOYMONGER in GORGE
	 // AA change {
	 // 339 reintro + quest
	{"You again! It's good to see you. I was pondering how to make my financial recovery, and I decided "
	 "it wouldn't be prosperous with this villain about.",
	 0,340},
	 // 340
	{"All this political instability is bad for business! Ahem, and it's certainly unethical as well. So, "
	 "I thought I could lend a hand.",
	 0,341},
	 // 341
	{"I was able to salvage these 5 advanced prototype siege puppets from the ruins of my Factory, and I think "
	 "they could get you into Frostburn's castle.",
	 0,342},
	 // 342
	{"But, the vagaries of business are unmerciful. I was accosted by those stinkin' Bodzhas and they stole "
	 "the Control Crystals for the puppets!",
	 0,343},
	 // 343
	{"You will have to recover all 20 of them. But, I don't know where the Bodzhas are hiding! There must be some way "
	 "to draw them out...",
	 119,0},
	 // 344 quest not done
	{"I think the Bodzhas don't want to be found. If only you had some irresistible way of bringing them to you...",
	 0,0},
	 // 345 got em all
	{"You recovered them all! Give me a second to blow the dust out, and I'll just slot these in. If these puppets get damaged, "
	 "bring them back for repairs.",
	 0,346},
	 // 346
	{"Now be careful in there! The Baron stole the biggest and most dangerous of my toys when he was squatting in my factory. Keep your wits about you.",
	 120,0},
	 // 347 back with injured/dead bots
	{"Bye-bye, boys! Have fun storming the castle!",
	 121,0},
	 // AA done }
	 // MAGICSMITH STUFF
	 // 348 mimic quest not done
	{"I mean, I could just store my tools in a chest or a barrel. But mimics can teleport to you. "
	 "They're so useful! I hear they can be found in the Gelid Grove.",
	 0,0},
	 // 349 mimic quest done
	{"Thanks! Now I don't have to carry my stuff around myself. "
	 "A victory for laziness! And all it took was a bit of animal slavery!",
	 0,350},
	 // 350
	{"Hey, you seem like you're good with taming critters. Have this amulet as a reward. "
	 "And don't forget to tame a Mimic for yourself!",
	 16,0},
	 // STASH
	 // 351
	{"Move items in or out of your Mimic here. Arrows select, Fire moves the selected item. Press Magic to move all the items in a stack "
	 "at once. Jump or ESC to exit!",
	 0,0},
	// BARON
	// 352
	{"You doddering dunderhead! You ruined all my plans! My army of evil toys... My mighty allies from the depths... "
	 "Even my plot to use that stupid bunny to",
	 0,353},
	 // 353
	{"assassinate the King! But no matter. When I am united with the King of the Titans, nothing will be able to stop me! "
	 "This world will soon be mine! En garde!",
	 0,0},
	 // 354 you have weakened him
	{"I don't have time for this! The King of Titans has promised to merge my body with his. I shall claim ultimate power and destroy you! "
	 "Melton! It is time!",
	 0,0},
	 // MELTON
	 // 355
	{"At last, I am returned to the sunlit world! My conquest begins anew! I shall reward you, as I promised... come over here and "
	 "we shall \"merge...\"",
	 123,0},
	 // 356, baron's dead
	{"Yum, delicious! And now, to begin my rampage! I shall start with my revenge against you for destroying my Furnace, puny mortal!",
	 0,0},
	// 357, he's dead
	{"Ack! I yield! I was forced to arise too soon. I shall return to my home far below, and wait for a better time to return "
	 "and finish the job...",
	 0,358},
	 // 358
	{"But do not think this is ended. The Baron's scheme was not merely his own idea. No mere mortal could have come up with a plan this... "
	 "Dumb.",
	 0,359},
	 // 359
	{"The true fiend behind all this still waits for you. Take this key, and climb to the highest place you can find. From there, "
	 "you have have been watched all along.",
	 0,360},
	 // 360
	{"I know not this being's true purpose, nor if you have the power to overcome its will. But if you can best me, "
	 "the King of Titans...",
	 0,361},
	 // 361
	{"Well, I don't like its chances. I shall watch you with interest from my frozen home. I just got this sweet new HDTV.",
	 122,0},
	 // BIRD
	 // 362
	{"At last, a champion has arrived, both powerful enough and Dumb enough to be worthy of Our attention. "
	 "We have watched you long.",
	 0,363},
	 // 363
	{"For hundreds of years, We have caused calamities like this, and watched to see who rose to Our challenges. "
	 "It is through adversity that true greatness is revealed.",
	 0,364},
	 // 364
	{"It's a bit like that one Bruce Willis movie. The one where he can't get wet, and the bad guy is some dude with weak bones.",
	 0,365},
	 // 365
	{"And when the catastrophe is overwhelmingly Dumb, only the greatest and Dumbest will have the character to excel.",
	 0,366},
	 // 366
	{"Whether they take advantage of Our disruption, or correct it, We are waiting to add their essences to Happy Stick! "
	 "Our power and Dumbness grow without limit.",
	 0,367},
	 // 367
	{"We are a bird and a man, a boy and a witch, and many more... but the one thing We are not, is a hero. "
	 "But after We defeat you today, We will be unstoppable!",
	 0,368},
	 // 368
	{"All will become Happy Stick! Hahahaha squawk squawk squawk!",
	 125,0},
	 // dead
	 // 369
	{"We have failed today, but We cannot be defeated. Happy Stick is eternal, and eternal is Our patience! "
	 "We will see you again, and claim a Happy Stick Champion!",
	 124,0},
	 // WOODY STARTING MADCAP
	 // 370
	{"Welcome to Madcap Mode!  Madcap Mode works exactly the same in Absurd Area. You've lost all quest items, and all the quests are undone.",
	 0,371},
	 // 371
	{"The enemies are about ten times more powerful than they were.  But, you can use the new option in the Pause Menu, \"Madcap Items\", "
	 "to spend money and",
	 0,372},
	 // 372
	{"Crystal Points to buy Crystals that boost your power so you can actually defeat them.  Sometimes.  You get a "
	 "Crystal Point for every 1000 XP you earn.",
	 0,373},
	 // 373
	{"And hey! Since everything reset, you have another shot at the Super Secret! Have you found any hints about it? To avoid locking yourself out of it, you will need to",
	 0,0},
	 // GURU AGAIN
	 // 374
	{"I can reset all your assigned Skill Points... for a fee. Eating vegan isn't cheap, you know!",
	 130,0},
	 // 375 have all talents but haven't completed quest // AA CHANGE TO shop done
	{"OK, now repeat after me: \"Ommmmmmmmmm!\" Great! Now, make better choices this time!",
	 0,0},
	 // WOODY FINAL BIT
	 // 376
	 {"Hey, you found five of those glowy things!  Let me try something...",
	 0,377},
	 // 377
	{"I saw this trick in a LifeHacks tutorial...",
	0,378},
	// 378
	{"Follow the pictures in the instruction manual...",
	0,379},
	// 379
	 {"This part goes in the square hole...",
	 0,380},
	 // 380
	 {"Ta-da!  An artifact!  ... No refunds.",
	 126,0},

};

word curChat,curVillager;
int chatBright;
byte noButtons;
sprite_set_t *faceSpr=NULL;

void ExitChatStuff(void)
{
	if(faceSpr)
	{
		delete faceSpr;
		faceSpr=NULL;
	}
}

char *QuestName(byte quest)
{
	if(player.var[VAR_SNUGGLY] && (quest>=QUEST_ALLIES && quest<=QUEST_FURNACE))
		return altQuestName[quest-QUEST_ALLIES];
	else
		return questName[quest];
}

char *QuestReport(byte quest,byte doneOk)
{
	static char s[64];
	byte i,p;

	s[0]='\0';

	if(player.var[VAR_QUESTDONE+quest])
	{
		if(doneOk)
			strcpy(s,"(COMPLETED!)");
	}
	else switch(quest)
	{
		case QUEST_PRESENTS:
			p=GotItem(IT_PRESENT);
			if (p > 99)
				p = 99;
			sprintf(s,"(%d/99 Presents found)",p);
			break;
		case QUEST_PROVE: // changed for AA
			if(player.level >= 25)
				sprintf(s,"(Level 25/25)");
			else
				sprintf(s,"(Level %d/25)",player.level);
			break;
		case QUEST_PROVE2: // changed for AA
			sprintf(s,"(%d/10 Highest Combat Talent Level)",MaxTalentLevel(TLTP_COMBAT));
			break;
		case QUEST_HERBS:
			if(EquipCount(IT_HERBF)>=3)
				sprintf(s,"(3/3 Frostbane collected)");
			else
				sprintf(s,"(%d/3 Frostbane collected)",EquipCount(IT_HERBF));
			break;
		case QUEST_WOOD:
			if(player.var[VAA_UPGRADENUM] > 1)
				sprintf(s,"(2/2 Upgrades Selected)");
			else
				sprintf(s,"(%d/2 Upgrades Selected)",player.var[VAA_UPGRADENUM]);
			break;
		case QUEST_SHARPEN:
			sprintf(s,"(%d/1 Sharpening Stone found)",GotItem(IT_SHARPSTONE));
			break;
		case QUEST_MIMIC:
			if(player.var[VAR_TAMED]&1)
				sprintf(s,"(1/1 Mimics tamed)");
			else
				sprintf(s,"(0/1 Mimics tamed)");
			break;
		case QUEST_CRYSTAL:
			sprintf(s,"(%d/1 Crystal Of Binding found)",GotItem(IT_BCRYSTAL));
			break;
		case QUEST_AXES:
			p=0;
			p+=player.axe.count-1;
			for(i=0;i<MAX_EQUIP;i++)
				if(player.items[i].type==IT_AXE || player.items[i].type==IT_MAGICAXE || player.items[i].type==IT_GOLDENAXE)
					p+=player.items[i].count;
			if(p>=3)
				sprintf(s,"(3/3 extra Axes collected)");
			else
				sprintf(s,"(%d/3 extra Axes collected)",p);
			break;
		case QUEST_BOBSWIFE:
			sprintf(s,"(%d/1 Wife found)",player.var[VAR_QUESTDONE+QUEST_BOBSWIFE2]);
			break;
		case QUEST_BOBSWIFE2:
			sprintf(s,"(%d/1 Wife unchained)",player.var[VAR_WIFEUNLOCK]);
			break;
		case QUEST_GURU:
			//p=0;
			//for(i=0;i<MAX_TALENTS;i++)
			//	if(player.talentLevel[i]!=255)
			//		p++;
			sprintf(s,"(%d/10 Highest Talent Level)",MaxTalentLevel(TLTP_ANY)); // changed for AA
			break;
		case QUEST_RATS:
			if(player.var[VAR_RATS]==1)
				sprintf(s,"(%d rat remains)",player.var[VAR_RATS]);
			else
				sprintf(s,"(%d rats remain)",player.var[VAR_RATS]);
			break;
		case QUEST_UNDIES:
			p=EquipCount(IT_HERBA);
			if(p>5)
				p=5;
			i=EquipCount(IT_SOCK);
			if(i>5)
				i=5;
			sprintf(s,"(%d/5 Asbestos and %d/5 Socks)",p,i);
			break;
		case QUEST_GLADIATOR:
			sprintf(s,"(%d/5 Gladiator Rank)",(int)TalentBonus(TLT_GLADIATOR));
			break;
		case QUEST_TOYS:
			sprintf(s,"(%d/200 Toys trounced)",player.var[VAR_TOYSDEAD]);
			break;
		case QUEST_BOOK:
			sprintf(s,"(%d/1 Book returned)",player.var[VAR_BOOKGIVEN]);
			break;
		case QUEST_LETTER:
			sprintf(s,"(0 Letters delivered)");
			break;
		case QUEST_ROPES:
			sprintf(s,"(0/1 Ropemakers rescued)");
			break;
		case QUEST_BALLOON:
			if (player.var[VAA_PEAKLIONS])
				sprintf(s, "(Safe To Land)");
			else
				sprintf(s,"(Not Safe Yet)"); // AA change
			break;
		case QUEST_HANDY:
			sprintf(s,"(%d/1 Toolboxes found)",GotItem(IT_TOOLBOX));
			break;
		case QUEST_SOUP:
			sprintf(s, "(%d/10 Highest Craft Talent Level)", MaxTalentLevel(TLTP_CRAFT)); // AA changed to talents
			break;
		case QUEST_BEARS:
			sprintf(s,"(%d/30 Bears beaten)",player.var[VAR_BEARSDEAD]);
			break;
		case QUEST_BIGBEAR:
			sprintf(s,"(0/1 Berserker Bears bonked)");
			break;
		case QUEST_DEVILGRASS:
			if(player.var[VAA_UPGRADESTAT+UPG_ENCHANT5] == 0
				&& player.var[VAA_UPGRADESTAT + UPG_SORCERY5] == 0) // AA changed to mage upgrades
				sprintf(s,"(0/1 Level 5 Mage Upgrades)");
			else
				sprintf(s,"(1/1 Level 5 Mage Upgrades)");
			break;
		case QUEST_DELIVERY:
			sprintf(s, "(%d/10 Highest Magic Talent Level)", MaxTalentLevel(TLTP_MAGIC)); // AA changed to magic maxtalent
			break;
		case QUEST_TITANS:
			if(EquipCount(IT_ICECUBE)<=3)
				sprintf(s,"(%d/3 Ice Cubes)",EquipCount(IT_ICECUBE));
			else
				sprintf(s,"(3/3 Ice Cubes)");
			break;
		case QUEST_SKILLS:
			sprintf(s,"(%d/30 Upgrades)",player.var[VAA_UPGRADENUM]); // AA changed to upgrades
			break;
		case QUEST_FACTORY:
			p = player.var[VAA_FACTORY1] + player.var[VAA_FACTORY2];
			sprintf(s,"(%d/3 Factory Sections demolished)", p);
			break;
		case QUEST_FACTORY2:
			strcpy(s,"(0/1 Power Cores crushed)");
			break;
		case QUEST_ALLIES:
			sprintf(s,"(%d/1 group of allies befriended)",player.var[VAR_QUESTDONE+QUEST_QUESTS]);
			break;
		case QUEST_QUESTS:
			p=0;
			for(i=QUEST_CLOCKJUNK;i<=QUEST_RUMBLE;i++)
				if(player.var[VAR_QUESTDONE+i])
					p++;
			if(p>3)
				p=3;
			sprintf(s,"(%d/3 Quests completed)",p);
			break;
		case QUEST_CLOCKJUNK:
			if(player.var[VAR_SNUGGLY])
			{
				if(player.levelNum!=LVL_SNUGGLYHQ || TaggedMonsterExists(2))
					strcpy(s,"(The Robots are still ticking!)");
				else
					strcpy(s,"(All Robots dismantled)");
			}
			else
			{
				//p = CurrentTalentLevel(TLT_JUNKSMITH);
				//if (p > 4)
				//	p = 4;
				//sprintf(s,"(%d/4 Junksmith Talent Level)", p);
				sprintf(s, "(%d/1 Magic Items Junksmithed)",player.var[VAA_MAGICJUNKED]);
			}
			break;
		case QUEST_EASYMINER:
			if(player.var[VAR_SNUGGLY])
			{
				sprintf(s,"(%d/1 Clockbots supercharged)",player.var[VAA_CLOCKGRADE]); // AA change requirement
			}
			else
			{
				sprintf(s,"(%d/1 Candy taken)",GotItem(IT_CANDY));
			}
			break;
		case QUEST_HIGHWAY:
			if(player.var[VAR_SNUGGLY])
			{
				sprintf(s,"(%d/1 Towns defended)",player.var[VAR_HIGHWAY]);
			}
			else
			{
				if(EquipCount(IT_STOLEN)<10)
					sprintf(s,"(%d/10 Goods stolen)",EquipCount(IT_STOLEN));
				else
					strcpy(s,"(10/10 Goods stolen)");
			}
			break;
		case QUEST_SHAKEDOWN:
			if(player.var[VAR_SNUGGLY])
			{
				sprintf(s,"(%d/1 Operation shut down)",player.var[VAR_SHAKEDOWN]);
			}
			else
			{
				sprintf(s,"(%d/1 Operation defended)",player.var[VAR_SHAKEDOWN]); // AA changed
			}
			break;
		case QUEST_SUPPLIES:
			sprintf(s,"(%d/20 Supplies destroyed)",player.var[VAR_SUPPLIES]);
			break;
		case QUEST_RUMBLE:
			if(player.var[VAR_SNUGGLY])
			{
				sprintf(s,"(%d/1 Rumbles won)",player.var[VAR_RUMBLE]);
			}
			else
			{
				sprintf(s,"(%d/1 Rumbles won)",player.var[VAR_RUMBLE]);
			}
			break;
		case QUEST_LEADER:
			if(player.var[VAR_SNUGGLY])
			{
				strcpy(s,"(0/1 Onions diced)");
			}
			else
			{
				strcpy(s,"(0/1 Captains capped)");
			}
			break;
		case QUEST_FURNACE:
			sprintf(s,"(%d/4 bomb parts)",GotItem(IT_THING)+GotItem(IT_THING2)+GotItem(IT_WATCH)+1*(player.var[VAR_CAPTAIN2]==3));
			break;
		case QUEST_FRAGILE:
			strcpy(s,"(0/1 Chip found)");
			break;
		case QUEST_PLANTS:
			strcpy(s,"(0/1 Clock found)");
			break;
		case QUEST_FOREST:
			sprintf(s,"(%d/100 Bodzhas bonked)",player.var[VAR_DEFILE]+player.var[VAR_DEFILE+1]);
			break;
		case QUEST_BOKBOK:
			p=0;
			for(i=0;i<10;i++)
				if(player.var[VAR_BOKFOUND+i])
					p++;
			sprintf(s,"(%d/10 Bokboks found)",p);
			break;
		case QUEST_HIPPY:
			if(EquipCount(IT_SEED)<20)
				sprintf(s,"(%d/20 Seeds collected)",EquipCount(IT_SEED));
			else
				sprintf(s,"(20/20 Seeds collected)");
			break;
		case QUEST_TITANS2:
			//p=0;
			//for(i=0;i<10;i++)
			//	if(player.var[VAR_TITANIC+i])
			//		p++;
			//sprintf(s,"(%d/10 Titans toppled)",p);
			sprintf(s, "(0/1 Boots recovered)");
			break;
		case QUEST_TOYS2:
			if(EquipCount(IT_TOYCRYSTAL)<=20)
				sprintf(s,"(%d/20 Control Crystals)",EquipCount(IT_TOYCRYSTAL));
			else
				sprintf(s,"(20/20 Control Crystals)");
			break;
		case QUEST_BARON:
			sprintf(s,"(%d/1 Barons beaten)",player.var[VAR_BARONDEAD]);
			break;
		case QUEST_CHAMPION:
			p=0;
			for (int i=0; i<5; ++i)
				if (player.var[VAA_TRIALSTAT+i] >= 3)
					++p;
			sprintf(s,"(%d/5 Trials Completed)",p);
			break;
		case QUEST_HAPPYSTICK:
			strcpy(s,"(0/1 Happy endings)");
			break;
	}
	return s;
}

void ReportQuestReport(byte quest)
{
	char *s;

	if(player.var[VAR_QUESTASSIGN+quest] && !player.var[VAR_QUESTDONE+quest])
	{
		s=QuestReport(quest,0);
		if(s[0]!='\0')
			NewMessage(s,0);
	}
}

void QuestReportEvent(byte event,int val)
{
	switch(event)
	{
		case QE_GET:	// got an item
			switch(val)
			{
				case IT_LOG:
					//ReportQuestReport(QUEST_WOOD); // AA removed
					break;
				case IT_HERBF:
					ReportQuestReport(QUEST_HERBS);
					break;
				case IT_SHARPSTONE:
					ReportQuestReport(QUEST_SHARPEN);
					break;
				case IT_BCRYSTAL:
					ReportQuestReport(QUEST_CRYSTAL);
					break;
				case IT_HERBA:
				case IT_SOCK:
					ReportQuestReport(QUEST_UNDIES);
					break;
				case IT_AXE:
				case IT_MAGICAXE:
				case IT_GOLDENAXE:
					ReportQuestReport(QUEST_AXES);
					break;
				case IT_ROPE:
					// AA removed
					break;
				case IT_TOOLBOX:
					ReportQuestReport(QUEST_HANDY);
					break;
				case IT_CAN:
					// AA removed
					break;
				case IT_HERBD:
					// AA removed
					break;
				case IT_ICECUBE:
					ReportQuestReport(QUEST_TITANS);
					break;
				case IT_PACKAGE:
					// AA removed
					break;
				case IT_CANDY:
					if(player.var[VAR_SNUGGLY]==0)
						ReportQuestReport(QUEST_EASYMINER);
					break;
				case IT_STOLEN:
					if(player.var[VAR_SNUGGLY]==0)
						ReportQuestReport(QUEST_HIGHWAY);
					break;
				case IT_TOYCRYSTAL:
					ReportQuestReport(QUEST_TOYS2);
					break;
				case IT_SEED:
					ReportQuestReport(QUEST_HIPPY);
					break;
				case IT_PRESENT:
					ReportQuestReport(QUEST_PRESENTS);
					break;
				case IT_THING:
				case IT_THING2:
				case IT_WATCH:
					ReportQuestReport(QUEST_FURNACE);	// AA addition
					break;
			}
			//if(val>=IT_LOG && val<=IT_CLAW && player.var[VAR_SNUGGLY]==0)
			//	ReportQuestReport(QUEST_CLOCKJUNK); // AA no junk
			break;
		case QE_KILL:	// killed a monster
			break;
		case QE_SKILL:	// picked up skill scroll
			//ReportQuestReport(QUEST_SKILLS); // AA removed
			break;
		case QE_TALENT:	// got a talent
			//ReportQuestReport(QUEST_GURU); // removed for AA
			break;
		case QE_QUEST:	// finished a quest
			if(val>=QUEST_CLOCKJUNK && val<=QUEST_RUMBLE)
				ReportQuestReport(QUEST_QUESTS);
			break;
		case QE_VAR:
			//if(val==VAR_PROVEKILLS && player.var[VAR_PROVEKILLS]<=20)
			//	ReportQuestReport(QUEST_PROVE);
			//if(val==VAR_PROVEKILLS && player.var[VAR_PROVEKILLS]<=100)
			//	ReportQuestReport(QUEST_PROVE2);
			if(val==VAR_TOYSDEAD && player.var[VAR_TOYSDEAD]<=200)
				ReportQuestReport(QUEST_TOYS);
			//if(val>=VAR_PRESENT && val<VAR_PRESENT+99)
			//	ReportQuestReport(QUEST_PRESENTS); // AA now it's items
			if(val>=VAR_SHAKEDOWN && val<=VAR_SHAKEDOWN+3)
				ReportQuestReport(QUEST_SHAKEDOWN);
			if(val==VAR_WIFEUNLOCK)
				ReportQuestReport(QUEST_BOBSWIFE2);
			if(val==VAR_RATS && player.var[VAR_RATS]<17)	// don't announce the full number
				ReportQuestReport(QUEST_RATS);
			if(val==VAR_BEARSDEAD)
				ReportQuestReport(QUEST_BEARS);
			if(val==VAR_RUMBLE)
				ReportQuestReport(QUEST_RUMBLE);
			if(val==VAR_QUESTDONE+QUEST_QUESTS)
				ReportQuestReport(QUEST_ALLIES);
			if(val==VAR_SUPPLIES && player.var[VAR_SUPPLIES]>0)
				ReportQuestReport(QUEST_SUPPLIES);
			if(val==VAR_HIGHWAY)
				ReportQuestReport(QUEST_HIGHWAY);
			if(val==VAR_BOOKGIVEN)
				ReportQuestReport(QUEST_BOOK);
			if(val==VAR_DEFILE || val==VAR_DEFILE+1)
				ReportQuestReport(QUEST_FOREST);
			if(val>=VAR_BOKFOUND && val<VAR_BOKFOUND+10)
				ReportQuestReport(QUEST_BOKBOK);
			//if(val>=VAR_TITANIC && val<VAR_TITANIC+10)
			//	ReportQuestReport(QUEST_TITANS2);	// AA removed
			if(val==VAR_TAMED)
				ReportQuestReport(QUEST_MIMIC);
			// Absurd Area quest vars
			if (val == VAA_FACTORY1 || val == VAA_FACTORY2)
				ReportQuestReport(QUEST_FACTORY);
			if (val == VAA_UPGRADENUM)
			{
				ReportQuestReport(QUEST_WOOD);
				ReportQuestReport(QUEST_SKILLS);
				if (player.var[VAA_UPGRADENUM] == 30)
					AchieveNow(55);
			}
			if (val>=VAA_TRIALSTAT && val < VAA_TRIALSTAT+5)
			{
				AchieveNow(66);
				if (player.var[val] == 3)
				{
					ReportQuestReport(QUEST_CHAMPION);
#ifdef DIRECTORS
					picToDrop=(val-VAA_TRIALSTAT+8);
					if (opt.galPix[picToDrop - 1] == 0) // && player.var[VAR_TEMP+15]==0)
					{
						opt.galPix[picToDrop - 1] = 1;
						SaveOptions();
						bullet_t* b;
						b = FireBullet(goodguy->ID, goodguy->x, goodguy->y, FIXAMT * 15, 64, BLT_ITEM, 0);
						if (b)
						{
							//player.var[VAR_TEMP+15]=1;
							b->anim = IT_PICTURE;
							b->target = picToDrop;
						}
						NewMessage("Hidden Trial cleared!", 0);
						MakeNormalSound(SND_TALENTUP);
						if (goodguy)
						{
							FXRing(1, goodguy->x, goodguy->y, goodguy->z, 10, 1);
							FXRing(1, goodguy->x, goodguy->y, goodguy->z, 10, 2);
						}
					}
					picToDrop = 0;
#endif
				}
			}
			if (val==VAA_PEAKLIONS)
				ReportQuestReport(QUEST_BALLOON);
			if (val==VAA_UPGRADESTAT+UPG_ENCHANT5 || val==VAA_UPGRADESTAT+UPG_SORCERY5)
				ReportQuestReport(QUEST_DEVILGRASS);
			if (val == VAA_MAGICJUNKED && player.var[VAR_SNUGGLY] == 0)
				ReportQuestReport(QUEST_CLOCKJUNK); // craft magic item
			if (val == VAA_CLOCKGRADE && player.var[VAR_SNUGGLY] == 1)
				ReportQuestReport(QUEST_EASYMINER);
			if (val == VAR_CAPTAIN2 && player.var[val] == 3)
				ReportQuestReport(QUEST_FURNACE);
			break;
			// new cases for AA
		case QE_LEVELXP:
			ReportQuestReport(QUEST_PROVE);
			break;
		case QE_LEVELTL:
			ReportQuestReport(QUEST_GURU);
			if (val >= TLT_CHOPPING && val <= TLT_GLADIATOR)
				ReportQuestReport(QUEST_PROVE2);
			if (val >= TLT_FOCUS && val <= TLT_WINDBAG)
				ReportQuestReport(QUEST_DELIVERY);
			if (val >= TLT_MOBILITY && val <= TLT_HERBALISM)
				ReportQuestReport(QUEST_SOUP);
			//if (val == TLT_JUNKSMITH && player.var[VAR_SNUGGLY] == 0)
			//	ReportQuestReport(QUEST_CLOCKJUNK); // no, actually we are crafting magic
			break;
	}
}

void PrintQuestInfo(byte quest,int x,int y)
{
	if(player.var[VAR_SNUGGLY] && (quest>=QUEST_ALLIES && quest<=QUEST_FURNACE))
	{
		Print(x+208,y+352,altQuestDesc[(quest-QUEST_ALLIES)*4+0],0,1);
		Print(x+208,y+352+12,altQuestDesc[(quest-QUEST_ALLIES)*4+1],0,1);
		Print(x+208,y+352+24,altQuestDesc[(quest-QUEST_ALLIES)*4+2],0,1);
		Print(x+208,y+352+36,altQuestDesc[(quest-QUEST_ALLIES)*4+3],0,1);
	}
	else
	{
		Print(x+208,y+352,questDesc[quest*4+0],0,1);
		Print(x+208,y+352+12,questDesc[quest*4+1],0,1);
		Print(x+208,y+352+24,questDesc[quest*4+2],0,1);
		Print(x+208,y+352+36,questDesc[quest*4+3],0,1);
	}

	Print(x+208,y+352+48,QuestReport(quest,1),0,1);
}

// --------------------------------
// CHATTING

void BeginChatting(byte tag)
{
	if(player.chatClock)
		return;

	if(player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX)
		return;

	if(tag==19)
		return;	// buried ropemaker doesn't talk

	player.chatClock=30;
	EnterChatMode();
	noButtons=30;	// for one second, you can't skip anything in chat, to avoid
					// blowing over Silver Bullet and other such messages
	curVillager=tag;
	if(tag<50 && (tag!=43 || player.var[VAR_BIRDCHAT]==0))
		MakeNormalSound(SND_VILLAGERHELLO+tag);

	// AA TEST
	//curChat = 247;
	//for (int i = 0; i < 380; ++i)
	//{
	//	talk[i].action = 0;
	//	talk[i].next = i+1;
	//}
	//curVillager = 5;
	//chatBright = 32;
	//return;
	// END AA TEST

	switch(tag)
	{
		case 0:
		case 1:	// nobody of interest
			curChat=0;
			break;
		case 2:	// Kinyova Sr.
			if(player.var[VAR_QUESTDONE+QUEST_PROVE])
			{
				if(player.var[VAR_QUESTDONE+QUEST_PROVE2])
				{
					curChat=10;	// all done
				}
				else if(player.var[VAR_QUESTASSIGN+QUEST_PROVE2])
				{
					if(MaxTalentLevel(TLTP_COMBAT) > 9)
						curChat=9;
					else
						curChat=8;
				}
				else
					curChat=7;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_PROVE])
			{
				if(player.level > 24) // changed for AA
					curChat=5;
				else
					curChat=4;
			}
			else
			{
				curChat=1;
			}
			break;
		case 3:	// Bobby Joe
			if(player.var[VAR_QUESTASSIGN+QUEST_EASYMINER] && !player.var[VAR_QUESTDONE+QUEST_EASYMINER] && player.var[VAR_SNUGGLY]==0 &&
				ItemExists(IT_CANDY)==0) // AA change
			{
				curChat=221;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_PRESENTS])
			{
				curChat=14;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_PRESENTS])
			{
				if(EquipCount(IT_PRESENT)>=99) // AA change
					curChat=13;	// got em all, quest done!
				else
					curChat=12;	// not done yet
			}
			else
			{
				curChat=11;	// assign the quest
			}
			break;
		case 4:	// Healiana
			if(player.var[VAR_QUESTDONE+QUEST_HERBS])
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_LETTER])
					curChat=19;
				else
					curChat=146;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_HERBS])
			{
				if(EquipCount(IT_HERBF)>=3)
					curChat=18;	// got em all, quest done!
				else
					curChat=17;	// not done yet
			}
			else
			{
				curChat=15;	// assign the quest
			}
			break;
		case 5:	// Woody Woodsman
			if(EquipCount(IT_ARTBIT)>=5)
			{
				curChat=376;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_SHARPEN])
			{
				curChat=29;
				EnterShop(SVC_SHARPEN);
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_SHARPEN])
			{
				if(EquipCount(IT_SHARPSTONE)>0)
					curChat=28;
				else
					curChat=27;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_WOOD])
			{
				curChat=26;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_WOOD])
			{
				if(player.var[VAA_UPGRADENUM] > 1) // AA change
					curChat=25;
				else
					curChat=24;
			}
			else
			{
				curChat=21;
			}
			break;
		case 6:	// Magicsmith
			if(player.var[VAR_QUESTDONE+QUEST_CRYSTAL])
			{
				curChat=38;
				EnterShop(SVC_MAGICSMITH);
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_CRYSTAL])
			{
				if(EquipCount(IT_BCRYSTAL))
					curChat=37;
				else
					curChat=36;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_MIMIC])
			{
				curChat=33;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_MIMIC])
			{
				if(player.var[VAR_TAMED]&1)
					curChat=349;
				else
					curChat=348;
			}
			else
			{
				curChat=30;
			}
			break;
		case 7:	// geoffrey
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN] && !player.var[VAR_QUESTDONE+QUEST_SHAKEDOWN] && player.levelNum == LVL_ARMSCAVE) // AA changed requirement
			{
				if (player.var[VAR_SHAKEDOWN] == 0)
					curChat = 217;
				else
					curChat = 207;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_AXES])
			{
				curChat=42;
				EnterShop(SHOP_AXES);
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_AXES])
			{
				byte p;
				int i;
				p=0;
				p+=player.axe.count-1;
				for(i=0;i<MAX_EQUIP;i++)
					if(player.items[i].type==IT_AXE || player.items[i].type==IT_MAGICAXE || player.items[i].type==IT_GOLDENAXE)
						p+=player.items[i].count;
				if(p>=3)
					curChat=41;
				else
					curChat=40;
			}
			else
			{
				curChat=39;
			}
			break;
		case 8:	// black market bob
			if(player.var[VAR_QUESTDONE+QUEST_BOBSWIFE])
			{
				curChat=49;
				EnterShop(SHOP_BLACKMARKET);
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_BOBSWIFE])
			{
				if(player.var[VAR_QUESTDONE+QUEST_BOBSWIFE2])
					curChat=47;
				else
					curChat=45;
			}
			else
			{
				curChat=43;
			}
			break;
		case 9:	// melody
			if(player.var[VAR_QUESTASSIGN+QUEST_BOBSWIFE])
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_BOBSWIFE2])
				{
					if(GotItem(IT_KEY1))
					{
						curChat=52;
					}
					else
						curChat=51;
				}
				else
					curChat=50;
			}
			else
			{
				curChat=77;
			}
			break;
		case 10:	// unchained melody
			curChat=78;
			break;
		case 11: // Mickey
			if(player.var[VAR_QUESTASSIGN+QUEST_TOYS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_TOYS])
					curChat=64;
				else if(player.var[VAR_TOYSDEAD]>=200)
					curChat=160;
				else
					curChat=159;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_RATS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_RATS])
					curChat=156;
				else if(player.var[VAR_RATS]==0)
					curChat=63;
				else
					curChat=62;
			}
			else
				curChat=60;
			break;
		case 12: // Talent Guru
			// AA completely reworked
			if(player.var[VAR_QUESTASSIGN+QUEST_GURU])
			{
				if (player.var[VAR_QUESTDONE+QUEST_GURU] == 0 && MaxTalentLevel(TLTP_ANY) > 9)
					curChat = 76;
				else if (player.talentLevel[player.var[VAR_GURUTALENT]] != 255)	// already have this talent
				{
					curChat = 374; // AA: shop
					EnterShop(SVC_GURU);
				}
				else
					curChat=71+(rand()%5);
			}
			else
				curChat=68;
			break;
		case 13: // Tina Tsarina
			if(player.var[VAR_QUESTASSIGN+QUEST_CHAMPION])
			{
				byte p, i;
				p = 0;
				for (i = 0; i < 5; ++i)
					if (player.var[VAA_TRIALSTAT + i] >= 3)
						++p;
				if(p > 4 && !player.var[VAR_QUESTDONE+QUEST_CHAMPION])
					curChat=145;
				else
				{
					curChat=79;
					EnterShop(SVC_ARENA);
				}
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_GLADIATOR])//gohere
			{
				if(player.var[VAR_QUESTDONE+QUEST_GLADIATOR] && TalentBonus(TLT_GLADIATOR)>=7)
				{
					curChat=144;
				}
				else if(TalentBonus(TLT_GLADIATOR)>=5 && !player.var[VAR_QUESTDONE+QUEST_GLADIATOR])
				{
					curChat=143;
				}
				else
				{
					curChat=79;
					EnterShop(SVC_ARENA);
				}
			}
			else
			{
				if(TalentBonus(TLT_GLADIATOR)>=1)
				{
					curChat=142;
				}
				else
				{
					curChat=79;
					EnterShop(SVC_ARENA);
				}
			}
			break;
		case 14:	// Taylor
			// AA no stealing
			if(player.var[VAR_QUESTASSIGN+QUEST_UNDIES])
			{
				if(player.var[VAR_QUESTDONE+QUEST_UNDIES] || EquipCount(IT_SOCK)<5 || EquipCount(IT_HERBA)<5)
				{
					// either finished the quest or don't have the items needed for it yet, so just do the shop
					curChat=84;
					EnterShop(SHOP_TAILOR);
				}
				else
				{
					// you have the items!
					curChat=85;
				}
			}
			else
				curChat=80;
			break;
		case 15:	// balloon
			player.var[VAR_VISITED + PLC_EISBURG] = 1; // AA just set all these
			player.var[VAR_VISITED + PLC_ARENA] = 1;
			player.var[VAR_VISITED + PLC_BASKER] = 1;
			player.var[VAR_VISITED + PLC_CLIFFS] = 1;
			player.var[VAR_VISITED + PLC_HIGHWAY] = (player.var[VAR_SNUGGLY]==0||player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY]==0||player.var[VAR_HIGHWAY]);
			player.var[VAR_VISITED + PLC_GORGE] = 1;
			if(player.var[VAR_QUESTASSIGN+QUEST_BALLOON])
			{
				if(player.var[VAR_QUESTDONE+QUEST_BALLOON])
				{
					curChat=99;
					EnterShop(SVC_BALLOON);
				}
				else if(player.var[VAA_PEAKLIONS] != 0)
					curChat=98;
				else
					curChat=97;
			}
			else
				curChat=95;
			break;
		case 16:	// handyman
			if(player.var[VAR_QUESTASSIGN+QUEST_BIGBEAR])
			{
				if(player.var[VAR_QUESTDONE+QUEST_BIGBEAR])
					curChat=141;
				else
					curChat=140;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_BEARS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_BEARS])
					curChat=138;
				else if(player.var[VAR_BEARSDEAD]>=30)
					curChat=137;
				else
					curChat=136;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_HANDY])
			{
				if(player.var[VAR_QUESTDONE+QUEST_HANDY])
					curChat=94;
				else if(EquipCount(IT_TOOLBOX))
					curChat=93;
				else
					curChat=92;
			}
			else
				curChat=88;
			break;
		case 17:	// historian
			if(EquipCount(IT_BOOK))
			{
				curChat=284;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_SKILLS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_SKILLS])
					curChat=134;
				else
				{
					if (player.var[VAA_UPGRADENUM] > 29) // AA changed to upgrades
						curChat=133;
					else
						curChat=132;
				}

			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_ALLIES])
			{
				if(player.var[VAR_QUESTDONE+QUEST_ALLIES])
					curChat=130;
				else if(player.var[VAR_QUESTDONE+QUEST_QUESTS])
					curChat=115;
				else
					curChat=114;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_TITANS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_TITANS])
					curChat=112;	// AA has builtin skate guard
				else if(EquipCount(IT_ICECUBE)>=3)
					curChat=111;
				else
					curChat=110;
			}
			else
				curChat=100;
			break;
		case 18:	// magic guy
			{
				curChat=215;	// AA has nothing else for this guy to do
				EnterShop(SHOP_MAGIC);
			}
			break;
		case 20:	// ropemaker
			// AA shakedown removed
			if(player.var[VAR_QUESTASSIGN+QUEST_FACTORY])
			{
				curChat=129;
				EnterShop(SHOP_ROPES);
			}
			else if(player.var[VAR_QUESTDONE+QUEST_ROPES])
				curChat=126;
			else
				curChat=125;
			break;
		case 21:	// ropewife
			if(player.var[VAR_QUESTASSIGN+QUEST_SOUP])
			{
				if(player.var[VAR_QUESTDONE+QUEST_SOUP])
					curChat=124;
				else if(MaxTalentLevel(TLTP_CRAFT) > 9) // AA change
					curChat=123;
				else
					curChat=122;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_ROPES])
			{
				if(player.var[VAR_QUESTDONE+QUEST_ROPES])
					curChat=119;
				else
					curChat=118;
			}
			else
				curChat=116;
			break;
		case 22:	// CEO
			if(player.levelNum==LVL_GORGE)
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_TOYS2])
				{
					if(player.var[VAR_QUESTDONE+QUEST_TOYS2])
					{
						curChat=347;
					}
					else if(EquipCount(IT_TOYCRYSTAL)>=20)
						curChat=345;
					else
						curChat=344;
				}
				else
					curChat=339;
			}
			else
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_FACTORY2])
				{
					if(player.var[VAR_QUESTDONE+QUEST_FACTORY2])
						curChat=155;
					else
						curChat=154;
				}
				else
					curChat=148;
			}
			break;
		case 23: // Curaga
			if(EquipCount(IT_LETTER))
			{
				curChat=164;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_DELIVERY])
			{
				if(player.var[VAR_QUESTDONE+QUEST_DELIVERY])
					curChat=170;
				else if(MaxTalentLevel(TLTP_MAGIC) > 9) // AA changed requirement
					curChat=169;
				else
					curChat=168;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_DEVILGRASS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_DEVILGRASS])
					curChat=166;
				else if(player.var[VAA_UPGRADESTAT+UPG_ENCHANT5] || player.var[VAA_UPGRADESTAT+UPG_SORCERY5]) // AA change
					curChat=163;
				else
					curChat=162;
			}
			else	// nothing assigned, letter delivered or not gotten yet
			{
				curChat=161;
			}
			break;
		case 24:	// onion
			if(!player.var[VAR_ONIONCHAT])	// have yet to speak to him at all
				curChat=171;
			else if(player.levelNum==LVL_TOYFACTORY)
				curChat=177;
			else if(player.var[VAR_SNUGGLY]==1)
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_LEADER])
				{
					if(player.var[VAR_TEMP+4]==0)
					{
						player.var[VAR_TEMP+4]=1;
						curChat=261;
					}
					else
						curChat=269;
				}
				else
					curChat=269;
			}
			else
			{

				if(player.var[VAR_QUESTASSIGN+QUEST_QUESTS])
				{
					if(player.var[VAR_QUESTDONE+QUEST_QUESTS])
					{
						if(player.var[VAR_QUESTDONE+QUEST_LEADER])
							curChat=184;
						else
							curChat=183;
					}
					else
					{
						byte p,i;
						p=0;
						for(i=QUEST_CLOCKJUNK;i<=QUEST_RUMBLE;i++)
							if(player.var[VAR_QUESTDONE+i])
								p++;

						if(p>=3)
							curChat=180;
						else
							curChat=179;
					}
				}
				else
					curChat=178;
			}
			break;
		case 25:	// junksmith
			if(player.var[VAR_QUESTASSIGN+QUEST_CLOCKJUNK])
			{
				if(player.var[VAR_QUESTDONE+QUEST_CLOCKJUNK])
					curChat=189;
				else if(player.var[VAA_MAGICJUNKED]) // AA craft magic item // lol nope
				//else if (CurrentTalentLevel(TLT_JUNKSMITH) > 3)
					curChat=188;
				else
					curChat=187;
			}
			else
				curChat=185;
			break;
		case 26:	// frankie beans
			if(player.var[VAR_QUESTASSIGN+QUEST_RUMBLE])
			{
				if(player.var[VAR_QUESTDONE+QUEST_RUMBLE])
					curChat=198;
				else if(player.var[VAR_RUMBLE])
					curChat=197;
				else
					curChat=196;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_EASYMINER])
			{
				if(player.var[VAR_QUESTDONE+QUEST_EASYMINER])
					curChat=194;
				else if(EquipCount(IT_CANDY))
					curChat=193;
				else
					curChat=192;
			}
			else
				curChat=190;
			break;
		case 27:	// crush
			if(player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY])
			{
				if(player.var[VAR_QUESTDONE+QUEST_HIGHWAY])
					curChat=202;
				else if(EquipCount(IT_STOLEN)>=10)
					curChat=201;
				else
					curChat=200;
			}
			else
				curChat=199;
			break;
		case 28:	// jimmy two-fingers
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN])
			{
				if(player.var[VAR_QUESTDONE+QUEST_SHAKEDOWN])
					curChat=209;
				// AA changed talk requirements
				else
					curChat=206;
			}
			else
				curChat=203;
			break;
		case 29:	// smash
			if(player.var[VAR_QUESTASSIGN+QUEST_SUPPLIES])
			{
				if(player.var[VAR_QUESTDONE+QUEST_SUPPLIES])
					curChat=214;
				else if(player.var[VAR_SUPPLIES]==20)
					curChat=213;
				else
					curChat=212;
			}
			else
				curChat=210;
			break;
		case 30:	// captain capitan
			if(player.levelNum==LVL_FURNACE)
			{
				if(player.var[VAR_CAPTAIN2]==1)
					curChat=333;
				else if(player.var[VAR_CAPTAIN2]==3)
					curChat=334;
			}
			else if(player.levelNum==LVL_CAPTAIN && player.var[VAR_SNUGGLY]==0 && player.var[VAR_QUESTASSIGN+QUEST_LEADER])
			{
				if(player.var[VAR_TEMP]==0)
					curChat=270;
				else
					curChat=274;
			}
			else if(player.levelNum==LVL_ONIONCAMP)
			{
				if(player.var[VAR_QUESTDONE+QUEST_LEADER])
				{
					if(player.var[VAR_QUESTASSIGN+QUEST_FURNACE])
						curChat=268;
					else
						curChat=264;
				}
				else if(player.var[VAR_PRISONBREAK]==1)
					curChat=226;
				else
					curChat=222;
			}
			else
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_QUESTS])
				{
					if(player.var[VAR_QUESTDONE+QUEST_QUESTS])
					{
						curChat=233;
					}
					else
					{
						byte p,i;
						p=0;
						for(i=QUEST_CLOCKJUNK;i<=QUEST_RUMBLE;i++)
							if(player.var[VAR_QUESTDONE+i])
								p++;

						if(p>=3)
							curChat=230;
						else
							curChat=228;
					}
				}
				else
					curChat=227;
			}
			break;
		case 31:	// major fishbug
			if(player.levelNum==LVL_FURNACE)
			{
				if (player.var[VAR_QUESTDONE + QUEST_FURNACE] == 0)
				{
					if (player.var[VAR_FURNACE])
						curChat=338;
					else
						curChat=337;
				}
				else
					curChat=247;
			}
			else if(player.levelNum==LVL_CAPTAIN && player.var[VAR_SNUGGLY]==0 && player.var[VAR_QUESTASSIGN+QUEST_LEADER])
			{
				if(player.var[VAR_QUESTDONE+QUEST_LEADER])
					curChat=275;
				else
					curChat=272;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_SUPPLIES])
			{
				if(player.var[VAR_QUESTDONE+QUEST_SUPPLIES])
				{
					if(player.var[VAR_QUESTASSIGN+QUEST_RUMBLE])
					{
						if(player.var[VAR_QUESTDONE+QUEST_RUMBLE])
							curChat=247;
						else if(player.var[VAR_RUMBLE])
							curChat=246;
						else
							curChat=245;
					}
					else
						curChat=243;
				}
				else
					curChat=242;
			}
			else
				curChat=240;
			break;
		case 32:	// clockmaker
			if(player.var[VAR_SNUGGLY]==0)	// you are an enemy!
				curChat=239;
			else if(player.var[VAR_QUESTASSIGN+QUEST_CLOCKJUNK])
			{
				if(player.var[VAR_QUESTDONE+QUEST_CLOCKJUNK])
				{
					if(player.var[VAR_QUESTDONE+QUEST_EASYMINER])
						curChat=238;
					else if(player.var[VAR_QUESTASSIGN+QUEST_EASYMINER])
					{
						if(player.var[VAA_CLOCKGRADE]) // AA changed requirement
							curChat=251;
						else
							curChat=250;
					}
					else
						curChat=248;
				}
				else if(!TaggedMonsterExists(2))
					curChat=237;
				else
					curChat=236;
			}
			else
				curChat=234;
			break;
		case 33:	// hornswaggle
			if(player.var[VAR_QUESTASSIGN+QUEST_HIGHWAY])
			{
				if(player.var[VAR_QUESTDONE+QUEST_HIGHWAY])
					curChat=255;
				else if(player.var[VAR_HIGHWAY])
					curChat=254;
				else
					curChat=253;
			}
			else
				curChat=252;
			break;
		case 34:	// public
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN])
			{
				if(player.var[VAR_QUESTDONE+QUEST_SHAKEDOWN])
					curChat=260;
				else if(player.var[VAR_SHAKEDOWN])
					curChat=259;
				else
					curChat=258;
			}
			else
				curChat=256;
			break;
		case 35:	// betty
			if(player.var[VAR_QUESTASSIGN+QUEST_BOOK])
			{
				if(player.var[VAR_QUESTDONE+QUEST_BOOK])
				{
					if(player.var[VAR_QUESTDONE+QUEST_CHAMPION]) // AA change
						curChat=283;
					else
						curChat=282;
				}
				else if(player.var[VAR_BOOKGIVEN])
					curChat=281;
				else
					curChat=280;
			}
			else
				curChat=277;
			break;
		case 36:	// schrodinger
			if (player.var[VAR_QUESTDONE+QUEST_FURNACE])
			{
				curChat = 328;
			}
			else if(!player.var[VAR_QUESTDONE+QUEST_FRAGILE])
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_FRAGILE])
					curChat=292;
				else
					curChat=286;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_FOREST])
			{
				if(player.levelNum==LVL_FOREST)
				{
					if(player.var[VAR_YOUTH])
						curChat=307;
					else
						curChat=306;
				}
				else
				{
					if(player.var[VAR_YOUTH])
						curChat=308;
					else
						curChat=305;
				}
			}
			else if(player.var[VAR_QUESTDONE+QUEST_PLANTS])
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_FOREST])
				{
					if(player.var[VAR_DEFILE]+player.var[VAR_DEFILE+1]==100)
						curChat=304;
					else
						curChat=303;
				}
				else
					curChat=295;
			}
			else	// fragile is done
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_PLANTS])
					curChat=294;
				else
					curChat=293;
			}
			break;
		case 37:	// sharona
			if(player.levelNum==LVL_FOREST)
			{
				if(player.var[VAR_YOUTH])
					curChat=330;
				else
					curChat=329;
			}
			else
			{
				if(player.var[VAR_QUESTASSIGN+QUEST_BOKBOK])
				{
					if(player.var[VAR_QUESTDONE+QUEST_BOKBOK])
					{
						if(player.var[VAR_QUESTASSIGN+QUEST_HIPPY])
						{
							if(player.var[VAR_QUESTDONE+QUEST_HIPPY])
							{
								curChat=319; // AA fix: binding crystal thing
							}
							else if(EquipCount(IT_SEED)>=20)
								curChat=318;
							else
								curChat=317;
						}
						else
							curChat=314;
					}
					else
					{
						// check for collected bokboks
						byte p,i;
						p=0;
						for(i=0;i<10;i++)
							if(player.var[VAR_BOKFOUND+i])
								p++;
						if(p==10)
						{
							curChat=312;	// got them all
						}
						else if(p==player.var[VAR_BOKGIVEN])
							curChat=310;	// didn't bring any new ones
						else
							curChat=311;	// brought new ones
					}
				}
				else
					curChat=309;
			}
			break;
		case 38:	// shrumford old
			if(player.levelNum==LVL_SHROOMCAMP)
				curChat=285;
			else
				curChat=321;
			break;
		case 39:	// shrumford young
			if(GotItem(IT_THING2)==0 && player.var[VAR_TEMP+1]==0 && !player.var[VAR_FURNACE])
				curChat=322;
			else if(player.var[VAR_QUESTDONE+QUEST_TITANS2])
			{
				//if(GotItem(IT_KEY4)==0 && player.var[VAR_TEMP+2]==0)
				//	curChat=327;
				//else
				//	curChat=328;
				curChat=327;	// AA redo
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_TITANS2])
			{
				//byte p,i;
				//p=0;
				//for(i=0;i<10;i++)
				//	if(player.var[VAR_TITANIC+i])
				//		p++;
				//if(p==10)
				//	curChat=327;
				//else
				curChat=326;	// AA redo
			}
			else if(player.var[VAR_QUESTDONE+QUEST_FURNACE])
				curChat=324;
			else
				curChat=323;
			break;
		case 40:	// shroud
			if(player.levelNum==LVL_FOREST)
			{
				if(player.var[VAR_YOUTH])
					curChat=332;
				else
					curChat=331;
			}
			else
			{
				curChat=320;
				EnterShop(SHOP_SHROOM);
			}
			break;
		case 41:	// baron
			if(player.var[VAR_TEMP+1]==0)
				curChat=352;	// intro
			else if(player.var[VAR_TEMP]==1)
				curChat=354;
			break;
		case 42:	// melton
			if(player.var[VAR_TEMP]==1)
				curChat=355;
			else if(player.var[VAR_TEMP]==4)
				curChat=356;
			else
				curChat=357;
			break;
		case 43:	// stick bird
			if(player.var[VAR_BIRDCHAT])
				curChat=369;
			else
				curChat=362;
			break;
		case 251:	// stash
			curChat=351;
			curVillager=1;
			EnterShop(SHOP_STASH);
			break;
		case 252:	// clockwork start up
			curChat=0;
			curVillager=1;
			EnterShop(SVC_CLOCKWORK);
			break;
		case 253:	// junksmith start up
			curChat=0;
			curVillager=1;
			EnterShop(SVC_JUNKSMITH);
			break;
		case 254:	// alchemy start up
			curChat=0;
			curVillager=1;
			EnterShop(SVC_ALCHEMY);
			break;
		case 255:	// the game-start chat
			if(player.var[VAR_MADCAP])
				curChat=370;
			else
				curChat=20;
			curVillager=5;
			break;
	}
	chatBright=32;
}

void DoChatAction(byte a)
{
	int i;
	byte amt;
	Guy *g;
	char s[64];

	switch(a)
	{
		case 1:
			// assign prove yourself quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_PROVE,1);
			PlayerSetVar(VAR_PROVEKILLS,0);
			break;
		case 2:
			// complete prove yourself quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_PROVE,1);
			PlayerSetVar(VAR_PROVEKILLS,0);
			// AA removed throwing
			break;
		case 3:
			// assign prove yourself 2
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_PROVE2,1);
			PlayerSetVar(VAR_PROVEKILLS,0);
			break;
		case 4:
			// complete prove yourself 2 quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_PROVE2,1);
			PlayerSetVar(VAR_PROVEKILLS,0);
			// AA removed bash
			break;
		case 5:
			// assign presents quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_PRESENTS,1);
			break;
		case 6:
			// complete presents quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_PRESENTS,1);
			DropItemOfType(IT_PRESENT,99);
			if (!GenerateArtifact()) // AA addition
				while(1)
				{
					byte etype;
					int eval1,eval2;

					EquipCreateItem(30+(50*player.var[VAR_MADCAP]),&etype,&eval1,&eval2);
					if(etype==IT_GOLDENAXE || etype==IT_GOLDENAMULET || etype==IT_GOLDENPARKA)
					{
						bullet_t *b;
						b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
						if(b)
						{
							b->anim=etype;
							b->target=eval1;
							b->owner=eval2;
						}
						break;
					}
				}
			break;
		case 7:
			// assign herb quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HERBS,1);
			break;
		case 8:
			// complete herb quest + heal
			PlayerSetVar(VAR_QUESTDONE+QUEST_HERBS,1);
			DropItemOfType(IT_HERBF,3);
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 9:
			// heal
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 10:
			// assign wood quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_WOOD,1);
			break;
		case 11:
			// complete wood quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_WOOD,1);
			// AA removed axemastery and logs
			break;
		case 12:
			// assign sharpen quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SHARPEN,1);
			break;
		case 13:
			// complete sharpen quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SHARPEN,1);
			DropItemOfType(IT_SHARPSTONE,1);
			if(player.axe.sharpened==0)
			{
				MakeNormalSound(SND_SHARPEN);
				player.axe.sharpened=1;
				player.axe.value++;
				PlayerCalcStats();
				NewMessage("Your axe was sharpened!",0);
			}
			break;
		case 14:
			// do axe sharpening shop
			break;
		case 15:
			// assign mimic quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_MIMIC,1);
			break;
		case 16:
			// finished mimic quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_MIMIC,1);
			// AA removed Training skill
			break;
		case 17:
			// assign crystal quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_CRYSTAL,1);
			break;
		case 18:
			// finished crystal quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_CRYSTAL,1);
			DropItemOfType(IT_BCRYSTAL,1);
			break;
		case 19:
			// do magicsmithing shop
			break;
		case 20:
			// assign rat quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_RATS,1);
			if(player.var[VAR_RATS]>0)
				PlayerSetVar(VAR_RATS,17);
			break;
		case 21:
			// complete rat quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_RATS,1);
			PlayerGetMoney(50);
			NewMessage("You made $50!",0);
			break;
		case 22:
			// assign axe quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_AXES,1);
			break;
		case 23:
			// complete axe quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_AXES,1);

			amt=3;
			for(i=0;i<MAX_EQUIP;i++)
			{
				if(player.items[i].type>=IT_AXE && player.items[i].type<=IT_GOLDENAXE)
				{
					if(player.items[i].count>amt)
					{
						player.items[i].count-=amt;
						amt=0;
						break;
					}
					else
					{
						amt-=player.items[i].count;
						player.items[i].count=0;
						player.items[i].type=IT_NONE;
						if(amt==0)
							break;
					}
				}
			}
			if(amt>0)	// still some left, must be in hand
			{
				player.axe.count-=amt;
			}
			SortEquipment();
			PlayerGetMoney(20);
			NewMessage("You were paid $20 for the axes!",0);
			break;
		case 24:
			// assign Talent quest and give talent and vanish in a puff of smoke
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_GURU,1);
			LearnTalent(player.var[VAR_GURUTALENT]);
			if (player.var[VAR_GURUTALENT] != TLT_GLADIATOR) // AA addition
				GuruVanish();
			break;
		case 25:
			// give talent and vanish in a puff of smoke
			LearnTalent(player.var[VAR_GURUTALENT]);
			if (player.var[VAR_GURUTALENT] != TLT_GLADIATOR) // AA addition
				GuruVanish();
			break;
		case 26:
			// give talent and vanish in a puff of smoke and complete quest
			// AA addition: skill points
			LearnTalent(player.var[VAR_GURUTALENT]);
			if (player.var[VAR_GURUTALENT] != TLT_GLADIATOR) // AA addition
				GuruVanish();
			PlayerSetVar(VAR_QUESTDONE+QUEST_GURU,1);
			PlayerGetSkillPoints(5);	// AA cap guard
			NewMessage("You got 5 Skill Points!",0);
			sprintf(s, "You have %d Skill Points.", (word)(player.skillPts)+(word)(player.var[VAA_SKILLBONUS]));
			NewMessage(s, 0);
			break;
		case 27:
			// assign bob's wife quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BOBSWIFE,1);
			break;
		case 28:
			// complete bob's wife2 quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_BOBSWIFE,1);
			while(1)
			{
				byte etype;
				int eval1,eval2;

				EquipCreateItem(10+(50*player.var[VAR_MADCAP]),&etype,&eval1,&eval2);
				if(etype==IT_MAGICPARKA)
				{
					bullet_t *b;
					b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=etype;
						b->target=eval1;
						b->owner=eval2;
					}
					break;
				}
			}
			break;
		case 29:
			// do black market shop
			break;
		case 30:
			// assign wife quest 2
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BOBSWIFE2,1);
			break;
		case 31:
			// unlock melody
			PlayerSetVar(VAR_WIFEUNLOCK,1);
			PlayerSetVar(VAR_QUESTDONE+QUEST_BOBSWIFE2,1);
			word w;
			w=GuyList_First();
			while(w!=65535)
			{
				if(GetGuy(w)->tag==9)
				{
					GetGuy(w)->tag=10;
					GetGuy(w)->mind1=0;
					MakeNormalSound(SND_DOOROPEN);
					break;
				}
				w=GuyList_Next();
			}
			break;
		case 32:
			// gladiator 'shop'
			break;
		case 33:
			// assign underwear quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_UNDIES,1);
			break;
		case 34:
			// finish underwear quest
			if(EquipAdd(IT_UNDIES,0,0))
			{
				PlayerSetVar(VAR_QUESTDONE+QUEST_UNDIES,1);
				DropItemOfType(IT_SOCK,5);
				DropItemOfType(IT_HERBA,5);
			}
			else
				NewMessage("Drop some stuff to make room!",0);
			break;
		case 35:
			// assign handyman quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HANDY,1);
			break;
		case 36:
			// finish handyman quest
			DropItemOfType(IT_TOOLBOX, EquipCount(IT_TOOLBOX));
			EquipAdd(IT_SHOVEL, 0, 0);
			PlayerSetVar(VAR_QUESTDONE+QUEST_HANDY,1);
			TipEvent(TIP_SHOVEL);
			break;
		case 37:
			// assign balloon quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BALLOON,1);
			break;
		case 38:
			// complete balloon quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_BALLOON,1);
			// AA doesn't remove ropes
			break;
		case 39:
			// assign titan quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_TITANS,1);
			break;
		case 40:
			// complete titan quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_TITANS,1);
			DropItemOfType(IT_ICECUBE,3);
			TryToGetItem(IT_ICESKATE,0,0); // AA fix
			break;
		case 41:
			// assign ally quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_ALLIES,1);
			if (!ItemExists(IT_ICESKATE))
				TryToGetItem(IT_ICESKATE, 0, 0);
			break;
		case 42:
			// assign ropemaker quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_ROPES,1);
			break;
		case 43:
			// assign soup quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SOUP,1);
			break;
		case 44:
			// finish soup quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SOUP,1);
			// AA doesn't drop soup
			// AA doesn't give strength
			break;
		case 45:
			// complete ropes quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_ROPES,1);
			break;
		case 46:
			// assign factory clearance
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FACTORY,1);
			break;
		case 47:
			// assign skill quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SKILLS,1);
			if (!ItemExists(IT_ICESKATE))
				TryToGetItem(IT_ICESKATE, 0, 0);
			break;
		case 48:
			// finish skill quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SKILLS,1);
			PlayerGetSkillPoints(5);	// AA cap  guard
			NewMessage("You got 5 Skill Points!",0);
			sprintf(s, "You have %d Skill Points.", (word)(player.skillPts)+(word)(player.var[VAA_SKILLBONUS]));
			NewMessage(s, 0);
			if (!ItemExists(IT_ICESKATE))
				TryToGetItem(IT_ICESKATE, 0, 0);
			break;
		case 49:
			// assign bear quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BEARS,1);
			PlayerSetVar(VAR_BEARSDEAD,0);
			break;
		case 50:
			// complete bear quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_BEARS,1);
			bullet_t *b;
			b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
			if(b)
			{
				b->speed=0;
				b->anim=IT_POT1+rand()%20;
				b->target=9;
				b->owner=0;
			}
			break;
		case 51:
			// assign big bear quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BIGBEAR,1);
			// AA doesn't summon bear
			break;
		case 52:
			// assign gladiator quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_GLADIATOR,1);
			break;
		case 53:
			// finish gladiator quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_GLADIATOR,1);
			// AA doesn't give whirl
			break;
		case 54:
			// assign champion quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_CHAMPION,1);
			break;
		case 55:
			// complete champion quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_CHAMPION,1);
			PlayerGetMoney(5000);
			NewMessage("You won $5,000!",0);
			break;
		case 56:
			// assign letter quest
			if(EquipAdd(IT_LETTER,0,0))
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_LETTER,1);
			else
				NewMessage("Drop some stuff to make room!",0);
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x, goodguy->y, 0, 64); // AA add heal to this
			break;
		case 57:
			// assign factory quest 2
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FACTORY2,1);
			break;
		case 58:
			// assign toy quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_TOYS,1);
			break;
		case 59:
			// finish toy quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_TOYS,1);
			PlayerGetMoney(1000);	 // AA gives more money
			NewMessage("You got $1000!", 0);
			TryToGetItem(IT_RUBY, 0, 0);
			TryToGetItem(IT_EMERALD, 0, 0);
			TryToGetItem(IT_DIAMOND, 0, 0); // AA slso gives gems
			break;
		case 60:
			// assign devil grass
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_DEVILGRASS,1);
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 61:
			// devil grass done
			PlayerSetVar(VAR_QUESTDONE+QUEST_DEVILGRASS,1);
			// AA doesn't remove herbs
			// AA doesn't give ignite
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 62:
			// letter quest done
			PlayerSetVar(VAR_QUESTDONE+QUEST_LETTER,1);
			DropItemOfType(IT_LETTER,1);
			// AA doesn't give endurance
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 63:
			// assign package quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_DELIVERY,1);
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 64:
			// package quest done
			PlayerSetVar(VAR_QUESTDONE+QUEST_DELIVERY,1);
			// AA doesn't remove package
			// AA doesn't give energy
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 65:
			// onion chat is done, make your choice
			EnterShop(SVC_ONION);
			break;
		case 66:
			// chose to join
			PlayerSetVar(VAR_ONIONCHAT,1);
			if(PlayerGetItem(IT_KEY2,0,0))
			{
				bullet_t *b;
				b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_KEY2;
					b->target=0;
					b->owner=0;
				}
			}
			break;
		case 67:
			// chose to die
			PlayerSetVar(VAR_ONIONCHAT,1);
			BeatLoonyUp();
			break;
		case 68:
			// assign quests quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_QUESTS,1);
			break;
		case 69:
			// finish quests quest, assign slay quest
			if(player.var[VAR_QUESTDONE+QUEST_QUESTS]==0)
				PlayerSetVar(VAR_QUESTDONE+QUEST_QUESTS,1);
			if(player.var[VAR_QUESTASSIGN+QUEST_LEADER]==0)
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_LEADER,1);
			if(!ItemExists(IT_KEY3)) // AA fix
			{
				if(PlayerGetItem(IT_KEY3,0,0))
				{
					bullet_t *b;
					b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_KEY3;
						b->target=0;
						b->owner=0;
					}
				}
			}
			break;
		case 70:
			// assign junk quest
			// player.var[VAA_MAGICJUNKED] = 0; // AA preset junkvar (no nvm)
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_CLOCKJUNK,1);
			break;
		case 71:
			// finish junk quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_CLOCKJUNK,1);

			// take away the 50 junk items, one of each type until done (so as not to wipe out one thing)
			//amt=50;
			//while(amt>0)
			//{
			//	for(i=0;i<MAX_EQUIP;i++)
			//	{
			//		if(player.items[i].type==IT_LOG || player.items[i].type==IT_GEAR || player.items[i].type==IT_CAN ||
			//			player.items[i].type==IT_STUFFING || player.items[i].type==IT_SOCK || player.items[i].type==IT_ICECUBE ||
			//			player.items[i].type==IT_SEED || player.items[i].type==IT_STONE || player.items[i].type==IT_ECTO || player.items[i].type==IT_CLAW)
			//		{
			//			if(player.items[i].count>1)
			//			{
			//				player.items[i].count--;
			//				amt--;
			//			}
			//			else
			//			{
			//				amt--;
			//				player.items[i].count=0;
			//				player.items[i].type=IT_NONE;
			//			}
			//		}
			//		if(amt==0)
			//			break;
			//	}
			//}
			//SortEquipment(); // AA doesn't remove junk
			curMap->map[14+20*curMap->width].item=IT_NONE;	// clear the block so you can get in
			break;
		case 72:
			// assign candy quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_EASYMINER,1);
			break;
		case 73:
			// complete candy quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_EASYMINER,1);
			DropItemOfType(IT_CANDY,1);
			break;
		case 74:
			// assign rumble quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_RUMBLE,1);
			break;
		case 75:
			// complete rumble quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_RUMBLE,1);
			break;
		case 76:
			// assign highway quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HIGHWAY,1);
			break;
		case 77:
			// complete highway quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_HIGHWAY,1);
			amt=EquipCount(IT_STOLEN);
			PlayerGetMoney(amt*10);
			sprintf(s,"Crush paid you $%d.",amt*10);
			NewMessage(s,0);
			DropItemOfType(IT_STOLEN,amt);
			break;
		case 78:
			// assign shakedown quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SHAKEDOWN,1);
			break;
		case 79:
			// complete shakedown quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SHAKEDOWN,1);
			// AA doesn't remove money
			break;
		case 80:
			// assign supplies quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SUPPLIES,1);
			break;
		case 81:
			// complete supplies quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SUPPLIES,1);
			// AA supplies doesn't give spike
			break;
		case 82:
			// give the package to you
			if(PlayerGetItem(IT_PACKAGE,0,0))
			{
				bullet_t *b;
				b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_PACKAGE;
					b->target=0;
					b->owner=0;
				}
			}
			player.var[VAR_TEMP]=1;
			break;
		case 83: // shakedown geoffrey
			PlayerSetVar(VAR_SHAKEDOWN+0,1);
			PlayerGetMoney(1000);
			NewMessage("Geoffrey paid you $1000.",0);
			break;
		case 84: // shakedown Taylor
			PlayerSetVar(VAR_SHAKEDOWN+1,1);
			PlayerGetMoney(1000);
			NewMessage("You took $1000.",0);
			break;
		case 85: // shakedown magic guy
			PlayerSetVar(VAR_SHAKEDOWN+2,1);
			PlayerGetMoney(1000);
			NewMessage("Guy paid you $1000.",0);
			break;
		case 86:	// shakedown rope guy
			PlayerSetVar(VAR_SHAKEDOWN+3,1);
			PlayerGetMoney(1000);
			NewMessage("Rupert paid you $1000.",0);
			break;
		case 87:	// candy from a baby
			{
				bullet_t *b;
				b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_CANDY;
					b->target=0;
					b->owner=0;
				}
			}
			break;
		case 88:	// rescued by capitan
			PlayerSetVar(VAR_PRISONBREAK,1);
			break;
		case 89:	// snuggly: complete rank quest and assign kill onion quest and give key
			if(player.var[VAR_QUESTDONE+QUEST_QUESTS]==0)
				PlayerSetVar(VAR_QUESTDONE+QUEST_QUESTS,1);
			if(player.var[VAR_QUESTASSIGN+QUEST_LEADER]==0)
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_LEADER,1);
			if(!ItemExists(IT_KEY2)) // AA fix
			{
				if(PlayerGetItem(IT_KEY2,0,0))
				{
					bullet_t *b;
					b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_KEY2;
						b->target=0;
						b->owner=0;
					}
				}
			}
			// AA doesn't give tornado
			break;
		case 90:	// finish clock quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_CLOCKJUNK,1);
			break;
		case 91:	// complete miner quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_EASYMINER,1);
			// AA doesn't remove diamond
			// AA doesn't give spike
			break;
		case 92:	// complete highway patrol
			PlayerSetVar(VAR_QUESTDONE+QUEST_HIGHWAY,1);
			break;
		case 93:	// complete shutdown
			PlayerSetVar(VAR_QUESTDONE+QUEST_SHAKEDOWN,1);
			break;
		case 94:	// capitan drops in, squishing onion and completing quest
			g=AddGuy((42*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(4*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,300*FIXAMT,MONS_BUNNYBOSS);
			if(g)
			{
				g->tag=30;
				g->team=GOOD;
			}
			player.chatClock=30*5;
			MakeNormalSound(SND_TORNADO);
			MakeNormalSound(SND_VILLAGERHELLO+30);	// huzzah!!
			break;
		case 95:	// assign furnace quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FURNACE,1);
			break;
		case 96:
			PlayerSetVar(VAR_QUESTDONE+QUEST_ALLIES,1);
			if (!ItemExists(IT_ICESKATE))
				TryToGetItem(IT_ICESKATE, 0, 0);
			break;
		case 97:	// Capitan has talked, let Fishbug know to get into it
			player.var[VAR_TEMP]=1;
			break;
		case 98:	// Capitan vanishes in puff of smoke and quest is done
			PlayerSetVar(VAR_QUESTDONE+QUEST_LEADER,1);
			break;
		case 99:	// Fishbug assigns furnace quest and does his own puff of smoke
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FURNACE,1);
			break;
		case 100:	// assign book quest
			if(EquipAdd(IT_BOOK,0,0))
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_BOOK,1);
			else
				NewMessage("Drop some stuff to make room!",0);
			break;
		case 101:	// complete book quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_BOOK,1);
			break;
		case 102:	// give book
			DropItemOfType(IT_BOOK,1);
			PlayerSetVar(VAR_BOOKGIVEN,1);
			break;
		case 103: // assign fragile
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FRAGILE,1);
			break;
		case 104: // assign plants
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_PLANTS,1);
			break;
		case 105: // assign defile
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FOREST,1);
			break;
		case 106: // complete defile
			PlayerSetVar(VAR_QUESTDONE+QUEST_FOREST,1);
			break;
		case 107:	// shroom ritual
			curMap->map[72+51*curMap->width].item=IT_NONE;
			i=GuyList_First();
			while(i!=65535)
			{
				if(GetGuy(i)->type==MONS_VILLAGER && GetGuy(i)->tag==38)
				{
					MakeNormalSound(SND_SHROOMIFY);
					curMap->BrightTorch(72,51,90,5);
					FXRing(0,GetGuy(i)->x,GetGuy(i)->y,0,32,1);
					GetGuy(i)->tag=39;
					GetGuy(i)->x=(72*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT;
					GetGuy(i)->y=(51*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT;
					GetGuy(i)->bright=32;
					FXRing(0,GetGuy(i)->x,GetGuy(i)->y,0,32,1);
					break;
				}
				i=GuyList_Next();
			}
			PlayerSetVar(VAR_YOUTH,1);
			break;
		case 108:	// deliver some bokboks
			amt=0;
			for(i=0;i<10;i++)
				if(player.var[VAR_BOKFOUND+i])
					amt++;
			player.var[VAR_BOKGIVEN]=amt;
			i=GuyList_First();
			while(i!=65535)
			{
				if(GetGuy(i)->type==MONS_BOKBOK)
					GetGuy(i)->mind2=2;
				i=GuyList_Next();
			}
			break;
		case 109:	// bokbok quest done
			PlayerSetVar(VAR_QUESTDONE+QUEST_BOKBOK,1);
			player.var[VAR_BOKGIVEN]=10;
			i=GuyList_First();
			while(i!=65535)
			{
				if(GetGuy(i)->type==MONS_BOKBOK)
					GetGuy(i)->mind2=2;
				i=GuyList_Next();
			}
			TryToGetItem(IT_BCRYSTAL, 0, 0);
			break;
		case 110:
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HIPPY,1);
			if (player.var[VAR_QUESTDONE + QUEST_CRYSTAL]== 0 && !ItemExists(IT_BCRYSTAL))
				TryToGetItem(IT_BCRYSTAL, 0, 0);
			break;
		case 111:
			PlayerSetVar(VAR_QUESTDONE+QUEST_HIPPY,1);
			// AA doesn't give shroom
			DropItemOfType(IT_SEED,20);
			if (player.var[VAR_QUESTDONE + QUEST_CRYSTAL]== 0 && !ItemExists(IT_BCRYSTAL))
				TryToGetItem(IT_BCRYSTAL, 0, 0);
			break;
		case 112: // assign bokbok quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BOKBOK,1);
			break;
		case 113: // give thing of mabob
			if (!ItemExists(IT_THING2))
				if(PlayerGetItem(IT_THING2,0,0))
				{
					bullet_t *b;
					b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_THING2;
						b->target=0;
						b->owner=0;
						player.var[VAR_TEMP+1]=1;
					}
				}
			break;
		case 114:	// assign titan quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_TITANS2,1);
			if (PlayerGetItem(IT_KEY4, 0, 0))
			{
				bullet_t* b;
				b = FireBullet(goodguy->ID, goodguy->x, goodguy->y, FIXAMT * 15, rand() % 256, BLT_ITEM, 0);
				if (b)
				{
					b->anim = IT_KEY4;
					b->target = 0;
					b->owner = 0;
					player.var[VAR_TEMP + 2] = 1;
				}
			}
			break;
		case 115:	// complete titan quest
//			PlayerSetVar(VAR_QUESTDONE+QUEST_TITANS2,1);	// AA this doesn't finish quest
			if (!ItemExists(IT_KEY4))
				if(PlayerGetItem(IT_KEY4,0,0))
				{
					bullet_t *b;
					b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
					if(b)
					{
						b->anim=IT_KEY4;
						b->target=0;
						b->owner=0;
						player.var[VAR_TEMP+2]=1;
					}
				}
			break;
		case 116:	// add frostburn quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BARON,1);
			break;
		case 117: // switch to Major Fishbug
			curVillager=31;
			break;
		case 118:	// finish furnace quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_FURNACE,1);
			break;
		case 119:	// assign toy quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_TOYS2,1);
			break;
		case 120:	// complete toy quest
			DropItemOfType(IT_TOYCRYSTAL,20);
			PlayerSetVar(VAR_QUESTDONE+QUEST_TOYS2,1);
			for(i=0;i<5;i++)
				SetPuppetLife(i, MonsterHP(MONS_PROTOTYPE, GOOD));
				//player.var[VAR_PUPPETLIFE+i]=50;
			AddPuppetPals();
			break;
		case 121: // repair puppets
			RepairPuppets();
			break;
		case 122:
			PlayerSetVar(VAR_SUPERTITAN,1);
			PlayerSetVar(VAR_QUESTDONE+QUEST_BARON,1);
			MakeNormalSound(SND_TITANDIE);
			curMap->map[32+57*curMap->width].item=IT_KEY5;
			break;
		case 123:
			PlayerSetVar(VAR_BARONDEAD,1);
			break;
		case 124:
			SendMessageToGame(MSG_WINGAME,5);
			break;
		case 125:
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HAPPYSTICK,1);
			break;
		case 126:
			if (GenerateArtifact())
			{
				DropItemOfType(IT_ARTBIT,5);
#ifdef DIRECTORS
				picToDrop = 3;
				if (opt.galPix[picToDrop - 1] == 0) // && player.var[VAR_TEMP+15]==0)
				{
					opt.galPix[picToDrop - 1] = 1;
					SaveOptions();
					bullet_t* b;
					b = FireBullet(goodguy->ID, goodguy->x, goodguy->y, FIXAMT * 15, 64, BLT_ITEM, 0);
					if (b)
					{
						//player.var[VAR_TEMP+15]=1;
						b->anim = IT_PICTURE;
						b->target = picToDrop;
					}
					NewMessage("Artifact crafted!", 0);
					MakeNormalSound(SND_TALENTUP);
					if (goodguy)
					{
						FXRing(1, goodguy->x, goodguy->y, goodguy->z, 10, 1);
						FXRing(1, goodguy->x, goodguy->y, goodguy->z, 10, 2);
					}
				}
				picToDrop = 0;
#endif
			}
			break;
			// AA cases
		case 127:
			{
				amt = EquipCount(IT_PRESENT);
				if (amt)
				{
					PlayerGetMoney((int)amt * 1000);
					sprintf(s, "Bobby Joe paid you $%d.", (int)amt * 1000);
					NewMessage(s, 0);
					DropItemOfType(IT_PRESENT, amt);
				}

				byte i;
				byte p = 0;
				for (i = 0; i < 99; ++i)
					if (player.var[VAR_PRESENT + i] > 0)
						++p;
				for (i = 0; i < 21; ++i)
					if (player.var[VAA_AAPRESENT + i] > 0)
						++p;
				sprintf(s, "You have found %d/120 Presents.", p);
				NewMessage(s, 0);
			}
			break;
		case 128:
			if (player.var[VAR_QUESTDONE + QUEST_CRYSTAL] == 0 && !ItemExists(IT_BCRYSTAL))
				TryToGetItem(IT_BCRYSTAL, 0, 0);
			break;
		case 129:
			if (player.var[VAR_QUESTDONE + QUEST_TITANS] == 1 && !ItemExists(IT_ICESKATE))
				TryToGetItem(IT_ICESKATE, 0, 0);
			break;
		case 130:
			// guru shop
			break;
	}
}

void NewChat(word ch)
{
	chatBright=32;
	curChat=ch;
}

void UpdateChat(byte inShop)
{
	byte c;

	if(chatBright>-(int)strlen(talk[curChat].line)-4)
	{
		chatBright-=2;
	}

	if(inShop)
		return;

	if(noButtons)
	{
		noButtons--;
		GetTaps();
		c=0;
	}
	else
		c=GetTaps();

	if(c&CONTROL_B1)
	{
		if(chatBright>-(int)strlen(talk[curChat].line)-4)
			chatBright=-(int)strlen(talk[curChat].line)-4;
		else
		{
			if(talk[curChat].next)
			{
				if(talk[curChat].action)
					DoChatAction(talk[curChat].action);
				curChat=talk[curChat].next;
				chatBright=32;
			}
			else
			{
				ExitChatMode();
				if(talk[curChat].action)
					DoChatAction(talk[curChat].action);
			}
		}
	}
}

void RenderChat(MGLDraw *mgl)
{
	if(faceSpr==NULL)
	{
		faceSpr=new sprite_set_t("graphics/vgrfaces.jsp");
	}
	if(curVillager>=faceSpr->GetCount())
	{
		RenderChatNoName(mgl);
		return;
	}
	PauseBox(0,380,639,479,250);
	faceSpr->GetSprite(curVillager)->Draw(chatOfs[curVillager][0],chatOfs[curVillager][1]+480,mgl);
	PrintDark(90-1,355-1,chatName[curVillager],0);
	PrintDark(90+1,355+1,chatName[curVillager],0);
	Print(90,355,chatName[curVillager],0,0);
	PrintRectBlack(90,380,635,477,talk[curChat].line,22,chatBright,0);

}

void RenderChatNoName(MGLDraw *mgl)
{
	PauseBox(0,380,639,479,250);
	PrintRectBlack(4,380,635,477,talk[curChat].line,22,chatBright,0);
}
//--------------------------------------------
// END CHATTING

