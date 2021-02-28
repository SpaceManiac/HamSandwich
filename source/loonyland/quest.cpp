#include "quest.h"
#include "game.h"
#include "control.h"
#include "display.h"
#include "badge.h"
#include "spacegame.h"
#include "options.h"
//#include "randomizer.h"
#include "ioext.h"
#include "appdata.h"
#include <vector>
#include <sstream> 

rItem randoReward[11];

char questName[NUM_QUESTS][64]={
	"Save Halloween Hill",
	"Tree Trimming",
	"Scaredy Cat",
	"Sticky Shoes",
	"Mushroom Hunt",
	"Picking Daisies",
	"Curse The Darkness",
	"Zombie Stomp",
	"Smashing Pumpkins",
	"Mystery Lake",
	"Beneath The Lake",
	"Happy Stick?!?",
	"A True Hero",
	"Silver Bullet",
	"Hairy Larry",
	"Ghostbusting",
	"The Haunted Tower",
	"The Last Gate",
	"The Rescue",
	"The Collection",
};

Convo talk[]={
	// 0
	{"It's a lovely day, eh?",
	 "",
	 "",
	 "",
	 0,0},
	 // 1
	{"Oh, please help me!  I want to visit my sister in",
	 "Zombiton, but the path leads through Terror Glade!",
	 "I can't handle that.  If you'll destroy the Terror",
	 "Trees for me, I will reward you!",
	 1,0},
	 // 2
	{"Oh thank heavens!!  Now the path is safe once again!",
	 "Here, take this Super Heart as a reward.",
	 "",
	 "",
	 2,0},
	 // 3
	{"Terror Glade is just south of here. I hope you can",
	 "defeat the Terror Trees!",
	 "",
	 "",
	 0,0},
	 // 4
	{"Thank you for helping me. But I changed my mind.",
	 "I think I'll just stay at home today.",
	 "",
	 "",
	 0,0},
	 // 5
	{"I fear our fair land, with its rolling hills",
	 "(of mud) and its lush forests (of dead trees),",
	 "is in grave danger! There is an evil force",
	 "which lurks in Vampy Land to the north -",
	 0,6},
	 // 6
	{"I don't know what it is, but it seems to be",
	 "the cause of all these roaming monsters, and",
	 "the outrageous misbehavior of the local frogs",
	 "and bats.  If only there were someone who",
	 0,7},
	 // 7
	{"could save us... not you of course!!  You're",
	 "just a small boy.  But you know, someone",
	 "mighty and all that.  Hmmm... you don't know",
	 "anyone like that?  Oh well.",
	 3,0},
	 // 8
	{"Hi, I'm Biff, and I'm a traveling salesman.",
	 "Would you like to buy some rubber booties?",
	 "Perfect for walking in thick muck!  Only",
	 "30 gems!  I'm having a sale.",
	 4,0},
	 // 9
	{"Ooh, a customer!  Here you go sir, the finest",
	 "muck-proof boots in the business!  Pleasure",
	 "doing business with you, my friend.",
	 "",
	 5,0},
	 // 10
	{"The boots working for you?  They're my best",
	 "model!  Step in a mudpile for me.",
	 "",
	 "",
	 0,0},
	 // 11
	{"Oh, hi kid.  I'm just sitting here worrying",
	 "about the future of Halloween Hill.  Will we",
	 "survive this horrible nightmare?  Probably.",
	 "But still, it's pretty unpleasant.",
	 0,0},
	 // 12
	{"Wow... you did save Halloween Hill.  Color",
	 "me moderately impressed.",
	 "",
	 "",
	 0,0},
	 // 13
	{"Whoa, I can't get anywhere through this",
	 "muck! I need to find some better footwear!",
	 "",
	 "",
	 6,0},
	 // 14
	{"Hi!  I'm Bonita Barrel.  I see you're",
	 "checking out my collection.  Pretty nice,",
	 "huh?  The ones that are standing up can",
	 "be pushed around.  Try to get the cactus!",
	 0,15},
	 // 15
	{"Oh, and if you mess it up so that you can't",
	 "reach the cactus, don't worry about it.  Come",
	 "back later and I'll have it all straightened",
	 "up again for you.  I love guests!!",
	 0,0},
	 // 16
	{"Hail, young lad!  I am the mighty Kinyova,",
	 "weaponsmaster extraordinaire.  Why don't I",
	 "explain the use and care of Special Weapons to",
	 "you?",
	 0,17},
	 // 17
	{"When you find a special weapon, like the Potted",
	 "Cactus that Bonita Barrel has, it becomes",
	 "equipped.  Then pressing Button 2 (or Shift or X",
	 "on the keyboard) will fire the weapon.  But it",
	 0,18},
	 // 18
	{"costs Gems to fire your weapons.  The number of",
	 "Gems per shot is displayed on the screen by the",
	 "picture of which weapon you have.  It also shows",
	 "a roman numeral I.  This means your weapon is",
	 0,19},
	 // 19
	{"at Level I.  If you get another of the same",
	 "weapon, it will go up to Level II, and another",
	 "will bring it to Level III.  Each level is far",
	 "more powerful than the last, BUT it costs many",
	 0,20},
	 // 20
	{"more Gems to fire at higher levels.  So if you",
	 "don't have a lot of Gems, you might want to keep",
	 "using a Level I weapon and avoid picking up any",
	 "upgrades.  If you pick up a different weapon,",
	 0,21},
	 // 21
	{"you will be back to Level I firepower.  It's easy",
	 "to collect upgrades for your weapons from",
	 "defeated monsters, so it's really a matter of",
	 "deciding what level of power you want to use!",
	 0,186},
	 // 22
	{"Hey! If I can light a spark that's not surrounded",
	 "by rocks, then I can use that to light this torch!",
	 "",
	 "",
	 7,0},
	 // 23
	{"Hmm... If I can light a spark that's not surrounded",
	 "by rocks, I could use the flame to light a torch...",
	 "if I HAD a torch.",
	 "",
	 7,0},
	 // 24
	{"Boo hoo hoo!  My poor little kitten has run off into",
	 "Wicked Woods and gotten lost! Do you think you could",
	 "help me find her?  I'm afraid she's gotten stuck in a",
	 "tree!",
	 8,0},
	 // 25
	{"Have you found my kitten yet??",
	 "",
	 "",
	 "",
	 0,0},
	 // 26
	{"Oh thank heavens!!  You've brought my little snuggum",
	 "wuggums back to me! I'm afraid I don't have anything",
	 "much I can offer as a reward... how about this stick?",
	 "It used to be a torch, but it burned out.",
	 9,0},
	 // 27
	{"My widdle snuggum wuggums is just de most precious",
	 "widdle kitty cat in de world, huh snookums?  Yes",
	 "you are! Hey, you like the stick? Once, I lit it",
	 "in some cave in Slurpy Swamp.",
	 0,0},
	 // 28
	{"Hello, young traveler.  I am Zizwalda, and I'm a ",
	 "witch! Oh, don't be frightened... people twist that",
	 "word around a lot, but really it just means someone",
	 "who could squash you like a bug with a thought.",
	 0,29},
	 // 29
	{"With that in mind, how about you lend me a hand?",
	 "I need 10 Giant Mushrooms from Slurpy Swamp for my",
	 "latest potion. Get them for me, and I'll reward you.",
	 "Fail, and you won't be a happy camper.",
	 10,0},
	 // 30
	{"Hey kid, where are my mushrooms???",
	 "",
	 "",
	 "",
	 0,0},
	 // 31
	{"Great!  Here, I'll give you this Super Heart, an",
	 "amazing new health product I've invented.  But there",
	 "is something else I need to make my potion now.  If",
	 "you go fetch me the fabled Doom Daisy from the peak",
	 0,32},
	 // 32
	{"of the Rocky Cliffs, I'm sure I can find another",
	 "reward for you.",
	 "",
	 "",
	 11,0},
	 // 33
	{"Alright, that's IT!!  Consider yourself ZAPPED.",
	 "",
	 "",
	 "",
	 12,0},
	 // 34
	{"You sir, are a nuisance.",
	 "",
	 "",
	 "",
	 0,0},
	 // 35
	{"Hello there!  Back to see my incredible barrel",
	 "collection again?  Have fun!",
	 "",
	 "",
	 0,0},
	 // 36
	{"Get off my bridge!!  I am guarding this bridge",
	 "with my irrepressible might.  Pay up 75 gems,",
	 "or you'll never see the wonders of the Rocky",
	 "Cliffs!",
	 13,0},
	 // 37
	{"Thank you kindly sir, and do watch your step as",
	 "you cross.  Oh, and look out for the evil mummies",
	 "that live up in those cliffs.  They're almost as",
	 "mighty as me!",
	 14,0},
	 // 38
	{"It is a lovely bridge, isn't it?  My grandfather",
	 "built it!",
	 "",
	 "",
	 0,0},
	 // 39
	{"Oh, woe is me... I used to be a farmer, and a dang",
	 "good one too, but I guess I was experimenting too",
	 "much with super grow fertilizer, and my pumpkins",
	 "went nuts!  They're trying to kill me!  I ran off",
	 0,40},
	 // 40
	{"and here I am, drowning my sorrows.  I wish I could",
	 "get my farm back from them!",
	 "",
	 "",
	 15,0},
	 // 41
	{"*sob*... dang pumpkins done run me off'n my farm!",
	 "",
	 "",
	 "",
	 0,0},
	 // 42
	{"Wow, just when I got up the courage to come back",
 	 "and see what's been happening, I find you smashing",
	 "the last of the pumpkins!  I can't thank you",
	 "enough!  Here, why don't you try the super growth",
	 0,43},
	 // 43
	{"fertilizer I was using?  Maybe you'll have better",
	 "luck with it than I did!",
	 "",
	 "",
	 16,0},
	 // 44
	{"Ah, the pleasures of toiling in the fields...",
	 "Hey, I wonder what I could grow if I... Nah, I",
	 "think I'll just stick to the basics from now on."
	 "",
	 0,0},
	 // 45
	{"Hey! Thanks to you, I'm finally back in business!",
	 "Drinks are on the house all around!  Hey wait a",
	 "second... that's not good business at all!  Drinks",
	 "are double price!",
	 0,0},
	 // 46
	{"Thanks for cleaning the zombies out of our town!",
	 "",
	 "",
	 "",
	 0,0},
	 // 47
	{"Hey kid, great work in those crypts... very classy",
	 "monster slaying you did.",
	 "",
	 "",
	 0,0},
	 // 48
	{"Wow, it feels so much less zombie-like around here",
	 "these days!  Have you noticed?",
	 "",
	 "",
	 0,0},
	 // 49
	{"Without all the zombies around, I can get back to",
	 "having a monopoly on stumbling around drunkenly,",
	 "shouting for brains!  BRRRAAAIIINNNNSSSS!!!",
	 "",
	 0,0},
	 // 50
	{"Cheers!",
	 "",
	 "",
	 "",
	 0,0},
	 // 51
	{"A toast to you, good sir!",
	 "",
	 "",
	 "",
	 0,0},
	 // 52
	{"I've composed a haiku in your honor!",
	 "        He slew the zombies,",
	 "   For this we are quite grateful,",
	 "       Let's all have a beer.",
	 0,0},
	 // 53
	{"Yikes! It's pitch black in here!  I won't be able",
	 "to explore here until I find a torch!",
	 "",
	 "",
	 7,0},
	 // 54
	{"Yikes! It's pitch black in here!  I won't be able",
	 "to explore here until I find a way to light my",
	 "torch!",
	 "",
	 7,0},
	 // 55
	{"Help us!!  Our town has been overrun, totally",
	 "unexpectedly, by zombies!!  They are coming out of",
	 "the three crypts in our graveyards.  Clean them out",
	 "and you will be handsomely rewarded!",
	 17,0},
	 // 56
	{"Hooray!  You've defeated the zombies and saved our",
	 "fair town!  Here's your 100 Gem reward.  Don't spend",
	 "it all in one place!",
	 "",
	 18,0},
	 // 57
	{"Thanks for rescuing us from those dreadful zombies.",
	 "Now I can get back to the important job of maying.",
	 "That's what us mayors do, you know.",
	 "",
	 0,0},
	 // 58
	{"Oh, do please save us from these zombies!!  It won't",
	 "bode well for the next mayoral elections if my",
	 "constituency has been devoured by flesh eating",
	 "monsters!",
	 0,0},
	 // 59
	{"Ah, it's a lovely day for farming and experimenting",
	 "with new and dangerous super growth plant foods!",
	 "",
	 "",
	 0,0},
	 // 60
	{"Oh, great!  Now just a pinch of this and a dash of",
	 "that... voila!  A Ghost Slaying Potion!  Come to",
	 "think of it, I don't have any use for this.  If you",
	 "want it, I'll give it to you for 75 gems.",
	 19,0},
	 // 61
	{"Magic ain't cheap, chump.  Cough up the 75 gems, or",
	 "no Ghost Slaying Potion for you.",
	 "",
	 "",
	 0,0},
	 // 62
	{"Congratulations!  You're now the proud owner of a",
	 "Ghost Slaying Potion.  With this, your weapons should",
	 "no longer just pass through ghosts.",
	 "",
	 20,0},
	 // 63
	{"Well, where's my Doom Daisy?  I haven't got all day!",
	 "",
	 "",
	 "",
	 0,0},
	 // 64
	{"Get out of my house, you punk!!",
	 "",
	 "",
	 "",
	 0,0},
	 // 65
	{"Warning: this Electro-Gate is on the same circuit",
	 "as the Bonehead-O-Matics.  Do not shut them down",
	 "or the gate will overload and short out.",
	 "(Please ignore this message if you are a goodguy)",
	 0,0},
	 // 66
	{"Puny human!  I am Frankenjulie.  I was built",
	 "centuries ago to guard the Skull Key from little",
	 "twerps like you.  You will be crushed.  I am not",
	 "kidding.  Don't roll your eyes at me!  I'm mighty!",
	 25,0},
	 // 67
	{"Jeepers!!  My shots go right through these ghosts!",
	 "I'll need to find some way to hurt them or I won't",
	 "get very far in here.",
	 "",
	 32,0},
	 // 68
	{"Who be this doofy little twerp that hath ascended",
	 "my tower of horrors?  No matter, for ye shan't wrest",
	 "yon Bat Key from mine grasp!  I am Polterguy, and",
	 "it's time you met my flying junk of doom!!",
	 25,0},
	 // 69
	{"Oh hello!  I wonder if you can help me... there's a",
	 "big fat reward in it for you!  If you go to the",
	 "abandoned mine I used to work in and bring me back",
	 "the last ingot of silver in there, I'll use some of",
	 0,70},
	 // 70
	{"it to coat your slingshot in silver.  See, I would",
	 "get it myself, but mummies invaded the mines and we",
	 "all had to run off in a hurry.  Besides the mummies,",
	 "there's one other problem... I forgot exactly which",
	 0,71},
	 // 71
	{"mine shaft is the one with the silver in it, and the",
	 "only key left is the old cracked one in the manager's",
	 "office, which will probably break as soon as you use",
	 "it.  So you'll need to guess the right shaft.",
	 0, 72},
	 // 72
	{"Let me tell you what I can remember... there are five",
	 "mine shafts. We dug for different things in each one.",
	 "The emerald mine is not on either end.  Let's see...",
	 "okay, I remember that the silver and gold mines are",
	 0,73},
	 // 73
	{"both to the left of the diamond mine.  Neither the",
	 "silver or gold mines are the center mine though. Um,",
	 "the coal mine is to the left of the gold mine, I",
	 "remember that... and the only other thing I can think",
	 0,74},
	 // 74
	{"of is that the silver mine is to the right of the",
	 "emerald mine.  That's it... if you can't figure out",
	 "which shaft to use from that, then we're both out of",
	 "luck!  I hope you can do it!",
	 21,0},
	 // 75
	{"Alright! You got the silver! I'll just heat some up,",
	 "and KERBANG! Your slingshot is silver coated now.",
	 "Thanks a lot for recovering this, I really missed",
	 "being filthy rich!",
	 22,0},
	 // 76
	{"Ah, what to buy, what to buy...",
	 "",
	 "",
	 "",
	 0,0},
	 // 77
	{"Help! Help! I'm stuck in this cave and there's all",
	 "manner of horrible monsters lurking about!  Could",
	 "you please guide me to the nearest exit? My mommy",
	 "must be worried sick about me.",
	 23,0},
	 // 78
	{"I'm so scared!!!",
	 "",
	 "",
	 "",
	 0,0},
	 // 79
	{"Hooray! *SMOOCH* My hero!  Here, now that I'm out",
	 "of this nasty cave, I don't need this lantern",
	 "anymore, you can have it.  Ta ta!",
	 "",
	 24,0},
	 // 80
	{"I'm just gonna rest here a while before heading",
	 "back home.  My heart can't take much more!",
	 "",
	 "",
	 0,0},
	 // 81
	{"Hey... I can pick this lock!  I wasn't always a",
	 "good little girl, you know.",
	 "",
	 "",
	 0,0},
	 // 82
	{"Phew! Thanks for returning me to normal!  It was",
	 "rough being a wolfman... all that roaming around",
	 "free under the moonlight, doing anything you want,",
	 "the incredible raw power... hey!  Change me back!!",
	 0,0},
	 // 83
	{"Oh woe is me... my husband Larry has disappeared!",
	 "Lately I've woken up to find him gone almost every",
	 "night, and he never shaves anymore... he's getting",
	 "positively furry!  And today he hasn't come back at",
	 0,84},
	 // 84
	{"all!  I'm so worried.  If you see him, tell him to",
	 "come home, won't you?",
	 "",
	 "",
	 26,0},
	 // 85
	{"Oh thank you so for returning my Larry to me!!",
	 "Here, take this key.  It looks pretty valuable",
	 "but it doesn't fit anything in our house, so I",
	 "don't need it.",
	 27,0},
	 // 86
	{"It's so nice having sweet Larry back home!",
	 "",
	 "",
	 "",
	 0,0},
	 // 87
	{"*grumble* chores *grumble* nagging *grumble*",
	 "wish I was still a wolf...",
	 "",
	 "",
	 0,0},
	 // 88
	{"And I don't even want to *tell* you what he",
	 "did on the rug!",
	 "",
	 "",
	 0,0},
	 // 89
	{"Everybody knows you need a silver weapon to",
	 "hurt werewolves!",
	 "",
	 "",
	 21,0},
	 // 90
	{"Hi, my name is Mary Louise Anna Marybell. I",
	 "collect dollies!  I like to cuddle them and",
	 "snuggle them, and sell the rare ones in the",
	 "Ebayton market for exorbitant prices.",
	 0,91},
	 // 91
	{"If I can just complete these 8 doll sets, I",
	 "can retire!  I just need one more of each!",
	 "If you see them, bring them to me and I'll",
	 "give you 25 gems for each one!  Plus, I have",
	 0,92},
	 // 92
	{"a special toy here you might like.  I'll give",
	 "it to you if you get all 8 dolls for me!",
	 "",
	 "",
	 28,0},
	 // 93
	{"Have you found any more dolls for me?  I bet",
	 "the monsters took them all!",
	 "",
	 "",
	 0,0},
	 // 94
	{"Ooh goody!!  Another doll for the collection!",
	 "Keep up the good work!",
	 "",
	 "",
	 0,0},
	 // 95
	{"Wow!  That's almost all of them!  This is",
	 "great!",
	 "",
	 "",
	 0,0},
	 // 96
	{"Hooray!!  You've completed all my sets of",
	 "dolls!  Here, you can have this Reflect",
	 "Gem.  It'll probably work on that slingshot",
	 "you have.  Thank you so much for your help!!",
	 29,0},
	 // 97
	{"Now to sit back and rake in the cash as I",
	 "auction these things off...",
	 "",
	 "",
	 0,0},
	 // 98
	{"Hey, there's something that could light the",
	 "torch!  Now if only I can get gas flowing to",
	 "it...",
	 "",
	 0,0},
	 // 99
	{"Greetings warrior!  I am Farley The Helpful",
	 "Ghost Bat!  I am very into the whole heroism",
	 "thing, so I am hoping to one day be the side-",
	 "kick to a great hero.  Maybe you are just the",
	 0,100},
	 // 100
	{"guy I'm looking for!  I don't know... you have",
	 "to prove you're a true hero.  Come back here",
	 "with 5,000 Monster Points so I know you really",
	 "are a mighty warrior.",
	 31,0},
	 // 101
	{"Wow, you did it!  You're just the kind of guy",
	 "I want to sidekick for.  But there's a few ",
	 "rules first.  I only want to help out a guy",
	 "who is really smashing up the badguys, so if",
	 0,102},
	 // 102
	{"you're at less than full health, I'm outta there.",
	 "I'm only gonna be your sidekick while you're in",
	 "perfect health - it's an image thing, you",
	 "understand. The other thing you have to know is",
	 0,103},
	 // 103
	{"that it's a rough job being a ghost. I'm a",
	 "mighty warrior, but once I materialize, I need",
	 "energy to work up to my full potential. So we",
	 "have to beat badguys to power me up. And if",
	 0,104},
	 // 104
	{"you blow it and I have to disappear, we're",
	 "back to square one.  Sorry, pal, but that's",
	 "how ghosts work. I don't just make this",
	 "stuff up! Okay, let's go smash something!!",
	 30,0},
	 // 105
	{"Hey, don't come crawling back to me until you",
	 "can prove you're a true warrior with 5,000",
	 "Monster Points!  I have a certain image to",
	 "think of here.",
	 0,0},
	 // 106
	{"Greetings puny human...  I am Gusty, the Elder",
	 "Vampire.  I have lived for eons, and picked up a",
	 "trick or two along the way.  For instance, I am",
	 "completely immune to slingshot fire!  A bit odd,",
	 0,107},
	 // 107
	{"perhaps, but I find it comes in handy sometimes.",
	 "Anyway, enough with the pleasantries, now let us",
	 "be on with the dismembering and such!",
	 "",
	 25,0},
	 // 108
	{"Hello pitiful weakling.  I am Stoney, the Elder",
	 "Vampire.  Through the centuries I have honed my",
	 "skills and pushed my body to the limit.  I have",
	 "finally developed the unique feature of being",
	 0,109},
	 // 109
	{"utterly impervious to any harm.  Well, except for",
	 "falling from extreme heights of course!  But I",
	 "digress.  Time to kill you now.",
	 "",
	 25,0},
	 // 110
	{"Yipes, sure looks like a long way DOOOOWWWWNNNN...",
	 "",
	 "",
	 "",
	 0,0},
	 // 111
	{"Good evening, wretched mortal.  I am Drippy, the",
	 "Elder Vampire.  I have spent countless millenia",
	 "honing my power over the element of water.  With this",
	 "mastery, I have turned my entire body to crystallized",
	 0,112},
	 // 112
	{"ice, totally unbreakable. So anyway, I was thinking",
	 "I should shatter your bones.  Is now-ish good with",
	 "you?",
	 "",
	 25,0},
	 // 113
	{"Howdy, insignificant pardner. I'm Toasty, the Elder",
	 "Vampire.  I reckon that over all them centuries, I've",
	 "really become a true master of the element of fire.",
	 "Anything you try to throw at me is just gonna flat",
	 0,114},
	 // 114
	{"out melt. Why heck, I wouldn't even try to tussle with",
	 "me if I were ya'll. But if that's the way you want it,",
	 "then I spose it's time for a good old-fashioned",
	 "bar-B-Q!  YEE-HAH!!!!",
	 25,0},
	 //115
	{"Oh hi again... looks like I got a little lost on the",
	 "way home.  Could you possibly lead me out again?",
	 "",
	 "",
	 23,0},
	 // 116
	{"Hey, thanks!  It's good to be free again.  Now let's",
	 "see if I can find the way home THIS time...",
	 "",
	 "",
	 24,0},
	 // 117
	{"Heidy-ho there! My name is Bonkula, and I'm the king",
	 "of the vampires! I may not be the biggest vampire,",
	 "but I do have the biggest hammer. Anyhoo, I know",
	 "you're here to foil my fiendish plans, but you're too",
	 0,118},
	 // 118
	{"late.The Evilizer is already activated and it's going",
	 "to turn your entire world into a hideous nightmare",
	 "lickety-split! And I've got even worse news for you...",
	 "as the king of all vampires, I happen to be totally",
	 0,119},
	 // 119
	{"indestructible, except that whole sunlight issue, but",
	 "I mean, c'mon, no sunlight's gonna get into my fortress",
	 "of evil. That's just silly! Now, stand still so I",
	 "can bonk you!",
	 25,0},
	 // 120
	{"Little pint-sized hypochondriac... it's not even light",
	 "outside! The window was just painted! Wait a minute -",
	 "it's noon! The Evilizer must be causing this darkness!",
	 "Perhaps there's still a chance to stop it...",
	 33,0},
	 // 121
	{"Hooray, our town is zombie-free!",
	 "",
	 "",
	 "",
	 0,0},
	 // 122
	{"I wonder if we'll have to rename the town now...",
	 "",
	 "",
	 "",
	 0,0},
	 // 123
	{"This is truly a rather pleasant day.",
	 "",
	 "",
	 "",
	 0,0},
	 // 124
	{"Zombies bad.  Loony good!",
	 "",
	 "",
	 "",
	 0,0},
	 // 125
	{"Yay, now Zombiton can return to its prosperous",
	 "tourist trade!  I better go polish the headstones.",
	 "",
	 "",
	 0,0},
	 // 126
	{"Zombies... zombies... why is it always zombies?",
	 "",
	 "",
	 "",
	 0,0},
	 // 127
	{"Yo dude, thanks for saving us.",
	 "",
	 "",
	 "",
	 0,0},
	 // 128
	{"What!? I like it this way! You want it clean,",
	 "YOU clean it up!!",
	 "",
	 "",
	 0,0},
	 // 129
	{"Greetings, young Loony.  I am Kwala Zalawash, the",
	 "town seer here in Zombiton.  I can see things no",
	 "other person can.  Let me gaze into your future...",
	 "",
	 0,0},
	 // 130
	{"Wait, speaking of seeing, have you seen my crystal",
	 "ball? I always keep it by my bed and now it's gone!",
	 "Oh well, it didn't work anyway.  Now, let me gaze",
	 "into your future...",
	 0,0},
	 // 131
	{"Back for another reading?  Okay, let me gaze into",
	 "your future...",
	 "",
	 "",
	 0,0},
	 // 132 -seer-hill
	{"I see you entering a huge castle... facing vicious",
	 "fangs... leathery wings... giant hammers... some...",
	 "hmm... a machine? The vision is lost.",
	 "",
	 0,0},
	 // 133 -seer-hill
	{"In a dark castle in a lonely land, your goal awaits.",
	 "It has such power... you will need all the strength",
	 "you can muster!  Statues... so many statues... then",
	 "a portal beyond which I cannot see.",
	 0,0},
	 // 134 -seer-trees
	{"You haven't finished the Tree Trimming quest yet!??",
	 "What are you waiting for??",
	 "",
	 "",
	 0,0},
	 // 135 -seer-trees
	{"I see trees leering with angry faces... spitting up",
	 "frogs... I see you should have destroyed them long",
	 "ago... what's keeping you?",
	 "",
	 0,0},
	 // 136 -seer-shroom
	{"You are wandering through a mucky swamp... you are",
	 "looking for mushy objects... you are looking in the",
	 "Underground Tunnel... battling swampy monsters...",
	 "searching the Bog Pit as well.",
	 0,0},
	 // 137 -seer-shroom
	{"I see you looking for mushrooms in the swamp. They",
	 "are hidden... you find several in the Underground",
	 "Tunnel, even guarded by large monsters. The Bog Pit",
	 "too holds some of these mushrooms.",
	 0,0},
	 // 138 -seer-daisy
	{"I see you ascending cliffs... climbing ever higher",
	 "until you stand looking out over an infinite expanse.",
	 "It is here that you find what you seek.",
	 "",
	 0,0},
	 // 139 -seer-daisy
	{"The daisy you seek grows in only one place... atop the",
	 "highest peak in the Rocky Cliffs. Stop dawdling and",
	 "go get it!",
	 "",
	 0,0},
	 // 140 -seer-crops
	{"I see you battling orange globes.  You feel silly",
	 "doing it, but they are nearly overwhelming you. I see",
	 "gravestones south of you...",
	 "",
	 0,0},
	 // 141 -seer-crops
	{"Pumpkins! PUMPKINS!!!!  AUGH!!!!!!!!!!!!!!!! I can",
	 "tell you no more.",
	 "",
	 "",
	 0,0},
	 // 142 -seer-orbs
	{"I see... hey! I see you stealing my crystal ball! Jerk.",
	 "Then you grab three others from the four corners of",
	 "Halloween Hill and place them around a shimmering lake.",
	 "You're still a jerk though.",
	 0,0},
	 // 143 -seer-orbs
	{"I see glowing orbs of energy... they surround a placid",
	 "body of water... then the water is disturbed... for the",
	 "first time in centuries, something arises... there is",
	 "darkness within.",
	 0,0},
	 // 144 -seer-frank
	{"I see you descending into a lake. Within, you conquer",
	 "the mechanized creations of a deranged mind.  In the",
	 "depths... far beneath the surface... you encounter...",
	 "holy frijoles... that thing is big!",
	 0,0},
	 // 145 -seer-frank
	{"Beneath Mystery Lake lies a hideous creation of man",
	 "and machine... something enormous and deadly.  With",
	 "guided missiles no less.  When you best this thing,",
	 "you find the key that signifies death.",
	 0,0},
	 // 146 -seer-witch
	{"In the woods... in the south... there is an evil area.",
	 "A place with potent magicks that disturb me.  Do not go",
	 "near this place!  It is the lair of something too",
	 "terrible to comprehend!",
	 0,0},
	 // 147 -seer-witch
	{"The south of the Wicked Woods... my vision gets fuzzy,",
	 "as powerful dark magic clouds it... stay away from",
	 "there!  There is an evil there so terrible none dare",
	 "speak its name!",
	 0,0},
	 // 148 -seer-farley
	{"I see you smacking around badguys a whole heck of a",
	 "lot, at the behest of an insecure insubstantial lump",
	 "of ectoplasm... in the end it's worth it.  So smack",
	 "those badguys around.",
	 0,0},
	 // 149 -seer-farley
	{"Brutal pummelings are the key to appeasing the ghost",
	 "bat... find many enemies in a small area and defeat",
	 "them all quickly... then you will earn his trust.",
	 "",
	 0,0},
	 // 150 -seer-silver
	{"Ooh ooh, pick door number 4!  Door number 4!",
	 "",
	 "",
	 "",
	 0,0},
	 // 151 -seer-silver
	{"I see you heading into a cave in the cliffs. In this",
	 "cave, the mummies rule.  Even once you defeat them,",
	 "you will have a choice to make.  Be sure you know",
	 "what the correct choice is... or you'll be sorry!",
	 0,0},
	 // 152 -seer-wolf
	{"In the Pointless Plains, there is a small cave. It",
	 "is used by creatures both animal and man. It is in",
	 "this cave that you will find your hairy adversary.",
	 "",
	 0,0},
	 // 153 -seer-wolf
	{"Men can be such animals. Especially in the cave in",
	 "the Pointless Plains. At least until you lay the",
	 "smack down and send them home.",
	 "",
	 0,0},
	 // 154 -seer-ghostpotion
	{"I see you paying a witch a large sum of money.",
	 "Just do it. You need the potion.",
	 "",
	 "",
	 0,0},
	 // 155 -seer-ghostpotion
	{"Pay up the witch or you will never defeat your",
	 "most transparent adversaries.",
	 "",
	 "",
	 0,0},
	 // 156 -seer-tower
	{"The second floor of the tower... not as hard as it",
	 "seems.  Crank all four sideways to see where the",
	 "open rooms are, then use careful vertical cranking",
	 "to put one open room in each row.",
	 0,0},
	 // 157 -seer-tower
	{"I see you being very stymied on the second floor",
	 "of a tower full of ghosts. It's not as hard as it",
	 "seems. The left cranks move the rooms sideways, and",
	 "the bottom ones move them vertically.",
	 0,0},
	 // 158 -seer-busts
	{"I see you ascending four towers, and battling",
	 "invincible foes.  Somehow you succeed, then as you",
	 "ascend the central chamber, you discover a bust on",
	 "each floor.  At the top, you enter a strange portal.",
	 0,0},
	 // 159 -seer-busts
	{"You must find one bat statue and one vampire bust on",
	 "each floor of the castle... then use the bat statues",
	 "on the first floor to ascend the towers and claim the",
	 "remaining for vampire busts with any means necessary.",
	 0,0},
	 // 160 -seer-noquest
	{"I see you having a blast playing a very cool game and",
	 "posting lots of good stuff on the forum at",
	 "http://www.hamumu.com.",
	 "",
	 0,0},
	 // 161 -seer-noquest
	{"I see you earning merit badge after merit badge, and",
	 "even earning whole new game modes, making this game",
	 "totally entertaining for you for years and years! If",
	 "not decades.",
	 0,0},
	 // 162
	{"Who's that trip-trapping in my secret magical lair?",
	 "Well, say your prayers sonny, because I'm gonna get",
	 "you and your little bat too!",
	 "",
	 0,0},
	 // ---------- After you've won (and done all they offer you)
	 // 163 -kinyova
	{"See?  That stuff I told you about special weapons",
	 "really saved the day, didn't it?  Yessiree, I sure",
	 "am some kind of hero!  Yay, Kinyova!  Hooray!!!",
	 "",
	 0,0},
	 // 164 -zizwalda
	{"Even I have to admit... my ghost potion really did",
	 "the trick!  I mean it pretty much solved this crisis",
	 "all by itself!  Wow, I'm really an amazing witch!",
	 "",
	 0,0},
	 // 165 -bonita
	{"With the world safe once more, I feel like I can",
	 "finally get out of this house and find some more",
	 "barrels to put in my collection!",
	 "",
	 0,0},
	 // 166 -cat lady
	{"Hooray!  Now it's safe out there... except for the",
	 "occasional Bonehead attack.  I can go fetch my cat",
	 "myself the next time she runs off.",
	 "",
	 0,0},
	 // 167 -shoe salesman
	{"With this newfound resurgence in tourism, I bet lots",
	 "of people are going to need to come to me for muck",
	 "walking boots!  After all, they don't come here for",
	 "the food - it's Slurpy Swamp they wanna see!",
	 0,0},
	 // 168 -terror glade guy
	{"Great!  Now the wilderness is safe once again, and",
     "I don't have to be afraid to go visit Zombiton any",
	 "time at all!  But my feet hurt, I think I'll stick",
	 "around here for a while.",
	 0,0},
	 // 169 -zombiton mayor
	{"Oh the joy!  The peace!  The harmony!  Without the",
	 "looming threat of the Evilizer, Zombiton is sure to",
	 "grow to become a major world power.",
	 "",
	 0,0},
	 // 170 -Larry
	{"Oh 'yay'.  I'm so 'happy' that I won't get bit and",
	 "turned into a wolf again.  'Yippee'....",
	 "",
	 "Yes dear, I'll go sweep the bedroom...*grumble*",
	 0,0},
	 // 171 -Larry's wife
	{"Without all that evil darkness around, I can feel",
	 "safe here at home in the arms of my darling Larry!",
	 "*sigh*",
	 "",
	 0,0},
	 // 172 -messy guy
	{"Wow, now that I don't have to fear horrible monster",
	 "attacks, maybe I'll find the time to clean up this",
	 "place!",
	 "",
	 0,173},
	 // 173 -messy guy cont'd
	{"HA!!!!!!!  JUST KIDDING!!!!!",
	 "",
	 "",
	 "",
	 0,0},
	 // 174 -generic Zombitonite
	{"Thank you for saving us all from Count Bonkula's",
	 "diabolical scheme!",
	 "",
	 "",
	 0,0},
	 // 175 -generic #2
	{"We owe you a great debt, Loony... I've got some",
	 "spare cabbage, will that settle it up?"
	 "",
	 "",
	 0,0},
	 // 176 -generic #3
	{"Hooray!  First the zombies, now the vampires! I",
	 "swear, you're turning into a regular... um...",
	 "monster-get-rid-of-er.  Uh, yeah.",
	 "",
	 0,0},
	 // 177 -generic #4
	{"You da man!",
	 "",
	 "",
	 "",
	 0,0},
	 // 178 -generic #5
	{"So, hey, what's a 'machine' anyway?",
	 "",
	 "",
	 "",
	 0,0},
	 // 179 -generic #6
	{"Hey, I wrote you another haiku!",
	 "        Evilizer boom.",
	 "    The people rejoice today.",
	 "     Life is good, ain't it?",
	 0,0},
	 // 180 - generic #7
	{"You're a good kid, Loony.",
	 "",
	 "",
	 "",
	 0,0},
	 // 181 - doll collector
	{"Wow, now that the actual monsters have been",
	 "'retired', these dolls are going to be worth",
	 "a fortune!!  I can retire!",
	 "",
	 0,0},
	 // 182 -mine worker
	{"Hey, with all this silver I got, maybe I can",
	 "afford to get some of those cool Monster Dolls",
	 "I always see in Ebayton!",
	 "",
	 0,0},
	 // 183 -bartender
	{"Hey Loony!  Thanks to you, my bar is filled",
	 "living people!  It's great!  If you were old",
	 "enough, I'd give you a drink on the house.",
	 "Maybe even two!",
	 0,0},
	 // 184 -farmer
	{"Well, looks like the world is safe again!  At",
	 "least until some insane farmer mixes the wrong",
	 "chemicals together and creates a super-race of",
	 "killer vegetables!",
	 0,0},
	 // 185 - bridge guard
	{"Cross my bridge all you like, Loony!  You're",
	 "a big fat hero!",
	 "",
	 "",
	 0,0},
	 // ---- THAT'S IT FOR VICTORY COMMENTS
	 // 186 - kinyova finishes his blabber
	{"And when you do decide, turn \"Wpn Lock\" on in",
	 "the Pause menu.  When it is on, you can't pick",
	 "up any other weapons.  You'll see how useful that",
	 "is later on, trust me!",
	 0,0},
	 // 187
	{"Leave me alone!  I don't want to go back to that",
	 "woman!  It's always 'clean this' and 'sweep that'!",
	 "I can't take it anymore!  It's enough to turn a",
	 "man into a monster!",
	 25,0},
	 // 188
	{"Okay okay!!!  Quit it!  I'll stop being a were-",
	 "wolf!  I'll go back home... and sweep... and",
	 "clean... and dust... *sniff*",
	 "",
	 0,0},
	 // 189
	{"I'm going, I'm going... just give me some time",
	 "to think up a good excuse first.  Boy is she",
	 "gonna be steamed at me... She'll probably give",
	 "YOU some kind of reward though!  No fair.",
	 0,0},
	 // 190 - the seer if you have all badges
	{"Well geez... you've got all the badges.  Well,",
	 "since you've saved THIS world about as much as",
	 "possible, I will zap you to another world where",
	 "your help is still needed!  Get ready...",
	 34,0},
	 // 191 - if you got the doom daisy before the shrooms, this is the witch giving you the heart before
	{"Hey, you got me the mushrooms AND the Doom Daisy!",
	 "You're a peach!  I mean, you know, for a lousy",
	 "worm.  Here, have this Super Heart.",
	 "",
	 11,60},
	 // 192 - the guy who wants to sell you the hot pants
	{"Psst, hey buddy!  Wanna buy some Hot Pants?",
	 "They're the latest rage in Losangelton!  I'll",
	 "cut you a special deal - just 150 gems!",
	 "",
	 0,0},
	 // 193 - the pants seller if you lack the funds
	{"Hey pal, I said 150 gems!  These are top quality",
	 "Hot Pants!",
	 "",
	 "",
	 0,0},
	 // 194 - Hot pants sale complete
	{"Great!  One pair of hot pants, coming up!",
	 "",
	 "",
	 "",
	 35,0},
	 // 195 - hot pants have been sold
	{"Thank you, come again.",
	 "",
	 "",
	 "",
	 0,0},
	 // 196 - Arms dealer
	 {"Hi there, I'm the friendly neighborhood arms",
	  "dealer!  Unfortunately, I have no arms to sell.",
	  "Well, I mean... I have ARMS... I just don't have",
	  "any weapons, which is typically what an arms",
	  0,197},
	 // 197 arms dealer 2
	 {"dealer sells.  If only I had every weapon in the",
	  "world...",
	  "",
	  "",
	  0,0},
	 // 198 - deliver all weapons to arms dealer
	 {"Wow!  For me??  I can't thank you enough!",
	  "Now I just need to trump up an illegal war",
	  "somewhere, so I have someone to sell these to.",
	  "Hey, take this Vampire Bust - I won't have",
	 0,199},
	  // 199 #2
	 {"time for my sculpting hobby with all the",
	  "business I'm gonna have now!",
	  "",
	  "",
	  36,0},
	  // 200 - arms dealer has all arms
	 {"Don't bother me now, I'm trying to come up",
      "with ways to get Halloween Hill and Easter",
	  "Island mad at each other.",
	  "Hmm, or maybe Valentine Valley...",
	  0,0},
	  // 201 - mini-frankenjulie
	 {"Large human!  I am Mini-Frankenjulie.  I was built",
	  "years ago to be a part of this art piece, you",
	  "twerp!  You will be crushed by my teeny might!",
	  "I am not kidding.  Don't roll your eyes at me!",
	  25,0},
	  // 202 - mecha-bonkula
	 {"Behold!  I am Mecha-Bonkula, the ultimate in",
	  "robo-vampire technology!  I must confess that",
	  "it is highly unlikely that you could defeat me.",
	  "But give it a shot, it's what I'm here for!",
	  0,0},
	  // 203 - mecha beaten
	 {"FATAL EXCEPTION 0x01A3!  Windows is crashing!",
	  "Open, that is.  Hop out an window and meets the",
	  "Evilizer!  Failing be grammar circuits mine!",
	  "All my ... base.... are.... bEeeloOoOoOooo...",
	  33,0},
};

