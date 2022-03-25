package com.platymuus.hamsandwich;

import android.os.Bundle;

import org.libsdl.app.SDLActivity;

public class HamSandwichActivity extends SDLActivity {

	private String game;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		game = getIntent().getStringExtra("game");
		if (game == null) {
			finish();
			return;
		}

		super.onCreate(savedInstanceState);
	}

	@Override
	protected String[] getLibraries() {
		return new String[]{
			"SDL2",
			"SDL2_image",
			"SDL2_mixer",
			game
		};
	}

}
