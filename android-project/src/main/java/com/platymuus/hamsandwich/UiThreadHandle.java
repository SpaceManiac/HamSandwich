package com.platymuus.hamsandwich;

public interface UiThreadHandle {
	void runOnUiThread(Runnable runnable);

	void someDownloadFinished();
}