byte seerTable[]={132,134,0,0,136,138,0,0,140,142,144,146,148,150,152,154,156,158,0,0};

byte curChat;
byte curLine,curChar;
byte noButtons;

char *QuestName(byte quest)
{
	return questName[quest];
}

// --------------------------------
// CHATTING

byte PickSeerChat(void)
{
	byte q;
	int tries;

	q=0;
	for(tries=0;tries<NUM_BADGES;tries++)
		if(opt.meritBadge[tries])
			q++;

	if(q==40)
		return 190;	// if you have all badges, a special event!

	tries=0;
	while(tries++<1000)
	{
		q=(byte)Random(20);
		if((player.var[VAR_QUESTASSIGN+q]) && (!player.var[VAR_QUESTDONE+q]) && (seerTable[q]))
		{
			return seerTable[q]+(byte)Random(2);
		}
	}
	// no quest was picked
	return 160+(byte)Random(2);
}

void BeginChatting(byte tag)
{
	int i,j,k;

	if(player.chatClock)
		return;

	if(player.worldNum!=WORLD_NORMAL && player.worldNum!=WORLD_REMIX && player.worldNum!=WORLD_RANDOMIZER)
		return;

	player.chatClock=30;
	EnterChatMode();
	curChar=0;
	curLine=0;
	noButtons=30;	// for one second, you can't skip anything in chat, to avoid
					// blowing over Silver Bullet and other such messages

	switch(tag)
	{
		case 0:
		case 1:	// nobody of interest
			curChat=0;
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=174+(byte)Random(7);
			break;
		case 2:	// guy who wants Terror Trees quest done
			if(player.var[VAR_QUESTDONE+QUEST_TREES])
			{
				// if the quest is completed
				if(player.var[VAR_TREEREWARD])	// he has already given you the heart
				{
					if(player.var[VAR_QUESTDONE+QUEST_HILL])
						curChat=168;
					else
						curChat=4;
				}
				else	// he says thanks and gives you a super heart
					curChat=2;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_TREES])
				// if this quest has been assigned (but not completed)
				curChat=3;
			else	// hasn't been assigned at all
				curChat=1;
			break;
		case 3:	// girl who wants you to Save Halloween Hill
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
			{
				// you have completed the quest
				curChat=12;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_HILL])
			{
				// it's been assigned, but you haven't finished yet
				curChat=11;
			}
			else
				curChat=5;
			break;
		case 4: // Biff the traveling boot salesman
			if(player.var[VAR_QUESTDONE+QUEST_BOOTS])
			{
				// you have completed the quest
				if(player.var[VAR_QUESTDONE+QUEST_HILL])
					curChat=167;
				else
					curChat=10;
			}
			// whether or not it's assigned, this works the same
			else if(player.var[VAR_BOOTSALES]==1)
			{
				if(player.money>=30)
				{
					player.money-=30;
					curChat=9;
				}
				else
					curChat=8;
			}
			else
				curChat=8;
			break;
		case 5:	// Bonita Barrel in Luniton
			if(player.var[VAR_WEAPON+3]==1)
			{
				if(player.var[VAR_QUESTDONE+QUEST_HILL])
					curChat=165;
				else
					curChat=35;
			}
			else
				curChat=14;
			break;
		case 6:	// Weaponmaster Kinyova
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=163;
			else
				curChat=16;
			break;
		case 7:	// the cat lady
			if(player.var[VAR_QUESTASSIGN+QUEST_CAT]==0)
				curChat=24;
			else
			{
				if(player.var[VAR_QUESTDONE+QUEST_CAT])
				{
					if(player.var[VAR_QUESTDONE+QUEST_HILL])
						curChat=166;
					else
						curChat=27;
				}
				else
				{
					if(player.var[VAR_CAT])
						curChat=26;
					else
						curChat=25;
				}
			}
			break;
		case 8:	// Zizwalda the witch
			if(player.var[VAR_QUESTASSIGN+QUEST_SHROOM]==0)
				curChat=28;
			else
			{
				if(player.var[VAR_QUESTDONE+QUEST_SHROOM]==0)
				{
					if(player.var[VAR_WITCHTALK]<5)
					{
						player.var[VAR_WITCHTALK]++;
						if(player.var[VAR_WITCHTALK]<5)
							curChat=30;
						else
							curChat=33;	// turn to toad
					}
					else
						curChat=34;
				}
				else
				{
					if(player.var[VAR_QUESTASSIGN+QUEST_DAISY]==0)
						curChat=31;
					else
					{
						if(player.var[VAR_QUESTDONE+QUEST_DAISY]==1)
						{
							if(player.var[VAR_WITCHREWARD==0])
								curChat=191;
							else if(player.var[VAR_GAVEDAISY]==0)
								curChat=60;
							else if(player.var[VAR_QUESTDONE+QUEST_GHOST]==0)
							{
								if(player.money<75)
									curChat=61;
								else
								{
									curChat=62;
									player.money-=75;
								}
							}
							else
							{
								if(player.var[VAR_QUESTDONE+QUEST_HILL])
									curChat=164;
								else
									curChat=64;
							}
						}
						else
							curChat=63;
					}
				}
			}
			break;
		case 9:	// the bridge guard
			if(player.var[VAR_TALKBRIDGE]==0)
				curChat=36;
			else if(player.var[VAR_PAIDBRIDGE])
			{
				if(player.var[VAR_QUESTDONE+QUEST_HILL])
					curChat=185;
				else
					curChat=38;
			}
			else
			{
				if(player.money<75)
					curChat=36;
				else
					curChat=37;
			}

			break;
		case 10: // the farmer
			if(player.var[VAR_FERTILIZER])
			{
				if(player.var[VAR_QUESTDONE+QUEST_HILL])
					curChat=184;
				else
					curChat=44;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_CROPS])
				curChat=42;
			else if(player.var[VAR_QUESTASSIGN+QUEST_CROPS])
				curChat=41;
			else if(player.var[VAR_QUESTDONE+QUEST_ZOMBIES])
				curChat=39;
			else
				curChat=59;
			break;
		case 11: // bartender
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=183;
			else
				curChat=45;
			break;
		case 12: // bar patron
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=174+(byte)Random(7);
			else
				curChat=46+(byte)Random(7);
			break;
		case 13: // mayor of zombiton
			if(player.var[VAR_QUESTDONE+QUEST_ZOMBIES])
			{
				if(player.var[VAR_ZOMBIEREWARD])
				{
					if(player.var[VAR_QUESTDONE+QUEST_HILL])
						curChat=169;
					else
						curChat=57;
				}
				else
					curChat=56;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_ZOMBIES])
				curChat=58;
			else
				curChat=55;
			break;
		case 14:	// mine worker in zombiton
			if(!player.var[VAR_MINERTALK])
			{
				player.var[VAR_MINERTALK]=1;
				curChat=69;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_SILVER])
			{
				if(player.var[VAR_QUESTDONE+QUEST_HILL])
					curChat=182;
				else
					curChat=76;
			}
			else if(player.var[VAR_SILVER])
				curChat=75;
			else if(player.var[VAR_QUESTASSIGN+QUEST_SILVER])
				curChat=72;
			else
				curChat=69;
			break;
		case 15:	// girl trapped in cavern
			if(player.var[VAR_QUESTDONE+QUEST_RESCUE]==0)
			{
				if(player.var[240])	// have talked to her
				{
					if(player.var[240]==2)	// have rescued her already
						curChat=80;
					else
						curChat=78;
				}
				else	// haven't talked yet!
					curChat=77;
			}
			else
			{
				if(player.var[240])	// have talked to her
				{
					if(player.var[240]==2)	// have rescued her already
						curChat=116;
					else
						curChat=78;
				}
				else	// haven't talked yet!
					curChat=115;
			}
			break;
		case 16:	// Larry after returning to human
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=172;
			else
			{
				if(player.levelNum!=46)
					curChat=87;
				else
					curChat=189;
			}
			break;
		case 17:	// Larry's wife
			if(player.var[VAR_QUESTDONE+QUEST_WOLF])
			{
				if(player.var[VAR_LARRYREWARD])
				{
					if(player.var[VAR_QUESTDONE+QUEST_HILL])
						curChat=171;
					else
						curChat=86;
				}
				else
					curChat=85;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_WOLF])
				curChat=88;
			else
				curChat=83;
			break;
		case 18:	// doll collector
			if(player.var[VAR_QUESTASSIGN+QUEST_DOLLS]==0)
				curChat=90;
			else
			{
				if(player.var[VAR_QUESTDONE+QUEST_DOLLS])
				{
					if(player.var[VAR_QUESTDONE+QUEST_HILL])
						curChat=181;
					else
						curChat=97;
				}
				else
				{
					j=0;
					k=0;
					for(i=0;i<8;i++)
					{
						if(player.var[VAR_DOLL+i] && !player.var[VAR_DOLLGIVEN+i])
						{
							player.money+=25;
							player.gemsGotten+=25;
							BadgeCheck(BE_GEMSUP,0,curMap);
							if(player.money>player.maxMoney)
							{
								player.money=player.maxMoney;
							}
							PlayerSetVar(VAR_DOLLGIVEN+i,1);
							MakeNormalSound(SND_BIGGEMGET);
							j++;
							k=1;
						}
						else if(player.var[VAR_DOLLGIVEN+i])
							j++;
					}
					if(k)
					{
						if(j==8)
							curChat=96;
						else if(j>4)
							curChat=95;
						else
							curChat=94;
					}
					else
						curChat=93;
				}
			}
			break;
		case 19:	// zombiton nobody
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=174+(byte)Random(7);
			else
				curChat=(byte)Random(7)+121;
			break;
		case 20:	// messy guy in messy cabin
			if(player.var[VAR_QUESTDONE+QUEST_HILL])
				curChat=172;
			else
				curChat=128;
			break;
		case 21:	// seer in zombiton
			if(player.var[VAR_SEERTALK])
			{
				curChat=131;
				talk[curChat].next=PickSeerChat();
			}
			else
			{
				curChat=129;
				if(player.var[VAR_MYSORB+2])
				{
					talk[curChat].next=130;
					talk[130].next=PickSeerChat();
				}
				else
					talk[curChat].next=PickSeerChat();
				player.var[VAR_SEERTALK]=1;
			}
			break;
		case 22:	// hot pants seller
			if(player.var[VAR_HPSALES]==0)
			{
				curChat=192;
				player.var[VAR_HPSALES]=1;
			}
			else if(player.var[VAR_WEAPON+ITM_WHOTPANTS-ITM_WBOMB])	// have hotpants
			{
				curChat=195;
			}
			else
			{
				if(player.money<150)
				{
					curChat=193;
				}
				else
				{
					curChat=194;
				}
			}
			break;
		case 23:	// arms dealer
			if(player.var[VAR_DEALERTALK]==0)
			{
				curChat=196;
				player.var[VAR_DEALERTALK]=1;
			}
			else if(player.var[123])	// already have that bust
			{
				curChat=200;
			}
			else
			{
				k=0;

				for(i=0;i<7;i++)
					if(player.var[VAR_WEAPON+i])
						k++;

				if(k==7)
					curChat=198;
				else
					curChat=196;
			}
			break;
		case 231:
			// mecha-bonkula dies
			curChat=203;
			break;
		case 232:
			// meet Mecha-Bonkula
			curChat=202;
			break;
		case 233:
			// Mini-Frankenjulie talking to you
			curChat=201;
			break;
		case 234:	// larry is beaten
			curChat=188;
			break;
		case 235:	// larry says hi
			curChat=187;
			break;
		case 236:	// stick witch intro
			curChat=162;
			break;
		case 237:	// Bonkula dead
			if(player.var[VAR_DEADBONK])
			{
				ExitChatMode();
				return;
			}
			curChat=120;
			break;
		case 238:	// Bonkula intro
			curChat=117;
			break;
		case 239:	// toasty talks
			curChat=113;
			break;
		case 240:	// drippy talks
			curChat=111;
			break;
		case 241:	// Stoney dies
			curChat=110;
			break;
		case 242:	// talking to Stoney
			curChat=108;
			break;
		case 243:	// talking to Gusty
			curChat=106;
			break;
		case 244:	// talking to Farley
			if(player.var[VAR_QUESTASSIGN+QUEST_FARLEY]==0)
			{
				curChat=99;
			}
			else
			{
				if(player.monsterPoints<5000)
					curChat=105;
				else
					curChat=101;
			}
			break;
		case 245:	// talking to self about flames
			if(player.var[VAR_QUESTDONE+QUEST_DARK])
			{
				ExitChatMode();
				return;
			}
			curChat=98;
			break;
		case 246:	// talking to self about werewolves
			curChat=89;
			break;
		case 247:	// the girl picks a lock
			curChat=81;
			break;
		case 248:	// the girl speaks upon rescuing
			if(player.var[VAR_QUESTDONE+QUEST_RESCUE])
				curChat=116;
			else
				curChat=79;
			break;
		case 249:
			// Polterguy talking to you
			curChat=68;
			break;
		case 250:
			// talking to self about ghosts being invincible
			curChat=67;
			break;
		case 251:
			// Frankenjulie talking to you
			curChat=66;
			break;
		case 252:
			// reading the electro-gate sign
			curChat=65;
			break;
		case 253:
			// talking to yourself about darkness
			if(player.var[VAR_STICK]==1)
				curChat=54;
			else
				curChat=53;
			break;
		case 254:
			// talking to yourself about torches
			if(player.var[VAR_QUESTDONE+QUEST_DARK])
			{
				ExitChatMode();
				return;
			}
			if(player.var[VAR_STICK]==1)
				curChat=22;
			else
				curChat=23;
			break;
		case 255:
			// talking to yourself about mud
			curChat=13;
			break;
	}
}

