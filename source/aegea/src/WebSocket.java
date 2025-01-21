package com.platymuus.aegea;

import android.util.Base64;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.util.concurrent.ConcurrentLinkedQueue;

import javax.net.SocketFactory;
import javax.net.ssl.SSLSocketFactory;

public class WebSocket {

    private static native void onConnect(long handle);

    private static native void onMessage(long handle, boolean text, byte[] data, int offset, int len);

    private static native void onError(long handle, String error);

    private static native void onClose(long handle, int reason);

    private long handle;
    private final ConcurrentLinkedQueue<Runnable> callbacks = new ConcurrentLinkedQueue<>();
    private Socket socket;
    private OutputStream outputStream;

    public WebSocket(long handle, String urlString) throws URISyntaxException, IOException, NoSuchAlgorithmException {
        this.handle = handle;
        new Thread(() -> this.threadMain(urlString), "WebSocket: " + urlString).start();
    }

    public void dispose() {
        handle = 0;
        if (socket != null) {
            try {
                // Interrupts pending read()s with a SocketException
                socket.close();
            } catch (IOException ignored) {
            }
        }
    }

    public void recv() {
        Runnable poll = callbacks.poll();
        while (poll != null) {
            poll.run();
            poll = callbacks.poll();
        }
    }

    public void send(boolean text, byte[] data) throws IOException {
        if (outputStream == null) {
            // TODO: should be queued up
            onError(handle, "send() too early");
            return;
        }
        writeFrame(text ? OPCODE_TEXT : OPCODE_BINARY, data);
    }

