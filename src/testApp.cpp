#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(320,240);
	#else
        vidPlayer.loadMovie("arcade-soft.dv");
        //vidPlayer.setFrame(30*30);
        vidPlayer.play();
	#endif

    //int mix = 32;

    colorImg.allocate(720,480);

    pb1Rect.set(65,47,255,12);
    pb2Rect.set(398,47,255,12);

    p1m1p.set(316,77);
    p1m2p.set(300,77);
    p2m1p.set(402,77);
    p2m2p.set(417,77);


    diffImg.allocate(720,480,OF_IMAGE_COLOR);


    powerBarMask.loadImage("powerbar-mask.png");

    powerBarImg.loadImage("powerbar.png");

	bLearnBakground = true;
	mThreshold = 150;
	pbThreshold = 80;

	for(int i=0;i<1000;i++) {
	    pbLog[i]=0;
	}

	arduino.connect("/dev/ttyACM0",57600);

	arduinoReady = false;
	//ofAddListener(arduino.EInitialized, this, &testApp::setupArduino);
}

void testApp::setupArduino(){
    arduinoReady = true;
    arduino.sendDigitalPinMode(12,ARD_OUTPUT);
    arduino.sendDigitalPinMode(13,ARD_OUTPUT);
}


void testApp::drawNotchedBox(ofPixelsRef p, ofColor b, ofColor n, int x, int y, int w, int h, int wn, int hn) {
    for (int hi=0;hi<h;++hi) {
        for (int wi=0;wi<w;++wi) {
            ofColor c = b;
            if(wn!=0 && !(wi%wn)) c = n;
            if(hn!=0 && !(hi%hn)) c = n;
            p.setColor(x+wi,y+hi,c);
        }
    }
}

