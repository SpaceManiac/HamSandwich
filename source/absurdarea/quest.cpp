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
	"Wood You Help?",
	"Presents Absent",
	"Prove Yourself",
	"Show Your Stuff",
	"Herbal Essences",
	"The Sharpest Tool",
	"The Mimic Whisperer",
	"Crystal Quest",
	"Stockboy",
	"Offer You Can't Refuse",
	"Unchain Melody",
	"Guru Hide & Seek",
	"Mickey's Mice",
	"Hot Pants",
	"Be A Contender",
	"Toys R Dust",
	"Don't Overdue It",
	"The Loony Express",
	"All Tied Up",
	"Show Him The Ropes",
	"Lend A Hand",
	"Can You Help?",
	"Unbearable",
	"Bear With It",
	"Medicinal Purposes",
	"Deliverin' The Goods",
	"Remember The Titans",
	"Mad Skillz",
	"Factory Clearance",
	"Power Down",
	// these are Onion quests, the Bunny quests are in altQuestName
	"The Buddy System",
	"Loyalty Test",
	"Junk n' Stuff",
	"An Easy Quest",
	"Highway Robbery",
	"Shakedown",
	"Supplies!",
	"The Rumble",
	"Bop 'Em On The Head",
	"Furnace Repair",
	// back to regular
	"Frajeelay",
	"Herbicidal Tendencies",
	"Sacred Forest",
	"Bok Bok",
	"Treehugger",
	"Clash With The Titans",
	"Baron Beatdown",
	"Take Control",
	"You Are The Champion",
	"Happy Stick Showdown",
};

char altQuestName[10][32]={
	"The Buddy System",
	"Rising Ranks",
	"Clean Their Clocks",
	"A Miner Quest",
	"Highway Patrol",
	"Shutdown",
	"Supplies!",
	"The Rumble",
	"Dice The Onion",
	"Something's Fishy",
};

char questDesc[NUM_QUESTS*4][64]={
	// #0 - Collect Logs
	"Collect 10 Logs from wooden monsters and bring them to",
	"Woody Woodsman, and he will teach you Axe Mastery.",
	"",
	"",
	// #1 - Presents Absent
	"Collect the 99 Festivus presents that have been stolen",
	"from the village folk and return them to Bobby Joe, who",
	"claims he will distribute them fairly.",
	"",
	// #2 - Prove Yourself
	"Slay 20 monsters, then return and tell Kinyova to earn",
	"the Throwing Skill Scroll.",
	"",
	"",
	// #3 - Show Your Stuff
	"Slay 100 monsters, then return and tell Kinyova to earn",
	"the Bash Skill Scroll.",
	"",
	"",
	// #4 - Herbal Essences
	"Collect 3 Frostbane Herbs and bring them to Healiana in",
	"town, so she can brew some Healium.",
	"",
	"",
	// #5 - Sharpening Stone
	"Find the Woodsman's Sharpening Stone and return it to",
	"him.  Then he'll be able to sharpen your axe!",
	"",
	"",
	// #6 - The Mimic Whisperer
	"Tame a Mimic for the Magicsmith.  She'll reward you with",
	"the Training skill.  To tame a Mimic, feed it delicious",
	"items, and avoid hitting it or letting it bite you.",
	"",
	// #7 - Crystal Method
	"Find the Crystal Of Binding in the underworld and bring it",
	"to the Magicsmith, so she can magically combine items for",
	"you.",
	"",
	// #8 - Stockboy
	"Collect any 3 Axes (besides the one you need!) and bring",
	"them to Geoffrey so he can re-open Axes R Us.",
	"",
	"",
	// #9 - An Offer You Can't Refuse
	"Find Black Market Bob's wife, apparently lost in the",
	"forest somewhere.  Bob will make it worth your while.",
	"",
	"",
	// #10 - Unchain Melody
	"Find the Snowman Key to unlock the chains holding",
	"Melody.  THEN Bob will make it worth your while.",
	"",
	"",
	// #11 - Guru Hide & Seek
	"Find The Great Guru 30 times to earn all 30 Talents.",
	"",
	"",
	"",
	// #12 - Mickey's Mice
	"Clear the rats out of Mickey's cellar for a $50 reward.",
	"",
	"",
	"",
	// #13 - Hot Pants
	"Collect 5 Asbestos Herbs and 5 Socks, so Taylor can knit",
	"you a comfy set of Asbestos Underwear.",
	"",
	"",
	// #14 - Be A Contender
	"Demonstrate your gladiatorial skills and Tina will teach you",
	"a real fighting technique.",
	"",
	"",
	// #15 - Toys
	"Mickey is disturbed by the deadly toys lurking all around.",
	"Put his mind at ease.",
	"",
	"",
	// #16 - book
	"Return the book Betty borrowed from The Halls Of History",
	"beneath Terrible Tundra.",
	"",
	"",
	// #17 - The Loony Express
	"Deliver Healiana's letter to her sister in the Frosty Cliffs.",
	"",
	"",
	"",
	// #18 - All Tied Up
	"Find the missing Ropemaker in the Cliffs.",
	"",
	"",
	"",
	// #19 - Show Him The Ropes
	"The balloonist needs 3 Ropes to fix up his balloon before",
	"it will be airworthy again.",
	"",
	"",
	// #20 - Lend A Hand
	"Find Handyman Hank's toolbox in Baskerville and return it",
	"to him in Chilly Cavern.",
	"",
	"",
	// #21 - Can You Help?
	"Collect Tin Cans for Rosetta Roper to store her very",
	"special soup in.",
	"",
	"",
	// #22 - Unbearable
	"Defeat 30 Cave Bears to make the Halls Of History safer.",
	"",
	"",
	"",
	// #23 - Bear With It
	"Defeat the leader of the Cave Bears.",
	"",
	"",
	"",
	// #24 - Medicinal Purposes
	"Collect 10 Devil Grass for Curaga.  For medicinal purposes,",
	"of course.",
	"",
	"",
	// #25 - Deliverin' The Goods
	"Pick up Curaga's package from the Magic Shop by",
	"the highway and bring it to her.",
	"",
	"",
	// #26 - Remember The Titans
	"Obtain 3 Ice Cubes from the Ice Hounds for The ",
	"Historian to study.",
	"",
	"",
	// #27 - Mad Skillz
	"Collect all 50 Skill Scrolls and bring them to The",
	"Historian.",
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
	"Befriend some allies on the surface before risking the",
	"horrors of the underworld.",
	"",
	"",
	// #31 - Loyalty Test
	"Prove your loyalty to The Onion Ring by completing any",
	"3 of the missions they offer you.",
	"No, not including this one.  That would be a paradox.",
	"",
	// #32 - Junksmith
	"Bring the Junksmith 50 pieces of junk and he'll let you",
	"into the hallowed halls of his junkyard.",
	"",
	"",
	// #33 - An Easy Quest
	"Steal candy from a baby.",
	"",
	"",
	"",
	// #34 - Highway Robbery
	"Rob caravans on the Haunted Highway to collect Stolen",
	"Goods.",
	"",
	"",
	// #35 - Shakedown
	"Shake down some merchants for protection money:",
	"Axes R Us, Ropes N' Ropes, The Magic Box, and Taylor's",
	"Knits.",
	"",
	// #36 - Supplies!
	"Destroy the Snuggly supply depot hidden in a cave in the",
	"Frosty Cliffs.",
	"",
	"",
	// #37 - The Rumble
	"With the rest of the Onion Ring, go pummel the",
	"Order Of The Snuggly Bunny in the clearing south",
	"of Northwood.",
	"",
	// #38 - Bop Em On The Head
	"Destroy the Order by taking out their leader, Captain",
	"Capitan.",
	"",
	"",
	// #39 - Fix The Furnace
	"Find the three missing Furnace pieces and put the",
	"Furnace back together.",
	"",
	"",
	// #40 - Frajeelay
	"Get a Sounding Crystal to break the barrier.  Don't drop",
	"it!  Bring the Thing Of Majig back to Schrodinger.",
	"",
	"",
	// #41 - Plants
	"Slay the dreaded Gourdzilla to obtain the Watch of Makalit",
	"and return it to Schrodinger.",
	"",
	"",
	// #42 - Defiling
	"Clean the forest of defiling Bodzhas, so that Shrumford",
	"can rejuvenate and tell you something important.",
	"",
	"",
	// #43 - Bok Bok
	"Rescue the ten lost Bokboks for Sharona.  She'll give you a",
	"Binding Crystal in return.",
	"",
	"",
	// #44 - Treehugger
	"Bring 20 Seeds to Sharona so she can plant less vindictive",
	"shrubbery, and teach you the Shroom spell.",
	"",
	"",
	// #45 - Clash With Titans
	"Defeat the Titanic Titans lurking in the Titan Tunnels.",
	"",
	"",
	"",
	// #46 - Baron
	"Learn what Baron Von Frostburn knows.  Then",
	"learn him a lesson.",
	"",
	"",
	// #47 - Toy Turnabout
	"Collect the Toy Upgrades scattered around Glacial Gorge",
	"and bring them to J.P. Toymonger III.",
	"",
	"",
	// #48 - Champion
	"Become the champion of the arena by dethroning the",
	"former champion in the final battle.",
	"The prize is $5,000.",
	"",
	// #49 - Happy Stick
	"Put an end to this madness by destroying the",
	"fiend that started it all.",
	"",
	"",
};

