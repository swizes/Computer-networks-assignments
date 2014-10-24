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
public class VideoStreamChecker {

    private static void usage(){
                System.out.println("Benutze:");
                System.out.println(" async <host> <port> <count>");
                System.out.println(" sync <host> <port> <count>");
                System.out.println(" syncdiff <host> <port> <count>");        
    }
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            if(args.length != 4){
                usage();
                System.exit(1);
            }
            int port = Integer.parseInt(args[2]);
            int count = Integer.parseInt(args[3]);
            VideoStreamManager vsman = new VideoStreamManager(args[1], port, count);
            if ("async".equalsIgnoreCase(args[0])) {
                vsman.asyncRun();
            }
            if ("sync".equalsIgnoreCase(args[0])) {
                vsman.syncRun();
            }
            if ("syncdiff".equalsIgnoreCase(args[0])) {
                vsman.syncRunDiff();
            } else {
            }


        } catch (UnknownHostException ex) {
            Logger.getLogger(VideoStreamChecker.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(VideoStreamChecker.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
