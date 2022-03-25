package com.platymuus.hamsandwich;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONException;

import java.io.IOException;

public class LauncherActivity extends AppCompatActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_launcher);

		Launcher launcher = null;
		try {
			launcher = new Launcher(getApplicationContext().getAssets());
			((TextView) findViewById(R.id.textView)).setText(launcher.toString());
		} catch (IOException e) {
			e.printStackTrace();
			((TextView) findViewById(R.id.textView)).setText(e.toString());
		} catch (JSONException e) {
			e.printStackTrace();
			((TextView) findViewById(R.id.textView)).setText(e.toString());
		}

		findViewById(R.id.button).setOnClickListener(view -> {
			startActivity(new Intent(this, HamSandwichActivity.class));
		});
	}
}
