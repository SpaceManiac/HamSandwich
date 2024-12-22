package com.platymuus.aegea;

import android.util.Base64;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.Socket;
import java.net.URI;
import java.net.URISyntaxException;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class WebSocket {

    private static native void onConnect(long handle);

    private static native void onMessage(long handle, boolean text, byte[] data, int offset, int len);

    private static native void onError(long handle, String error);

    private static native void onClose(long handle, int reason);

    private long handle;
    private final ConcurrentLinkedQueue<Runnable> callbacks = new ConcurrentLinkedQueue<>();
    private final HttpURLConnection conn;
    private boolean canWrite;

    public WebSocket(long handle, String urlString) throws URISyntaxException, IOException, NoSuchAlgorithmException {
        this.handle = handle;

        Socket socket;
        URI uri = new URI(urlString);

        Log.d("Aegea", "uri ok: " + uri);

        if (uri.getScheme().equals("ws")) {
            URI uri2 = new URI("http", uri.getUserInfo(), uri.getHost(), uri.getPort(), uri.getPath(), uri.getQuery(), uri.getFragment());
            Log.d("Aegea", "uri2 ws->http ok: " + uri2);
            conn = (HttpURLConnection) uri2.toURL().openConnection();
        } else if (uri.getScheme().equals("wss")) {
            URI uri2 = new URI("https", uri.getUserInfo(), uri.getHost(), uri.getPort(), uri.getPath(), uri.getQuery(), uri.getFragment());
            Log.d("Aegea", "uri2 wss->https ok: " + uri2);
            conn = (HttpURLConnection) uri2.toURL().openConnection();
        } else {
            throw new URISyntaxException(urlString, "WebSocket URL must be ws:// or wss://");
        }
        Log.d("Aegea", "conn ok");

        byte[] nonce = new byte[16];
        new SecureRandom().nextBytes(nonce);

        conn.setRequestProperty("Upgrade", "websocket");
        conn.setRequestProperty("Connection", "Upgrade");
        conn.setRequestProperty("Sec-WebSocket-Key", Base64.encodeToString(nonce, Base64.DEFAULT));
        conn.setRequestProperty("Sec-WebSocket-Version", "13");
        //conn.setDoInput(true);
        //conn.setDoOutput(true);

        Log.d("Aegea", "conn setup");
        new Thread(() -> this.threadMain(conn), "WebSocket: " + urlString).start();
    }

    public void dispose() {
        handle = 0;
    }

    public void recv() {
        Runnable poll = callbacks.poll();
        while (poll != null) {
            poll.run();
            poll = callbacks.poll();
        }
    }

    public void send(boolean text, byte[] data) throws IOException {
        if (canWrite) {
            OutputStream os = conn.getOutputStream();
        } else {
            onError(handle, "send() too early");
        }
    }

    private void threadMain(HttpURLConnection conn) {
        try {
            conn.connect();
            int responseCode = conn.getResponseCode();
            if (responseCode != 101) {
                String message = conn.getResponseMessage();
                callbacks.add(() -> onError(handle, "HTTP " + responseCode + " " + message));
                return;
            }

            Log.d("Aegea", "upgraded to websocket ok");
        } catch (Exception e) {
            callbacks.add(() -> onError(handle, e.getMessage()));
        }
    }
}
