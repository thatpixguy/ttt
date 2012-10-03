#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

class testApp : public ofBaseApp{

	public:
        void setupArduino();

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        void drawBox(ofPixelsRef p, ofColor c, int x, int y, int w, int h);
        void drawNotchedBox(ofPixelsRef p, ofColor c, ofColor n, int x, int y, int w, int h, int wn, int hn);

        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif


        ofxCvColorImage			colorImg;

        ofImage                powerBarImg;
        ofImage                powerBarMask;

        ofImage                  diffImg;

        ofRectangle             pb1Rect;
        ofRectangle             pb2Rect;

        ofPoint             p1m1p,p1m2p,p2m1p,p2m2p;
        bool                p1m1,p1m2,p2m1,p2m2;

		int 				pbThreshold;
		int                 mThreshold;
		bool				bLearnBakground;

        float                 pbConfidence;
        float               pb1,pb2;

        float               pb1_slow;
        float               pb2_slow;

        int                 cursor_x;
        int                 cursor_y;

        float           pbLog[1000];

        ofArduino       arduino;
        bool            arduinoReady;

};

