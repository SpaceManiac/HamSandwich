package com.platymuus.hamsandwich;

import android.content.res.AssetManager;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class Launcher {

	public final ArrayList<Game> games = new ArrayList<>();
	public Game currentGame;
	public boolean wantsToPlay = false;

	public final File installersDir;

	public Launcher(File internalStorage, AssetManager assets) throws IOException, JSONException {
		installersDir = new File(internalStorage, "installers");
		//noinspection ResultOfMethodCallIgnored
		installersDir.mkdirs();

		String launcherJsonString;
		try (InputStream inputStream = assets.open("launcher.json")) {
			ByteArrayOutputStream result = new ByteArrayOutputStream();
			byte[] buffer = new byte[16 * 1024];
			for (int length; (length = inputStream.read(buffer)) != -1; ) {
				result.write(buffer, 0, length);
			}
			launcherJsonString = result.toString("UTF-8");
		}

		JSONObject metadata = new JSONObject(launcherJsonString);
		JSONArray project_list = metadata.getJSONArray("project_list");
		for (int i = 0; i < project_list.length(); ++i) {
			String id = project_list.getString(i);
			games.add(new Game(installersDir, id, metadata.getJSONObject("project_metadata").getJSONObject(id)));
		}
	}
}
