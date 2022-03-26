package com.platymuus.hamsandwich;

import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.gms.security.ProviderInstaller;

import org.libsdl.app.SDL;
import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.IOException;

public class LauncherActivity extends AppCompatActivity implements UiThreadHandle {

	private ActionBar actionBar;
	private Launcher launcher;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_launcher);

		// TODO: run this in a background thread or something
		try {
			ProviderInstaller.installIfNeeded(getApplicationContext());
		} catch (Exception e) {
			// TODO: do the good exception handling like https://developer.android.com/training/articles/security-gms-provider.html#example_sync
			e.printStackTrace();
		}

		actionBar = getSupportActionBar();
		assert actionBar != null;

		try {
			launcher = new Launcher(getApplicationContext().getFilesDir(), getApplicationContext().getAssets());

			// Populate game picker
			ViewGroup pageCollection = findViewById(R.id.page_collection);
			ViewGroup gameListPage = findViewById(R.id.game_list_page);
			for (Game game : launcher.games) {
				final Game thisGame = game;

				// Enter the game in the list on the first page.
				View fragment = getLayoutInflater().inflate(R.layout.fragment_list_game, gameListPage, false);
				Button button = fragment.findViewById(R.id.choose_game_button);
				button.setText(game.title);
				button.setOnClickListener(view -> selectGame(thisGame));
				gameListPage.addView(fragment);

				// Give the game its own page.
				game.setPage(getLayoutInflater().inflate(R.layout.fragment_game_page, pageCollection, false));
				game.getPage().setVisibility(View.GONE);
				game.getPage().findViewById(R.id.play_button).setOnClickListener(view -> playGameClicked(thisGame));
				ViewGroup assetContainer = game.getPage().findViewById(R.id.asset_container);
				for (Asset asset : game.assets) {
					View assetFragment = getLayoutInflater().inflate(R.layout.fragment_asset, assetContainer, false);
					asset.checkbox = assetFragment.findViewById(R.id.asset_checkbox);
					asset.checkbox.setText(asset.description);
					if (asset.required) {
						asset.checkbox.setChecked(true);
						asset.checkbox.setEnabled(false);
					} else {
						asset.checkbox.setChecked(asset.getEnabledFile().exists());
						asset.checkbox.setOnCheckedChangeListener((view, state) -> {
							asset.setEnabledFileTo(state);
							thisGame.calculateReadyToPlay(launcher.wantsToPlay);
						});
					}
					asset.setButton(assetFragment.findViewById(R.id.asset_button));
					asset.getButton().setOnClickListener(view -> asset.buttonClicked(this));
					assetContainer.addView(assetFragment);
				}
				game.calculateReadyToPlay(false);
				pageCollection.addView(game.getPage());
			}

			if (launcher.games.size() == 1) {
				// There's only one game, so skip straight to its page.
				Game game = launcher.games.get(0);
				selectGame(game);

				// If there are no optional assets, don't wait for user input.
				boolean allGood = true;
				for (Asset asset : game.assets) {
					if (!asset.required) {
						allGood = false;
						break;
					}
				}
				if (allGood) {
					playGameClicked(game);
				}
			} else {
				// Multiple games, so show the picker on launch.
				returnToGamePicker();
			}
		} catch (Exception e) {
			e.printStackTrace();
			new AlertDialog.Builder(this)
				.setTitle("HamSandwich launcher error")
				.setMessage(e.toString())
				.setPositiveButton("Quit", (dialog, i) -> finish())
				.show();
		}
	}

	@Override
	public boolean onOptionsItemSelected(@NonNull MenuItem item) {
		// Callback for action bar Back button
		returnToGamePicker();
		return true;
	}

	@Override
	public void onBackPressed() {
		if (launcher.games.size() == 1 || findViewById(R.id.game_list_page).getVisibility() == View.VISIBLE) {
			super.onBackPressed();
		} else {
			returnToGamePicker();
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		launcher.wantsToPlay = false;
		if (launcher.currentGame != null)
			launcher.currentGame.calculateReadyToPlay(false);
	}

	private void showPage(View page) {
		ViewGroup collection = findViewById(R.id.page_collection);
		for (int i = 0; i < collection.getChildCount(); ++i) {
			View child = collection.getChildAt(i);
			child.setVisibility(child == page ? View.VISIBLE : View.GONE);
		}
	}

	private void returnToGamePicker() {
		actionBar.setDisplayHomeAsUpEnabled(false);
		actionBar.setTitle("HamSandwich");
		showPage(findViewById(R.id.game_list_page));
	}

	private void selectGame(@NonNull Game game) {
		launcher.currentGame = game;
		actionBar.setDisplayHomeAsUpEnabled(launcher.games.size() != 1);
		actionBar.setTitle(game.title);
		showPage(game.getPage());
	}

	private void playGameClicked(@NonNull Game game) {
		if (launcher.wantsToPlay) {
			launcher.wantsToPlay = false;
		} else if (game.startMissingDownloads(this)) {
			launcher.wantsToPlay = true;
		} else {
			playGameActually(game);
		}
		game.calculateReadyToPlay(launcher.wantsToPlay);
	}

	private void playGameActually(@NonNull Game game) {
		launcher.wantsToPlay = false;
		Intent intent = new Intent(getApplicationContext(), HamSandwichActivity.class);
		intent.putExtra("game", game.id);

		int i = 0;
		String internal = new File(getApplicationContext().getFilesDir(), "appdata/" + game.id).getAbsolutePath();
		if ("mounted".equals(Environment.getExternalStorageState())) {
			intent.putExtra("ENV.HSW_APPDATA", new File(getApplicationContext().getExternalFilesDir(null), "appdata/" + game.id).getAbsolutePath());
			intent.putExtra("ENV.HSW_ASSETS_" + (i++), "@stdio@" + internal);
		} else {
			intent.putExtra("ENV.HSW_APPDATA", internal);
		}
		for (Asset asset : game.assets) {
			if (asset.required || asset.checkbox.isChecked()) {
				intent.putExtra("ENV.HSW_ASSETS_" + (i++), asset.mountpoint + "@" + asset.kind + "@" + asset.file.getAbsolutePath());
			}
		}
		// Sentinel to end iteration in case we go from more assets to fewer.
		// Not 100% sure this is needed? But just in case.
		intent.putExtra("ENV.HSW_ASSETS_" + i, "");

		startActivity(intent);
	}

	public void someDownloadFinished() {
		if (launcher.currentGame == null)
			return;

		launcher.currentGame.calculateReadyToPlay(launcher.wantsToPlay);
		if (launcher.currentGame.isReadyToPlay() && launcher.wantsToPlay)
			playGameActually(launcher.currentGame);
	}
}