void testApp::drawBox(ofPixelsRef p, ofColor c, int x, int y, int w, int h) {
    drawNotchedBox(p,c,c,x,y,w,h,0,0);
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), 720,480);
        #endif

        if (bLearnBakground == true){
			bLearnBakground = false;
		}


        ofPixelsRef colorPixels = colorImg.getPixelsRef();

        //ofPixelsRef diffPixels = diffImg.getPixelsRef();

        ofPixelsRef pbPixels = powerBarImg.getPixelsRef();
        ofPixelsRef pbmPixels = powerBarMask.getPixelsRef();

        int count = 0;
        int sum = 0;
        for(int y=powerBarMask.height-1;y>=0;--y) {
            for(int x=powerBarMask.width-1;x>=0;--x) {
                if(pbmPixels.getColor(x,y).r>0) {

                    ofColor pbc = pbPixels.getColor(x,y);
                    ofColor cp = colorPixels.getColor(x,y);
                    ofColor diff;
                    sum += diff.r = abs(pbc.r-cp.r);
                    sum += diff.g = abs(pbc.g-cp.g);
                    sum += diff.b = abs(pbc.b-cp.b);
                    count+=3;
                    diffImg.setColor(x,y,diff);

                }

            }
        }

        diffImg.update();

        //sum/=count;
        pbConfidence = 100-(sum*100.0 / (count*255));


        for(int i=100-1;i>=1;i--) {
            pbLog[i]=pbLog[i-1];
        }
        pbLog[0]=pbConfidence;



        if(pbConfidence>90) {
            //ofPixelsRef colorPixels = colorImg.getPixelsRef();

            pb1 = 100;

            //ofSetColor(ofColor::green);
            for(int i=pb1Rect.width-1;i>=0;--i) {
                int x = i+pb1Rect.x;
                int sum = 0;
                int count = 0;
                for(int j=pb1Rect.height-1;j>=0;--j) {
                    int y = j+pb1Rect.y;
                    ofColor c = colorPixels.getColor(x,y);
                    sum += c.g;
                }
                int avg = sum/pb1Rect.height;
                if(avg<pbThreshold) {
                    pb1 = 100-(i*100.0/pb1Rect.width);

                    drawBox(colorPixels,ofColor::green,x-3,pb1Rect.y-10,3,pb1Rect.height+20);

                    break;
                }
                //ofLine(x+20,pb1Rect.y+pb1Rect.height+20,x+20,pb1Rect.y+pb1Rect.height+(avg)+20);
            }

            pb2 = 100;
            for(int i=0;i<pb2Rect.width;i++) {
                int x = i+pb2Rect.x;
                int sum = 0;
                int count = 0;
                for(int j=pb2Rect.height-1;j>=0;--j) {
                    int y = j+pb2Rect.y;
                    ofColor c = colorPixels.getColor(x,y);
                    sum += c.g;
                }
                int avg = sum/pb2Rect.height;
                if(avg<pbThreshold) {
                    pb2 = (i*100.0/pb2Rect.width);

                    drawBox(colorPixels,ofColor::green,x,pb2Rect.y-10,3,pb2Rect.height+20);
                    break;
                }

            }

            //check match lights
            ofColor c;

            c = colorPixels.getColor(p2m1p.x,p2m1p.y);
            p2m1 = (c.r>mThreshold);

            drawNotchedBox(colorPixels,ofColor::red,ofColor::green,p2m1p.x-2,p2m1p.y,3,c.r,0,10);


            c = colorPixels.getColor(p2m2p.x,p2m2p.y);
            p2m2 = (c.r>mThreshold);

            drawNotchedBox(colorPixels,ofColor::red,ofColor::green,p2m2p.x-2,p2m2p.y,3,c.r,0,10);


            c = colorPixels.getColor(p1m1p.x,p1m1p.y);
            p1m1 = (c.r>mThreshold);

            drawNotchedBox(colorPixels,ofColor::red,ofColor::green,p1m1p.x-2,p1m1p.y,3,c.r,0,10);


            c = colorPixels.getColor(p1m2p.x,p1m2p.y);
            p1m2 = (c.r>mThreshold);

            drawNotchedBox(colorPixels,ofColor::red,ofColor::green,p1m2p.x-2,p1m2p.y,3,c.r,0,10);


        }




	}
    if (arduinoReady) {
        // just testing
        arduino.sendDigital(13,pbConfidence>90);
        arduino.sendDigital(12,pbConfidence<=90);
    } else {
        if (arduino.isArduinoReady()) {
            arduinoReady = true;
            setupArduino();
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg.draw(20,20);

	// then draw the contours:

    //diffImg.draw(400,20);

    /*
    ofSetColor(ofColor::blue);
    ofLine(0,(100-90)*3,500,(100-90)*3);
    ofSetColor(ofColor::white);
    for(int i=1;i<100;i++) {
        ofLine((i-1)*10,(100.0-pbLog[i-1])*3,i*10,(100.0-pbLog[i])*3);
    }
    */



	// we could draw the whole contour finder

	// or, instead we can draw each blob individually,
	// this is how to get access to them:
    //for (int i = 0; i < contourFinder.nBlobs; i++){
    //    contourFinder.blobs[i].draw(360,540);
    //}

	// finally, a report:

	ofSetHexColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "pbConfidence %f\npbThreshold: %d\nfps: %f\npb1: %f\npb2: %f\nm: %d %d %d %d\n", pbConfidence, pbThreshold, ofGetFrameRate(),pb1,pb2, p1m2, p1m1, p2m1, p2m2);
	ofDrawBitmapString(reportStr, 20, 600);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
            if(vidPlayer.isPaused()) {
                vidPlayer.setPaused(false);
            } else {
                vidPlayer.setPaused(true);
            }
			break;
		case '+':
			pbThreshold ++;
			if (pbThreshold > 255) pbThreshold = 255;
			break;
		case '-':
			pbThreshold --;
			if (pbThreshold < 0) pbThreshold = 0;
			break;
        case 356:
            vidPlayer.setFrame(vidPlayer.getCurrentFrame()-30);
            break;
        case 358:
            vidPlayer.setFrame(vidPlayer.getCurrentFrame()+30);
            break;
        default:
            printf("unknown key [%d]\n",key);
            break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
