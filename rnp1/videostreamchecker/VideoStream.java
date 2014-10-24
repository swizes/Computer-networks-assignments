/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package videostreamchecker;

import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author heitmann
 */
public class VideoStream implements Runnable {

    final static int PICTURESIZE = 320 * 240 * 3;
    Socket s;
    DataInputStream in;
    byte[] data;
    int id;
    int count;
    boolean closed;
    int frame;

    public VideoStream(String host, int port, int id) throws UnknownHostException, IOException {
        this.s = new Socket(host, port);
        this.in = new DataInputStream(s.getInputStream());
        this.data = new byte[PICTURESIZE];
        this.id = id;
        this.closed = false;
    }

    @Override
    public void run() {
        count = 0;
        try {
            while (!closed) {
                step();
            }
            System.out.printf("%4d: Stream closed, Count = %d\n", id, count);
        } catch (IOException ex) {
            Logger.getLogger(VideoStream.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public void step() throws IOException {
        if (closed) {
            return;
        }
        int pos = 0;
        while (pos < PICTURESIZE) {
            int n = in.read(data, pos, PICTURESIZE - pos);
            if (n <= 0) {
                closed = true;
                break;
            }
            pos += n;
        }
        if (!closed) {
            count++;
            frame = toInt(data[data.length - 4], data[data.length - 3], data[data.length - 2], data[data.length - 1]);
        } else {
            s.close();
        }
    }

    public void close() throws IOException {
        s.close();
        closed = true;
    }

    boolean isClosed() {
        return closed;
    }

    public int getCount() {
        return count;
    }

    public int getFrame() {
        if(closed){
            return -1;
        }
        return frame;
    }

    private static int toInt(byte b1, byte b2, byte b3, byte b4) {
        return (b1 & 0xFF) | ((b2 & 0xFF) << 8) | ((b3 & 0xFF) << 16) | ((b4 & 0xFF) << 24);
    }
}
