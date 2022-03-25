package com.platymuus.hamsandwich;

import android.widget.Button;
import android.widget.CheckBox;

import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

public class Asset {
	public String mountpoint, kind, filename, sha256sum, link;
	public long file_id;
	public String description;
	public boolean required;

	public CheckBox checkbox;
	public Button button;

	private boolean downloading = false;

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
					}
					connection.disconnect();
					connection = null;

					metadata = new JSONObject(result.toString("UTF-8"));
				}

				// Download content
				URL contentUrl = new URL(metadata.getString("url"));
				connection = (HttpURLConnection) contentUrl.openConnection();
				int contentLength = connection.getContentLength();
				InputStream inputStream = connection.getInputStream();

				int amountRead = 0;
				File installers = new File(uiThread.getApplicationContext().getFilesDir(), "installers");
				//noinspection ResultOfMethodCallIgnored
				installers.mkdirs();
				File partFile = new File(installers, filename + ".part");
				try (FileOutputStream file = new FileOutputStream(partFile)) {
					for (int read; (read = inputStream.read(buffer)) != -1; ) {
						file.write(buffer, 0, read);
						amountRead += read;
						int percent = amountRead * 100 / contentLength;
						uiThread.runOnUiThread(() -> button.setText(percent + "%"));
					}
				}
				connection.disconnect();
				connection = null;

				// Commit the rename only at the very end.
				assert partFile.renameTo(new File(installers, filename));
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
}
