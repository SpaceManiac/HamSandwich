#include "edithelp.h"
#include "editor.h"
#include "dialogbits.h"
#include "worldstitch.h"

static byte help;
static byte exitCode;

char helpName[NUM_HELPS][32]={
	"Basic Help",
	"Floor Tool Help",
	"Wall Tool Help",
	"Item Tool Help",
	"Badguy Tool Help",
	"Light Tool Help",
	"Special Tool Help",
	"Select Tool Help",
	"Eraser Tool Help",
	"Item Edit Help",
	"Item Select Help",
	"Monster Select Help",
	"Sound Edit Help",
	"Sound Select Help",
	"Special Edit Help",
	"Terrain Edit Help",
	"Tile Select Help",
};

char helpTxt[NUM_HELPS][612]={
	// basics
	"Press F1 any time to get help with any menu or tool. "
	"^ ^ Basic Editor Usage: ^ ^ "
	"Click and hold on the top left button (where it says "
	"'Floor') to get the list of tools. Click and hold the "
	"'Menus' button to get the list of menus. You can scroll "
	"the screen with the arrow keys, or by moving the mouse "
	"to the screen edge.",
	// floor tool
	"Click on any of the four floor tiles to select one to "
	"be active.  Right-click on any of them to choose a new "
	"tile to replace it.  Left or right click on 'Brush' to "
	"change the brush size.  Click anywhere on the map to "
	"'paint' your new floor tiles on. ^ 'Plop' controls how "
	"the four different tiles are used. See the manual for "
	"details on the different plop modes. 'Normal' will plop "
	"down only the active tile and is the most useful.",
	// wall tool
	"Click on any of the four walls to select one to be active. "
	"Right-click on the top half of any of them to change the "
	"roof for it, or the bottom half to change the wall.  Left "
	"or right click on 'Brush' to change the brush size.  Click "
	"anywhere on the map to paint your walls on.  'Plop' controls "
	"how the four different walls are used - check the manual "
	"for details on what the different plop modes mean.",
	// item tool
	"Click on any of the four items to select one to be active. "
	"Right-click on an item to choose a new item to replace it. "
	"Left or right click on 'Brush' to change the brush size. "
	"Click anywhere on the map to paint your new items on. 'Plop' "
	"controls how the four items are used.  See the manual for "
	"details.  Or just leave it on Normal and don't think about it!",
	// badguy tool
	"Click on any of the four monster names to select one to be "
	"active.  Right click them to choose new monsters. Left or "
	"right click on 'Brush' to change the brush size. Click anywhere "
	"on the map to plant your monsters.  There is a limit of 128 "
	"monsters on each level - That's not as many as you think! "
	"^ Left click on the item box to cycle through: No Item, Random Item, "
	"Specific Item.  Whatever it is on, that's what the monsters will "
	"drop when defeated. Right click on it to select a specific item.",
	// light tool
	"Click 'Plop' to select what the cursor should do: ^ ^ "
	"Solid - set tiles immediately to the chosen Bright value. ^ "
	"Fade - move tiles towards the chosen Bright value by one notch. ^ "
	"Torch - make a blob of light. Bright affects how big and bright. ^ "
	"Smooth - smooths the light differences between neighboring tiles. ^ ^ "
	"Random allows you to add randomness to the Bright value. If you "
	"turn on Line Of Sight, the tool won't pass through walls.",
	// special tool
	"This one is special!  Click anywhere on the map to place a Special, and "
	"start up the Special editor to define what it does.  Clicking on an "
	"existing Special will edit it.  You can right click Specials to delete them.",
	// select tool
	"All the other tools will only function on tiles that are selected. "
	"By default, every tile is selected. Click on the map to select or "
	"unselect tiles.  Plop chooses whether you want to Toggle the "
	"selection of tiles, always Select, or always Unselect. "
	"Show sets how it displays which tiles are selected. "
	"Select None unselects all tiles, then changes to Select All. "
	"Copy Selection pastes a copy of the selected area where you choose, "
	"and Swap Selection is similar, but puts the target area where your "
	"selection was.",
	// eraser tool
	"The simplest tool of all!  It just erases what you click on.  The "
	"checkboxes let you limit what it will erase.  For instance if you "
	"only want to remove monsters, uncheck everything but 'Badguy'. Erasing "
	"lights means setting the light level to zero. Erasing Walls just flattens "
	"them, leaving their roof tile as the floor.",
	// item edit menu
	"Be careful when editing an item - any changes you make will affect all "
	"copies of that item in your entire world.  If you want a variant of an "
	"existing item without changing the original, click on 'Copy Item', then make "
	"your changes to the copy.  Any custom-made items will be in the Extras "
	"category.  If you mess up one of the original items, click 'Reset Item' to "
	"restore it to normal.  Overall, editing an item is more complicated than we "
	"can explain here, so read the manual!",
	// item picker
	"You're picking an Item now.  Not a difficult task - just click on a "
	"category at the top and you'll see a list of all items in that category "
	"down the left side.  Click on the one you want!  If there are more items "
	"than will fit in the list, a \"More Items...\" button will be present. "
	"You can also click Exit to not pick an item.",
	// monster picker
	"Easy as pie: just click on a category at the top of the screen, and a list "
	"of monsters that fit that category will appear down the left side of the screen. "
	"Click on the one you want, or click 'More Monsters' to see more. "
	"If you've scanned that monster, you'll see a description of it in the middle of the "
	"screen.  If the description begins with the word \"(bby)\", that "
	"means that this monster creates other monsters. "
	"This is important because if a monster drops a specific item, ALL of the "
	"monsters it creates will drop the same item!",
	// sound edit
	"Click 'Add New Sound' to put your own custom sound in.  This lets you choose "
	"from any WAV files which are in the 'user' folder.  Then give it a name.  The "
	"name is just for your use so you recognize it when editing.  You can delete any "
	"sound you create, rename it, or reload it.  Reload means to load a new wav file "
	"to replace the one you already had.  The sounds you add are saved into the DLW "
	"file, so don't need to be included separately.  You can't rename, delete, or "
	"reload any of the original sounds.",
	// sound pick
	"Click a category on the left side, and the sound effects in that category "
	"will be listed on the right.  Right click any sound effect to hear it, and "
	"left click to select it!  Any sounds you added yourself will be in the Custom "
	"Sounds category.",
	// special editor
	"You'll have to read the manual to get good info on how to create Specials! "
	"But hold the mouse button on a 'Select' box to get the list of Triggers or "
	"Effects and pick one.  Then set the values for it by clicking or right "
	"clicking the various boxes.  Many buttons will work in different ways when "
	"right clicked versus left clicking.  Learn what each does!  You can right "
	"click the '# of Uses' button to set it to Unlimited.",
	// terrain edit
	"Click on a tile to select it, or drag to select multiple.  Click checkboxes to "
	"assign flags to the terrain.  The specifics of each flag are explained in the "
	"manual. Click on Next Tile to set which tile this tile becomes when it animates, "
	"is pushed, etc.  If you select a group of tiles, you can click Make Tile Loop or "
	"Make Rvrs Loop to set the Next Tile of each one in the group to the one following "
	"it.  Delete Unused Tiles deletes all tiles that aren't used anywhere in your world, "
	"making your DLW file smaller.  Click Next Page to see more tiles.",
	// tile picking
	"Just click on any tile to select it!  Click Next Page to see more tiles.",
	// world stitcher
	"",
};

void InitEditHelp(byte mode)
{
	exitCode=0;
	help=mode;
	if(mode==HELP_WORLDSTITCH)
		strcpy(helpTxt[HELP_WORLDSTITCH],GetStitchError());
}

void ExitEditHelp(void)
{
}

void RenderEditHelp(int msx,int msy,MGLDraw *mgl)
{
	// box for the whole dialog
	mgl->FillBox(10,100,630,300,8);
	mgl->Box(10,100,630,300,31);

	CenterPrint(320,104,helpName[help],0,1);

	PrintRect(12,120,628,280,13,helpTxt[help],1);
	RenderButtonImage(msx,msy,588,283,40,15,"Exit");
}

byte EditHelpKey(char key)
{
	if(key==27)	// esc
	{
		exitCode=1;
		return 0;
	}

	if(key==13)	// enter
	{
		exitCode=1;
		return 0;
	}

	return 1;
}

byte EditHelpClick(int msx,int msy)
{
	if(exitCode)
		return exitCode;

	if(PointInRect(msx,msy,588,283,588+40,283+15))
	{
		exitCode=1;
		return 1;
	}

	return 0;
}
