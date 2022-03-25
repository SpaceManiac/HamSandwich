package com.platymuus.hamsandwich;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

@SuppressWarnings("CharsetObjectCanBeUsed")
public class LauncherActivity extends AppCompatActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_launcher);

		String whatWeGot = "";
		try (InputStream stream = getApplicationContext().getAssets().open("test.txt")) {
			byte[] array = new byte[1024];
			int amount = stream.read(array);
			whatWeGot = new String(array, 0, amount, Charset.forName("UTF-8"));
		} catch (IOException e) {
			whatWeGot = e.toString();
		}

		((Button) findViewById(R.id.button)).setText(whatWeGot);

		findViewById(R.id.button).setOnClickListener(view -> {
			startActivity(new Intent(this, HamSandwichActivity.class));
		});
	}
}
