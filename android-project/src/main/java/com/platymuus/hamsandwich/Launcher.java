package com.platymuus.hamsandwich;

import android.content.res.AssetManager;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class Launcher {

	public static class Asset {
		public String mountpoint, kind, filename, sha256sum, link;
		public long file_id;
		public String description;
		public boolean required;
		// TODO: enabled setting, download progress
	}

	public static class Game {
		public String id;
		public String title;
		public boolean excluded;

		public final ArrayList<Asset> assets = new ArrayList<>();
		// TODO: icon
	}

	public final ArrayList<Game> games = new ArrayList<>();
	public Game currentGame = null;
	public boolean wants_to_play = false;

	public Launcher(AssetManager assets) throws IOException, JSONException {
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
			JSONObject project = metadata.getJSONObject("project_metadata").getJSONObject(id);
			Game game = new Game();
			games.add(game);
			game.id = id;
			game.title = project.getString("title");
			game.excluded = project.has("excluded") && project.getBoolean("excluded");
			// TODO: icon
			JSONArray installers = project.getJSONArray("installers");
			for (int j = 0; j < installers.length(); ++j) {
				JSONObject installer = installers.getJSONObject(j);
				Asset asset = new Asset();
				game.assets.add(asset);
				asset.mountpoint = installer.has("mountpoint") ? installer.getString("mountpoint") : "";
				asset.kind = installer.getString("kind");
				asset.filename = installer.getString("filename");
				asset.sha256sum = installer.getString("sha256sum");
				asset.link = installer.getString("link");
				asset.file_id = installer.getLong("file_id");
				asset.description = installer.getString("description");
				asset.required = !installer.has("optional") || !installer.getBoolean("optional");
				// TODO: enabled setting
			}
		}
	}
}
