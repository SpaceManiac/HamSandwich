package com.platymuus.aegea;

public class WebSocket {

    private static native void onConnect(long handle);

    private static native void onMessage(long handle, boolean text, byte[] data, int offset, int len);

    private static native void onError(long handle, String error);

    private static native void onClose(long handle, int reason);

    private long handle;

    public WebSocket(long handle, String url) {
        this.handle = handle;
        onConnect(handle);
    }

    public void dispose() {
        handle = 0;
    }

    public void recv() {
        onError(handle, "Big Meme");
    }

    public void send(boolean text, byte[] data) {
    }
}
