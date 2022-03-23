package com.platymuus.hamsandwich;

import android.os.Bundle;

import org.libsdl.app.SDLActivity;

public class HamSandwichActivity extends SDLActivity {

	@Override
	protected String[] getLibraries() {
		return new String[]{
			"SDL2",
			"SDL2_image",
			"SDL2_mixer",
			"lunatic"
		};
	}

}
