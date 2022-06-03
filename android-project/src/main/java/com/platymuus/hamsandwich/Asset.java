package com.platymuus.hamsandwich;

import android.widget.Button;
import android.widget.CheckBox;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class Asset {
	public final String mountpoint, kind, filename, sha256sum, link;
	public final long file_id;
	public final String description;
	public final boolean required;

	public final File file;

	public CheckBox checkbox;
	private Button button;

	private boolean downloading = false;
	private boolean wantsCancel = false;

	public Asset(File installersDir, JSONObject installer) throws JSONException {
		mountpoint = installer.has("mountpoint") ? installer.getString("mountpoint") : "";
		kind = installer.getString("kind");
		filename = installer.getString("filename");
		sha256sum = installer.getString("sha256sum");
		link = installer.getString("link");
		file_id = installer.getLong("file_id");
		description = installer.getString("description");
		required = !installer.has("optional") || !installer.getBoolean("optional");

		file = new File(installersDir, filename);
	}

	private File getPartFile() {
		return new File(file.getPath() + ".part");
	}

	public File getEnabledFile() {
		return new File(file.getPath() + ".enabled");
	}

	public boolean isDownloading() {
		return downloading;
	}

	public void setButton(Button button) {
		this.button = button;
		if (file.exists()) {
			button.setText("Delete");
		} else {
			button.setText("Download");
		}
	}

	public Button getButton() {
		return button;
	}

	public void buttonClicked(UiThreadHandle uiThread) {
		// Maybe racy but whatever for now
		if (downloading) {
			wantsCancel = true;
		} else if (file.exists()) {
			if (file.delete()) {
				button.setText("Download");
			}
		} else {
			startDownload(uiThread);
		}
	}

	public void startDownload(UiThreadHandle uiThread) {
		if (downloading)
			return;
		downloading = true;

		new Thread(() -> {
			HttpURLConnection connection = null;
			try {
				byte[] buffer = new byte[16 * 1024];

				// Download metadata
				uiThread.runOnUiThread(() -> button.setText("Starting..."));
				JSONObject metadata;
				{
					URL metadataUrl = new URL(link + "/file/" + file_id);
					connection = (HttpURLConnection) metadataUrl.openConnection();
					connection.setRequestMethod("POST");
					InputStream inputStream = connection.getInputStream();

					ByteArrayOutputStream result = new ByteArrayOutputStream();
					for (int read; (read = inputStream.read(buffer)) != -1; ) {
						result.write(buffer, 0, read);
						if (wantsCancel)
							return;
					}
					connection.disconnect();
					connection = null;

					metadata = new JSONObject(result.toString("UTF-8"));
				}

				if (wantsCancel)
					return;

				// Download content
				URL contentUrl = new URL(metadata.getString("url"));
				connection = (HttpURLConnection) contentUrl.openConnection();
				long contentLength;
				if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.N) {
					contentLength = connection.getContentLengthLong();
				} else {
					contentLength = connection.getContentLength();
				}
				InputStream inputStream = connection.getInputStream();

				long amountRead = 0;
				File partFile = getPartFile();
				try (FileOutputStream file = new FileOutputStream(partFile)) {
					for (int read; (read = inputStream.read(buffer)) != -1; ) {
						file.write(buffer, 0, read);
						amountRead += read;
						long percent = amountRead * 100 / contentLength;
						uiThread.runOnUiThread(() -> button.setText(percent + "%"));
						if (wantsCancel)
							return;
					}
				}
				connection.disconnect();
				connection = null;

				// Commit the rename only at the very end.
				assert partFile.renameTo(file);
				uiThread.runOnUiThread(() -> button.setText("Delete"));
				uiThread.runOnUiThread(uiThread::someDownloadFinished);
			} catch (Exception e) {
				uiThread.runOnUiThread(() -> button.setText("Errored"));
				e.printStackTrace();
			} finally {
				if (connection != null) {
					connection.disconnect();
				}
				downloading = false;
			}
		}).start();
	}

	@SuppressWarnings("ResultOfMethodCallIgnored")
	public void setEnabledFileTo(boolean state) {
		try {
			if (state) {
				getEnabledFile().createNewFile();
			} else {
				getEnabledFile().delete();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
