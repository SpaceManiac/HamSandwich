package com.platymuus.hamsandwich;

import android.widget.CheckBox;

public class Asset {
	public String mountpoint, kind, filename, sha256sum, link;
	public long file_id;
	public String description;
	public boolean required;

	public CheckBox checkbox;
	// TODO: download progress
}
