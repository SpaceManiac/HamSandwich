package com.platymuus.hamsandwich;

import android.content.Context;

public interface UiThreadHandle {
	Context getApplicationContext();
	void runOnUiThread(Runnable runnable);
}
