include ../../external/SDL2/Android.mk

USE_STBIMAGE := true  # supports png and jpg
SUPPORT_AVIF := false
SUPPORT_JPG := false
SUPPORT_JXL := false
SUPPORT_PNG := false  # specific to libpng
SUPPORT_WEBP := false
include ../../external/SDL2_image/Android.mk

SUPPORT_WAV := true
SUPPORT_FLAC_DRFLAC := false
SUPPORT_FLAC_LIBFLAC := false
SUPPORT_OGG_STB := true
SUPPORT_OGG := false  # specifically tremor + libogg
SUPPORT_MP3_MINIMP3 := false
SUPPORT_MP3_MPG123 := false
SUPPORT_WAVPACK := false
SUPPORT_MOD_XMP := false
SUPPORT_MID_TIMIDITY := false
include ../../external/SDL2_mixer/Android.mk
