package com.platymuus.hamsandwich;

import android.view.View;

import java.io.File;
import java.util.ArrayList;

public class Game {
	public String id;
	public String title;
	public boolean excluded;

	public final ArrayList<Asset> assets = new ArrayList<>();

	public View page;
	// TODO: icon

	public boolean startMissingDownloads(UiThreadHandle uiThreadHandle) {
		boolean anyMissing = false;
		for (Asset asset : assets) {
			if (asset.required || asset.checkbox.isChecked()) {
				File file = new File("installers/" + asset.filename);
				if (!file.exists()) {
					asset.startDownload(uiThreadHandle);
					anyMissing = true;
				}
			}
		}
		return anyMissing;
	}

	public boolean isReadyToPlay() {
		boolean anyMissing = false;
		for (Asset asset : assets) {
			if (asset.required || asset.checkbox.isChecked()) {
				File file = new File("installers/" + asset.filename);
				if (!file.exists()) {
					return false;
				}
			}
		}
		return true;
	}
}
