include ../../external/SDL2/Android.mk

# Get .png and .jpg support from STB
USE_STBIMAGE := true
SUPPORT_AVIF := false
SUPPORT_JPG := false
SUPPORT_JXL := false
# SUPPORT_PNG is specific to libpng; we want USE_STBIMAGE
SUPPORT_PNG := false
SUPPORT_WEBP := false
include ../../external/SDL2_image/Android.mk

SUPPORT_WAV := true
SUPPORT_FLAC_DRFLAC := false
SUPPORT_FLAC_LIBFLAC := false
# Get .ogg support from STB
SUPPORT_OGG_STB := true
# Not from tremor + libogg
SUPPORT_OGG := false
SUPPORT_MP3_MINIMP3 := false
SUPPORT_MP3_MPG123 := false
SUPPORT_WAVPACK := false
SUPPORT_MOD_XMP := false
SUPPORT_MID_TIMIDITY := false
include ../../external/SDL2_mixer/Android.mk
