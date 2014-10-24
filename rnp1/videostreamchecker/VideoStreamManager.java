/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package videostreamchecker;

import java.io.IOException;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author heitmann
 */
public class VideoStreamManager {

    VideoStream[] vs;

    public VideoStreamManager(String host, int port, int count) throws UnknownHostException, IOException {
        vs = new VideoStream[count];
        for (int i = 0; i < vs.length; i++) {
            vs[i] = new VideoStream(host, port, i);
        }
    }

    public void asyncRun() {
        for (int i = 0; i < vs.length; i++) {
            Thread t = new Thread(vs[i]);
            t.start();
        }
    }

    public void syncRun() {
        try {
            while (true) {
                for (int i = 0; i < vs.length; i++) {
                    vs[i].step();
                }
                int count0 = vs[0].getCount();
                System.out.printf("%6d: ", count0);
                for (int i = 0; i < vs.length; i++) {
                    System.out.printf("%6d %2d -- ", vs[i].getFrame(), vs[i].getCount() - vs[i].getFrame());
                }
                System.out.println();
            }
        } catch (IOException ex) {
            Logger.getLogger(VideoStreamManager.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public void syncRunDiff() {
        int n = 0;
        int k = vs.length;
        try {
            while (true) {
                for (int i = 0; i < vs.length; i++) {
                    vs[i].step();
                }
                n++;
                if (n % 40 == 0) {
                    k--;
                    if (k <= 0) {
                        k = vs.length - 1;
                    }
                    for (int i = 0; i < k; i++) {
                        vs[i].step();
                    }

                }

                int count0 = vs[0].getCount();
                System.out.printf("%6d: ", count0);
                for (int i = 0; i < vs.length; i++) {
                    System.out.printf("%4d %6d %6d | ", vs[i].getCount() - count0, vs[i].getCount() - vs[i].getFrame(), vs[i].getFrame());
                }
                System.out.println();
            }
        } catch (IOException ex) {
            Logger.getLogger(VideoStreamManager.class.getName()).log(Level.SEVERE, null, ex);
        }
    }


    public void syncRunOpenClose() {
        int n = 0;
        int k = vs.length;
        try {
            while (true) {
                for (int i = 0; i < vs.length; i++) {
                    vs[i].step();
                }

                int count0 = vs[0].getCount();
                System.out.printf("%6d: ", count0);
                for (int i = 0; i < vs.length; i++) {
                    System.out.printf("%6d %2d -- ", vs[i].getCount() - count0, vs[i].getCount() - vs[i].getFrame());
                }
                System.out.println();
            }
        } catch (IOException ex) {
            Logger.getLogger(VideoStreamManager.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
