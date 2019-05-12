static txtItem_t baseItems[]={
	{	// TI_NONE
		// type names
		{"","","",""},
		// full name
		"",
		// description
		{""},
		// flags
		0
	},
	{	// TI_JAR
		// type names
		{"JAR","GLASS","",""},
		// full name
		"an empty glass jar",
		// description
		{"This is a jar... made of glass... and guess what?  It's empty!",
		 "Perhaps you could fill it with something and thusly solve a ",
		 "puzzle or two."},
		// flags
		0
	},
	{	// TI_FULLJAR
		// type names
		{"","","",""},
		// full name
		"a glass jar filled with water",
		// description
		{"This is a glass jar filled with refreshing water.  Sure looks refreshing!",
		 ""},
		 // flags
		 0
	},
	{	// TI_LOONYKEY
		// type names
		{"KEY","","",""},
		// full name
		"a Key Of Lunacy",
		// description
		{"This is a Key Of Lunacy.  I think you know what it's for!"},
		// flags
		0
	},
	{	// TI_ASYLDOOR
		// type names
		{"DOOR","","",""},
		// full name
		"the Asylum door",
		// description
		{"The door that leads into the Crazy Asylum Of Madness.  You better",
		 "find a way to open it, so you can stop Dr. Lunatic's evil scheme!"},
		// flags
		TIF_NOGET|TIF_NOLIST
	},
	{	// TI_SAND
		// type names
		{"SAND","","",""},
		// full name
		"some sand",
		// description
		{"A little bit of desert sand.  Surely the most useful item ever in",
		 "video game history!"},
		// flags
		0
	},
	{	// TI_WATER
		// type names
		{"WATER","OASIS","SPRING",""},
		// full name
		"water",
		// description
		{"Wet clear stuff, mostly made of hydrogen and oxygen.  It's crisp",
		 "and refreshing!"},
		// flags
		TIF_NOLIST|TIF_WATER
	},
	{	// TI_PICK
		// type names
		{"PICK","PICKAXE","AXE",""},
		// full name
		"an old rusted pickaxe",
		// description
		{"This pick has been laying out in the desert a long time, but",
		 "it still looks sturdy enough to do some damage."},
		// flags
		0
	},
	{	// TI_BOULDER
		// type names
		{"ROCK","BOULDER","STONE",""},
		// full name
		"a cracked boulder",
		// description
		{"This boulder is sitting in a depression.  It's covered with",
		 "cracks large and small.  It looks rather unstable for a rock."},
		// flags
		TIF_NOGET
	},
	{	// TI_HOLE
		// type names
		{"HOLE","","",""},
		// full name
		"a gaping hole",
		// description
		{"With the boulder smashed, a hole has opened up, and you can see",
		 "down into some sort of cave.  You can 'GO NORTH' to enter the",
		 "hole, but it'll be a one-way trip!"},
		// flags
		TIF_NOGET
	},
	{	// TI_SPHINXTER
		// type names
		{"SPHINX","SPHINXTER","",""},
		// full name
		"Sphinxter, Lord Of The Pyramids",
		// description
		{"He's a giant sphinx, made of stone.  He is gazing at you quizzically.",
		 "He doesn't move a whole lot.  One of his paws is held out expectantly."},
		// flags
		TIF_NOGET
	},
	{	// TI_BUGSPRAY
		// type names
		{"SPRAY","BUGSPRAY","CAN",""},
		// full name
		"a can of bug spray",
		// description
		{"This stuff looks very lethal - it's covered with skull symbols and ",
		 "warnings for pregnant women and whatnot.  Type 'SPRAY' to use it."},
		// flags
		0
	},
	{	// TI_POT
		// type names
		{"POT","FLOWERPOT","",""},
		// full name
		"a huge flower pot",
		// description
		{"A gigantic stone flowerpot sits here next to the chasm.  It's filled with",
		 "strangely unfrozen soil.  I daresay this could be a magical flower pot!"},
		// flags
		TIF_NOGET
	},
	{	// TI_SEED
		// type names
		{"SEED","","",""},
		// full name
		"a magical seed",
		// description
		{"This seed is clearly magical.  You can tell because it's glowing, shooting",
		 "off colorful sparks, and is yodeling a mountain tune."},
		// flags
		0
	},
	{	// TI_BLOCK
		// type names
		{"BLOCK","ICE","",""},
		// full name
		"a block of ice",
		// description
		{"This ice block is enormous, and insanely cold."},
		// flags
		TIF_NOGET
	},
	{	// TI_ICE
		// type names
		{"ICE","CUBE","",""},
		// full name
		"an ice cube",
		// description
		{"This is just a chip off the old block!"},
		// flags
		0
	},
	{	// TI_COIN
		// type names
		{"COIN","QUARTER","",""},
		// full name
		"a quarter",
		// description
		{"It's a round piece of metal with a value of 25 cents."},
		// flags
		0
	},
	{	// TI_SVEN
		// type names
		{"SVEN","","",""},
		// full name
		"Sven The Yeti",
		// description
		{"He's a yeti.  Like all yetis, he looks like a big fuzzy teddy bear",
		 "in a Robin Hood hat."},
		// flags
		TIF_NOGET
	},
	{	// TI_BJORN
		// type names
		{"BJORN","","",""},
		// full name
		"Bjorn The Yeti",
		// description
		{"He's a yeti.  Like all yetis, he looks like a big fuzzy teddy bear",
		 "in a Robin Hood hat."},
		// flags
		TIF_NOGET
	},
	{	// TI_OLAF
		// type names
		{"OLAF","","",""},
		// full name
		"Olaf The Yeti",
		// description
		{"He's a yeti.  Like all yetis, he looks like a big fuzzy teddy bear",
		 "in a Robin Hood hat."},
		// flags
		TIF_NOGET
	},
	{	// TI_KONGOR
		// type names
		{"KONGOR","APE","",""},
		// full name
		"Kongor The Ape",
		// description
		{"This is the biggest darn monkey you have ever seen!  Also it looks",
		 "more like a bear than the average monkey does, but you think it'd",
		 "be more polite to not mention that oddity."},
		// flags
		TIF_NOGET
	},
	{	// TI_THING
		// type names
		{"THING","","",""},
		// full name
		"The Thing",
		// description
		{"A huge blob of green protoplasm with tentacles flailing about in",
		 "everything direction.  Just looking at this thing threatens to tear",
		 "your mind apart!"},
		// flags
		TIF_NOGET
	},
	{	// TI_MATILDA
		// type names
		{"MATILDA","QUEEN","",""},
		// full name
		"Matilda, Queen Of Chaos",
		// description
		{"Looking like a cross between a spider, a scorpion, a skeleton, and",
		 "an overly large gymnasium, Matilda gazes down upon you with contempt",
		 "and pity.  Also, she has chocolate smeared across what passes for",
		 "her lips."},
		// flags
		TIF_NOGET
	},
	{	// TI_BANANA
		// type names
		{"BANANA","BANANAS","BUNCH","FRUIT"},
		// full name
		"a whole bunch of bananas",
		// description
		{"This is really quite a lot of bananas.",
		 },
		// flags
		0
	},
	{	// TI_HOOK
		// type names
		{"HOOK","HAND","",""},
		// full name
		"a pirate's hook hand",
		// description
		{"At one time, a pirate used this in place of a hand, and probably was very",
		 "cautious about scratching his itches.  Now, it's just a metal hook with a",
		 "handy loop on it for tying things to it.",
		 },
		// flags
		0
	},
	{	// TI_BOOTS
		// type names
		{"BOOTS","BOOT","",""},
		// full name
		"a pair of pirate boots",
		// description
		{"These must have belonged to a very nasty pirate at one point, because ",
		 "they're equipped with spikes all along the bottom!  Either he used them",
		 "to kick people viciously, or he liked to play golf.",
		 },
		// flags
		0
	},
	{	// TI_VEND
		// type names
		{"VENDOR","VEND","VENDING","MACHINE"},
		// full name
		"a vending machine",
		// description
		{"Stocked with delicious-looking candy bars.  A sign on the machine reads",
		 "\"Candy Bars: $0.25\".  This must be a really out-of-date machine, ",
		 "because that is crazy cheap!",
		 },
		// flags
		TIF_NOGET
	},
	{	// TI_SPIDERS
		// type names
		{"SPIDER","SPIDERS","BUG","BUGS"},
		// full name
		"spiders",
		// description
		{"They're horrible and crawly, quite venomous, and there are thousands",
		 "of them!!",
		 },
		// flags
		TIF_NOGET|TIF_NOLIST
	},
	{	// TI_WEBS
		// type names
		{"WEB","WEBS","SPIDERWEB","SPIDERWEBS"},
		// full name
		"webs",
		// description
		{"Thick and sticky, these webs seem abnormally strong.  They're also",
		 "quite yucky.",
		 },
		// flags
		TIF_NOGET|TIF_NOLIST
	},
	{	// TI_SILK
		// type names
		{"SILK","WEB","SPIDERWEB","CORD"},
		// full name
		"a cord of spider's silk",
		// description
		{"This bunch of spider's silk is all twisted up into a long rope.  It's",
		 "unbelievably strong!",
		 },
		// flags
		0
	},
	{	// TI_GEAR
		// type names
		{"GEAR","CLIMBING","HOOK","SILK"},
		// full name
		"a set of climbing gear",
		// description
		{"You've cleverly tied extremely strong spider's silk to a pirate's hook",
		 "hand to make the world's most believable climbing equipment.",
		 },
		// flags
		0
	},
	{	// TI_CANDY
		// type names
		{"CANDY","BAR","CHOCOLATE",""},
		// full name
		"a candy bar",
		// description
		{"It's a chocolate bar, with nuts, caramel, and coconut.  Why do they",
		 "always ruin it with coconut?!"},
		// flags
		0
	},
	{	// TI_FROZENCANDY
		// type names
		{"CANDY","BAR","CHOCOLATE",""},
		// full name
		"a frozen candy bar",
		// description
		{"It's a chocolate bar, with nuts, caramel, and coconut, frozen solid."},
		// flags
		0
	},
	{	// TI_PEEL
		// type names
		{"PEEL","","",""},
		// full name
		"a banana peel",
		// description
		{"All that's left from the yetis' feeding frenzy."},
		// flags
		0
	},
	{	// TI_MONEY
		// type names
		{"MONEY","5","DOLLARS","CASH"},
		// full name
		"$5.00 in cash",
		// description
		{"It's $5 in spare change.  No, you can't split it up.  Spend it all in",
		 "one place!"},
		// flags
		0
	},
	{	// TI_GHOST
		// type names
		{"GHOST","PIRATE","",""},
		// full name
		"a spooky pirate ghost",
		// description
		{"This is the ghost of Pegbeard the Pirate.  He is making spooky noises",
		 "and drifting around."},
		// flags
		TIF_NOGET
	},
	{	// TI_LUNATIC
		// type names
		{"DOCTOR","DR","LUNATIC",""},
		// full name
		"Dr. Lunatic",
		// description
		{"It's Dr. Lunatic himself!  He's spotted you and is running around to",
		 "avoid being caught."},
		// flags
		TIF_NOGET
	},
	{	// TI_VINE
		// type names
		{"VINE","","",""},
		// full name
		"a giant vine",
		// description
		{"This vine crosses the chasm like a bridge.  It's terribly convenient."},
		// flags
		TIF_NOGET
	},
	{	// TI_DOOR
		// type names
		{"DOOR","LOCK","LOCKS",""},
		// full name
		"the Asylum door",
		// description
		{"It's an extremely sturdy door.  You aren't getting through it without",
		 "four Keys Of Lunacy."},
		// flags
		TIF_NOGET|TIF_NOLIST
	},
};