    private void threadMain(String urlString) {
        try {
            URI uri = new URI(urlString);
            Log.d("Aegea", "uri ok: " + uri);
            if (uri.getScheme().equals("ws")) {
                int port = uri.getPort() == -1 ? 80 : uri.getPort();
                socket = SocketFactory.getDefault().createSocket(uri.getHost(), port);
            } else if (uri.getScheme().equals("wss")) {
                int port = uri.getPort() == -1 ? 443 : uri.getPort();
                socket = SSLSocketFactory.getDefault().createSocket(uri.getHost(), port);
            } else {
                callbacks.add(() -> onError(handle, "WebSocket URL must be ws:// or wss://"));
                return;
            }
            if (handle == 0) {
                socket.close();
                return;
            }

            byte[] nonce = new byte[16];
            new SecureRandom().nextBytes(nonce);

            InputStream in = socket.getInputStream();
            OutputStream out = socket.getOutputStream();

            StringBuilder httpRequest = new StringBuilder();
            httpRequest.append("GET ");
            httpRequest.append(uri.getRawPath() == null || uri.getRawPath().isEmpty() ? "/" : uri.getRawPath());
            if (uri.getRawQuery() != null && !uri.getRawQuery().isEmpty()) {
                httpRequest.append("?").append(uri.getRawQuery());
            }
            httpRequest.append(" HTTP/1.1\r\n");

            httpRequest.append("Host: ").append(uri.getHost());
            if (uri.getPort() != -1) {
                httpRequest.append(":").append(uri.getPort());
            }
            httpRequest.append("\r\n");

            httpRequest.append("Upgrade: websocket\r\n");
            httpRequest.append("Connection: Upgrade\r\n");
            httpRequest.append("Sec-WebSocket-Version: 13\r\n");
            httpRequest.append("Sec-WebSocket-Key: ").append(Base64.encodeToString(nonce, Base64.NO_WRAP)).append("\r\n");
            httpRequest.append("\r\n");

            out.write(httpRequest.toString().getBytes("ASCII"));
            Log.d("Aegea", "wrote HTTP request");

            ByteBuffer buffer = ByteBuffer.allocate(8 * 1024);
            buffer.order(ByteOrder.BIG_ENDIAN);
            /* Expected form of response:
                HTTP/1.1 101 Switching Protocols
                Upgrade: websocket
                Connection: Upgrade
                Sec-WebSocket-Accept: ...
             */
            boolean seenStatus = false, readingHeaders = true;
            while (readingHeaders) {
                // Fill the buffer
                Log.d("Aegea", "buffer: " + buffer.position() + ", " + buffer.limit() + ", " + buffer.capacity());
                if (buffer.position() == buffer.limit()) {
                    buffer = resize(buffer, buffer.capacity() * 2);
                }
                int read = in.read(buffer.array(), buffer.arrayOffset() + buffer.position(), buffer.remaining());
                if (handle == 0) {
                    socket.close();
                    return;
                }
                if (read < 0) {
                    callbacks.add(() -> onError(handle, "EOF reading HTTP response"));
                    return;
                }
                buffer.position(buffer.position() + read);

                // Drain the buffer
                buffer.flip();
                buffer.mark();
                while (buffer.remaining() >= 2) {
                    // Scan for \r\n
                    if (buffer.get(buffer.position()) == '\r' && buffer.get(buffer.position() + 1) == '\n') {
                        int lineEnd = buffer.position();
                        buffer.reset();
                        String line = new String(buffer.array(), buffer.arrayOffset() + buffer.position(), lineEnd - buffer.position(), "ASCII");
                        buffer.position(lineEnd + 2);
                        buffer.mark();

                        if (line.equals("")) {
                            // End of headers, break.
                            readingHeaders = false;
                        } else if (!seenStatus) {
                            // First status line
                            String[] bits = line.split(" ", 3);
                            int status = Integer.parseInt(bits[1]);
                            if (status != 101) {
                                callbacks.add(() -> onError(handle, "HTTP " + status + " " + bits[2]));
                                return;
                            }
                            seenStatus = true;
                        } else {
                            // TODO: validate Upgrade, Connection, and Sec-WebSocket-Accept
                            Log.d("Aegea", "header " + line);
                        }
                    } else {
                        buffer.position(buffer.position() + 1);
                    }
                }
                buffer.reset();
                buffer.compact();
            }

            // Permit writing and signal connectivity
            Log.d("Aegea", "upgraded to websocket ok");
            outputStream = out;
            callbacks.add(() -> onConnect(handle));

            while (true) {
                // Fill the buffer
                Log.d("Aegea", "buffer: " + buffer.position() + ", " + buffer.limit() + ", " + buffer.capacity());
                if (buffer.position() == buffer.limit()) {
                    buffer = resize(buffer, buffer.capacity() * 2);
                }
                int read = in.read(buffer.array(), buffer.arrayOffset() + buffer.position(), buffer.remaining());
                if (handle == 0) {
                    socket.close();
                    return;
                }
                if (read < 0) {
                    callbacks.add(() -> onError(handle, "EOF reading frames"));
                    return;
                }
                buffer.position(buffer.position() + read);

                // Drain the buffer
                buffer.flip();
                buffer.mark();
                while (buffer.remaining() >= 2) {
                    Log.d("Aegea", "remaining=" + buffer.remaining());

                    // https://datatracker.ietf.org/doc/html/rfc6455#section-5.2
                    // Fin flag, opcode, and payload len
                    byte b1 = buffer.get(), b2 = buffer.get();
                    boolean fin = (b1 & FIN) != 0;
                    int opcode = b1 & 0b1111;
                    if ((b1 & 0b111000) != 0) {
                        callbacks.add(() -> onError(handle, "Server set unknown extension bits"));
                        return;
                    }
                    boolean mask = (b2 & MASK) != 0;
                    int length = b2 & 0b1111111;
                    int wantHeaderRemaining = (length == 126 ? 2 : length == 127 ? 8 : 0) + (mask ? 4 : 0);
                    Log.d("Aegea", "fin=" + fin + " opcode=" + opcode + " mask=" + mask + " length=" + length + " wantHeaderRemaining=" + wantHeaderRemaining);
                    if (buffer.remaining() < wantHeaderRemaining) {
                        break;
                    }
                    // Extended payload length
                    if (length == LENGTH_SHORT) {
                        length = buffer.getShort();
                    } else if (length == LENGTH_LONG) {
                        long longLength = buffer.getLong();
                        if (longLength < 0) {
                            callbacks.add(() -> onError(handle, "Server sent malformed payload length"));
                            return;
                        } else if (longLength > Integer.MAX_VALUE) {
                            callbacks.add(() -> onError(handle, "Server sent too-big payload length"));
                            return;
                        }
                        length = (int) longLength;
                    }
                    // Masking key
                    int maskKey = 0;
                    if (mask) {
                        throw new UnsupportedOperationException("TODO server->client masking");
                        //maskKey = buffer.getInt();
                    }
                    Log.d("Aegea", "length=" + length + " maskKey=" + maskKey);
                    // Payload data
                    if (buffer.remaining() < length) {
                        break;
                    }

                    int payloadStart = buffer.position(), payloadEnd = payloadStart + length;
                    switch (opcode) {
                        case OPCODE_TEXT:
                        case OPCODE_BINARY: {
                            byte[] body = new byte[length];
                            buffer.get(body);
                            boolean isText = opcode == OPCODE_TEXT;
                            callbacks.add(() -> onMessage(handle, isText, body, 0, body.length));
                            break;
                        }
                        case OPCODE_PING: {
                            byte[] body = new byte[length];
                            buffer.get(body);
                            callbacks.add(() -> {
                                // TODO: this means update() does need to be pumped or else timeouts,
                                // a difference from other platforms.
                                writeFrame(OPCODE_PONG, body);
                            });
                            break;
                        }
                        case OPCODE_PONG:
                            // Just ignore it.
                            break;
                        case OPCODE_CLOSE: {
                            short reason = length > 2 ? buffer.getShort() : -1;
                            callbacks.add(() -> {
                                writeFrame(OPCODE_CLOSE, new byte[0]);
                                onClose(handle, reason);
                            });
                            return;
                        }
                        // TODO: OPCOCDE_CONTINUATION
                        default:
                            Log.d("Aegea", "unhandled opcode: " + opcode);
                            break;
                    }

                    buffer.position(payloadEnd);
                    buffer.mark();
                }
                buffer.reset();
                buffer.compact();
            }
        } catch (Exception e) {
            if (handle != 0) {
                callbacks.add(() -> onError(handle, e.toString()));
            }
        }
    }

