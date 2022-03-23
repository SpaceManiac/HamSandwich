SUPPORT_PNG := true
SUPPORT_JPG := false
SUPPORT_WEBP := false
SUPPORT_OGG := true
SUPPORT_FLAC := false
SUPPORT_MP3_MPG123 := false
SUPPORT_MOD_MODPLUG := false
SUPPORT_MID_TIMIDITY := false

include ../../external/SDL2/Android.mk
include ../../external/SDL2_image/Android.mk
include ../../external/SDL2_mixer/Android.mk
