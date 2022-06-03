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
* Add global <kbd>F11</kbd> hotkey to toggle fullscreen mode, and make window resizeable
* Add global <kbd>F10</kbd> hotkey to toggle scaling mode between nearest-neighbor and linear filtering
* Allow games to optionally load resources directly from the official installer rather than extracting it first
* Add a proper close curly brace `}` character to the font used in the editors
* Port to new platforms
    * Linux
    * Android
        * Add onscreen touch joystick and controls
        * Add mapping so Android "back" button acts like pressing the Escape key
    * Web
        * Add backtick <kbd>`</kbd> hotkey to replace Escape in fullscreen
* Clean up source code, add flexibility, or fix compiler warnings in ways that should have no noticeable effect
    * Support 64-bit builds with full save and add-on compatibility
    * Combine each game's control, music, movie, font, sound, and sprite handling into a common "Ham" core

### [Source code blog](http://hamumu.com/blog.php?entry=1558391916)

## Dr. Lunatic Supreme With Cheese

### HamSandwich (2019–present)

* Fix crash if world select is empty
* Add <kbd>Page Up</kbd> and <kbd>Page Down</kbd> hotkeys to scroll world select
* Speed up the world select loading
* Add a tentative new world format that allows exceeding the 256 monster type and other limits
    * Worlds save in the old format unless they actually exceed these limits

Also applicable to Sleepless Hollow:
* Add "Jump to" button to Goto/Win Level effects
* Allow using the mouse wheel to scroll the File and World menus
* Allow using the mouse wheel to scroll the Special editor
    * Color the Up/Down buttons differently if there are triggers or effects off-screen
* Allow cancelling profile creation, except on first run
* Automatically pause the game if you tab out while playing

### Unreleased pre-HamSandwich (2014–2018)

* Add Oxygen Meter level flag
* Add Bullet In Rect special trigger
* Fix non-profiles showing up in the profile menu
* Expand floor tool from 4 tiles to 8
* When switching maps in the editor, place the camera on Bouapha
* Fix memory corruption if the Delete Special effect deletes a used-up special

### v8.0d (2012-10-01) - [Blog](http://hamumu.com/blog.php?entry=1349133281) - [Download](https://hamumu.itch.io/dr-lunatic-supreme-with-cheese)

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

### v8.0a (2012-05-24) - [Blog](http://hamumu.com/blog.php?entry=1337832568) - [Photo album](https://imgur.com/a/qnseO#0)

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

See "Also applicable to Sleepless Hollow" above.

### Unreleased pre-HamSandwich (2012)

* Disallow placement of monsters in the AI Only category

### v2.0d (2012-10-31) - [Blog](http://hamumu.com/blog.php?entry=1351698249) - [Download](https://hamumu.itch.io/sleepless-hollow)

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

### v3.0 (2011-10-11) - [Blog](http://hamumu.com/blog.php?entry=1318300063) - [Source code blog](http://hamumu.com/blog.php?entry=1318711596) - [Download](https://hamumu.itch.io/dr-lunatic)

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

### HamSandwich (2019–present)

* Allow using the arrow keys to scroll around in the editor
* Show special ID numbers in the editor

### Editor (2009-07-07) - [Download](https://hamumu.itch.io/loonyland-halloween-hill)

* Add "Editor" option to main menu

### v2.0 (2004) - [Download](https://hamumu.itch.io/loonyland-halloween-hill)

## Loonyland 2: Winter Woods

### HamSandwich (2019–present)

* Fix high Stone Spike damage numbers wrapping to low again
* Fix player-aligned melee monsters (especially Bruisers) hitting themselves
* Fix high-level Restoration invincibility time and healing wrapping to low again
* Allow using the arrow keys to scroll around in the editor
* Show special ID numbers in the editor

### Unknown pre-HamSandwich

* Fix high-level Training's bonus XP overflowing and becoming useless

### v1.2CE (2007) - [Download](https://hamumu.itch.io/loonyland-2-winter-woods)