char altQuestDesc[10*4][64]={
	// #30 - The Buddy System
	"Befriend some allies on the surface before risking",
	"the horrors of the underworld.",
	"",
	"",
	// #31 - Rising Ranks
	"Improve your rank in the Order Of The Snuggly",
	"Bunny.  Do this by completing any 3 of the missions",
	"they offer you.",
	"This one doesn't count.",
	// #32 - Clean Their Clocks
	"Destroy the failed clockwork experiments in the",
	"Clockwork lab.",
	"",
	"",
	// #33 - A Miner Quest
	"Mine a Diamond to help the Clockmaker with his",
	"experiments.",
	"",
	"",
	// #34 - Highway Patrol
	"Protect the caravans on the Haunted Highway from",
	"robbers.  Defeat all Thugs and Bruisers.",
	"",
	"",
	// #35 - Shutdown
	"Shut down the illegal axe trading operation in a",
	"cave in the Frosty Cliffs.",
	"",
	"",
	// #36 - Supplies!
	"Dig up and destroy the Onion Ring's hidden supplies",
	"in Westwood.",
	"",
	"",
	// #37 - The Rumble
	"Join the Order in a battle with The Onion Ring,",
	"in a clearing south of Northwood.",
	"",
	"",
	// #38 - Dice The Onion
	"Beat The Diabolical Onion himself into submission.",
	"",
	"",
	"",
	// #39 - Something is Fishy
	"Ignoring Raphael's instructions, find the missing",
	"Furnace parts and repair it.",
	"",
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
	{"Howdy stranger!  I am Kinyova Senior.  Oh, you know my no-good layabout son who's never mastered "
	 "more than 118 weapons in his entire worthless",
	 0,2},
	 // 2
	{"existence?  A pox on him!  I see you've got what could laughably termed an axe there in your hand.  "
	 "Here's a deal: you prove yourself to me by slaying",
	 0,3},
	 // 3
	{"20 monsters with that thing, and I'll teach you a whole different way to use it!  Well?  Hop to it, boy!",
	 1,0},
	 // 4 waiting to finish first quest
	{"I'm not impressed yet.  You're worse than that lazy good-for-nothing who calls himself my son!  Where's your spine?",
	 0,0},
	 // 5 complete quest 1
	{"Excellent!  You just might amount to something after all!  Here, let me show you how to throw your axe.  With "
	 "how puny you are, you'll want to stay as far",
	 0,6},
	 // 6
	{"from the monsters as possible.  Here's the Skill Scroll.  Remember - you have to add points to the skill "
	 "or it's not doing anything for you!",
	 2,0},
	 // 7 quest 1 finished, offer quest 2
	{"You again?  You wanna learn some more of my infinite arsenal of tricks?  Okay... but first, show me what you got!  "
	 "Take down 100 monsters and I'll set you up.",
	 3,0},
	 // 8 waiting on quest 2
	{"What's the matter, 100 monsters too tough for you?  I swear, kids today have no discipline.  In my day, we had to fight 200 "
	 "monsters just to get to the bathroom!",
	 0,0},
	 // 9 complete quest 2
	{"Oh, good!  If only I could've had a kid like you.  Okay, here's another Skill Scroll.  It'll show you how to smash "
	 "enemies away from you.",
	 4,0},
	// 10 quests done
	{"I've got better things to do than lead kids around by the hand all day!  Buzz off!",
	 0,0},
	 // BOBBY JOE
	 // 11 introduction + quest assign
	{"WAAAAHHH!!!!  I woke up this morning and all my Festivus presents were missing!  All the other kids' presents are gone "
	 "too!  Could you bring them back... pwease??",
	 5,0},
	 // 12 waiting on presents
	{"WAAHHH!!!  Where are my presents?!?",
	 0,0},
	 // 13 got all presents, quest complete
	{"WAAAaat?  Presents?  W00T!  Let's see, one for me, one for Darla, one... hey, I think this one's for you!  Thanks a "
	 "lot, mister!  You've saved Festivus!",
	 6,0},
	 // 14 all done
	{"Meh.  I'm bored with those toys now.  How long until next Festivus?",
	 0,0},
	 // HEALIANA
	 // 15 introduction + herb quest
	{"Greetings, I am this town's healer.  Yet alas, I can heal no one!  I have run out of Frostbane Herbs, the "
	 "key ingredient in Healium.  If you would be a dear and fetch",
	 0,16},
	 // 16
	{"me 3 Frostbane Herbs from the woods, I'll gladly heal you for free from now on!",
	 7,0},
	 // 17 herb quest incomplete
	{"Do you need healing?  Ah, that's a shame, since I SOMEHOW lack enough Frostbane Herbs to make a batch of Healium. "
	 "If only that were remedied in some way...",
	 0,0},
	 // 18 herb quest done
	{"Oh thank you!  Let me just toss these herbs in with 10 other secret herbs and spices, "
	 "and... voila!  Healium!  Now, let me tend to those wounds...",
	 8,0},
	 // 19 all done, heals you each time now
	{"Greetings.  Please enjoy this complimentary Healium sample!",
	 9,0},
	 // WOODY WOODSMAN
	 // 20 intro + wood quest
	{"Hey, you're up and moving!  I found you out in the snow. You're lucky - you would've frozen to death if I hadn't! "
	 "Come talk to me when you feel up to it.",
	 0,0},
	 // 21
	{"Great!  Now that you're feeling better, could you help me out?  It's been "
	 "getting oddly colder lately, and I spent so much time feeding you soup, I haven't managed",
	 0,22},
	 // 22
	{"to collect enough firewood.  I gave you my old axe and parka, so that should be all you need. "
	 "Just one thing... This forest is a national park, so don't chop down any",
	 0,23},
	 // 23
	{"trees!  You'll need to find another source of wood... "
	 "Once we've warmed up a little, maybe I'll teach you a thing or two about using that axe!",
	 10,0},
	 // 24 wood quest unfinished
	{"Brrr... have you gotten enough wood yet?",
	 0,0},
	 // 25 wood quest done
	{"Oh, thank you!  This should tide us over a while.  We've never had a winter this bad "
	 "before, it's strange!  Now let me show you a few tricks with your axe!",
	 11,0},
	 // 26 assign sharpening stone
	{"Hmm... I could've sworn it was right here! Hey, if you see my old sharpening stone, "
	 "let me know.  I can't sharpen up any axes without it!",
	 12,0},
	 // 27 sharpening stone not done
	{"Ack, where is that thing?  Did I drop it when I went to the Arena yesterday?",
	 0,0},
	 // 28 finished sharpening stone
	{"Wow, you found it!  Here, let me show you how it works... first one's free, but if "
	 "you need another axe sharpened, it's $50. Special price, just for you!",
	 13,0},
	 // 29 all done, axe sharpening shop
	{"Sharpen your axe?  Adds +1 Damage to it for only $50!  It's also handy for chopping "
	 "down dead trees that are too tough for a dull axe.",
	 14,0},
	 // MAGICSMITH
	 // 30 intro + mimic quest
	{"Hey there!  I am a Magicsmith. My job is to take magic items and smith them "
	 "together.  It's pretty self-explanatory, I guess.",
	 0,31},
	 // 31
	{"But can you do me a favor?  I really need something to store my supplies in. "
	 "If you tame me a Mimic, it will hold my gear for me!",
	 0,32},
	 // 32
	{"Mimics are found on Mimic Isle, in the middle of Limpid Lake.  They love to eat "
	 "items and adventurers.  If you feed one enough items, it won't eat you!",
	 15,0},
	 // 33 assign crystal quest
	{"So, I bet you're wondering what a magicsmith does!  Sadly, nothing right "
	 "now.  My Crystal Of Binding was stolen by some monsters from the underworld.",
	 0,34},
	 // 34
	{"If you can bring it back to me, I can magicsmith together items for you.  That "
	 "means I combine two Magic items to make a Golden item with their combined",
	 0,35},
	 // 35
	{"power.  It's not cheap, but it makes some nice items!  Can you help?",
	 17,0},
	 // 36 crystal quest not done
	{"Have you found my crystal yet?",
	 0,0},
	 // 37 crystal quest done
	{"Hooray!  Now I can magicsmith like nobody's business!  Come on back with "
	 "some Magic items, and I'll combine them for you.",
	 18,0},
	 // 38 all done, now it's a magicsmith shop
	{"Select two Magic items of the same type, and I'll quote you a price to combine them. "
	 "It costs more with more powerful items.",
	 19,0},
	 // GEOFFREY, AXES R US
	 // 39 intro, axe quest
	{"Oh woe is me... I used to run the biggest axe shop in town, but somebody robbed me! "
	 "I could reopen my shop if only I had a little bit of stock to start from.",
	 22,0},
	 // 40 axe quest undone
	{"Hi, I wish I could sell you some axes, but unfortunately I have none to offer!",
	 0,0},
	 // 41 axe quest done
	{"Wha... for me?!  Oh thank you, sir!  Here, I'll pay you um, fair market value for those. "
	 "And at last, I can re-open Axes R Us!  Be sure to come back for our grand opening!",
	 23,0},
	 // 42 axe shop open
	{"Hello, we offer the finest in axes, both used and new.  We're also happy to accept "
	 "trade-ins, so bring in your old junker!",
	 0,0},
	 // BLACK MARKET BOB
	 // 43 intro, wife quest
	{"Psst, Hey pal, you wanna do a guy a favor?  I can make it worth your while..."
	 "See, my wife was out collecting on some debts that some gentlemen owed",
	 0,44},
	{"me, and she hasn't come back.  If you could find her, I'd be much obliged.  You might even "
	 "say I'd owe you a favor in return. So how's about it, pal?",
	 27,0},
	 // 45 wife quest not done
	{"You still ain't seen her?  She went to have a 'chat' with some people on the south "
	 "side is all I know.",
	 0,0},
	 // 46 have finished wife quest, but not wife quest 2
	{"They chained her up?  Those rats!  I can't pick a Snowman lock... we'll have to find a "
	 "key.  Who did this?  Was it Klonk?",
	 0,0},
	 // 47 both wife quests done
	{"I appreciate you looking out for the family. Tell you what I'm gonna do.  I got this here "
	 "parka that, uh, fell off a caravan.  You take it, it's my gift to you.  And if you",
	 0,48},
	 // 48
	{"ever need anything, you come on back.  I sell all the best stuff.  I get deals from my cousin out in "
	 "Jerseyton.",
	 28,0},
	 // 49 everything done, now black market shop
	{"Take what you like, it's all guaranteed at least to be magical.  No touching the merchandise "
	 "though.  And by guaranteed, I mean NO REFUNDS.",
	 29,0},
	 // BOB'S WIFE MELODY
	 // 50 intro, assign wife quest 2
	{"Help!  I've been kidnapped by that disgusting troll!  Please, find him and get the key from "
	 "him.  I'm really rather tired of sitting in these chains.",
	 30,0},
	 // 51 wife quest 2 not done
	{"Little help?",
	 0,0},
	 // 52 wife quest 2 done
	{"Oh thank you so much!  Be sure to go see my husband Bob.  I know he'll reward you "
	 "for your help.",
	 31,0},
	 // SHOPKEEPER STUFF
	 // 53 already been sharpened
	{"That's already as sharp as it's gonna get!",
	 0,0},
	 // 54 can't afford it
	{"You don't have enough money!",
	 0,0},
	 // 55 sharpened it
	{"There, it's all sharpened up!  Thanks!",
	 0,0},
	 // 56 items must be magic
	{"I can only smith Magic items, not normal or Golden ones!",
	 0,0},
	 // 57 first item chosen
	{"Okay, pick another item to merge it with!  The cost is displayed above.",
	 0,0},
	 // 58 items are smithed!
	{"Voila!  Your items are now one!",
	 0,0},
	  // 59 trying to smith an item with itself
	{"Umm... that's one item, two times.",
	 0,0},
	// MICKEY
	// 60 intro + rat quest
	{"Oh, I see you've chosen to barge into my house uninvited!  How quaint.  Well, as long as "
	 "you're here, how about some help?",
	 0,61},
	 // 61
	{"My cellar has been overrun with those horrible mechanical rats.  If you can clear up the "
	 "infestation, I'll pay you $50.  Deal?",
	 20,0},
	 // 62 quest unfinished
	{"Oh I see, you tromp around my house with muddy boots and DON'T solve my rat problem. "
	 "You are just the best houseguest ever! That's sarcasm, by the way.",
	 0,0},
	 // 63 complete quest
	{"Great! I can finally stop worrying about those stupid mice and maybe put some effort into "
	 "getting some decent locks on my doors. Did you catch the subtle hint?",
	 21,0},
	 // 64 both quests done
	{"WHY are you back?!  Go away!  Is personal space a complicated concept for you?",
	 0,0},
	 // SHOPKEEPERS AGAIN
	 // 65 don't want it
	{"Sorry, I can't buy that from you.",
	 0,0},
	 // 66 thanks for buying/selling
	{"Thank you very much!",
	 0,0},
	 // 67 no room to hold it
	{"You don't have room to carry that!",
	 0,0},
	 // GREAT GURU
	 // 68 intro + quest
	{"Peace and enlightenment be upon you, my child.  Speaking of which, wanna play hide & seek? "
	 "I have the ability to imbue you with Talents, so how about each time",
	 0,69},
	 // 69
	{"you find me, I'll grant you a new Talent.  Technically, you just found me once, so I'll give you your "
	 "first Talent to get started.",
	 0,70},
	 // 70
	{"Talents are a lot like Skills.  But unlike Skills, you can't level them up. They just get "
	 "better as you use them.  Okay, so close your eyes and count to infinity.",
	 24,0},
	 // 71 found him
	{"Ah, you got me!  I thought this would be the perfect place.  Okay, have another Talent. "
	 "I'm gonna go find a better place to hide.  Don't watch which way I teleport!",
	 25,0},
	 // 72 alt found him
	{"Oh no!  You're too good at this!  Okay, I owe you a Talent.  Enjoy it, because you "
	 "definitely won't find me where I'm going next!",
	 25,0},
	 // 73 alt found him
	{"What?  I just got here!  Are you cheating?  I guess unless I can prove it, I just have "
	 "to give you a Talent.  But I've got my eye on you.  I'm off to another spot!",
	 25,0},
	 // 74 alt found him
	{"Good seeking!  I think I need to work on my hiding.  Here, have another Talent while I "
	 "shuffle off to ... nevermind where!",
	 25,0},
	 // 75 alt found him
	{"You found me again!  What does a guy have to do to stay hidden around here?  Alright, "
	 "one Talent as promised.  Now turn around so I can hide again.",
	 25,0},
	 // 76 final found
	{"Phew... okay okay!  I give up.  You've found me thirty times, and I've only got thirty "
	 "Talents in my brain.  So you win.  Catch you in the eighth dimension!",
	 26,0},
	 // MORE MELODY
	// 77 met her before quest assigned
	{"Excuse me, I'm a little tied up at the moment.  Could we chat later?",
	 0,0},
	 // 78 unchained, just relaxing at home
	{"Yeah, you unchained me.  Whee.  What do you want, a freakin' medal?",
	 0,0},
	 // TINA TSARINA
	// 79 intro
	{"Hail and well met, puny man-boy!  I am called Tina Tsarina, and this is Tina's Arena. "
	 "Come ye to struggle and strive for great victory in pure combat?",
	 32,0},
	 // TAYLOR
	 // 80 intro + quest
	{"Eh? What?  Who's there??  Oh, a customer!  Afore you peruse my fine wares, would you "
	 "care to hear an old man out?",
	 0,81},
	 // 81 intro 2
	{"I have invented an incredible new piece of clothing - Asbestos Underwear!  But alas, "
	 "I do not have the materials I need to make it.",
	 0,82},
	 // 82 intro 3
	{"If you can bring them to me, I'll give you the underwear for free.  I just want a "
	 "chance to make them, just once, before my eyes fail me.",
	 0,83},
	 // 83 intro 4
	{"All I need are 5 Asbestos herbs and 5 Socks (for the elastic, you see).  In the "
	 "meantime, though, try on a parka or two!",
	 33,0},
	 // 84 shop
	{"Oh, just resting my eyes... Hello there, ma'am or sir!  Please try our lovely winter collection!"
	 "",
	 0,0},
	 // 85 quest done
	{"What?  You've brought the stuff!  Thank you so much!  Now just give me a minute, "
	 "and I'll have your undies all sewn up.",
	 0,86},
	 // 86 quest done 2
	{"... there!  I'm still quick with a needl... oh.  Lemme just unstitch it from my "
	 "shirt real quick.  Okay, these should keep you safe from any steaming hot geysers you",
	 0,87},
	 // 87 quest done 3
	{"you might come across.  Good luck!  Oh, "
	 "and don't mind the itch.  Asbestos does that.",
	 34,0},
	 // HANDYMAN HANK
	 // 88 intro+quest
	{"Help!  This tunnel leads to the Halls Of History, but apparently it's caved in. "
	 "I'm a handyman, and I could get this cleared up in a jif... but I don't have my tools.",
	 0,89},
	 // 89 intro 2
	{"See, I used to live in Baskerville, but it got overrun by those ice monsters, and we "
	 "all had to get out.  I was in such a hurry, I left them behind!",
	 0,90},
	 // 90 intro 3
	{"If you can go back there and get them, maybe I can get this opened up before the "
	 "Historian runs out of air!",
	 0,91},
	 // 91 intro 4
	{"Baskerville is to the east, across the icy lake.  Stay away from "
	 "the Ice Hounds!  They're nasty.",
	 35,0},
	 // 92 quest undone
	{"Hurry up, man!  The Historian is still in there!",
	 0,0},
	 // 93 quest done
	{"Alright!  Now this should just take a second, then we can get in there.  Oh, you "
	 "brought my shovel too.  I don't need that, you take it.",
	 36,0},
	 // 94 assign next quest, bears
	{"Now that we've opened up the halls again, it occurs to me that the sudden influx of "
	 "killer toys could pose a threat to our historical volumes!",
	 0,135},
	 // PHILEAS
	 // 95 intro+quest
	{"Hey there!  How would you like to take a ride in my balloon?  Actually, I'd love "
	 "to myself, but unfortunately it got damaged in the latest snowstorm.",
	 0,96},
	 // 96
	{"If you can find 3 ropes for me, I can make it airworthy again, "
	 "and I'd be happy to transport you wherever you need.  For a small fee.",
	 37,0},
	 // 97 quest unfinished
	{"I know, it's a pretty weak ballooning service without a working balloon.  Found "
	 "any ropes?",
	 0,0},
	 // 98 got the ropes
	{"Ah, great!  Gimme a minute to tie some of the more important pieces into place, "
	 "and I'll be ready to fly you around!",
	 38,0},
	 // 99 ballooning shop active
	{"Okay, choose a destination!  You'll have to describe where we're going exactly, "
	 "so I can only take you places you've been before.  Just $100 a ride!",
	 0,0},
	 // HISTORIAN
	 // 100 intro+quest
	{"Oh heavens!  Oxygen!  Very nice, indeed.  I thank you and Hank for the swift "
	 "rescue.  But I've had time to go over the old books while I was waiting and I'm afraid Hank's",
	 0,101},
	 // 101
	{"tale of how his town was overrun by Ice Hounds is a very bad omen.  Let me share some "
	 "history...  Thousands of years ago, the land was covered in ice.",
	 0,102},
	 // 102
	{"People lived in caves, laboring as slaves for the Ice Titans.  This tyranny lasted for "
	 "centuries, until a great group of heroes discovered the weakness of the Titans.",
	 0,103},
	 // 103
	{"In retrospect, it should've been obvious: They don't like heat.  So, in secret, "
	 "the heroes constructed The Great Furnace, deep beneath the ground.",
	 0,104},
	 // 104
	{"When they turned it on, the Titans were driven away.  None of them could get close "
	 "enough to shut it down, and they climbed deeper into the earth, leaving",
	 0,105},
	 // 105
	{"the people free to live again - in a much warmer world!  But now I fear the Titans "
	 "are returning.  The books call the Ice Hounds the harbingers of the Titans' return.",
	 0,106},
	 // 106
	{"And it has been getting much colder.  Could the Furnace have really failed?  We don't "
	 "even know where it is anymore, and the books are no help.  Frankly, I had sort",
	 0,107},
	 // 107
	{"of thought it was all a myth! You can help me - if you can collect samples from the Ice Hounds, I may be able to "
	 "learn more.  Will you do it?",
	 39,0},
	 // 108 phileas shop stuff
	{"I'll save you some money by pointing out that you're already there."
	 "",
	 0,0},
	 // 109 phileas shop stuff
	{"You haven't been to \"???\", so I can't take you there.  What an odd name for a place!"
	 "",
	 0,0},
	 // 110 quest not done
	{"If I just had a few Ice Cubes to examine, I might know more.  And I'll give you these "
	 "lovely ice skates as a prize!",
	 0,0},
	 // 111 got enough cubes
	{"Excellent!  Give me a moment to analyze these.  Here, go play with these ice skates "
	 "while I work.",
	 40,0},
	 // 112 quests are done, results + assign next quest
	{"Yes, it is as I feared.  The Ice Titans must be returning!  We (by which I mean you) "
	 "will need to find the Furnace and see what's wrong.",
	 0,113},
	 // 113
	{"But the underworld is terribly dangerous.  First, you'll need to find some allies up "
	 "here to have a chance!",
	 41,0},
	 // 114 ally quest assigned
	{"Have you found anybody to help?  I'm an old man, I don't count.",
	 0,0},
	 // 115 ally quest done
	{"Good work!  Now you may just survive the underworld!  Take your allies down there and go fix "
	 "that Furnace!",
	 96,0},
	 // ROPEWIFE
	 // 116 intro+quest
	{"Oh, are you looking to buy some ropes?  Unfortunately, my husband, who could sell them "
	 "to you, has vanished!  He went out to get more ropemaking supplies, and he",
	 0,117},
	 // 117
	{"hasn't returned!  I'm getting very worried.  He usually heads out east, toward "
	 "Glacial Gorge.",
	 42,0},
	 // 118 quest unfinished
	{"You still haven't seen him?  Oh, where could he be??",
	 0,0},
	 // 119 have found him
	{"Thank you so much for rescuing my husband!  He's always been a bit of a klutz, but an "
	 "avalanche is truly an epic feat of clumsiness.",
	 0,120},
	 // 120 now adding a new quest
	{"Now, I hate to bother you further, but if you're interested, I could really use 10 "
	 "Tin Cans, to store my special recipe of soup.",
	 0,121},
	 // 121
	{"I'll give you a cup of it if you bring me some!  It doesn't sound like much of a trade, "
	 "I know, but this is VERY special soup.",
	 43,0},
	 // 122 cans not collected
	{"Come on, didn't I mention how special the soup is?",
	 0,0},
	 // 123 cans collected
	{"Great!  Just when my latest batch had finished too.  Here, try a sip.  Doesn't that just "
	 "make you feel like you can take on a thousand evil toys?",
	 44,0},
	 // 124 quests all done
	{"Thanks for all your help.  That soup's really got some kick, huh?  You want more soup? "
	 "No soup for you!",
	 0,0},
	 // ROPEMAKER
	 // 125 thanks for rescue
	{"Whooo, that was brisk, baby!  Thank you kindly for digging me out of that oddly localized "
	 "avalanche.  That'll teach me to yodel while I work.",
	 45,0},
	 // 126 assign factory quest
	{"Hey, have you by any chance noticed the overwhelming hordes of psychotic killer toys "
	 "terrorizing this entire area?  Yeah, me too. On my latest supply run, I found",
	 0,127},
	 // 127
	{"where they were coming from.  Barely got out with all my limbs!  There's a "
	 "factory, just south of Limpid Lake.  You'll need one of my handy Ropes to get there, not to",
	 0,128},
	 // 128
	{"mention some Ice Skates.  That is, if you wanted to get there. But "
	 "the people of Winter Woods would sure be grateful!",
	 46,0},
	 // 129 rope shop
	{"Finest ropes north of Roquefort River!  Just $50 each.  They're especially handy for hot "
	 "air balloons, and making bridges between conveniently-placed stumps.",
	 0,0},
	 // BACK TO HISTORIAN
	 // 130 ally quest done, giving skill quest
	{"For some reason, there are Skill Scrolls scattered all around this land.  Now, while that "
	 "makes no sense, I'd really like to catalog them.",
	 0,131},
	 // 131
	{"All you need to do is find all 50 Skill Scrolls that the prophecies have foretold.  I don't "
	 "have much I can offer in return, but I'll think of something.",
	 47,0},
	 // 132 skill quest unfinished
	{"Oh, let me jot down the scrolls you've found so far...",
	 0,0},
	 // 133 got all skills
	{"Marvelous!  This will make a great article for Prophecies & Portents!  Here's a reward for "
	 "you: five Skill Points!",
	 48,0},
	 // 134 quests done
	{"I can't wait to see what the editors think of my article.  I'm going to make Historian "
	 "history!",
	 0,0},
	 // HANK AGAIN
	 // 135 bear quest
	{"We can't just bury it again, so how about you do some cleanup work?  Could you get rid of "
	 "30 Cave Bears?  That should make it safer.",
	 49,0},
	 // 136 bear quest undone
	{"Keep smashing those bears!  I think you're making a difference.  Maybe.",
	 0,0},
	 // 137 bear quest finished
	{"Great job!  I feel safer already.  I mean the documents do.  Here, have a potion!",
	 50,0},
	 // 138 assign big bear
	{"Well, now that you've gotten rid of those bears, things are a lot safer.  Or they would "
	 "be if Berserker Bear hadn't shown up!  I saw him in a chamber to the northeast.",
	 0,139},
	 // 139
	{"Hard to miss, he's bright pink.  But watch out, he's "
	 "FAST!",
	 51,0},
	 // 140 big bear quest undone
	{"Did you find him yet?  Did I mention he's pink?  Pink!  Like some kind of psycho care bear.",
	 0,0},
	 // 141 all quests done
	{"Well, looks like there are just as many toys around here as ever.  Noble effort, at least! "
	 "I'll stand guard here until things get better.",
	 0,0},
	 // TINA AGAIN
	 // 142 assign rank quest
	{"Hmm, you show promise in the arena, man-child!  Show me more skill, and I will bestow unto "
	 "ye the most ancient of gladiatorial secrets.",
	 52,0},
	 // 143 rank quest done
	{"Well-met, young warrior!  Your prowess is as vast as the thirteen seas.  For such a show of "
	 "might, I grant you this boon.",
	 53,0},
	 // 144 assign champion quest
	{"You have shown might and fury unmatched by most.  But there is yet one who scoffs at all you "
	 "have achieved.  Dare ye confront... KILLBOR?",
	 54,0},
	 // 145 championship achieved
	{"I bow before you, great Loony... KING OF THE ARENA.  HAIL!  HAIL!  HAIL!",
	 55,0},
	 // HEALIANA AGAIN
	 // 146 assign letter quest
	{"Hey, since you're out there adventuring all the time, and I'm just standing here, could you "
	 "deliver something for me?  It's a letter to my sister Curaga.",
	 0,147},
	 // 147
	{"She lives in a cave up in the Frosty Cliffs.  Thanks!",
	 56,0},
	 // CEO
	 // 148 intro+quest
	{"Oh thank heavens!  You've shut down the factory at last.  The shareholders will go nuts, but "
	 "it's for the best.  You see, Not-Evil Toys, Inc. was once a respected name",
	 0,149},
	 // 149
	{"in toys that won't kill you.  Then the Onion Ring came along and took over our factory, "
	 "locking me up here in my office.  They even took all my furniture!",
	 0, 150},
	 // 150
	{"They switched all the toys to Evil Mode - which voids the warranty, incidentally - and set the "
	 "factory into overdrive. Now you've almost foiled their scheme!",
	 0,151},
	 // 151
	{"If you don't destroy the factory's Power Core, the Onion thugs will just come back and repair "
	 "the Toymotrons.  That was an extremely expensive Power Core, but",
	 0, 152},
	 // 152
	{"despite Marketing's insistence that there's no such thing as bad publicity, I just have a gut "
	 "feeling that we don't want to be known as the manufacturers of psychotic",
	 0,153},
	 // 153
	{"killing machines.  Now hurry!  When you hit the emergency shut-down button, the "
	  "basement elevator was unlocked.  Go down there and finish this!",
	 57,0},
	 //154 quest not done
	{"Hurry!  The gangsters will be back any time!",
	 0,0},
	 // 155 quest done
	{"Thank you for all your help.  I'm just going to sit here and plan out the return of the Toymonger empire!",
	 0,0},
	 // MICKEY AGAIN
	 // 156 toy quest
	{"It's getting totally brutal out there lately.  Have you seen all those evil toys?  I mean, "
	 "it's getting so a body can't take a relaxing walk.  Or, come to think of it,",
	 0, 157},
	 // 157
	{"even relax in his own home unmolested by intruders!  I hope you caught "
	 "that subtle reference to your behavior.  But anyway, here's the deal:",
	 0, 158},
	 // 158
	{"If you can do something about the evil toy problem around here, I'll give you my life's savings. "
	 "$500!  So?  I'm not getting any younger!  Move!",
	 58,0},
	 // 159 quest undone
	{"What am I paying you for?!",
	 0,0},
	 // 160 killed enough
	{"Alright, I guess you did what you could.  Now take the money and stay out of my house from now on!",
	 59,0},
	 // CURAGA
	 // 161 intro + devil grass quest
	{"Whoooaaa little dude!  Okay, seriously.  I gotta get me some Devil Grass.  Can you hook me up?",
	 60,0},
	 // 162 devil grass undone
	{"Aww... c'mon, dude.",
	 9,0},
	 // 163 devil grass done
	{"Excellent!  You are like my savior, kid.  I mean like for real.  I'm being totally cereal.",
	 61,0},
	 // 164 brought her the letter
	{"Hey man.  Oh, a letter from my sister?  Cool.  Let's see... aww man, she's always on my case! "
	 "She doesn't like me living up here.  What a chicken.  So you brought this",
	 0,165},
	 // 165
	{"all the way up from town on foot?  That must've built up some muscles!  Lookin' good, dude.",
	 62,0},
	 // 166 assign package quest
	{"Hey, can you help me out one more time?  I ordered some stuff from The Magic Box, but with "
	 "all the whacked out toys, they're not delivering anymore.",
	 0, 167},
	 // 167
	{"Can you go pick up my package?  It's like real important.  You'd be like my hero and stuff. "
	 "Right on!",
	 63,0},
	 // 168 package undone
	{"The Magic Box is a roadside stand by the Haunted Highway, you know, just west of Westwood?",
	 9,0},
	 // 169 package done
	{"Sweet!  Man, you did so much stuff for me, I'm gonna teach you something better than a spell. "
	 "Here's how to make all your spells better!",
	 64,0},
	 // 170 everything done
	{"How's it going, man?  I'm just chillin' in my cave.",
	 9,0},
	 // THE ONION
	 // 171 beat power core, make choice
	{"MUAHAHAHAHA!  It is I, the Diabolical Onion!  It seems that you have been causing quite a stir "
	 "around here.  First, you take out Klonk, one of my top guys on the",
	 0,172},
	 // 172
	{"South side, and now you come in here and shut down my little toy operation.  I must confess to being a bit "
	 "flabbergasted.  After much diabolical plotting, I have",
	 0,173},
	 // 173
	{"come up with what I believe is a solution to this little dilemma.  You show great promise, my boy.  Not just anybody could "
	 "have pulled off the jobs you did.",
	 0,174},
	 // 174
	{"So, you've got a choice: you come work for me as a member of The Onion Ring... or the big guys here show you what they can do. "
	 "What'll it be?  MUAHAHAHAHA!",
	 65,0},
	 // 175 chose to join him
	{"You made the right choice, kiddo.  We're heading off to our camp in Westwood.  Meet us there, and "
	 "we'll get started with the initiation!  Here's a key to get in.",
	 66,0},
	 // 176 chose to get beaten up
	{"Well, it's disappointing... but this is gonna hurt you more than it hurts me!  MUAHAHAHAHA!",
	 67,0},
	 // 177 waiting for you to get moving
	{"Get going!  Just head south into the woods, then west, you can't miss it.  I got my eye on you...",
	 0,0},
	 // 178 at HQ, assign quest-doing quest
	{"Welcome aboard!  MUAHAHAHA!  Now, before I can trust you with serious jobs, we need to do a "
	 "loyalty test.  Complete any 3 missions and come see me again.",
	 68,0},
	 // 179 loyalty test not done
	{"This is how you show your loyalty?  You're useless!",
	 0,0},
	 // 180 loyalty test done, assign slay quest
	{"Excellent!  Looks like we can trust you just fine.  You're a true member of The Onion Ring.  So I've "
	 "got a real big job for you.  Those twerps from The Order Of The",
	 0,181},
	 // 181
	{"Snuggly Bunny have been harassing us ever since we moved into this turf.  What you need to do is simple: take out "
	 "their leader.  He's bound to be hiding out in their",
	 0,182},
	 // 182
	{"camp up in the Frosty Cliffs.  Here's a key we made that will get you in.  Find him, and take him down!  MUAHAHAHAHA!",
	69,0},
	 // 183 haven't slain
	{"What are you waiting for?  Do my diabolical bidding!!  MUAHAHAHAHA!",
	 69,0},
	 // 184 have slain
	{"Nice work, kiddo!  With the Bunnies in disarray, we'll be free to be as diabolical as necessary. MUAHAHAHA! "
	 "I'll let you know if I have any more jobs for you.",
	 0,0},
	 // JUNKSMITH
	 // 185 intro + quest
	{"Meh.  What's your problem, Cleany McGee?  Never smelled a working man before?  This here junkyard ain't "
	 "for little preppy boys like you.  Here's a deal:",
	 0,186},
	 // 186
	{"You bring me 50 pieces of junk - you know, the useless stuff that monsters drop, like Logs and "
	 "Gears? - and I'll let you explore the wonders of my junkpile.",
	 70,0},
	 // 187 quest not done
	{"I told you this work was too much for you!",
	 0,0},
	 // 188 got enough
	{"Well, that's something!  It's not quite the caliber of junk I like to collect, but I honor my deals.  Enter and "
	 "be amazed by the greatest pile of junk in all of Winter Woods!",
	 71,0},
	 // 189 quest done
	{"Some pretty nice junk, right?  Well, it's mine!  You can't have it!",
	 0,0},
	 // FRANKIE BEANS
	 // 190 intro + quest1
	{"A pleasure to meet you, I'm sure.  They call me Frankie Beans, on account of I grow beans during the crime off-"
	 "season.  I guess it's not the most creative nickname.",
	 0,191},
	 // 191
	{"Speaking of crime, have I got a job for you!  It'll be like taking candy from a baby...",
	 72,0},
	 // 192 candy untaken
	{"Whatsa matter, baby too tough for you?",
	 0,0},
	 // 193 candy taken
	{"Nice work, I guess.  I mean, I suppose it's not really impressive.  But you did do it.  So that's good.  Come back "
	 "later, I might have something a little rougher.",
	 73,0},
	 // 194 quest2
	{"Back for more, eh?  Well, here's the deal: as you may know, the Order Of The Snuggly Bunny is always on our back for "
	 "some reason.  So we've arranged to settle",
	 0,195},
	 // 195
	{"things!  Head up to the clearing in Northwood.  We're having a big rumble with them.  We'll show them how The Onion Ring rolls!",
	 74,0},
	 // 196 rumble undone
	{"Get going, man!  You wanna miss the fight?",
	 0,0},
	 // 197 rumble won
	{"Great work, pal.  Of course, I did most of the work, but you showed up, and that's not bad.  You keep it up, you'll "
	 "be a made man in no time!",
	 75,0},
	 // 198
	{"Onion Ring forever!",
	 0,0},
	 // CRUSH
	 // 199 intro+quest
	{"Crush smash you!  You friend?  Okay, no smash.  Want to smash something?  You go smash caravans on highway, steal "
	 "stuff.  Crush reward you with cut of money!",
	 76,0},
	 // 200 quest not done
	{"Crush smash you if you not bring stolen goods!",
	 0,0},
	 // 201 quest done
	{"Crush pleased!  Here you go, Crush give eleventy-seven percent to you.  No ask for more!",
	 77,0},
	 // 202 all done
	{"Crush already give cut!  Now Crush have headache from doing math.  Don't make Crush smash you.",
	 0,0},
	 // JIMMY TWO-FINGERS
	 //203 intro+quest
	{"Yo, I'm Jimmy Two-Fingers.  They call me that cause I ain't such a good typer.  I gots a job for yous.  See, "
	 "we provides a service to the people of Winter Woods,",
	 0,204},
	 // 204
	{"making sure no 'accidents' happen at their places of business.  In exchange for this useful service, we charge "
	 "a nominal fee.  It would be a great help if you could",
	 0, 205},
	 // 205
	{"assist me by collecting the aforementioned fee.  Axes R Us, Taylor's Knits, The Magic Box, and Ropes N' Ropes "
	 "all need to pay up.  Be a pal!",
	 78,0},
	 // 206 not done
	{"What, you got lead in your boots?  Get rolling!",
	 0,0},
	 // 207 done, but don't have the money
	{"You're done?  Well, where's the cash?  Four stores, $1000 each, I can do the math.  Where's MY MONEY!?  You "
	 "holdin' out on me?",
	 0,0},
	 // 208 done
	{"Looks like it's all here... I appreciate the assistance.",
	 79,0},
	 // 209 all done
	{"Let me be, I'm trying to type out a letter, and it's gonna take a while.",
	 0,0},
	 // SMASH
	 // 210 intro+quest
	{"Smash crush you!  You friend?  Okay, no crush.  Want to go crush something?  Boss said me go crush Snuggly Bunny "
	 "supply depot in Frosty Cliffs, but me tired.",
	 0,211},
	 // 211
	{"You go crush supplies?  Me nap.",
	 80,0},
	 // 212 quest not done
	{"What?  No wake up Smash!!  You not done doing Smash's job!",
	 0,0},
	 // 213 quest done
	{"Good crushing.  Here, take Skill Scroll.  Smash not read good.  Now Smash crush you if you interrupt nap again.",
	 81,0},
	 // 214 all done
	{"Zzzzz crush crush crush zzzz",
	 0,0},
	 // GUY MAGIC
	 // 215 normal shopping
	{"Welcome to my roadside magic stand!  I've got all your magical needs, even enchanted glasses!",
	 0,0},
	 // 216 package for curaga
	{"Oh, you've come to pick up Curaga's package?  Great!  I'm sorry we don't deliver anymore, but you know how things "
	 "are lately... it's scary out there!",
	 82,0},
	 // 217 AXES R US shakedown
	{"Protection money?  What about when I was robbed just last week?!  Okay, okay, put down the axe.  Here you go.",
	 83,0},
	 // 218 TAYLOR shakedown
	{"You want what now?  I can't quite hear you, but if you're collecting for charity, go ahead "
	 "and dig it out of my cashbox.  I can't really see the money so well.",
	 84,0},
	 // 219 MAGIC GUY shakedown
	{"I oughtta turn you into a frog, you low-down dirty.... ah, just take it!",
	 85,0},
	 // 220 ROPES shakedown
	{"I'm only giving you this money as a reward for rescuing me.  But I am very disappointed at the criminal turn you've taken.  Shame!",
	 86,0},
	 // 221 take candy from Bobby
	{"WAAAHHH!!  Oh, you want my lollipop?  Sure, I've got an extra.  How are you doing on those presents?",
	 87,0},
	 // CAPTAIN CAPITAN
	 // 222 rescuing you
	{"Tada!  It is I, Captain Capitan, leader of The Order Of The Snuggly Bunny, come to your rescue!  What?  You haven't heard of "
	 "me?  Oh, come on!  Where've you been,",
	 0,223},
	 // 223
	{"Halloween Hill?  The Order was commissioned by the King himself, to protect the people.  Sadly, he let young Princess Peapod name "
	 "our order... sigh.",
	 0,224},
	 // 224
	{"Nonetheless, we battle on against injustice, tyranny, and malfeasance!  Huzzah!  We could use a recruit like you. "
	 "We saw what you did at the toy factory,",
	 0,225},
	 // 225
	{"and we offer membership into our hallowed ranks. Come see me at our headquarters. It's in a cave up in the cliffs. I've left you a key just outside.",
	 88,0},
	 // 226 waiting for you to exit prison
	{"Get a move on!  I'll hold off these scoundrels!  Well, I will if they open the door.  For now, I shall taunt them!  Huzzah, fiends! "
	 "You are foul-smelling!",
	 0,0},
	 // 227 assign rank quest
	{"Aha!  Welcome aboard.  As a member of the Order, you'll gain ranks by completing missions.  Come back and see me when you have "
	 "completed 3 missions!",
	 68,0},
	 // 228 rank quest unfinished
	{"Keep on working at it!  Soon enough, you'll be a Bunny hero like me!  You like our uniforms?  A lot of people think "
	 "they're Princess Peapod's design, but I actually",
	 0,229},
	 // 229
	{"designed them myself.  Quite fierce, don't you think?",
	 0,0},
	 // 230 rank quest done
	{"Excellent!  You have demonstrated great ability.  I will teach you the Tornado spell as a reward.  Now, I have a more advanced "
	 "quest for you.",
	 0,231},
	 // 231
	{"Our purpose here in Winter Woods is to put a stop to the nefarious deeds of the dreaded Onion Ring.  Their leader "
	 "is of course the notorious Diabolical Onion,",
	 0,232},
	 // 232
	{"wanted for crimes all across Loonyland. Take this key and proceed to the Onion Ring Camp in Westwood, and defeat the Diabolical Onion!"
	 "",
	 89,0},
	 // 233 leader quest undone
	{"Don't be afraid, recruit!  If you lose the fight, you can always level up and try again!",
	89,0},
	// CLOCKMAKER
	// 234 assign clocks quest
	{"Oh, please do help!  I'm in charge of making the Clockwork Robots that the Order relies upon for heavy combat.  Lately, I've "
	 "been experimenting with their",
	 0,235},
	// 235
	{"control mechanisms, and, well... the lab is no longer safe!  If you would be so kind, please destroy the prototype robots before they ruin the entire lab!",
	70,0},
	// 236 quest undone
	{"Please hurry!  The robots are smashing all my delicate clockworks!",
	0,0},
	// 237 quest done
	{"Oh thank you!  I'll scratch that particular configuration off of my list, then.  Now what if I tried a tin can right in here...",
	90,0},
	// 238 all done
	{"If you ever want to experiment with your own clockworks, feel free to use my lab.  Anytime!",
	 0,0},
	 // 239 you are his enemy
	{"Back, you scoundrel!  I'll sic my Clockwork Robots on you!",
	 0,0},
	 // MAJOR FISHBUG
	 // 240 assign supplies quest
	{"Harrumph.  Don't you just love our costumes?  The captain's design.  Lovely.  Here's a job for you: The Onion Ring has "
	 "been keeping supplies for their criminal",
	 0, 241},
	// 241
	{"activities buried all around Westwood.  They have them very well hidden.  All you need to do is dig up 20 of them and destroy them, "
	 "to disrupt their operations.",
	 80,0},
	 // 242 supplies undone
	{"I'm an old man sitting here in a bunny suit.  You think I'm happy?  Find those supplies!",
	 0,0},
	// 243 assign rumble
	{"Looks like your supply disruption really upset the Diabolical Onion!  He's gathered a small army of his men in a clearing "
	 "just south of Northwood.  I'm forming a squad",
	 0,244},
	 // 244
	{"to take them on.  Are you in?",
	74,0},
	 // 245 Rumble unfinished
	{"Meet up with us in the clearing.  We'll show 'em how tough men in bunny suits can be!",
	 0,0},
	 // 246 rumble done
	{"Great work out there on the battlefield.  You can count on me as an ally whenever your back is to the wall!",
	75,0},
	 // 247 all done
	{"Keep fighting the good fight!",
	 0,0},
	 // CLOCKWORK AGAIN
	 // 248 assign miner quest
	{"Sigh... the higher-ups simply don't understand the importance of my research!  I'm so underfunded I can barely even produce "
	 "basic robots.",
	 0,249},
	 // 249
	{"Perhaps you can help?  There are certain exotic components that they just don't see fit to supply me with.  Can you dig up "
	 "a Diamond?  I'll teach you a spell in return.",
	 72,0},
	 // 250 not done
	{"Oh, I know diamonds are very hard to come by... Please, keep on digging!",
	 0,0},
	 // 251 got it
	{"Amazing!  You actually dug up a diamond.  Here, let me show you how to cast the Stone Spike spell before I try integrating "
	 "this into my latest design...",
	 91,0},
	 // HORNSWAGGLE
	 // 252 assign highway patrol
	{"Hey, a rookie!  Listen up, rookie.  I'm supposed to be heading over to the Haunted Highway to protect the caravans from bandits. "
	 "But I'm lazy.  So you do it.",
	 76,0},
	 // 253 not done
	{"What do I look like, some kind of guy who's not lazy?  Go do my work!",
	 0,0},
	 // 254 done
	{"Ah, very nice.  Now with my work all done, I can lazily lounge about.",
	 92,0},
	 // 255 all done
	{"Don't bother me, I'm trying to laze.  It's not easy to do standing up, but I find that leaning on my poleaxe helps.",
	 0,0},
	 // PUBLIC
	 //256 assign shutdown
	{"PRIVATE PUBLIC REPORTING FOR DUTY, SIR!  SIR YES SIR!  Wait a minute, you're no higher ranked than I am!  Care to join me on "
	 "a mission?",
	 0,257},
	 // 257
	{"We've discovered some illegal axe dealers operating out of a cave in these very cliffs.  Our job is to go in and shut down their "
	 "operation!  Think you're up to it?",
	 78,0},
	 // 258 not done
	{"They seem to be operating out of a cave near the lower end of the cliffs.  Check it out.",
	 0,0},
	 // 259 did it
	{"Nice work!  What do you mean, I wasn't there?  I was with you in spirit!  We were like a team!  GO TEAM!",
	 93,0},
	 // 260 all done
	{"Keep on moving, citizen.  I'm standing guard!",
	 0,0},
	 // ONION AGAIN
	 // 261
	{"MUAHAHAHA!!  So they're tying up loose ends, are they?  Boy, you have no idea what you've walked into here.  Let me just "
	 "tell you before I squish you -",
	 0,262},
	 // 262
	{"we're just subcontractors.  Who do you think hired us?  Who wanted an evil toy epidemic to "
	 "force the King to send the Order here?",
	 0, 263},
	 // 263
	{"Who wanted that Furnace disabled?  I'll TELL you wh-"
	 "",
	 94,0},
	 // 264 (now Capitan)
	{"Huzzah!!  Take that, fiend!  Just thought I'd drop in and lend a hand!  That Onion is so diabolical, I'm sure he tried "
	 "to fill your head with terrible lies.",
	 0, 265},
	 // 265
	{"Nevermind him!  Conspiracies...  Pshaw!  I'm sure this is just global cooling we're experiencing. "
	 "Nothing to worry about.  Now I'll stick around and write up a report.",
	 0, 266},
	 // 266
	{"Great work!  See you back at HQ.  Definitely don't bother looking for "
	 "the Furnace or any parts that might have been removed from it.",
	 0,267},
	 // 267
	{"The idea is preposterous.  Huzzah!",
	95,0},
	// 268 capitan sitting around in onion camp
	{"On your way, now!  Everything's handled here.  And remember not to look for the Furnace, it's surely a myth.  Huzzah!",
	0,0},
	// 269 onion just in case you see him before getting this quest somehow, but aren't on his side
	{"MUAHAHAHA!  Fear the Onion Ring!",
	0,0},
	// CAPITAN WHEN VICTIM OF ATTACK
	// 270
	{"So, the Onion sent you, did he?  Well, I suppose that was a smart move.  I actually was planning to dispose of him "
	 "to cover my tracks.",
	 0, 271},
	 // 271
	{"I guess I'll just have to start by disposing of you!  Huzzah!",
	 97,0},
	// 272 (now it's Fishbug)
	{"Not so fast, Capitan!  I can't believe this... The king suspected there was a traitor in our midst, but you?  Why, when "
	 "you're the most decorated Order Knight in our",
	 0,273},
	// 273
	{"history?  Not to mention the only one who likes the uniforms.  Well, you're about to lose that uniform! "
	 "I'm taking you into custody!",
	 0,0},
	 // 274 (Capitan has been beaten down to 50%, now escapes)
	{"Augh!  You are fools!  When the Baron is ruling this land, I shall be at his right hand - I shall be... The DOOM BUNNY!! "
	 "See ya, suckers!",
	 98,0},
	 // 275 (now fishbug assigns a quest)
	{"Drat, he escaped!  I think I may be able to find something out about this Baron from my contacts.  This all centers "
	 "around the Furnace somehow.",
	 0, 276},
	 // 276
	{"Can you go to the underworld and find and repair the Furnace?  I'll try to meet you there with information on the next step.",
	 99,0},
	 // BETTY
	 // 277 book quest
	{"Oh, I just love the arena, don't you?  Nothing quite like intense mindless violence.  Oh, Killbor's next match is coming up! "
	 "He's my favorite...",
	 0,278},
	 // 278
	{"Oh, pickles!  I have to return this book I borrowed from The Halls Of History, but I don't want to miss the match.  Would you "
	 "mind?  The Halls are in Chilly Cavern,",
	 0,279},
	 // 279
	{"underneath the tundra.  The entrance is southwest of here.  Thank you so much!  I'll let you know how badly Killbor beats "
	 "his opponent.",
	 100,0},
	 // 280 quest not done
	{"SIC 'EM, KILLBOR!!!!",
	 0,0},
	 // 281 quest done
	{"Oh, you did it!  You missed an amazing match, though.  Killbor took on 100 Shock Monkeys and Bodzhas, and then "
	 "he signed my... nevermind.",
	 101,0},
	 // 282 all done
	{"Boy, that Killbor sure is dreamy...",
	 0,0},
	 // 283 all done, you have beaten Killbor!
	{"AHHHH!!!!!  It's YOU!!  The champ!  Oh would you please please sign these for me?  AHHHHHHHH!!!!!!!",
	 0,0},
	// 284 historian given book
	{"Oh my word, how dilligent!  You're returning a book despite the cave-in!  I applaud your forthrightness.",
	 102,0},
	 // OLD SHRUMFORD
	 // 285
	{"ungh...",
	 0,0},
	 // SCHRODINGER
	 // 286 intro and first furnace quest
	{"What?  A human!  I never thought I'd see one of those!  What is it you want?  Oh, the Furnace!  Yes, of course "
	 "we know about it.  Normally, we Shroomulans live deeper in",
	 0,287},
	 // 287
	{"the earth than this, but all of a sudden, the Ice Titans came storming up "
	 "from below.  We were forced to scatter all over the underworld.",
	 0, 288},
	 // 288
	{"We have been around since before the Furnace was built, so I went to check on it, and it appears to be sabotaged! "
	 "Three vital parts have been taken:",
	 0, 289},
	 // 289
	{"The Thing Of Majig, The Watch of Makalit, and the Thing Of Mabob.  I've managed to locate two of these, but I can't "
	 "get to them.  Can you?",
	 0,290},
	 // 290
	{"The Thing Of Majig is in the Crystal Caverns to the west.  It's behind a mystical barrier, though.  The only way to "
	 "break such a barrier is with a Sounding Crystal.",
	 0, 291},
	 // 291
	{"That's that yellow crystal there.  Too bad they're so fragile!  You'll have to carry it to the "
	 "barrier without getting hit!",
	 103,0},
	 // 292 furnace 1 undone
	{"It's trickier than it looks, huh?  I tried getting it there ten times, and I just couldn't do it.",
	 0,0},
	 // 293 furnace 1 done, assign furnace 2
	{"You got the Thing Of Majig!  Great!  Maybe you'll be able to get the Watch Of Makalit too, then.  It's to the north, "
	 "guarded by Gourdzilla.",
	 104,0},
	 // 294 furnace 2 undone
	{"There's some nasty vegetation down here.",
	 0,0},
	 // 295 furnace 2 done
	{"Incredible!  Well, now there's only one more lead I can think of.  Old Shrumford there whispered something to me that "
	 "sounded like Mabob.",
	 0,296},
	 // 296
	{"If we can rejuvenate him, he may have some information.  Even if it doesn't, come on... the guy needs help!  Perhaps I "
	 "should explain.",
	 0, 297},
	 // 297
	{"We Shroomulans were once people, like yourselves.  We were a tribe known as the ... well, I forget.  It was about 500 "
	 "years ago!  One day, an evil wizard cast a",
	 0,298},
	 // 298
	{"spell on us, turning us all into mushrooms!  It was some argument about the price of mushrooms, I think. "
	 "Because of this curse, we now actually live forever.",
	 0, 299},
	 // 299
	{"However, we need to transmigrate our souls into new mushrooms whenever our current body gets too "
	 "old.  That is what the Sacred Forest is for.",
	 0,300},
	 // 300
	{"We have been its caretakers for hundreds of years.  When we grow old, we make a pilgrimage into it to find the "
	 "perfect mushroom to inhabit.",
	 0, 301},
	 // 301
	{"Shrumford is long past due for his rejuvenation, but those nasty Bodzhas have invaded our sacred forest!  It's too dangerous "
	 "for us now!",
	 0,302},
	 // 302 finally assign the quest
	{"So, long story short(ish), we need a warrior to cleanse the forest of defiling beasts.  Then Shrumford can rejuvenate "
	 "and tell us what he was trying to say.",
	 105,0},
	 // 303 quest not done
	{"See, since we live forever as long as we don't get killed, we've grown to be a bit cautious.  Actually, we're pacifists.  So "
	 "go get those Bodzhas for us!",
	 0,0},
	 // 304 got them all!
	{"It's safe now?  I mean, undefiled?  Great!  We'll rush Shrumford out there and let him choose his new body.  Meet us there to "
	 "see what he knows!",
	 106,0},
	// 305 quests done, haven't visited Shrumford
	{"Come on, meet us in the forest to see the amazing transformation!",
	 0,0},
	 // 306 in the forest
	{"Cover your eyes, no outsider has ever been allowed to witness this sacred ritual.",
	 0,0},
	 // 307 forest after transform
	{"Voila!  A young Shroomulan!",
	 0,0},
	 // 308 back in town
	{"Thank you for all your help.  Go get those Titans!",
	 0,0},
	 // SHARONA
	 // 309 intro+bokbok
	{"Great googly moogly!  In the mad dash to escape from the lower tunnels, most of my Bokboks got scattered.  Can you bring "
	 "them back to me, human boy?",
	 112,0},
	 // 310 have no bokboks in tow
	{"They must be out there somewhere!  The poor little critters...",
	 0,0},
	 // 311 have some
	{"Oh thank you!  I hope you can find the rest of them!",
	 108,0},
	 // 312 got them all
	{"You're my hero, you strange misshapen freak!  Thank you so much!  It looks like one of the Bokboks had swallowed something... "
	 "You can, uh, have that.",
	 0,313},
	 // 313
	{"You might want to wash it off.",
	 109,0},
	 // 314 treehugger quest
	{"Here in the underworld, plants don't do so well.  It could be because we're underground.  Regardless, we need to protect our "
	 "environment!  I have a plan.",
	 0, 315},
	 // 315
	{"If you can bring me 20 of the seeds from those scary homicidal plants, I may be able to crossbreed them with nice normal plants "
	 "to make tougher ones.",
	 0,316},
	 // 316
	{"Do it, and I'll share with you the magic that made us what we are.  It was such a powerful spell that it rubbed off on us!",
	110,0},
	// 317 quest not done
	{"Let me know when you find 20 evil plant seeds!",
	0,0},
	// 318 got them
	{"Oh great!  Now I will teach you how to turn other things into Shroomulans... well, into little Shrooms anyway.  Use this power "
	 "wisely!  It's dangerous.",
	111,0},
	// 319 all done
	{"Thanks for your help.  I'm working on that plant breeding program.  So far all I've got are meaner plants.",
	0,0},
	// SHROUD
	// 320 shop
	{"Oh woe and sorrow!  The underworld is bleak like the craggy blackness within my very heart... you gonna buy something or what?",
	 0,0},
	 // SHRUMFORD in forest
	 // 321
	{"ungh... nice mushroom...",
	107,0},
	// young SHRUMFORD
	// 322 give item
	{"Wheee!!!  Young again!!  Wheeeeeeeee!!!  What I was trying to say when I was so elderly was that I had sat on this here thingamabob. "
	 "You can have it.  Wheee!!",
	 113,0},
	 // 323 waiting for you to do furnace
	{"Thanks for giving me the chance to recover my youth.  I just wanna run and play all day!  Wheeeee!!  Were you planning on fixing the furnace soon?",
	0,0},
	// 324 furnace done, so titan quest
	{"Great work with the furnace!  It's already warming up in here.  But you know, the Titans aren't gone yet.  We won't be truly safe "
	 "until you dispose of the Titanic Titans.",
	 0,325},
	 // 325
	{"They lurk slightly deeper underground, in the Titan Tunnels, where it's still cool enough for them.  Unfortunately, "
	 "that's where we normally live!  Help?",
	 114,0},
	 // 326 quest not done
	{"Please save us from the Titanic Titans!  I miss my old home with all my stuff... hey, where'd my bed go when I rejuvenated?",
	0,0},
	 // 327 did it
	{"You did it!  We're free from the titanic tyranny!  Wheee!!  I'm gonna run off home and see what they didn't smash.  Here, take this "
	 "key.  I won't need it down there!",
	 115,0},
	 // 328 all done
	{"These other guys say I have to wait for them before I can go...  wheee.",
	 0,0},
	 // SHARONA in forest
	 // 329 before transform
	{"Behold the miracle of nature!  Oh, but don't behold it.  Outsiders aren't allowed to see this.",
	 0,0},
	 // 330 after transform
	{"Shrumford's going to be a pretty unbearable tyke for the next few years...",
	 0,0},
	 // SHROUD in forest
	 // 331 before transform
	{"Though foul death creep in upon Shrumford, he rises again, thwarting its chains!  I'm bored.",
	 0,0},
	 // 332 after
	{"Hooray.  A little kid running around annoying me.  It's enough to make me want to go lick an Ice Titan.",
	 0,0},
	 // CAPTAIN in furnace
	 // 333
	{"What do you think you're doing?!  Get your grubby mitts off that furnace!  Baron Von Frostburn will kill "
	 "me if it's fixed!",
	 0,0},
	 // 334 beat him
	{"Stop hitting me! Okay, I surrender. Boy, you are just not a normal person!  Yes, Baron Von Frostburn "
	 "made me an offer I couldn't refuse.",
	 0,335},
	 // 335
	{"My mistake was subcontracting out to that stupid gang!  That, and expecting loyalty from my men!",
	 117,336},
	 // 336 (now Major Fishbug)
	{"Loyalty??  I shall always be loyal to the king, you traitor.  His highness sent me here because "
	 "he suspected you all along.",
	 0,337},
	 // 337
	{"Now get out of my sight!  Loony, the Baron's castle is far across Glacial Gorge in the northeast. "
	 "Let's finish this.",
	 116,0},
	 // 338 finish furnace quest
	{"Ah, sweet warmth!  That should keep those Ice Titans away from the upper reaches of the underworld. "
	 "Now go get Baron Von Frostburn!",
	 118,0},
	 // TOYMONGER in GORGE
	 // 339 reintro + quest
	{"Ah, you've made it!  I was working on restoring my factory, and thinking about what I owed you... I "
	 "decided I couldn't just let",
	 0,340},
	 // 340
	{"you go on and get yourself killed for the sake of our little community.  I heard about the Baron, which "
	 "honestly comes as no surprise, really.  The old boy's been embroiled",
	 0,341},
	 // 341
	{"in scandals from his first day in office!  So I thought I should help you out.  I came out here with "
	 "these 5 advanced prototype puppets to back you up.",
	 0,342},
	 // 342
	{"So... on to the bad news.  Those stinkin' Bodzhas accosted me and grabbed all their Control Crystals! "
	 "If you can recover them, these puppets can bash in",
	 0,343},
	 // 343
	{"Frostburn's front door for you.  Each one has 4 Control Crystals, so there are 20 to recover.  Sorry... "
	 "I tried to help!",
	 119,0},
	 // 344 quest not done
	{"Bring back those crystals and you'll have yourself an army!",
	 0,0},
	 // 345 got em all
	{"Excellent!  I'll hook these up, and they'll be ready to go!  If they get damaged, bring them back and "
	 "I'll repair them.  Now watch out in there - I found records",
	 0,346},
	 // 346
	{"indicating the Baron ordered some real top-of-the-line Evil Toys from the Diabolical Onion while "
	 "he was running my factory.",
	 120,0},
	 // 347 back with injured/dead bots
	{"Here, let me just fix these right up...",
	 121,0},
	 // MAGICSMITH STUFF
	 // 348 mimic quest not done
	{"Make sure you don't hit a mimic that you're taming!  This seems to anger them.  Don't let them bite you "
	 "either. It just gives them a taste for meat.",
	 0,0},
	 // 349 mimic quest done
	{"Oh great!  Now I can keep my smithing tools in the little guy.  However, it occurred to me that there are chests and barrels "
	 "all over the place.",
	 0,350},
	 // 350
	{"I guess I could've used one of those instead.  Oh well!  Thanks!  Hey, you should tame another one for yourself.  It'll go "
	 "from town to town, keeping your stuff handy.",
	 16,0},
	 // STASH
	 // 351
	{"Move items in or out of your Mimic here.  Arrows select, Fire moves the selected item.  Press Magic to move all the items in a stack "
	 "at once.  Jump or ESC to exit!",
	 0,0},
	// BARON
	// 352
	{"You doddering dunderhead!  You took away my precious Titans!  How will I conquer all of Loonyland now?  Well, no matter.  I shall "
	 "destroy you, and then go",
	 0,353},
	 // 353
	{"dismantle that blasted furnace myself.  I was a fool to rely on lackies like that stupid bunny.  So, step 1 of my "
	 "new plan: You go squish now!",
	 0,0},
	 // 354 you have weakened him
	{"Enough toying around!  It's time to show you REAL power, boy!  I call forth the ancient power of the Ice Titans... Owa Tador "
	 "Kiyam!  Owa Tador Kiyam!",
	 0,0},
	 // MELTON
	 // 355
	{"You dare call me forth from the depths to resolve your petty disputes?!  We are not your playthings, mortal!  Let me demonstrate "
	 "what I think of that...",
	 123,0},
	 // 356, baron's dead
	{"Now, as for you, come a bit closer so I can tell you something.  Oh, don't be afraid, I just want to thank you for repairing the "
	 "furnace!",
	 0,0},
	// 357, he's dead
	{"Augh, fine!  Stop!  Just as the Baron sought to use our power to his own ends, so are you being used - by forces greater "
	 "than you can even comprehend.  The Baron,",
	 0,358},
	 // 358
	{"too, was a slave to a greater power.  His schemes were nothing more than one move on a great chessboard, "
	 "himself no more than a mere pawn.  Well, maybe a",
	 0,359},
	 // 359
	{"rook or a bishop.  Whatever.  But there is something different about you - no ordinary mortal could have driven me back, even in my half-melted state. "
	 "I shall",
	 0,360},
	 // 360
	{"be watching with great interest from beneath the earth... Now take this key!  Take it and climb the highest peak in Winter Woods.  There you will find what "
	 "",
	 0,361},
	 // 361
	{"you've been seeking.  I want to see how this game plays out.",
	 122,0},
	 // BIRD
	 // 362
	{"Again we meet, young Loony.  You do not recognize this form, but all of Happy Stick is one.  We are not a witch, not a bird. "
	 "Not a boy or a man.  We are all these things",
	 0,363},
	 // 363
	{"and all other things.  We are eternity!  An endless continuum of Happy Stick energy.  For centuries, We have caused tragedies "
	 "all throughout Loonyland.",
	 0,364},
	 // 364
	{"Plagues, wars, famines, and the occasional monster-making machine.  Each time, We had hoped a hero would arise.  At last, in "
	 "Halloween Hill, you appeared.",
	 0,365},
	 // 365
	{"You have proven yourself not once, but twice.  You must be the one We have been seeking!  A man unlike any other, divided yet "
	 "ultimately invulnerable.",
	 0,366},
	 // 366
	{"Driven to restore order where We create chaos.  There is but one more test.  If you pass, We will take you to Easter Island - "
	 "the focal point of Happy Stick energy.",
	 0,367},
	 // 367
	{"There, you shall be reborn within Us!  What?  You don't want the prize?  Luckily, the test is to defeat Us again.  So you "
	 "either win, or you die!",
	 0,368},
	 // 368
	{"Hahahaha squawk squawk squawk!",
	 125,0},
	 // dead
	 // 369
	{"We knew it!  You are truly the one!  Hope you brought your bathing suit, because it's tropical weather ahead!  Also, I'm going "
	 "to drop you in a volcano.",
	 124,0},
	 // WOODY STARTING MADCAP
	 // 370
	{"Welcome to Madcap Mode!  Sadly, you'll find that you've lost all quest items, and all the quests are undone.  Sadder still, "
	 "the enemies are now about ten times more",
	 0,371},
	 // 371
	{"powerful than they were.  Here's the one bright spot.  There's a new option in the Pause Menu, \"Madcap Items\".  From "
	 "that menu, you can spend money and",
	 0,372},
	 // 372
	{"Crystal Points to buy Crystals that boost your power so you can actually defeat them.  Sometimes.  You get a "
	 "Crystal Point for every 1000 XP you earn.",
	 0,373},
	 // 373
	{"Crystals get more expensive as you buy more of the same type, so choose your upgrades carefully.  Good luck!",
	 0,0},
	 // GURU AGAIN
	 // 374
	{"This doesn't count.  I'm not hiding here.  I'm resting from all the teleporting I've been doing!",
	 0,0},
	 // 375 have all talents but haven't completed quest
	{"Hey, you got all my talents!  Nice job!",
	 26,0},
	 // WOODY FINAL BIT
	 // 376
	 {"Hey, what are those glowy things?  Hmm, maybe if I went like this... and some of this...",
	 0,377},
	 // 377
	{"A bit of that...",
	0,378},
	// 378
	{"Twist this thing here...",
	0,379},
	// 379
	 {"Wrap this around there...",
	 0,380},
	 // 380
	 {"And... yeah!  Look, it's an artifact!",
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
			p=0;
			for(i=VAR_PRESENT;i<VAR_PRESENT+99;i++)
				if(player.var[i])
					p++;
			sprintf(s,"(%d/99 Presents found)",p);
			break;
		case QUEST_PROVE:
			if(player.var[VAR_PROVEKILLS]>20)
				sprintf(s,"(20/20 Monsters slain)");
			else
				sprintf(s,"(%d/20 Monsters slain)",player.var[VAR_PROVEKILLS]);
			break;
		case QUEST_PROVE2:
			if(player.var[VAR_PROVEKILLS]>100)
				sprintf(s,"(100/100 Monsters slain)");
			else
				sprintf(s,"(%d/100 Monsters slain)",player.var[VAR_PROVEKILLS]);
			break;
		case QUEST_HERBS:
			if(EquipCount(IT_HERBF)>=3)
				sprintf(s,"(3/3 Frostbane collected)");
			else
				sprintf(s,"(%d/3 Frostbane collected)",EquipCount(IT_HERBF));
			break;
		case QUEST_WOOD:
			if(EquipCount(IT_LOG)>10)
				sprintf(s,"(10/10 Logs collected)");
			else
				sprintf(s,"(%d/10 Logs collected)",EquipCount(IT_LOG));
			break;
		case QUEST_SHARPEN:
			sprintf(s,"(%d/1 Sharpening Stone found)",EquipCount(IT_SHARPSTONE));
			break;
		case QUEST_MIMIC:
			if(player.var[VAR_TAMED]&1)
				sprintf(s,"(1/1 Mimics tamed)");
			else
				sprintf(s,"(0/1 Mimics tamed)");
			break;
		case QUEST_CRYSTAL:
			sprintf(s,"(%d/1 Crystal Of Binding found)",EquipCount(IT_BCRYSTAL));
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
			p=0;
			for(i=0;i<MAX_TALENTS;i++)
				if(player.talentLevel[i]!=255)
					p++;
			sprintf(s,"(%d/30 Talents collected)",p);
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
			if(EquipCount(IT_ROPE)<=3)
				sprintf(s,"(%d/3 Ropes)",EquipCount(IT_ROPE));
			else
				sprintf(s,"(3/3 Ropes)");
			break;
		case QUEST_HANDY:
			sprintf(s,"(%d/1 Toolboxes found)",EquipCount(IT_TOOLBOX));
			break;
		case QUEST_SOUP:
			if(EquipCount(IT_CAN)<=10)
				sprintf(s,"(%d/10 Tin Cans)",EquipCount(IT_CAN));
			else
				sprintf(s,"(10/10 Tin Cans)");
			break;
		case QUEST_BEARS:
			sprintf(s,"(%d/30 Cave Bears beaten)",player.var[VAR_BEARSDEAD]);
			break;
		case QUEST_BIGBEAR:
			sprintf(s,"(0/1 Berserker Bears bonked)");
			break;
		case QUEST_DEVILGRASS:
			if(EquipCount(IT_HERBD)<=10)
				sprintf(s,"(%d/10 Devil Grass)",EquipCount(IT_HERBD));
			else
				sprintf(s,"(10/10 Devil Grass)");
			break;
		case QUEST_DELIVERY:
			sprintf(s,"(%d/1 Packages obtained)",EquipCount(IT_PACKAGE));
			break;
		case QUEST_TITANS:
			if(EquipCount(IT_ICECUBE)<=3)
				sprintf(s,"(%d/3 Ice Cubes)",EquipCount(IT_ICECUBE));
			else
				sprintf(s,"(3/3 Ice Cubes)");
			break;
		case QUEST_SKILLS:
			p=0;
			for(i=0;i<MAX_SKILLS;i++)
				if(player.skillHave[i])
					p++;
			sprintf(s,"(%d/50 Skill Scrolls)",p);
			break;
		case QUEST_FACTORY:
			strcpy(s,"(0/1 Toy Factories demolished)");
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
				int n;

				n=JunkCount();
				if(n>=50)
					strcpy(s,"(50/50 Junk items collected)");
				else
					sprintf(s,"(%d/50 Junk items collected)",n);
			}
			break;
		case QUEST_EASYMINER:
			if(player.var[VAR_SNUGGLY])
			{
				sprintf(s,"(%d/1 Diamond found)",EquipCount(IT_DIAMOND));
			}
			else
			{
				sprintf(s,"(%d/1 Candy taken)",EquipCount(IT_CANDY));
			}
			break;
		case QUEST_HIGHWAY:
			if(player.var[VAR_SNUGGLY])
			{
				sprintf(s,"(%d/1 Highway patrolled)",player.var[VAR_HIGHWAY]);
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
				p=player.var[VAR_SHAKEDOWN+0]+player.var[VAR_SHAKEDOWN+1]+player.var[VAR_SHAKEDOWN+2]+player.var[VAR_SHAKEDOWN+3];

				sprintf(s,"(%d/4 Merchants shaken down)",p);
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
			if(player.var[VAR_FURNACE])
				strcpy(s,"(1 Furnace repaired)");
			else
				sprintf(s,"(%d/3 parts found)",EquipCount(IT_THING)+EquipCount(IT_THING2)+EquipCount(IT_WATCH));
			break;
		case QUEST_FRAGILE:
			strcpy(s,"(0/1 Thing found)");
			break;
		case QUEST_PLANTS:
			strcpy(s,"(0/1 Watch found)");
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
			p=0;
			for(i=0;i<10;i++)
				if(player.var[VAR_TITANIC+i])
					p++;
			sprintf(s,"(%d/10 Titans toppled)",p);
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
			sprintf(s,"(%d/1 Championships won)",player.var[VAR_CHAMPION]);
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
					ReportQuestReport(QUEST_WOOD);
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
					ReportQuestReport(QUEST_BALLOON);
					break;
				case IT_TOOLBOX:
					ReportQuestReport(QUEST_HANDY);
					break;
				case IT_CAN:
					ReportQuestReport(QUEST_SOUP);
					break;
				case IT_HERBD:
					ReportQuestReport(QUEST_DEVILGRASS);
					break;
				case IT_ICECUBE:
					ReportQuestReport(QUEST_TITANS);
					break;
				case IT_PACKAGE:
					ReportQuestReport(QUEST_DELIVERY);
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
			}
			if(val>=IT_LOG && val<=IT_CLAW && player.var[VAR_SNUGGLY]==0)
				ReportQuestReport(QUEST_CLOCKJUNK);
			break;
		case QE_KILL:	// killed a monster
			break;
		case QE_SKILL:	// picked up skill scroll
			ReportQuestReport(QUEST_SKILLS);
			break;
		case QE_TALENT:	// got a talent
			ReportQuestReport(QUEST_GURU);
			break;
		case QE_QUEST:	// finished a quest
			if(val>=QUEST_CLOCKJUNK && val<=QUEST_RUMBLE)
				ReportQuestReport(QUEST_QUESTS);
			break;
		case QE_VAR:
			if(val==VAR_PROVEKILLS && player.var[VAR_PROVEKILLS]<=20)
				ReportQuestReport(QUEST_PROVE);
			if(val==VAR_PROVEKILLS && player.var[VAR_PROVEKILLS]<=100)
				ReportQuestReport(QUEST_PROVE2);
			if(val==VAR_TOYSDEAD && player.var[VAR_TOYSDEAD]<=200)
				ReportQuestReport(QUEST_TOYS);
			if(val>=VAR_PRESENT && val<VAR_PRESENT+99)
				ReportQuestReport(QUEST_PRESENTS);
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
			if(val>=VAR_TITANIC && val<VAR_TITANIC+10)
				ReportQuestReport(QUEST_TITANS2);
			if(val==VAR_TAMED)
				ReportQuestReport(QUEST_MIMIC);
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
					if(player.var[VAR_PROVEKILLS]>99)
						curChat=9;
					else
						curChat=8;
				}
				else
					curChat=7;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_PROVE])
			{
				if(player.var[VAR_PROVEKILLS]>19)
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
				GotItem(IT_CANDY)==0 && player.var[VAR_TEMP]==0)
			{
				curChat=221;
			}
			else if(player.var[VAR_QUESTDONE+QUEST_PRESENTS])
			{
				curChat=14;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_PRESENTS])
			{
				if(EquipCount(IT_PRESENT)==99)
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
				if(EquipCount(IT_LOG)>=10)
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
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN] && player.var[VAR_SHAKEDOWN+0]==0 && player.var[VAR_SNUGGLY]==0)
			{
				curChat=217;
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
			if(player.var[VAR_QUESTDONE+QUEST_GURU])
			{
				curChat=374;	// shouldn't happen, he shouldn't exist
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_GURU])
			{
				byte p;
				int i;
				p=0;
				for(i=0;i<MAX_TALENTS;i++)
					if(player.talentLevel[i]!=255)
						p++;

				if(player.talentLevel[player.var[VAR_GURUTALENT]]!=255)	// already have this talent
				{
					if(p==30)
						curChat=375;
					else
						curChat=374;
				}
				else
				{
					if(p==29)
						curChat=76;
					else if(p==30)
						curChat=375;
					else
						curChat=71+(rand()%5);
				}
			}
			else
				curChat=68;
			break;
		case 13: // Tina Tsarina
			if(player.var[VAR_QUESTASSIGN+QUEST_CHAMPION])
			{
				if(player.var[VAR_CHAMPION] && !player.var[VAR_QUESTDONE+QUEST_CHAMPION])
					curChat=145;
				else
				{
					curChat=79;
					EnterShop(SVC_ARENA);
				}
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_GLADIATOR])
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
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN] && player.var[VAR_SHAKEDOWN+1]==0 && player.var[VAR_SNUGGLY]==0)
			{
				curChat=218;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_UNDIES])
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
			player.var[VAR_VISITED+PLC_EISBURG]=1;
			switch(player.levelNum)
			{
				case LVL_TUNDRA:
					player.var[VAR_VISITED+PLC_ARENA]=1;
					break;
				case LVL_BASKERVILLE:
					player.var[VAR_VISITED+PLC_BASKER]=1;
					break;
				case LVL_CLIFFS:
					player.var[VAR_VISITED+PLC_CLIFFS]=1;
					break;
				case LVL_HIGHWAY:
					player.var[VAR_VISITED+PLC_HIGHWAY]=1;
					break;
				case LVL_GORGE:
					player.var[VAR_VISITED+PLC_GORGE]=1;
					break;
			}
			if(player.var[VAR_QUESTASSIGN+QUEST_BALLOON])
			{
				if(player.var[VAR_QUESTDONE+QUEST_BALLOON])
				{
					curChat=99;
					EnterShop(SVC_BALLOON);
				}
				else if(EquipCount(IT_ROPE)>=3)
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
					byte p;
					int i;
					p=0;
					for(i=0;i<MAX_SKILLS;i++)
						if(player.skillHave[i])
							p++;
					if(p==50)
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
				{
					if(EquipCount(IT_ICESKATE)==0)
						curChat=111;
					else
						curChat=112;
				}
				else if(EquipCount(IT_ICECUBE)>=3)
					curChat=111;
				else
					curChat=110;
			}
			else
				curChat=100;
			break;
		case 18:	// magic guy
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN] && player.var[VAR_SHAKEDOWN+2]==0 && player.var[VAR_SNUGGLY]==0)
			{
				curChat=219;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_DELIVERY] && !player.var[VAR_QUESTDONE+QUEST_DELIVERY] && GotItem(IT_PACKAGE)==0 && player.var[VAR_TEMP]==0)
			{
				curChat=216;
			}
			else
			{
				curChat=215;
				EnterShop(SHOP_MAGIC);
			}
			break;
		case 20:	// ropemaker
			if(player.var[VAR_QUESTASSIGN+QUEST_SHAKEDOWN] && player.var[VAR_SHAKEDOWN+3]==0 && player.var[VAR_SNUGGLY]==0)
			{
				curChat=220;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_FACTORY])
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
				else if(EquipCount(IT_CAN)>=10)
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
				else if(EquipCount(IT_PACKAGE))
					curChat=169;
				else
					curChat=168;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_DEVILGRASS])
			{
				if(player.var[VAR_QUESTDONE+QUEST_DEVILGRASS])
					curChat=166;
				else if(EquipCount(IT_HERBD)<10)
					curChat=162;
				else
					curChat=163;
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
				else if(JunkCount()>=50)
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
				byte p;
				p=player.var[VAR_SHAKEDOWN+0]+player.var[VAR_SHAKEDOWN+1]+player.var[VAR_SHAKEDOWN+2]+player.var[VAR_SHAKEDOWN+3];

				if(player.var[VAR_QUESTDONE+QUEST_SHAKEDOWN])
					curChat=209;
				else if(p==4 && player.money<4000)
					curChat=207;
				else if(p==4)
					curChat=208;
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
			if(player.levelNum==LVL_FURNACE && player.var[VAR_QUESTDONE+QUEST_FURNACE]==0)
			{
				curChat=338;
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
						if(EquipCount(IT_DIAMOND))
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
					if(player.var[VAR_CHAMPION])
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
			if(!player.var[VAR_QUESTDONE+QUEST_FRAGILE])
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
								if(GotItem(IT_BCRYSTAL)==0 && player.var[VAR_TEMP]==0 && !player.var[VAR_QUESTDONE+QUEST_CRYSTAL])
									curChat=312;	// repeat if you haven't gotten the binding crystal
								else
									curChat=319;
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
				if(GotItem(IT_KEY4)==0 && player.var[VAR_TEMP+2]==0)
					curChat=327;
				else
					curChat=328;
			}
			else if(player.var[VAR_QUESTASSIGN+QUEST_TITANS2])
			{
				byte p,i;
				p=0;
				for(i=0;i<10;i++)
					if(player.var[VAR_TITANIC+i])
						p++;
				if(p==10)
					curChat=327;
				else
					curChat=326;
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
			PlayerGetItem(IT_SCROLL,SKILL_THROWING,0);
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
			PlayerGetItem(IT_SCROLL,SKILL_BASH,0);
			break;
		case 5:
			// assign presents quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_PRESENTS,1);
			break;
		case 6:
			// complete presents quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_PRESENTS,1);
			DropItemOfType(IT_PRESENT,99);
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
			DropItemOfType(IT_LOG,10);
			PlayerGetItem(IT_SCROLL,SKILL_AXEMASTERY,0);
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
			PlayerGetItem(IT_SCROLL,SKILL_TRAINING,0);
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
			GuruVanish();
			break;
		case 25:
			// give talent and vanish in a puff of smoke
			LearnTalent(player.var[VAR_GURUTALENT]);
			GuruVanish();
			break;
		case 26:
			// give talent and vanish in a puff of smoke and complete quest
			LearnTalent(player.var[VAR_GURUTALENT]);
			GuruVanish();
			PlayerSetVar(VAR_QUESTDONE+QUEST_GURU,1);
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
			if(EquipAdd(IT_SHOVEL,0,0))
			{
				PlayerSetVar(VAR_QUESTDONE+QUEST_HANDY,1);
				DropItemOfType(IT_TOOLBOX,1);
				TipEvent(TIP_SHOVEL);
			}
			else
			{
				NewMessage("Drop some stuff to make room!",0);
			}
			break;
		case 37:
			// assign balloon quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BALLOON,1);
			break;
		case 38:
			// complete balloon quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_BALLOON,1);
			DropItemOfType(IT_ROPE,3);
			break;
		case 39:
			// assign titan quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_TITANS,1);
			break;
		case 40:
			// complete titan quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_TITANS,1);
			DropItemOfType(IT_ICECUBE,3);
			EquipAdd(IT_ICESKATE,0,0);
			break;
		case 41:
			// assign ally quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_ALLIES,1);
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
			DropItemOfType(IT_CAN,10);
			PlayerGetItem(IT_SCROLL,SKILL_STRENGTH,0);
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
			break;
		case 48:
			// finish skill quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SKILLS,1);
			player.skillPts+=5;
			NewMessage("You got 5 Skill Points!",0);
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
			AddGuy((90*TILE_WIDTH+TILE_WIDTH/2)*FIXAMT,(11*TILE_HEIGHT+TILE_HEIGHT/2)*FIXAMT,0,MONS_TEDDY3);
			break;
		case 52:
			// assign gladiator quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_GLADIATOR,1);
			break;
		case 53:
			// finish gladiator quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_GLADIATOR,1);
			PlayerGetItem(IT_SCROLL,SKILL_WHIRL,0);
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
			PlayerGetMoney(500);
			NewMessage("You got $500!",0);
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
			DropItemOfType(IT_HERBD,10);
			PlayerGetItem(IT_SCROLL,SKILL_IGNITE,0);
			HealGoodguy(9999);
			MakeNormalSound(SND_BIGHEAL);
			MakeColdRingParticle(goodguy->x,goodguy->y,0,64);
			break;
		case 62:
			// letter quest done
			PlayerSetVar(VAR_QUESTDONE+QUEST_LETTER,1);
			DropItemOfType(IT_LETTER,1);
			PlayerGetItem(IT_SCROLL,SKILL_ENDURANCE,0);
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
			DropItemOfType(IT_PACKAGE,1);
			PlayerGetItem(IT_SCROLL,SKILL_ENERGY,0);
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
			if(!GotItem(IT_KEY3))
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
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_CLOCKJUNK,1);
			break;
		case 71:
			// finish junk quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_CLOCKJUNK,1);

			// take away the 50 junk items, one of each type until done (so as not to wipe out one thing)
			amt=50;
			while(amt>0)
			{
				for(i=0;i<MAX_EQUIP;i++)
				{
					if(player.items[i].type==IT_LOG || player.items[i].type==IT_GEAR || player.items[i].type==IT_CAN ||
						player.items[i].type==IT_STUFFING || player.items[i].type==IT_SOCK || player.items[i].type==IT_ICECUBE ||
						player.items[i].type==IT_SEED || player.items[i].type==IT_STONE || player.items[i].type==IT_ECTO || player.items[i].type==IT_CLAW)
					{
						if(player.items[i].count>1)
						{
							player.items[i].count--;
							amt--;
						}
						else
						{
							amt--;
							player.items[i].count=0;
							player.items[i].type=IT_NONE;
						}
					}
					if(amt==0)
						break;
				}
			}
			SortEquipment();
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
			char s[32];

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
			if(player.money>4000)
				player.money-=4000;
			else
				player.money=0;
			break;
		case 80:
			// assign supplies quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_SUPPLIES,1);
			break;
		case 81:
			// complete supplies quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_SUPPLIES,1);
			PlayerGetItem(IT_SCROLL,SKILL_SPIKE,0);
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
			player.var[VAR_TEMP]=1;
			break;
		case 88:	// rescued by capitan
			PlayerSetVar(VAR_PRISONBREAK,1);
			break;
		case 89:	// snuggly: complete rank quest and assign kill onion quest and give key
			if(player.var[VAR_QUESTDONE+QUEST_QUESTS]==0)
				PlayerSetVar(VAR_QUESTDONE+QUEST_QUESTS,1);
			if(player.var[VAR_QUESTASSIGN+QUEST_LEADER]==0)
				PlayerSetVar(VAR_QUESTASSIGN+QUEST_LEADER,1);
			if(!GotItem(IT_KEY2))
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
			PlayerGetItem(IT_SCROLL,SKILL_TORNADO,0);
			break;
		case 90:	// finish clock quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_CLOCKJUNK,1);
			break;
		case 91:	// complete miner quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_EASYMINER,1);
			DropItemOfType(IT_DIAMOND,1);
			PlayerGetItem(IT_SCROLL,SKILL_SPIKE,0);
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
			if(PlayerGetItem(IT_BCRYSTAL,0,0))
			{
				bullet_t *b;
				b=FireBullet(goodguy->ID,goodguy->x,goodguy->y,FIXAMT*15,rand()%256,BLT_ITEM,0);
				if(b)
				{
					b->anim=IT_BCRYSTAL;
					b->target=0;
					b->owner=0;
					player.var[VAR_TEMP]=1;
				}
			}
			break;
		case 110:
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_HIPPY,1);
			break;
		case 111:
			PlayerSetVar(VAR_QUESTDONE+QUEST_HIPPY,1);
			PlayerGetItem(IT_SCROLL,SKILL_SHROOM,0);
			DropItemOfType(IT_SEED,20);
			break;
		case 112: // assign bokbok quest
			PlayerSetVar(VAR_QUESTASSIGN+QUEST_BOKBOK,1);
			break;
		case 113: // give thing of mabob
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
			break;
		case 115:	// complete titan quest
			PlayerSetVar(VAR_QUESTDONE+QUEST_TITANS2,1);
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
				player.var[VAR_PUPPETLIFE+i]=50;
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
			GenerateArtifact();
			DropItemOfType(IT_ARTBIT,5);
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

