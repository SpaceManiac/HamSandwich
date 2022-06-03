package com.platymuus.hamsandwich;

import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

import org.libsdl.app.SDLActivity;

public class HamSandwichActivity extends SDLActivity {

	private String game;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		game = getIntent().getStringExtra("game");
		if (game == null) {
			finish();
			return;
		}

		super.onCreate(savedInstanceState);

		if (!mBrokenLibraries) {
			for (String key : getIntent().getExtras().keySet()) {
				if (key.startsWith("ENV.")) {
					SDLActivity.nativeSetenv(key.substring("ENV.".length()), getIntent().getExtras().getString(key));
				}
			}
		}
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