void DoChatAction(byte a)
{
	switch(a)
	{
		case 1:
			// assign tree killing quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_TREES,1);
			break;
		case 2:
			// give the super heart
			PlayerSetVar(VAR_TREEREWARD, 1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(8);
			}
			else
			{
				player.var[VAR_HEART+15]=1;
				PlayerGetItem(ITM_SUPERHEART,0,0);
			}
			break;
		case 3:
			// assign Save Halloween Hill
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HILL,1);
			break;
		case 4:
			// set it so player HAS talked to boot salesman
			PlayerSetVar(VAR_BOOTSALES,1);
			break;
		case 5:
			// complete the boots quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_BOOTS,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(5);
			}
			else
			{
				PlayerSetVar(VAR_BOOTS,1);
			}
			break;
		case 6:
			// assign the boots quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BOOTS,1);
			break;
		case 7:
			// assign Curse The Darkness
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_DARK,1);
			break;
		case 8:
			// assign kitty cat
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_CAT,1);
			break;
		case 9:
			// complete kitty cat
			PlayerSetVar(VAR_QUESTDONE+QUEST_CAT,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(2);
			}
			else
			{
				PlayerSetVar(VAR_STICK,1);
			}
			break;
		case 10:
			// assign shroom
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SHROOM,1);
			break;
		case 11:
			// assign daisy quest and give a super heart
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_DAISY,1);
			PlayerSetVar(VAR_WITCHREWARD, 1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(9);
			}
			else
			{
				player.var[VAR_HEART+16]=1;
				PlayerGetItem(ITM_SUPERHEART,0,0);
			}
			break;
		case 12:
			// get turned into a frog
			TurnLoonyIntoToad();
			break;
		case 13:
			// mark bridge guy talked to
			PlayerSetVar(VAR_TALKBRIDGE,1);
			break;
		case 14:
			// pay bridge guy
			player.money-=75;
			PlayerSetVar(VAR_PAIDBRIDGE,1);
			break;
		case 15:
			// assign pumpkin quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_CROPS,1);
			break;
		case 16:
			// give fertilizer
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(4);
			}
			else
			{
				PlayerSetVar(VAR_FERTILIZER,1);
			}
			break;
		case 17:
			// assign zombie quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_ZOMBIES,1);
			break;
		case 18:
			// reward zombie quest
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(10);
			}
			else
			{
				PlayerSetVar(VAR_ZOMBIEREWARD,1);
				player.money+=100;
				player.gemsGotten+=100;
				BadgeCheck(BE_GEMSUP,0,curMap);
				if(player.money>player.maxMoney)
				{
					player.money=player.maxMoney;
				}
			}
			break;
		case 19:
			// give daisy to chick
			PlayerSetVar(VAR_GAVEDAISY,1);
			break;
		case 20:
			// complete ghost quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_GHOST,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(0);
			}
			else
			{
				PlayerSetVar(VAR_POTION,1);
			}
			break;
		case 21:
			// assign silver quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SILVER,1);
			break;
		case 22:
			// complete silver quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SILVER,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(3);
			}
			else
			{
				PlayerSetVar(VAR_SILVERSLING, 1);
			}
			break;
		case 23:
			// assign rescue quest and make girl obnoxious
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_RESCUE,1);
			PlayerSetVar(240,1);	// use temp var, so you can re-rescue her
			GirlChasePlayer(1);
			break;
		case 24:
			// complete rescue quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_DARK,1);
			PlayerSetVar(VAR_QUESTDONE+QUEST_RESCUE,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(7);
			}
			else
			{
				PlayerSetVar(VAR_TORCH,1);
				PlayerSetVar(VAR_LANTERN, 1);
			}
			PlayerSetVar(240,2);	// use temp var, so you can re-rescue her
			GirlChasePlayer(0);
			break;
		case 25:
			// make boss noise
			MakeNormalSound(SND_SCARYBOSS);
			break;
		case 26:
			// assign wolf quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_WOLF,1);
			break;
		case 27:
			// give pumpkin key
			PlayerSetVar(VAR_LARRYREWARD,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(1);
			}
			else
			{
				PlayerGetItem(ITM_KEY4,0,0);
				PlayerSetVar(VAR_KEY+2,1);
			}
			PlayerSetVar(VAR_LARRYREWARD,1);
			break;
		case 28:
			// assign doll quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_DOLLS,1);
			break;
		case 29:
			// finish doll quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_DOLLS,1);
			if (player.worldNum == WORLD_RANDOMIZER){
				GiveRandoItem(6);
			}
			else
			{
				PlayerSetVar(VAR_REFLECT,1);
				player.fireFlags|=FF_REFLECT;
			}
			break;
		case 30:
			// get the helper bat
			PlayerSetVar(VAR_HELPERBAT,1);
			PlayerSetVar(VAR_QUESTDONE+QUEST_FARLEY,1);
			player.fireFlags|=FF_HELPER|FF_HELPERHERE;
			break;
		case 31:
			// assign bat quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_FARLEY,1);
			break;
		case 32:
			// assign ghost quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_GHOST,1);
			break;
		case 33:
			// mark bonkula dead
			PlayerSetVar(VAR_DEADBONK,1);
			break;
		case 34:
			// play the space game!!!!
			coro::launch(std::bind(SpaceGame, GetDisplayMGL()));
			break;
		case 35:
			// buy the hot pants
			player.money-=150;
			PlayerSetVar(VAR_WEAPON+ITM_WHOTPANTS-ITM_WBOMB,1);
			PlayerGetWeapon(ITM_WHOTPANTS);
			break;
		case 36:
			PlayerSetVar(123,1);	// get the vampire bust
			break;
	}
}