    private static ByteBuffer resize(ByteBuffer src, int newLength) {
        ByteBuffer dest = ByteBuffer.allocate(newLength);
        src.flip();
        dest.put(src);
        return dest;
    }

    private void writeFrame(int opcode, byte[] payload) {
        byte[] maskingKey = new byte[4];
        new SecureRandom().nextBytes(maskingKey);

        ByteBuffer header = ByteBuffer.allocate(14);
        header.order(ByteOrder.BIG_ENDIAN);
        header.put((byte) (FIN | opcode));
        if (payload.length >= LENGTH_LONG_THRESHOLD) {
            header.put((byte) (MASK | LENGTH_LONG));
            header.putLong(payload.length);
        } else if (payload.length >= LENGTH_SHORT_THRESHOLD) {
            header.put((byte) (MASK | LENGTH_SHORT));
            header.putShort((short) payload.length);
        } else {
            header.put((byte) (MASK | payload.length));
        }
        header.put(maskingKey);
        header.flip();
        try {
            outputStream.write(header.array(), header.arrayOffset() + header.position(), header.remaining());
            for (int i = 0; i < payload.length; ++i) {
                outputStream.write(payload[i] ^ maskingKey[i % 4]);
            }
        } catch (IOException e) {
            Log.d("Aegea", "write error: " + e);
            onError(handle, e.toString());
        }
    }

    private static final int FIN = 0x80;
    private static final int MASK = 0x80;

    private static final int LENGTH_SHORT = 126;
    private static final int LENGTH_LONG = 127;
    private static final int LENGTH_SHORT_THRESHOLD = 126;
    private static final int LENGTH_LONG_THRESHOLD = 0x10000;

    private static final int OPCODE_CONTINUATION = 0;
    private static final int OPCODE_TEXT = 1;
    private static final int OPCODE_BINARY = 2;
    private static final int OPCODE_CLOSE = 8;
    private static final int OPCODE_PING = 9;
    private static final int OPCODE_PONG = 10;
}
