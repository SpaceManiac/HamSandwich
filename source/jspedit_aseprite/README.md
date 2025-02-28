# JspEdit extension for Aseprite

<a href="https://spacemaniac.itch.io/hamsandwich">
<img alt="Available on itch.io" src="../../docs/available-on-itch.svg" height="100" align="right">
</a>

This is an extension for [Aseprite](https://www.aseprite.org/) that adds import and export support for `.jsp` sprite packs and `.jft` fonts.

To install, download "**jspedit.aseprite-extension**" from [itch.io](https://spacemaniac.itch.io/hamsandwich). If it doesn't associate with Aseprite automatically, use **Edit > Preferences > Extensions > Add Extension**.

To install from source, copy this directory into the Aseprite extension directory:

1. **Edit > Preferences > General > Locate Configuration File**
2. Enter or create the `extensions` directory.
3. Copy this directory there, such that `extensions/jspedit_aseprite/package.json` exists.
4. Restart Aseprite.

## Palette

The plugin adds a preset palette named "**Dr. Lunatic**" to the preset palette list. This should generally be used when authoring JSP and JFT files. Exporting will convert the image to this palette.

## Import JSP

Location: **File > Import > Import JSP or JFT**

Once imported, the origin will be indicated by the **center** of the green slice.

## Import JFT

Location: **File > Import > Import JSP or JFT**

## Export JSP

Location: **File > Export > Export JSP**

Only visible layers will be exported.

The slice tool (<kbd>Shift+C</kbd>) can be used to create a slice to set as the
origin point. The origin point of the JSP file will be the **center**
of the selected slice on the **first frame**, due to Aseprite API limitations.

## Export JFT

Location: **File > Export > Export JFT**

Only visible layers will be exported.

* First character code: The decimal ASCII code of the character in the first frame.
  * Usually `33` for `!`.
  * Following frames will be assigned to the next ASCII character (`"`, then `#`, and so on) until the last frame of the sprite.
  * The maximum number of characters in a font is 128.
* Space width: the number of pixels wide a space should be.
* Gap between characters: the number of blank pixels between adjacent characters.
* Line height: the number of pixels to descend when going to a new line.
  * Usually a bit more than the height of the sprite.
  * Not all game screens use this number - it may be hardcoded in some places.

The maximum character sprite size is 255x255. The maximum space width, gap between characters, and line height is 255.
