#include "allusionary.h"

allusion_t allusion[NUM_ALLUSIONS]={
	{"Class System",
	 "The Absurd Area Class System is based on the Mini Monster Madness addon that came with the Collector's Edition. Mini Monster "
	 "Madness was kinda... unfinished? But I really liked the core idea of picking a class and its skills! That's why I've been "
	 "calling it the Class System instead of a \"skill tree\" or whatever. I took the idea and ran with it pretty far, and here we "
	 "are!"
	},
	{"Alliteration",
	 "I overused alliteration because it is a very cheap and easy way to be silly. However, I had to do extra work to make some of "
	 "the level names alliterate, so it didn't save much effort. I didn't know what \"Hiemal\" meant before I looked up synonyms for "
	 "\"cold,\" for example. \"Caliginous\" means \"dark,\" and so on. I am proud of \"Precipitous Precipices\" though."
	},
	{"General Quest Names",
	 "I'm not going to waste space explaining all the quest names, since so MANY of them are just slight parodies on the existing LL2 "
	 "quest name. I did want to change EVERY quest name, and all the quest descriptions, and all the dialogue. Even if I didn't "
	 "have a knee-slapper of an idea, I wanted to change it, and a simple play on words is enough."
	},
	{"Equipment Changes",
	 "I'm sure most people won't notice, but I made a few changes to magic items! The ones that heal you or recover stamina give you "
	 "more than they used to, because they were actually kind of lame. 1 HP every 25 seconds or whatever? Weaksauce. You get that "
	 "with 1 skill point in Survival. But the biggest change is the super artifacts... I figured those were SO powerful they could "
	 "break the game, and it's more likely than usual because I hand out Artifact Bits! So I halved their stats and halved the "
	 "damage from the Doom Parka. They're still really good, but they don't make the game trivial."
	},
	{"Town Names",
	 "The first town was what decided my naming convention. \"Eisburg\" becomes \"Einsburg,\" literally \"One + Village\" in German. "
	 "The other towns follow a similar pattern in other languages - \"Tweestad\" is \"Two + City\" in Dutch, \"Sankyo\" = \"Three + "
	 "Big City\" in Japanese, and the mushroom one, according to Google Translate, is \"Four + Camp\" in Maori. They also sound like "
	 "other English words - \"Twisted,\" \"Thank You,\" and \"Weapon,\" but those words don't mean anything here."
	},
	{"Curium",
	 "Well, in LL2 it's called \"Healium,\" so this one cures instead of heals. Also, the Frostbane is mixed with \"22 other "
	 "flavors\" because... well, did you read the LL2 allusion for Healium? She's mixing a batch of KFC? Well, Dr. Pepper is made "
	 "from a mix of \"23 flavors.\""
	},
	{"No, No, It's A Medicine",
	 "While on the subject of Curium... the real life Curium is an element like Helium. Except it's not like helium, because helium "
	 "is fairly nonreactive and harmless, whereas Curium is famously so radioactive that it killed the people who first isolated it, "
	 "because they didn't know that 1 gram of the stuff is all it takes to end you. But yeah, this stuff just heals you. Wouldn't "
	 "that be funny as a mean prank in a game though? Have a substance called \"Curium,\" mislead the player into thinking it's a "
	 "health potion, and then politely inform them that their character will die of radiation poisoning."
	},
	{"10,000th Customer",
	 "This is a reference to scammy internet ads from my childhood. In Absurd Area, Geoffrey is up to some shady stuff...",
	},
	{"Taylor",
	 "Honestly, half of my character writing for Absurd Area was \"make the character the opposite of their original personality\" "
	 "or \"make them an exaggeration of their original personality.\" Especially if they didn't have much character to begin with. "
	 "Taylor is one of those, since his only character trait was \"old.\" Not much to work with, so I just exaggerated it. Also, "
	 "when he says \"That's just grand, that is!\" It's a Wallace and Gromit reference."
	},	// 9
	{"Mickey",
	 "Mickey was kind of mean in the original, although it's understandable because you do kind of just barge into his house. I "
	 "decided to take that and run with it. Now he manages the inn, which appears to be the only residential building in town, "
	 "and he's very short-tempered without such a good reason. And yet, I still gave him more characterization than most. I think I "
	 "was just inspired. I was originally going to make Healiana be a bit snarky at you, \"I SOMEHOW still don't have enough "
	 "Frostbane herbs!\" But I figured they shouldn't ALL be jerks."
	},
	{"Teleporting Guru",
	 "I think the Guru Teleporting In effect in Absurd Area is really cool. That's just what happens when you summon monsters onto "
	 "a specific tile, but it's perfect for the Guru. Part of it is that villagers seem to always do their idle animation when you "
	 "summon them, so he will always be flapping his arms as he lands. It wasn't planned, but it was fortuitous."
	},
	{"Guru Skill Resetting",
	 "Originally, the Guru was going to \"reveal the true forms\" of equipment, since you couldn't just have the player pick up "
	 "magic items off the ground. After I resumed the project in Jan 2025, I decided to just code in a way to hand items to the "
	 "player. So... what was I going to do with all the teleporting Guru locations I had created? Well, I was ALSO thinking how "
	 "cool it would be to be able to reset your skills. It was a lot easier than I expected. You still can't reset Upgrades though!"
	},
	{"Early Bokboks",
	 "In the original, the Bokboks were in the Underworld, so you could only do the Magicsmithing very late. I wanted to let you do "
	 "it MUCH earlier, which is why you can find all the Bokboks so early on. It also is why Sharona left the Underworld to travel "
	 "above ground, which made it more fun writing her character dialogue. It worked out."
	},
	{"Magicsmithing",
	 "You know... it is nice that you CAN do Magicsmithing much earlier. But I did throw some useful magic items at you, so if you "
	 "go exploring enough, you might not WANT to spend your money on Magicsmithing. Which is why it's a good thing I gave you other "
	 "things to spend large sums of money on..."
	},
	{"To Tame Them Is Your",
	 "\"To Tame Them Is Your Cause!\" Who doesn't know this one? Is there anybody who hasn't memorized the Pokemon theme song? OK, "
	"I never watched the show. Actually, I've never even played one of the games. But yeah, it's a Pokemon reference."
	},
	{"Free Melly",
	 "A reference to \"Free Willy,\" a movie which I might have seen when I was 8. Or maybe I only saw part of it, asked my parents "
	 "if I could see the full movie, and never got around to it. I actually don't remember. The scene where the beached whale "
	 "exploded was cool, though. Why did they try to use dynamite? Terrible idea."
	},
	{"Melody",
	 "Melody's character writing started as my just turning her character upside down, but then I just kind of wrote a subplot "
	 "about it. It's fine to START with lazy writing, as long as you follow that little voice telling you to take it farther and "
	 "do something cool with it."
	},
	{"Polar Pond Ice Maze",
	 "The ice maze is based on similar mazes in Chip's Challenge, for which I enjoyed making maps to solve the levels. We actually "
	 "had a big ol' scrapbook full of graph paper maps of Chip's Challenge levels. I found out later that some gamers actually don't "
	 "enjoy making their own maps. Ridiculous! Anyway, if you want the parka early, you will have to make your own map and find your "
	 "way through. Lucky you!"
	},	// 18
	{"Rock 'N Stole",
	 "Rock And Roll. But they stole, a rock. OK, this one was a stretch.",
	},
	{"Not Folded Enough",
	 "If you try to sharpen an already-sharpened axe, Woody will tell you the metal hasn't been folded enough to hold an edge that "
	 "sharp. This is a reference to the myth that Japanese swords were SO sharp because they folded the metal over 1,000 times. "
	 "Actually, they folded the steel 1,000 times because it was poor quality and had too much carbon, and they had to fold it to "
	 "squeeze the extra carbon out. Or maybe Japanese metalsmiths had a former career as pastry chefs."
	},
	{"Tina",
	 "I dunno, I just think it's funny to have the super serious gladiator mistress become a ditzy valley girl.",
	},
	{"Loonymania",
	 "The quest name is a reference to Hulkamania, which is a Hulk Hogan thing. That's all I know, I don't watch wrestling. However, "
	 "ignorance never stopped me from putting in a reference!"
	},
	{"Creepy Copse Hut",
	 "This isn't really a reference to anything. You know those horror movies where the heroes are trying to run away from the "
	 "spooky forest, only they find that the way out has been blocked off? Let's say it's a reference to those movies. I also think "
	 "it's extra cool how it looks like there's light coming through a crack in the hut door."
	},
	{"Copse Skeletons",
	 "I have a text file in which I planned out the secrets that the copse skeletons give you, and the creepy things they say if you "
	 "don't have Bonehead. I wanted to give them a unique verb and adjective. Again with the thesaurus!"
	},
	{"Copse Warm Bones",
	 "One of the evil ghosts says something about how \"your bones look warm.\" This is sort of a reference to a book, \"The Last "
	 "Guardian of Everness,\" in which an evil necromancer craves the warmth in the bones of his victims, since shedding his "
	 "humanity has left him perpetually cold. He's a reference to Koschei the Deathless from Russian folklore. The takeaway is that "
	 "folklore is gross."
	},
	{"Oh my heavens!",
	 "The Historian says this, which is a direct reference to the Wacky Wizard. They just seem so similar. I wonder if their voice "
	 "actors are related."
	},
	{"Doing stuff... Things.",
	 "Handyman Hank says this, which is a reference to early on in The Walking Dead. This is another reference to something I "
	 "haven't actually seen, and am not even sure it's a real quote. I've just seen memes about it."
	},	// 27
	{"Titan Furnace Quest",
	 "This quest is kind of the core of the Absurd Area storyline. Like, when I set out to redo the dialogue, I had to decide what "
	 "to do with the plot. The simple formula of \"turn the plot upside down\" suggested an idea that was so ridiculous, so crazy, "
	 "so unspeakably DUMB, that I had to run with it."
	},
	{"Reverse Fetch Quest",
	 "Just a reference to the fact that this quest is kind of the opposite of the classic Fetch Quest. Except that the quest giver "
	 "has no way of knowing that you actually did the task. I suppose the Historian might have given you a receipt or something?"
	},
	{"Klunk's Revenge",
	 "The \"Klunk's Pirates\" fight is in the game entirely because of the flavor text for that one arena match. I thought it was a "
	 "cool idea, so I put it in the main game! I actually planned for Klunk to get MORE revenge, with Mutant Clone Klunk appearing "
	 "randomly to attack you! Imagine if every level had specials giving a 1/100 chance to spawn Mutant Klunk and some friends. "
	 "There was just one problem... Mutant Clone Klunk isn't actually a creature! That arena fight just spawns Klunk, and has a "
	 "special rule that heals all the enemies. Oh well."
	},
	{"Bobby Joe",
	 "I just thought it would be funny to have the baby character be a scheming mogul. I'm nowhere near this cynical in real life."
	},
	{"I've Got 99 Presents",
	 "There's some rap called \"I've Got 99 Problems.\" That's all. Once again, I've never actually heard the thing I'm referencing!"
	},
	{"Precipitous Precipices",
	 "I apologize for the name, but it alliterates. I have a text file where I listed all the exits in the Precipices and everywhere "
	 "else that connects to the Hiemal Holes, to make sure the maps \"line up\" as if it was one real map instead of several pocket "
	 "dimensions connected by portals. Awful hard, since the maps all have an offset. I had to do math to create an effect that "
	 "nobody will probably notice!"
	},
	{"Geoffrey",
	 "Somewhere in the LL2 developer commentary or something, it says Geoffrey was originally going to have some role in the axe "
	 "smugglers operation run by the Onion Ring. I decided, like many other things in Absurd Area, that I was just gonna put it in "
	 "if Jamul wasn't. It's like the old \"Are you gonna eat that?\" And then I just snatch it right off the plate. He wasn't eating "
	 "it, it's mine now."
	},
	{"Curaga",
	 "Curaga already gave off stoner vibes in the original... so I just leaned into it. Exaggeration is cheaply funny, and I never "
	 "would have come up with \"oops, I was accidentally reading my OWN Talents!\" or \"I found this cool amulet in the trash\" if "
	 "I hadn't decided on that as the \"core.\" Picking a Motif and running with it is a great way to break out of writer's block."
	},
	{"Quest Reworking",
	 "Many of the quests have completely redone requirements. This caused no end of problems originally, since I didn't have access "
	 "to the source code! I had to use complicated arrays of specials to prevent you from talking to certain characters until you "
	 "met MY quest requirements. Otherwise, Curaga's quests would auto complete if you brought her 10 Devil Grass and so on. "
	 "I think I hit special limit in the Hiemal Holes until I was able to re-code the quests and delete all those specials!"
	},	// 36
	{"Monkey Grotto Present",
	 "I guess if you're reading this, you've probably already found the Monkey Grotto cliff present, and this isn't a spoiler. "
	 "Spoiler alert: You need to jump right when a monkey hits you to get damage-boosted up onto the cliff! I originally had this "
	 "idea for a cut-content plan to have a tunnel system between the Onion and Bunny camps. Some sort of mission where you "
	 "traverse the tunnels with friends to invade the other team. A rogue clockbot, your enemy no matter which team you're on, was "
	 "supposed to damage-boost you. But the whole idea was axed."
	},
	{"Kinyova",
	 "I didn't really feel like changing Kinyova's character, since he's already kinda cool. But, since I was placing all sorts of "
	 "secret areas, I thought it would be awesome to hide one of the NPCs somewhere hard to find. Every game needs secrets guarded "
	 "by cryptic hints! And he gives you a hint to the Super Secret too."
	},
	{"Balloonist Text",
	 "The balloonist's shop text in LL2 was a reference to Dr. Seuss. I couldn't keep it, because Absurd Area lets you go to places "
	 "you haven't visited yet! So I looked up other things to reference. \"Everywhere You Want To Be\" is the slogan for Visa, "
	 "which fits better than I expected."
	},
	{"Toymonger",
	 "I just thought it would be funny to make Toymonger a little more morally questionable. OK, maybe I'm a little cynical. But, I "
	 "mean, I'm trying to make everything sillier. \"Making a nice character cynical\" is easy to be silly, whereas \"Making a mean "
	 "character nice\" gives you less to work with. Wait, now I'm imagining Darth Vader running an orphanage and it's hilarious. "
	 "OK, never mind."
	},
	{ "Crush and Smash",
	 "These two were basically the same character, so I thought it would be funny to take them in opposite directions. One of them "
	 "extra stupid, and the other as verbose as I can make him. I put the bookshelves there to make him look extra smart!"
	},
	{ "MacGregor Farm",
	 "The abandoned farm is called the MacGregor farm, as a reference to Peter Rabbit. And now it's occupied and used as a supply "
	 "depot by the Order of the Snuggly Bunny..."
	},
	{ "Captain Capitan",
	 "I don't really know why he's Texan. Maybe because you have to duel him at some point. I grabbed a motif and ran with it."
	},
	{"Your Creator, Obey!",
	 "This quest name is a reference to the old trope where a robot turns on its creator. The quest is such a obvious use of this "
	 "trope, I felt compelled. While I was at it, I threw in a bunch of other cliches. Robot Revolts are basically a giant pile of "
	 "cliches anyway. Maybe the clockbots are mad that he kept kicking them when they didn't do what he wanted the first time? Who "
	 "knows."
	},
	{"Defense Missions",
	 "It took a lot of work to get the two Defense missions working properly... I sure hope the balance is OK. If you have a build "
	 "to do them quickly, you will get a lot of powerful potions! And if you don't, you get NOTHING. Rather hard to tweak since it's "
	 "a bit swing-ey, but I think they're really fun! Also, if you fail, you can just leave the level and try again."
	},	// 45
	{"Does Whittling Count?",
	 "I just couldn't come up with a good name for the Craft Talent quest. So I made something up. There really isn't any hidden "
	 "meaning or reference, I just grabbed the idea that no matter how much time you spend on something, it's not going to impress "
	 "anybody unless it's useful or artistic. Ideally both."
	},
	{"The Rumble",
	 "This may have been the first idea I had for Absurd Area! It pretty much came to me in a dream. I really like this sort of "
	 "mission, where there is a bigger battle and you can influence it. I tested it in a test world to get the unit ratio correct - "
	 "3 bunnies to 4 thugs will fight to a standstill! Too bad the enemies also have big boys to give an advantage. Once I figured "
	 "out how it worked, it was simple to implement - I copied the specials in a hex editor and tweaked them. The part with enemies "
	 "in the walls to \"pull\" the combatants was so cool, I had to reuse it in the volcano."
	},
	{"Schrodinger",
	 "I decided I wanted the Shroomulans to have really exaggerated personalities, and that's why Sharona ran off to the overworld. "
	 "Schrodinger is well-dressed, so I decided he's a posh British guy. I've read a lot of P.G. Wodehouse, which I drew on to give "
	 "him his idioms."
	},
	{"Shrumford Old",
	 "I think he's the third character whose distinctive personality is just \"old dude.\" In the original he didn't really have "
	 "any lines, but I figured I could make him a stereotype. Stereotypes are funny (except when they're really not)."
	},
	{"Shrumford Young",
	 "I'm sorry. I'm so sorry for this. Anyway, I did Google up a list of Gen Alpha slang to throw in as much as I could. Sadly, I "
	 "couldn't work in the word \"mewing\" anywhere."
	},
	{"Supercalifragilistic",
	 "I think it's originally from \"Mary Poppins,\" but I used it because it kinda has the word \"fragile\" in there. Yeah, that "
	 "was bad."
	},
	{"B Feeder",
	 "The first two bomb parts were easy to name - the Fission Chip sounds very British, and the Doomsday Clock is an obvious "
	 "choice. But the third one... I can't even really tell what it's supposed to be! Made it awfully hard to come up with a name. "
	 "Eventually, I decided it looks like some kind of power cables. The term \"B Feeder\" came to me, and I had to make it work. "
	 "It's a reference to the \"Beefeaters,\" one of the most iconic British things ever. And I really had to stretch the dialogue "
	 "to make it fit!"
	},
	{"Crystal Caves",
	 "This level is named after the game by Apogee Software. I had a lot of fun with that one as a kid. If they included an editor, "
	 "I would have spent hours making not-very-good levels. Heck, I designed plenty of them on graph paper."
	},
	{"I've been defeated!",
	 "Capitan's quote when you defeat him at the furnace is ripped straight out of the Fire Emblem fan community. I don't know if "
	 "it's a quote from the games, but it's definitely making fun of all the times where you defeat an enemy who, instead of dying "
	 "like the vast majority of enemies or allies who get to 0 HP, suddenly retreats despite being completely surrounded, or maybe "
	 "they have some previously unused teleportation ability."
	},	//54
	{"Choose the Deuce",
	 "Fishbug's quote when arresting Capitan is from... the Minecraft movie. It was really stupid, and I wasn't originally going "
	 "to waste time on it. But the director did Napoleon Dynamite, so I had to give him a chance. I was shocked that it had this "
	 "quote, which I actually liked. Very deep and philosophical!"
	},
	{"Only thing that's left",
	 "Fishbug says, in the volcano, \"The only thing that's left to do...\" It's a phrase commonly used by the food YouTuber Guga. "
	 "He has a lot of idioms that don't entirely fit, like using the word \"because\" to link two paragraphs that have nothing "
	 "causally to do with each other."
	},
	{"Artifact Jack",
	 "In the original game, Artifact Jack was a rare Madcap-Only miniboss. I wasn't going to stand for that! I put MOST of the "
	 "Madcap-Only or Arena-Only enemies in the game somehow. Or is it all? I think it's all. Jack seemed a perfect choice for the "
	 "\"final\" Objective Point. I didn't want him coming up more often, since he drops an Artifact Bit, and I didn't want to give "
	 "you a replenishing supply! But then I was inspired to use him in two of the Trials, and again in the final level... so I just "
	 "contrived a way to make it impossible to collect drops from those."
	},
	{"Toymonger = Kinyova",
	 "In the castle, I had to make sure to place the Toymonger a bit further up from the entrance so he appears off screen. This is "
	 "because of a complication in how specials work with NPCs when you start a level... if you SUMMON a villager as the level "
	 "loads, it will appear with the default sprite for villagers - Kinyova. It won't change to the correct sprite until the "
	 "villager's AI function takes over. But AI functions are disabled when the level is fading in! So you'd see Kinyova standing "
	 "there for a second before turning into the Toymonger. I had the same issue with the Guru."
	},
	{"Puppet Pals",
	 "This quest name is a reference to the YouTube parody sketch, \"Potter Puppet Pals.\" Which... well, I might have seen one of "
	 "them."
	},
	{"Have fun storming!",
	 "Jamul mentioned he was going to have the Toymonger quote this line from \"The Princes Bride,\" \"Bye-bye, boys! Have fun "
	 "storming the castle!\" But, he decided not to. So, I did!"
	},
	{"Forbidden Secrets",
	 "I wanted the Baron's library to be a little more colorful, so I spent some time thinking up what an evil mastermind might have "
	 "in his library. Took me forever! But that means I had to settle for some very silly examples, which fits. I just grabbed "
	 "anything that would be forbidden from some reason or another. My favorite is the My Little Pony lore."
	},
	{"Wing Boots",
	 "This was cut content from LL2, which I thought was a cool idea. So I stole it! I really like the effect where you fall through "
	 "the clouds, but it takes specials, so I made sure there are only 2 places you can actually fall through. I didn't want to "
	 "spend any more time on a silly gimmick than I had to."
	},
	{"Bruce Willis Movie",
	 "\"Unbreakable,\" is the movie the bird refers to."
	},	// 63
	{"3D Arena",
	 "I wanted to really soup up the aesthetics of the Arena, so I designed a cool 3D effect. I really like how it worked out! "
	 "I had to do a lot of individual tile art to get it to work though... since walls don't cast shadows on other walls. So I had "
	 "to place walls in the editor, cast shadows onto some tiles, screenshot those, and import the screenshot into the tileset. I "
	 "used this trick in a few places, which is one reason the tileset is so confusing!"
	},
	{"Arena Maze",
	 "The maze gave me the most trouble, not just designing the maze part, but also the Souvenir Swords! I had to change how I did "
	 "the sections at the top and sides, with the NPCs watching you, because the Souvenir Swords can appear on any empty floor. And "
	 "you can't place creatures on top of an item, or an item under a creature... So Swords would appear under Tina et al! I had to "
	 "summon them with specials. Then, later, I found you can copy+paste items under a creature. Oh well. I am rather pleased with "
	 "hiding a Bokbok in the arena!"
	},
	{"Arena Capital",
	 "I drew circles in MS Paint to make sure I got the circular shapes correct! Also, the original reason the 3x3 walls are dark in "
	 "the middle is, I was going to add Tinpot Generals in there to make the arena missions more challenging. I thought the arena "
	 "matches would have you win when all the monsters of a certain tag were killed. That was just a guess, and I was wrong! It ends "
	 "when there is nobody left on the Badguy team. So I scrapped that. But I liked the look as it is now."
	},
	{ "Arena Side",
	 "I just like hiding things behind secrets. Hiding Betty in a secret side entrance seemed like a cool idea. I had to add "
	 "specials to only summon her if you're in Tweestad... hey, how does Tina go from one franchise to another? Does she teleport? "
	 "Anyway, I think it's cool how the Tweestad arena lets you see Betty and the present to give you a hint to look for the side "
	 "entrance. And I put a hint in the Creepy Copse. It's not supposed to be hard to find."
	},
	{"Firewall",
	 "The first monster I added! I wasn't originally going to add more, but that didn't last long. I was already going to have a "
	 "volcano area, so this was the obvious choice. A Firewall is something that blocks your network from attacks, which is why the "
	 "Firewall erases nearby projectiles! I then had to tweak the code to make sure it only erased the correct projectiles, and not "
	 "dropped items or explosions that have already dealt damage. More effort than I expected!"
	},
	{"Nice Titan",
	 "While I was adding monsters, I had this idea come to me. An enemy area-of-effect healer? Awesome. And it erases projectiles "
	 "just like your Restoration spell. I really had to mess around with the code to get this to work... and while I was at it, I "
	 "fixed an old issue in LL2. Ice Titans you have mind controlled always stunned enemies instead of freezing them... this is "
	 "because it uses the same function as Stomp! And it only checked for what team the attacker was on. After I fixed the function, "
	 "I had to fix it again so they could heal the bird!"
	},
	{"Spice Mice",
	 "Sort of a pun on the already-existing Ice Mice. I had to do crazy stuff in the code to let it know the difference between "
	 "getting set on fire by the player and by the Spice Mice, since I didn't want the enemy Ignite to use your Ignite skill level!"
	},
	{"One Punch",
	 "There's an anime or manga or whatever called \"One Punch Man,\" about a guy who is so powerful he can defeat anything in one "
	 "punch. But, that doesn't help him do his taxes or whatever. I haven't seen any of it. But I wanted to add a late-game puppet "
	 "enemy, since all the other toy types get a late game version. They're usually pretty easy to avoid, but when you DO get tagged "
	 "it is satisfyingly dramatic. And then I had to re-use the damage boost from the Monkey Grotto! If you're having trouble with "
	 "that jump, a Safety Amulet can be a massive help."
	},
	{"Rock Gorilla",
	 "I didn't need to add another monkey type, but I had the idea just because of how mean it is in combination with the Spice "
	 "Mice. The Spice Mice will halve your armor, and then the rock spikes can multi-hit you for massive damage! Same thing you "
	 "can do with Brambles + Arsenic, then casting Stone Spike. Turnabout is fair play!"
	},	// 72
	{"Square Hole",
	 "I kinda just threw random crafting phrases into the \"crafting an Artifact\" text. The square hole one is a reference to a "
	 "famous tiktok where somebody is increasingly aghast at... look, just Google it, it's quicker. As a side note, I placed 10 "
	 "Artifact Bits for you to collect without buying one in the shop. So you could craft 2 artifacts, as long as you find the "
	 "Super Secret! Also, with the Glaucoma modifier, there is a 1/1000 chance that a randomly dropped Lens will be an Artifact "
	 "Bit! Have you unlocked it yet?"
	},
	{"Infested Woods",
	 "The first secret \"themed\" level I placed. This one has specials to randomly respawn the rats - but they all appear behind "
	 "the trees! So it just looks like more and more keep coming from nowhere. The miniboss gave me trouble because I can't check for "
	 "whether the monster tag is dead - because it respawns more with tag 255. And I can't simply check whether the monster type is "
	 "dead - because what if you use Shroom? That actually changes their monster type! It was easy once I figured out the bugs, but "
	 "it meant a few annoying headbang moments."
	},
	{"Puppet Island",
	 "The island's name is a reference to... Rudolph? I think? I've never seen the movie. Originally, it was going to just have a "
	 "KO Punch as the miniboss, but that wasn't \"hard\" enough. So now there are randomly spawned KO Punches and the very dangerous "
	 "Fruit Punch! Well, except that it sometimes charges right past you without landing its hit. Still, it's quite dangerous at the "
	 "level it's first available to fight."
	},
	{"Bear Lair",
	 "I made this secret level because I wanted to spice up the \"kill the bears\" quest. A themed secret level seemed the perfect "
	 "way! I really like how it leads into a secret underground path back to the starting area. That way, you don't have to go "
	 "through the Perilous Pass ever again! I also think it's neat how the Grizzly is only summoned when you enter its part of the "
	 "level, like it's waiting to ambush you."
	},
	{"Army Camp",
	 "This one isn't technically a secret level, and neither is the Creepy Copse. However, I did sort of hide it by making the Taiga "
	 "level really big. Unless you're deliberately searching the whole level, you could easily miss it... which is why I put those "
	 "paths on the taiga. And the soldiers all spawn at a random location (out of 4) when you get close - they're on patrol! The "
	 "camp itself let me stress test my method for giving loot to the player, with the massive stash of potions. It took a lot of "
	 "testing to make sure the variables were all set and checked properly."
	},
	{"Monkey Grotto",
	 "One of my ideas for the secret levels was to make each one have a different environmental \"hint\" letting you know where to "
	 "look. For the audio cue, I put a friendly Arena Crystal off in the darkness with a few monkey monsters nearby, trying to "
	 "attack it but unable to reach the target. The miniboss fight is pretty cool, since the rocks lying around allow the Gorilla "
	 "projectile to pass over it, but it actually doesn't do that much damage."
	},
	{"Drafty Den",
	 "I am pretty happy with hiding this one in plain sight - if you visited the Polar Pond before getting the skates, you might "
	 "not even know this place exists and could skip it entirely! And it really is quite missable, unless you're going for the "
	 "Super Secret or 100%. If you found it early, let me know if the loot was useful to you!"
	},
	{"Poisonous Pit",
	 "Probably my favorite secret level. The theme is that the swirly blue is some kind of poison gas, and it's heavier than air so "
	 "you can avoid it by jumping. That still doesn't explain why it destroys items, though. This is technically the same level as "
	 "the Power Core fight, but that's just because I ran out of levels. The Power Core was going to be on the same level as the "
	 "Factory, but that caused problems if you save and load after beating the core - that would summon the Onion to recruit you "
	 "while you're still in the basement!"
	},
	{"Bodzha Nest",
	 "This one was a challenge to get right. The specials to swap-map the boulders in flight were a little finicky, and I had to "
	 "make some custom tiles to hide what was happening. But the result is a cool unique level that is shooting at you. I thought "
	 "of putting something similar in the Frostburn Castle, but decided against it. The castle is mean enough!"
	},	// 81
	{"Equipment",
	 "I spent a lot of effort originally to get pre-placed loot into Absurd Area, so you can understand it was something I really "
	 "wanted to have! It originally took a lot of annoying specials to convert the equipment, but after I got the code it became "
	 "really simple. They aren't all super broken - or else why would you ever buy anything or use Magicsmithing? But most builds "
	 "will find one of them useful. And the Historian Rewards have the potential to be VERY useful! Let me know which one is your "
	 "favorite!"
	},
	{"The Five Hidden Trials",
	 "I think every game should have something like this! These are supposed to be technically available early on - you can find all "
	 "of them before you even finish the Toy Factory. And yet, unless you have maxed out one of the skill paths, it will simply be "
	 "an ominous gate... And even if you DO max out a skill path, it might just be too hard for you! They are also deliberately not "
	 "the same difficulty... the two Mage trials give MUCH better rewards, so they are way harder! You don't get potions because "
	 "they could make it too easy. They are supposed to be a challenge, not a keyhole!"
	},
	{"Duelist's Trial",
	 "This one is just supposed to be a boss rush against a bunch of really tough monsters all in a room. It's mostly just a power "
	 "check, although you do need some skill against the Army Of One since you can't use potions! Duelist skills will come in handy "
	 "in this trial, and Parry is especially useful against the Army Of One. That was part of the design goal for the Trials - "
	 "make each Trial benefit from the skills you mastered to unlock it."
	},
	{"Gladiator's Trial",
	 "This trial is based on the arena defense missions. Only, this one is a lot more extra! There are a lot of weak enemies because "
	 "the Gladiator skill path focuses on crowd control, but you will have to strategize a bit. Boneheads are a big help; anything "
	 "that gives your opponents extra targets can be a big help! Plague fans will also have fun. But as long as you have a powerful "
	 "build you'll be fine. Just don't stray too far chasing down enemies and let the monsters swarm in from the other side."
	},
	{"Marksman's Trial",
	 "OK, this trial doesn't really have anything to do with marksmanship. It used to be called the Ranger, not the Marksman, and "
	 "it had skills like Dodge and Tracking. So, that kinda fit better. I wanted to keep the trial I'd designed, so when I redid the "
	 "class system, I just ignored the fact that it didn't fit. Hey, the Tossing Talent pushes enemies away, which will help here! "
	 "Anyway, this trial is also a boosted version of a type of arena match. Actually, they all are, except the Enchanter's Trial."
	},
	{"Enchanter's Trial",
	 "This one is definitely the hardest! Well, the Sorceror's Trial is close, depending on your build. I actually made it easier a "
	 "couple times, but there's only so much I can do. You will need a cheap ranged attack to keep the Nuclear Bokboks away, and you "
	 "will definitely find certain Enchantment spells useful - Tornado, Restoration obviously, Berserk definitely. You will "
	 "definitely have to use the crystals to trap Killbor, since he respawns! If you haven't beaten it yet... best I can say is keep "
	 "trying!"
	},
	{"Sorceror's Trial",
	 "This one is also pretty hard, and could be the hardest if you have the wrong build. Since you can't use potions, you will need "
	 "to use Berserk to keep your distance, and ranged attacks are a great friend. Throwing Axes are a great help, and I found Ice "
	 "Cube to be the most useful spell. Or you could use high-level Restoration and just wail on him. Good luck! You'll need it."
	},
	{"Mystery of the Trials",
	 "You may be wondering what's up with the trials. They're set in some kinda spooky buildings, embedded in some dark spooky "
	 "caves, and Phase 2 of the trials is against Titanic Titans? What's the deal? Well, I didn't actually write lore for them. "
	 "But if I had to, it would be something about somebody imprisoning them because they couldn't kill the things, waiting for "
	 "somebody to do the job for them. If this was a serious game I would have written something. But... I do plan to make more "
	 "games... And this is the sort of idea I would definitely re-use!"
	},
	{ "Soundtrack",
	 "The soundtrack is from the game Hero Core, by Daniel Remar. \"Brother Android\" composed the score. I liked "
	 "both the game and the music, so I emailed Brother Android and asked for permission. He said it's cool as "
	 "long as I tell you who I got it from. The soundtrack came with a couple of tunes that were cut from Hero "
	 "Core, but they fit perfectly into Absurd Area! (It's the music for the Trial Entrance and the Hidden Trials "
	 "themselves.)"
	},	// 90
	{"Gallery Goals",
	 "One of the Gallery Goals has spoilers, so I invented a system to encrypt them. No opening the Graphics folder to see them! "
	 "In case you are reading this without having found all the Gallery Goals yet, I'll give a one-word hint for each one. 01: "
	 "Damage. 02: ComBOOM! 03: Artifact. 04: Purchase. 05: Presents. 06: Golden. 07: Supreme. 08-12: Trial."
	},
	{"Secret To Everybody",
	 "This achievement name is a reference to the original Legend of Zelda. Well, the English translation, anyway. Anyway, Kinyova "
	 "is an old dude who hides in a secret cave and hands you free weapons, so it seemed to fit."
	},
	{"Bearmageddon",
	 "This achievement name is a reference to the webcomic of the same name, which sadly has been on hiatus since 2019. It's "
	 "basically a zombie apocalypse story, except instead of zombies, it's all the world's bears. Except some of the bears are also "
	 "mutant hybrids. So there's this one scene where the heroes are barricaded inside a building, but a star-nosed mole bear "
	 "burrows through the floor! Fortunately, there's a lawnmower in reach... It gets pretty gruesome sometimes."
	},
	{"Where's The Iron Boots",
	 "This achievement name is a reference to \"The Legend of Zelda: Ocarina of Time,\" in which an Ice Cavern holds the Iron Boots. "
	 "Pretty straightforward. This one just holds more secrets!"
	},
	{"Don't Bite Them",
	 "This achievement name is because it's a poison cave, full of all the poisonous enemies. It's a reference to the fact that "
	 "poisonous animals are the ones that kill you if you bite THEM, and it's VENOMOUS enemies that kill you when they bite you. "
	 "So the Poison damage effect should really be called \"Venom!\" Except that the colloquial definition encompasses both "
	 "scientific definitions, and you can use that to correct any annoying nerds who try to correct you. Same with when \"slander\" "
	 "is used instead of \"libel.\" When you correct them, shout \"OBJECTION!\" first!"
	},
	{"Badger Badger Badger",
	 "This achievement name is a reference to a mind melting animation from the old days of the internet. It's really really stupid, "
	 "but simple and catchy. Which just makes you hate it more. It's not as bad as the Hamster Dance, but that isn't saying much."
	 },
	{"The Great Gonzales",
	 "This achievement name is a reference to \"Paper Mario: The Thousand Year Door,\" in which one chapter requires you to join "
	 "an arena and fight your way to the title championship. I changed the achievement requirement because I didn't want you to "
	 "be able to get it by grinding an easy match. Gotta beat the annoying ones too!"
	},
	{"Pile Of Secrets",
	 "This achievement name is a reference to Dracula's quote from \"Castlevania: Symphony of the "
	 "Night:\" \"What is a man? A miserable little pile of secrets.\" I've never played the game. In fact, until I Googled it just "
	 "now, I could have sworn the quote was from Hamlet. It's also a reference to the fact that the Trials are a bunch of secrets. "
	 "Some of them might even make you miserable!"
	},
	{"The Super Secret",
	 "How to get the Super Secret: First, do NOT return the toolbox. You will also need the two Ropes from the Vulgar Vault and "
	 "Betty's reward. Use one to reach Kinyova and get the Hatchet. It's sharp enough to cut down the tree and get to the upper "
	 "Hiemal Holes. Use the other rope to cross to the Drafty Den. Go outside and push down the rock. Then go back to the Polar Pond "
	 "and skate across to the Drafty Den. Top-center, you must dislodge a rock blocking a spring. Kill the Icebox, and push its egg "
	 "into the hole. Cross, and traverse the Poisonous Pits to the Factory. Pick the lock with the toolbox!"
	},
};

char *All_EntryName(byte n)
{
	if(n<0 || n>99)
		return allusion[0].name;
	else
		return allusion[n].name;
}

char *All_EntryDesc(byte n)
{
	if(n<0 || n>99)
		return allusion[0].desc;
	else
		return allusion[n].desc;
}
