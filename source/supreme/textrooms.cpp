static txtRoom_t baseRooms[]={
	// 0 TR_ASYLDOOR
	{	// name
		"The Asylum Door",
		// desc
		{"You're in an open field.  To your North stands the Crazy Asylum Of Madness!",
		 "The door is sealed tightly shut with 4 padlocks.  Paths lead East, West, and ",
		 "South."},
		// item
		{TI_DOOR},
		// exits
		{TR_FOOTHILL,TR_DESERT,TR_CLEARING,TR_ASYLUM}
	},
	// 1 TR_FOOTHILL
	{	// name
		"The Icy Foothills",
		// desc
		{"You stand at the foothills of the Icy Mountains.  They stretch high into ",
		 "the sky to the East, their tops lost in the clouds.  If you had climbing ",
		 "gear, you could probably get up through a pass to the East.  An easier path ",
		 "slopes gently upward to the North.  A path leads away from the mountains ",
		 "to the West."},
		// item
		{TI_BUGSPRAY},
		// exits
		{TR_PEAK,255,TR_ASYLDOOR,TR_ICYPATH}
	},
	// 2 TR_DESERT
	{	// name
		"The Dusty Desert's Edge",
		// desc
		{"To your South, the Dusty Desert stretches on into infinity.  Or you assume",
		 "so anyway, you can't see too far through all the dust.  It's quite warm here.",
		 "To the North, a path leads to the Asylum, and to the West, there's a stone",
		 "cliff with a cave entrance in it.",},
		// item
		{0},
		// exits
		{255,TR_DESERT2,TR_CAVE,TR_ASYLDOOR}
	},
	// 3 TR_DESERT2
	{	// name
		"The Dusty Desert",
		// desc
		{"You are in the midst of the Dusty Desert.  It's outrageously, disgustingly hot",
		 "here!  To the South, the desert continues, possibly forever.  To the North,",
		 "you can see the end of the desert.  The East and West offer nothing more",
		 "enticing than instant-death quicksand.  You decide not to go in those",
		 "directions."},
		// item
		{0},
		// exits
		{255,TR_DESERT3,255,TR_DESERT}
	},
	// 4 TR_DESERT3
	{	// name
		"The Dusty Desert",
		// desc
		{"You are in the midst of the Dusty Desert.  This is possibly the hottest",
		 "place in the history of the world.  To both North and South, the desert",
		 "continues on. To the East and West there are sheer cliffs, which you can't",
		 "possibly climb."},
		// item
		{TI_SAND},
		// exits
		{255,TR_DESERT4,255,TR_DESERT2}
	},
	// 5 TR_DESERT4
	{	// name
		"The Dusty Desert",
		// desc
		{"You are in the midst of the Dusty Desert.  If it were any hotter, you would",
		 "be in the midst of the Flaming Pants.  Nothing but more desert to the North",
		 "and South.  To the East and West there's also more desert, but you are",
		 "firmly committed to this whole north/south thing you have going on."},
		// item
		{0},
		// exits
		{255,TR_DESERT5,255,TR_DESERT3}
	},
	// 6 TR_DESERT5
	{	// name
		"The Dusty Desert",
		// desc
		{"You are in the midst of the Dusty Desert.  You wish you weren't.  Also, it's",
		 "swelteringly hot.  Good news!  You can just make out some palm trees to",
		 "the South through the dust.  To the North is more dust and desert.  To the",
		 "East and West, there's...um... fire-breathing space aliens or something. Just",
		 "go North or South, okay?"},
		// item
		{0},
		// exits
		{255,TR_OASIS,255,TR_DESERT4}
	},
	// 7 TR_OASIS
	{	// name
		"The Dusty Oasis",
		// desc
		{"You're in front of a beautiful sparkling oasis in this horrific desert.  A big",
		 "pond of crystalline water shimmers before you, surrounded by palm trees.",
		 "To the East, you can see some sort of huge pyramid.  To the West there's",
		 "a small hill you could climb.  The palm trees block any progress to the South."},
		// item
		{TI_WATER},
		// exits
		{TR_SPHINX,255,TR_HILL,TR_DESERT5}
	},
	// 8 TR_HILL
	{	// name
		"A Dusty Hill",
		// desc
		{"You're atop a small hill from which you can see the vast extent of the Dusty",
		 "Desert.  To the East, you can see an oasis at the bottom of the hill.  All the",
		 "other directions are blocked by large boulders."},
		// item
		{TI_PICK,TI_BOULDER},
		// exits
		{TR_OASIS,255,255,TR_HOLEROOM}
	},
	// 9 TR_SPHINX
	{	// name
		"Sphinxter's Lair",
		// desc
		{"A giant pyramid rises up from the sands before you, rather nicely providing",
		 "shade.  Sphinxter is at the base of the pyramid, rather giant himself, seated",
		 "with one of his mighty stone paws held out.  You can escape his gaze by",
		 "going West back to the oasis."},
		// item
		{TI_SPHINXTER},
		// exits
		{255,255,TR_OASIS,255}
	},
	// 10 TR_CLEARING
	{	// name
		"A Clearing",
		// desc
		{"You're in a small clearing on the edge of The Spooky Forest.  The Forest",
		 "itself is to your North, where a path winds through the always spooky trees.",
		 "To the West, there's a grove of banana trees.  A cliff to the South has a",
		 "cave entrance in it.  To the East is the path to the Asylum."},
		// item
		{0},
		// exits
		{TR_ASYLDOOR,TR_CAVE,TR_BANANA,TR_FOREST}
	},
	// 11 TR_ASYLUM
	{	// name
		"The Crazy Asylum Of Madness",
		// desc
		{"You're finally inside the Asylum itself!  Yikes!  The way out is South."},
		// item
		{TI_LUNATIC},
		// exits
		{255,TR_ASYLDOOR,255,255}
	},
	// 12 TR_PEAK
	{	// name
		"The Icy Mountain Peak",
		// desc
		{"You are a bazillion feet up atop the Icy Mountain.  You can see the land",
		 "stretched out before you in every direction.  It's pretty nifty.  You can",
		 "climb down to the West or North, or travel along an icy ledge to the South."},
		// item
		{0},
		// exits
		{255,TR_LEDGE,TR_FOOTHILL,TR_CHASM}
	},
	// 13 TR_ICYPATH
	{	// name
		"The Icy Path",
		// desc
		{"This is a very slippery path of ice leading slowly up and around the Icy",
		 "Mountain.  It heads down the mountain to the South, or further up to the",
		 "East."},
		// item
		{TI_COIN},
		// exits
		{TR_CHASM,TR_FOOTHILL,255,255}
	},
	// 14 TR_CHASM
	{	// name
		"The Icy Chasm",
		// desc
		{"Your progress up the mountain is blocked here by an apparently bottomless",
		 "chasm to the East.  It's too wide to cross, and frigid winds come screaming",
		 "up from its depths.  To the South is a sheer peak, which you could climb if",
		 "you had the gear to do so.  The path down the mountain heads West."},
		// item
		{TI_POT},
		// exits
		{TR_YETI,TR_PEAK,TR_ICYPATH,255}
	},
	// 15 TR_LEDGE
	{	// name
		"The Icy Ledge",
		// desc
		{"You've felt safer in your life than you do now standing on a foot-wide walkway",
		 "made of ice, above a thousand-foot drop.  You can get back to solid ground",
		 "to the North.  Further progress on this ledge is blocked by an enormous",
		 "block of horrifically cold ice.  It seems almost unnatural!"},
		// item
		{TI_BLOCK},
		// exits
		{255,255,255,TR_PEAK}
	},
	// 16 TR_YETI
	{	// name
		"Lair Of The Yeti Bros.",
		// desc
		{"Here, protected from the wind by a shelf of rock, you find the Yeti Bros.",
		 "relaxing in front of a fire.  A chasm with a handy, yet illogical, vine bridge",
		 "across it provides the only exit to the West."},
		// item
		{TI_SVEN,TI_BJORN,TI_OLAF,TI_LOONYKEY},
		// exits
		{255,255,TR_CHASM,255}
	},
	// 17 TR_CAVE
	{	// name
		"Cave Entrance",
		// desc
		{"You're standing before a gigantic cliff face.  At the base of it, there's",
		 "a large hole you could enter to the South.  To the North is a forest clearing,",
		 "and to the East the land quickly begins to get drier."},
		// item
		{0},
		// exits
		{TR_DESERT,TR_CAVE2,255,TR_CLEARING}
	},
	// 18 TR_BANANA
	{	// name
		"Banana Grove",
		// desc
		{"Row upon row of huge banana trees loom before you.  Looming larger than",
		 "the trees, Kongor sits here, munching contentedly on an entire tree.  You",
		 "can flee from him to the East."},
		// item
		{TI_KONGOR,TI_BANANA},
		// exits
		{TR_CLEARING,255,255,255}
	},
	// 19 TR_FOREST
	{	// name
		"Spooky Forest",
		// desc
		{"The trees around you seem to be closing in, and it's unnaturally dim here.",
		 "It's spooky!  And it's a forest!  The undergrowth (SPOOKY undergrowth!) is",
		 "too thick to proceed in any directions except West and South."},
		// item
		{TI_SEED},
		// exits
		{255,TR_CLEARING,TR_SPIDERS,255}
	},
	// 20 TR_CAVE2
	{	// name
		"Cavernous Cave",
		// desc
		{"This cave is quite cavernous.  It continues deeper into the earth to the West,",
		 "and there is an exit into the sunlight to the North."},
		// item
		{0},
		// exits
		{255,255,TR_CAVE3,TR_CAVE}
	},
	// 21 TR_CAVE3
	{	// name
		"Twisty Passages",
		// desc
		{"You are in a maze of twisty passages, all alike.",
		 " ",
		 "Luckily, though, the passages bear clear signage marking their destinations.",
		 "To the North is 'Pirate Graveyard', to the East is 'Cave Entrance', to the ",
		 "South is 'Terrifying Tunnels', and to the West is 'Break Room'."},
		// item
		{0},
		// exits
		{TR_CAVE2,TR_TUNNEL,TR_VENDING,TR_PIRATE}
	},
	// 22 TR_TUNNEL
	{	// name
		"Terrifying Tunnels",
		// desc
		{"This tunnel curves, running to the North and the East.  Ooh, it's terrifying.",
		 "Eek."},
		// item
		{0},
		// exits
		{TR_HOLEROOM,255,255,TR_CAVE3}
	},
	// 23 TR_VENDING
	{	// name
		"Break Room",
		// desc
		{"This appears to be where monsters take their breaks.  There are a few",
		 "tables and chairs, as well as a candy vending machine.  Luckily no monsters",
		 "appear to be on break right now.  The door leading South is marked \"Boss's",
		 "Office\".  Another passage leads East."},
		// item
		{TI_VEND},
		// exits
		{TR_CAVE3,TR_THING,255,255}
	},
	// 24 TR_PIRATE
	{	// name
		"Pirate Graveyard",
		// desc
		{"An underground lake extends off into the darkness in every direction, except",
		 "South, where a tunnel leads back out of here.  The rotting remains of ancient",
		 "pirate ships jut from the water's surface like the teeth of a malevolent beast.",
		 "On the shore, the bones of long-dead pirates lay in silence."},
		// item
		{TI_HOOK,TI_BOOTS,TI_GHOST},
		// exits
		{255,TR_CAVE3,255,255}
	},
	// 25 TR_THING
	{	// name
		"Boss's Office",
		// desc
		{"This is an enormous cavern filled with lava, except for a small area of rock,",
		 "on which you are standing.  The Thing is lurking here, half-submerged in the ",
		 "lava.  It's horrifying beyond all comprehension.  It's eyes pierce your very",
		 "soul and seem to say \"Yowza, I'm a scary monster.\".  The only escape from",
		 "this unimaginable horror is to the North."},
		// item
		{TI_THING,TI_LOONYKEY},
		// exits
		{255,255,255,TR_VENDING}
	},
	// 26 TR_HOLEROOM
	{	// name
		"Dead End",
		// desc
		{"This room is just a dead end in the caves.  Looks like you'll have to go",
		 "back to the West."},
		// item
		{TI_JAR},
		// exits
		{255,255,TR_TUNNEL,255}
	},
	// 27 TR_THING2
	{	// name
		"Boss's Office",
		// desc
		{"This is an enormous cavern filled with lava, except for a small area of rock,",
		 "on which you are standing.  There is a comically Thing-shaped hole in the far",
		 "wall of the cavern.  To the North, a doorway provides the only exit."},
		// item
		{0},
		// exits
		{255,255,255,TR_VENDING}
	},
	// 28 TR_HOLEROOM2
	{	// name
		"Dead End",
		// desc
		{"This room is is just a dead end in the caves.  A hole in the ceiling lets light",
		 "shine in, but is too far up to reach.  A tunnel leads to the West"},
		// item
		{0},
		// exits
		{255,255,TR_TUNNEL,255}
	},
	// 29 TR_SPRING
	{	// name
		"Natural Spring",
		// desc
		{"A spring bubbles up from some rocks here, sending fresh water cascading",
		 "down the face of the rocks.  A path through the trees leads South."},
		// item
		{TI_WATER},
		// exits
		{255,TR_SPIDERS,255,255}
	},
	// 30 TR_SPIDERS
	{	// name
		"Spider Nest",
		// desc
		{"This place is completely crawling with thousands of tiny spiders!",
		 "They've covered every surface with thick, sticky webs.  Paths lead North,",
		 "East, and West, but the path to the West is completely blocked by webs."},
		// item
		{TI_SPIDERS,TI_WEBS},
		// exits
		{TR_FOREST,255,TR_MATILDA,TR_SPRING}
	},
	// 31 TR_SPIDERS2
	{	// name
		"Former Spider Nest",
		// desc
		{"This place was completely crawling with thousands of tiny spiders, but",
		 "now it's just littered with their bodies.  You brutally murdered them,",
		 "and I hope you're proud of yourself.  Paths away from the carnage lead",
		 "North, and East. There's also a web-covered hole leading West."},
		// item
		{0},
		// exits
		{TR_FOREST,255,TR_MATILDA,TR_SPRING}
	},
	// 31 TR_MATILDA
	{	// name
		"The Heart Of Chaos",
		// desc
		{"You've come to a very dead end in the forest.  Matilda stands before you,",
		 "massive and terrible.  In her claws and scattered all over the ground are",
		 "candy wrappers.  The only escape from Matilda is to the East."},
		// item
		{TI_MATILDA,TI_LOONYKEY,TI_SILK},
		// exits
		{TR_SPIDERS,255,255,255}
	},
};
