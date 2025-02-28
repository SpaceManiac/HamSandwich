# Hamumu JSP support for Aseprite

## Palette

The plugin adds a preset palette named "**Dr. Lunatic**" to the preset palette list. This should generally be used when authoring JSP files.

## Import JSP

Location: **File > Import > Import JSP or JFT**

Once imported, the origin will be indicated by the **center** corner of the generated slice.

## Export JSP

Location: **File > Export > Export JSP**

Only visible layers will be exported.

The slice tool (<kbd>Shift+C</kbd>) can be used to create a slice to set as the
origin point. The origin point of the JSP file will be the **center**
of the selected slice on the **first frame**, due to Aseprite API limitations.
