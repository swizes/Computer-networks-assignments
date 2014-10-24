#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    struct timeval timeOfNextFrame;
    struct timeval time;
    IplImage* image;
    CvSize size;
    int bubleX;
    int bubleY;
    int bubleDX;
    int bubleDY;
    int bubleR;
    int period;
    CvFont font;
    int loopcnt;

    //Struktur fuer Bild aktualisieren
    size = cvSize(320, 240);
    image = cvCreateImage(size, IPL_DEPTH_8U, 3);

    //Elemente festlegen
    bubleR = 40;
    bubleX = size.width / 2;
    bubleY = size.height / 2;
    bubleDX = 10;
    bubleDY = 10;
    period = 30;

    //Verwendeten Font erzeugen
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0.0, 1, 8);

    //und Fenster fuer Darstellung anlegen
    cvNamedWindow("Simulator", CV_WINDOW_AUTOSIZE);

    loopcnt = 0;
    gettimeofday(&timeOfNextFrame, 0);
    while (1) {
        char text[256];

        gettimeofday(&time, 0);
        //Warte auf den naechsten Zeitpunkt
        if ((time.tv_sec == timeOfNextFrame.tv_sec) ?
                (time.tv_usec >= timeOfNextFrame.tv_usec) :
                (time.tv_sec >= timeOfNextFrame.tv_sec)) {
            loopcnt++;
            //Ok, Bild neu erstellen
            //Alles loeschen
            cvRectangle(image, cvPoint(0, 0), cvPoint(size.width, size.height), CV_RGB(0, 255, 255), CV_FILLED, 8, 0);
            //Kreis zeichnen
            cvCircle(image, cvPoint(bubleX, bubleY), bubleR, CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
            //Text eintragen
            snprintf(text, sizeof (text), "%8d", loopcnt);
            cvPutText(image, text, cvPoint(10, 40), &font, cvScalar(0, 255, 255, 255));
            
            //Jetzt koennte man die Daten versenden:
            //send( image->imageData, image->imageSize );

            //Neue Position berechnen
            bubleX += bubleDX;
            if (bubleX > size.width - bubleR) {
                bubleDX = -bubleDX;
                bubleX = (size.width - bubleR)*2 - bubleX;
            } else if (bubleX < bubleR) {
                bubleDX = -bubleDX;
                bubleX = (bubleR)*2 - bubleX;
            }

            bubleY += bubleDY;
            if (bubleY > size.height - bubleR) {
                bubleDY = -bubleDY;
                bubleY = (size.height - bubleR)*2 - bubleY;
            } else if (bubleY < bubleR) {
                bubleDY = -bubleDY;
                bubleY = (bubleR)*2 - bubleY;
            }

            //und den naechsten Zeitpunkt festlegen
            timeOfNextFrame.tv_usec += period * 1000;
            if (timeOfNextFrame.tv_usec >= 1000000) {
                timeOfNextFrame.tv_usec -= 1000000;
                timeOfNextFrame.tv_sec += 1;
            }

            //zum Schluss das neue Bild anzeigen
            cvShowImage("Simulator", image);
            //auf Ereignisse warten (erst hier wird das Bild tatsaechlich aktualisiert
            if ((cvWaitKey(5) & 255) == 27) break;
        }
        usleep(1000);
    }
    cvReleaseImage(&image);
    return 0;
}