void UpdateChat(void)
{
	byte c;

	if(noButtons)
	{
		noButtons--;
		GetTaps();
		c=0;
	}
	else
		c=GetTaps();

	if(curLine<4 && talk[curChat].line[curLine][0]=='\0')
	{
		curLine=4;
	}

	if(curChar<48)
	{
		curChar++;
		if(talk[curChat].line[curLine][curChar]=='\0')
		{
			curChar=48;
		}
	}
	if(curChar>=48)
	{
		if(curLine<4)
			curLine++;
		curChar=0;
	}
	if(c&CONTROL_B1)
	{
		if(curLine<4)
			curLine=4;
		else
		{
			if(talk[curChat].action)
				DoChatAction(talk[curChat].action);
			if(talk[curChat].next)
			{
				curChat=talk[curChat].next;
				curChar=0;
				curLine=0;
			}
			else
			{
				ExitChatMode();
			}
		}
	}
}

void RenderChat(MGLDraw *mgl)
{
	int i;
	char s[50];

	DrawFillBox(65,35,639-65,35+10+4*22,4);
	DrawBox(65,35,639-65,35+10+4*22,31);

	for(i=0;i<4;i++)
	{
		if(curLine>i)
			Print(70,37+i*22,talk[curChat].line[i],0,0);
		else if(curLine==i)
		{
			strncpy(s,talk[curChat].line[i],curChar);
			s[curChar]='\0';
			Print(70,37+i*22,s,0,0);
		}
	}
}

void GiveRandoItem(int index)
{
	PlayerSetVar(randoReward[index].playerVarId, 1);
	PlayerGetItem(randoReward[index].itemId, 0, 0);
}
//--------------------------------------------
// END CHATTING

void LoadRandoItems(){
	std::FILE* f = AppdataOpen("quest.txt", "r");
	if (f == NULL)
	{
		return;
	}
	FilePtrStream stream(f);
	std::string line;
	std::string value;
	while (getline(stream, line)){

		std::stringstream ss(line);

		getline(ss, value, '\t');
		int questID = stoi(value);
		getline(ss, value, '\t');
		int varID = stoi(value);
		getline(ss, value, '\t');
		int itemID = stoi(value);

		randoReward[questID].itemId = itemID;
		randoReward[questID].playerVarId = varID;
	}

}
