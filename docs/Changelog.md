# Changelog

This document describes the differences between the HamSandwich versions of
each game and the last official releases. It also includes the historic
changelogs for the modernized versions of applicable games.

See also the [list of published mods](https://github.com/SpaceManiac/HamSandwich/wiki)
based on HamSandwich.

## All games (2019–present)

* Migrate graphics and audio to SDL2
    * Fix graphical glitches by implementing 8-bit to 32-bit conversion in software
    * Control settings are not compatible with official releases due to this change
    * Variously replaces SciTech MGL, Allegro, DirectSound, and FMOD
* Add global <kbd>F12</kbd> hotkey to take a `.bmp` screenshot in the native resolution of the game
* Add global <kbd>F11</kbd> hotkey to toggle fullscreen mode and window size
* Add global <kbd>F10</kbd> hotkey to toggle scaling mode between nearest-neighbor and linear filtering
* Allow games to optionally load resources directly from the official installer rather than extracting it first
* Add a proper close curly brace `}` character to the font used in the editors
* Fix FLC movies exported from modern tools like Aseprite displaying slightly incorrectly
* Port to new platforms
    * Linux
    * macOS
    * Android
        * Add onscreen touch joystick and controls
        * Add mapping so Android "back" button acts like pressing the Escape key
    * Web
        * Add backtick <kbd>`</kbd> hotkey to replace Escape in fullscreen
* Clean up source code, add flexibility, or fix compiler warnings in ways that should have no noticeable effect
    * Support 64-bit builds with full save and add-on compatibility
    * Combine each game's control, music, movie, font, sound, and sprite handling into a common "Ham" core

### Source code blogs
* [Dr. Lunatic](https://hamumu.com/blog.php?entry=1318711596) (2011-08-15)
* [Supreme With Cheese, Sleepless Hollow, Loonyland, Loonyland 2, and Kid Mystic](https://hamumu.com/blog.php?entry=1558391916) (2019-05-20)
* [Eddie Galaxy, Spooky Castle, and Stockboy](https://hamumu.com/blog.php?entry=1653318531) (2022-05-23)
* Amazin' SPISPOPD's code was lost, but was reverse-engineered for inclusion in HamSandwich.

## Dr. Lunatic Supreme With Cheese

### [Supreme With Steam](https://store.steampowered.com/app/2547330/Dr_Lunatic_Supreme_With_Steam/) special features

* Show off your Gallery Goals and track your progress with Steam Achievements
* Find and play over 400 new worlds on the Steam Workshop
* Upload your own worlds to the Steam Workshop from the Editor's "Save & Export" menu
* Compete with friends and foes on score and time Leaderboards for every world in the game
* Automatically transfer your saves across devices with Steam Cloud
* Smash pumpkins anywhere with a native Steam Deck build

### HamSandwich (since last release)

Nothing!

### v9.5 (2025-03-19) - [Steam](https://store.steampowered.com/news/app/2547330/view/831582351143208832)

* Allow loading `.ogg` files as custom sounds.
* Add page numbers and multi-filtering (right-click on category) to Monster, Sound, and Item lists.
* Add a fallback if Steam Workshop upload fails due to file permission issues.
* Fix SWC-only items appearing by mistake in some imported Dr. L worlds.
* Fix rare bugs with Suckers, Blowers, and Sphinx Arms with different AIs set.

### v9.4 (2024-08-10) - [Steam](https://store.steampowered.com/news/app/2547330/view/4370265995216804393)

* Add third control "Wpn Lock" that toggles the Wpn Lock setting while held. For example, if Wpn Lock is on, this acts as a "pick up" button. It's also usable in Specials. The default is <kbd>Tab</kbd>.
* Change Special Trigger selector to keep the NOT/AND state instead of clearing it.
* Fix graphical pop-in on very large items and monsters.
* Fix sounds far to one side sometimes wrapping around and coming from the other side instead.
* Change <kbd>F11</kbd> hotkey to cycle through possible sizes, instead of just tiny and huge.

### v9.3 (2024-03-05) - [Steam](https://store.steampowered.com/news/app/2547330/view/4127057666118798478)

* Add bullet selection menu (by Hyperme, [#17]).
* Increase per-world tile limit from 800 to 1000.
* Add editor help message explaining how to enable Custom JSP for items.
* Fix visual bugs when combining underwater/lava, handheld mode, and movies.
* (Steam) Fix Steam Workshop uploads occasionally re-including files from previous uploads.
* Fix Creeping Cone having an attack sound when it shouldn't.
* Fix water splash particles sometimes flashing the wrong color (especially obvious when raining on Splatter Mode).
* Fix a [missing text file] in Halloween Horror 2 - TG.
* Fix Summon Bullet prompt to indicate that equations are allowed.
* Fix Summon Bullet's facing information for Cheese Hammer (was 8, should be 255).

[#17]: https://github.com/SpaceManiac/HamSandwich/pull/17
[missing text file]: https://steamcommunity.com/app/2547330/discussions/0/4040354734347124363/

### v9.2 (2023-10-17) - [Steam](https://store.steampowered.com/news/app/2547330/view/3705958414150337742)

* Add new "Advanced" HUD option with a level timer and weapon lock status (by AutomaticFrenzy, [#15]).
* Make volume multiplicative instead of subtractive, so lower volumes doesn't cause quiet sounds to be fully muted.
* Fix issue where repeated sounds like AK-8087 fire could be [cut off too soon] and effectively silenced.
* Fix rare possibility of Traffic [phasing through an obstacle at the very start of the level][phase].
* Fix playable Dr. Lunatic sometimes firing one too few lightning zaps on hit.
* Fix Bestiary in SpisMall being [inescapable on gamepad]. The Back button now quits it.

[#15]: https://github.com/SpaceManiac/HamSandwich/pull/15
[cut off too soon]: https://steamcommunity.com/app/2547330/discussions/0/3884975615298288712/
[phase]: https://steamcommunity.com/app/2547330/discussions/0/3884975615298288712/
[inescapable on gamepad]: https://steamcommunity.com/app/2547330/discussions/0/3884975231686597346/

### v9.1 (2023-09-23) - [Steam](https://store.steampowered.com/news/app/2547330/view/3723970276367701832)

* (Steam) Add new Top Time online leaderboards to complement Top Score
* Add Record below Total Score on the level tally for comparison, like the other factors
* Fix Classic HUD option applying incorrectly to the SpisMall
* Fix sounds sometimes being cut off even though there are still free channels
* Fix a Linux case-sensitivity bug

### v9.0 (2023-09-18) - [Steam](https://store.steampowered.com/news/app/2547330/view/6738003788163234037)

* Show HUD Stealth eye in yellow when the player is not concealed
* Add Classic HUD option to the pause menu
* Add full gamepad support to the menus
* Fix some levels missing music due to `006cavernboss.ogg` bug
* Fix missing messages when playing some legacy Dr. Lunatic worlds
* Replace contents of Internet menu with some links that actually work

Editor features:
* Improve rounding behavior when entering times for "Timed" and "Delayed" special triggers
* Improve accuracy of automatic brain and candle counting in some cases
* Add "Save & Export" menu to create `.zip` files containing your world and all required files
    * <kbd>Shift+Q</kbd> now includes all the extra detail in `req_files.txt`
* Fix soft-lock on pressing <kbd>F1</kbd> twice in the Item Editor
* Fix some art errors in the default tiles for new worlds (does not affect existing worlds)

### HamSandwich (2019–2023)

* Add <kbd>Page Up</kbd> and <kbd>Page Down</kbd> hotkeys to scroll world select
* Speed up World Select load times, improve sorting, and fix a crash if there are zero worlds
* Fix rare crash when using Summon/Change Bullet with Scanner Shot, Scanner Lock, or Megabeam
* Fix "Monster Sprites" effect persisting if a monster ID is reused
* Allow cancelling profile creation, except on first run
* Automatically pause the game if you tab out while playing

Editor features:
* Increase per-map special limit to 250, up from 128
* Add Zoom Out option (hotkey: <kbd>F2</kbd>) to View menu that increases the resolution while editing
* Add "Jump to" button to Goto/Win Level effects
* Allow using the mouse wheel to scroll the File and World menus
* Allow using the mouse wheel to scroll the Special editor
    * Color the Up/Down buttons differently if there are triggers or effects off-screen

### Unreleased pre-HamSandwich (2014–2018)

* Add Oxygen Meter level flag
* Add Bullet In Rect special trigger
* Fix non-profiles showing up in the profile menu
* Expand floor tool from 4 tiles to 8
* When switching maps in the editor, place the camera on Bouapha
* Fix memory corruption if the Delete Special effect deletes a used-up special

### v8.0d (2012-10-01) - [Blog](https://hamumu.com/blog.php?entry=1349133281) - [Download](https://hamumu.itch.io/dr-lunatic-supreme-with-cheese)

* Fixed "Megabeam Endo" bullet to be named "Megabeam End"
* Added debugging display activatable via <kbd>F3</kbd>
* Added Shadowless Wall: set a wall's side tile to be Transparent Roof
* Added Change Bullet special effect
* Added note on Summon Bullet regarding bullet facing

### v8.0c (2012-07-01)

* Fixed selecting bogus negative Special effects.
* Fixed bad random values, occasionally causing crashes.
* Removed console window that was inadvertently added in 8.0b.
* Added ability to Summon Bullet from the tagged monster's location.
* Fixed coordinate picking visually updating the wrong trigger or special when pages are used.
* Added ability to count Orbiters and Orbit Bombers via "Have Item" trigger.
* Fixed crash on attempting to recover from a failed JSP load.
* Refactored much code to allow for Linux compatability.
* Added "A" arctan operator and "S" square-root operator.
* Renamed Visual Shock to Zap Wand Shock.
* Restored automatic down-conversion of truecolor bitmaps to 8-bit bitmaps when using "Show Pic/Movie."
* Fixed music cutting off a second or two early.

### v8.0b (2012-06-07)

* Added date and clock special variables (operates on local time):
    * DM - Current month (January is 0)
    * DD - Current day
    * DY - Current year
    * DW - Current weekday (Sunday is 0)
    * CH - Current hour
    * CM - Current minute
    * CS - Current second
* Added Summon Bullet effect with selectable type, location, and mathable facing.
* Updated level and world scans to account for new special effets.
* Cleaned up some graphics code in an attempt to reduce places issues might arise.

### v8.0a (2012-05-24) - [Blog](https://hamumu.com/blog.php?entry=1337832568) - [Photo album](https://imgur.com/a/qnseO#0)

* Add "big" option to Show Message effect
* Allow up to 7 triggers and 31 effects on a single special, up from 5 each
* Add Wavy level flag
* Add Bubbles item flag
* Add Monster Sprites effect, to use a custom JSP for a single monster
* Add Item Sprites effect, to use a custom JSP for items
* Add Variable Bar effect, to show a customizable bar below player health
    * BV - current value
    * BM - max value
* Add dummy playable character and vehicle monsters to Body Parts category

### v7.8 (2006)

## Sleepless Hollow

### HamSandwich (2019–present)

* Entirely replace Internet menu with Editor and make it always available
* Update Summon Bullet effect for Sleepless Hollow's bullet set
* In the editor's Monster tool, set the default item drop to None
* Allow cancelling profile creation, except on first run
* Automatically pause the game if you tab out while playing

Editor features:
* Add "Jump to" button to Goto/Win Level effects
* Allow using the mouse wheel to scroll the File and World menus
* Allow using the mouse wheel to scroll the Special editor
    * Color the Up/Down buttons differently if there are triggers or effects off-screen

### Unreleased pre-HamSandwich (2012)

* Disallow placement of monsters in the AI Only category

### v2.0d (2012-10-31) - [Blog](https://hamumu.com/blog.php?entry=1351698249) - [Download](https://hamumu.itch.io/sleepless-hollow)

* Add more monster notes, and a progress indicator
* Fix resetting progress to clear all 64 levels in case of custom worlds
* Hide Editor on main menu until the game is completed once
* Allow add-on worlds to override certain hardcoded behaviors:
    * `bonecandle` - set to 0 to disable a Tomb of Bones candle (default: 1)
    * `journal` - set to the item ID of the journal item (default: 150)
    * `electroreel` - set to the item ID of the electroreel item (default: 186)
* Disallow saving over `hollow.shw`

### v2.0c (2012-10-28)

* Add more monster notes
* Allow add-on worlds to customize fishing
* Disallow loading a modified `hollow.shw`

### v2.0b (2012-10-13)

* Add some monster notes
* Allow add-on worlds to customize gallery goals
* Allow add-on worlds to customize the inventory section of the pause menu

### v2.0a (2012-10-08)

* Allow access to editor
* Allow customizing add-on title and journal pages with a `.txt` file
* Add add-on world chooser when starting a profile if any are present
* Port new editor features from Supreme With Cheese 8.0d
* Categorize monsters in the editor according to their presence in Sleepless Hollow
* Add Death Turrets, Missile Turrets, and Yerfdog

### v1.9 (2008)

## Dr. Lunatic

### HamSandwich (2019–present)

* Restart the main menu music when exiting the editor
* Allow using the arrow keys to scroll around in the editor

### Unreleased pre-HamSandwich (2012)

* Show special ID numbers in the editor

### v3.0 (2011-10-11) - [Blog](https://hamumu.com/blog.php?entry=1318300063) - [Source code blog](https://hamumu.com/blog.php?entry=1318711596) - [Download](https://hamumu.itch.io/dr-lunatic)

* Fix graphical glitches by implementing 8-bit to 32-bit conversion in software
* Add Smooth Lighting option, backported from Supreme With Cheese
* Add Disco Mode option, backported from Supreme with Cheese

### v2.2 (2001)

## Kid Mystic

### HamSandwich (2019–present)

* Stop the intro music early if skipped
* Stop the main menu music when entering the editor
* Show special ID numbers in the editor

### Cheat Edition (2009-07-07)

* Add <kbd>e</kbd> hotkey on the main menu to open the editor

### v3.7 (2004) - [Download](https://hamumu.itch.io/kid-mystic)

## Loonyland: Halloween Hill

### [Steam edition](https://store.steampowered.com/app/2876900/Loonyland_Halloween_Hill/) special features

* Show off your Merit Badges with Steam Achievements
* Automatically transfer your saves across devices with Steam Cloud
* Save Halloween Hill wherever you are with a native Steam Deck build
* Compete in definitely-fair [Leaderboards on the Steam Community][llscore]

[llscore]: https://steamcommunity.com/stats/2876900/leaderboards/13198878

### HamSandwich (since last release)

* Fix Surprise Mode making game unwinnable due to removing Steam Pumps during the Evilizer fight.
* Fix Enemy Overload claiming to apply to new games only when it actually applies immediately.

### v3.1 (2024-06-08)

* Add Steam leaderboard support to Space Game
* Fix Survival and Boss Bash not having silver weapons (since v3.0)
* Fix Farley and summons not working on Castle Vampy II due to the monster limit
* Fix Larry's postgame dialogue
* Fix Farley vanishing if you load his area before acquiring him
* Fix issues when turning the Sidekick badge back off, including 100% becoming impossible
* Fix single-frame flicker in Thorns Aura animation
* Fix soft-lock if Molecular Dispersion is used to interrupt the Last Gate animation
* Fix Space Game being unquittable on controller
* Fix Mystery Lake sometimes appearing when placing the 3rd orb if the 4th orb is in hand and Quick Mode is on
* Fix overlap between Spcl and tag text in editor

### v3.0 (2024-05-31)

* Add Randomizer mode (by AutomaticFrenzy)
* Add Hard difficulty between Normal and Challenge (by AutomaticFrenzy)
* Allow selecting character on the New Game screen (by Hyperme, [#19])
* Increase save slots from 5 to 250 and show more information in the list
* While Weapon Lock is on, weapon pickups pass through you, and you can use a new third control to grab them
* Show invincible guys in aqua on the Radar and improve its accuracy on small maps
* For quests that get "completed" before being turned in, indicate this status in the menu
* Improve decoration on the first floor of Haunted Tower (by Jamul)
* Fix difficult-to-access Gangrene in A Hidey Hole (by Jamul)
* Fix Hardcore Mode not working correctly
* Fix Witchcraft badge to award at level 9, matching the description, instead of level 10
* Fix Vampire Slayin' badge description to say two minutes instead of one, matching award
* Improve controller support
* Add volume option

### HamSandwich (2019–2024)

* Allow using the arrow keys to scroll around in the editor
* ~~Show special ID numbers in the editor~~ (removed in v3.1)

[#19]: https://github.com/SpaceManiac/HamSandwich/pull/19

### Editor (2009-07-07) - [Download](https://hamumu.itch.io/loonyland-halloween-hill)

* Add "Editor" option to main menu

### v2.0 (2004) - [Download](https://hamumu.itch.io/loonyland-halloween-hill)

## Loonyland 2: Winter Woods

### HamSandwich (2019–present)

* Fix high Stone Spike damage numbers wrapping to low again
* Fix player-aligned melee monsters (especially Bruisers) hitting themselves
* Fix high-level Restoration invincibility time and healing wrapping to low again
* Allow using the arrow keys to scroll around in the editor
* Fix overlap between Spcl and tag text in editor
* Fix typing into the new save Name field while it's not selected

### Unknown pre-HamSandwich

* Fix high-level Training's bonus XP overflowing and becoming useless

### v1.2CE (2007) - [Download](https://hamumu.itch.io/loonyland-2-winter-woods)

## Stockboy

### HamSandwich (2022–present)

* Disable the internet addon downloading feature, since the server is long-gone
* Unlock the "Add-On Level" menu automatically when the "All Stockboy add-ons" pack is enabled

### v1.5 (June 2004) - [Download](https://hamumu.itch.io/stockboy)

## Spooky Castle

### HamSandwich (2022–present)

* Allow using the arrow keys to scroll around in the editor

### Re-release (May 2004) - [Download](https://hamumu.itch.io/spooky-castle)

## Eddie Galaxy

### HamSandwich (2022–present)

### Re-release (June 2004) - [Blog](https://hamumu.com/blog.php?yr=2004&mn=6&entry=1088030171) - [Download](https://hamumu.itch.io/eddie-galaxy)

## Amazin' SPISPOPD

### HamSandwich (2023–present)

* Add editor access by pressing <kbd>e</kbd> on the main menu
* The HamSandwich code was reverse-engineered from the released game with Hamumu's permission

### v2.0 (October 1999) - [Blog](https://web.archive.org/web/19991127183914/http://www.hamumu.com:80/journal.html) - [Download](https://hamumu.itch.io/amazin-spispopd)
