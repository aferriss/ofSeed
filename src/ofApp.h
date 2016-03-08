#pragma once

#include "ofMain.h"
#include "ofxNoise.h"
////#include "ofxQtVideoSaver.h"
#include "ofxVideoRecorder.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
        void updateSeeds();
		void draw();
        void exit();
    
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        struct Pixel {
            ofVec3f pixelColor;
            float pixelBri;
            //int pixelX;
            //int pixelY;
            int pixelLoc;
            
            bool operator<(const Pixel& a) const{
                return pixelLoc < a.pixelLoc;
            }

        };
    vector<Pixel> pixelArray;
    
    vector<int> seeds;
    //vector<bool> traversed;
    //bool traversed;
    int step, steps, w, h, numRandSeeds, rr;
    float speed, inc;
    bool finishIt, save, turnBlack;
    bool* traversed ;    
    
    ofImage screen, sort;
    ofTexture feedback;
    ofPixels fbPix;
    ofColor c;
    unsigned char * pixels; 
    unsigned char * pix;
    ofxPerlin *noise;
	ofxSimplex *simplex;
    //ofxQtVideoSaver saver;
    ofxVideoRecorder vidRecorder;
    
    ofShader noiseShader, reposShader, emboss;
    
    ofFbo noiseFbo, reposFbo, embossFbo, stencilFbo;
    
    ofVideoGrabber cam;
    ofImage sheepStencil;
    
    void swap(unsigned char a[], int rx, int ry, int gx, int gy, int bx, int by);
    
    
    //ofSoundStream soundStream;
    //void audioOut(float * input, int bufferSize, int nChannels);
    ofImage sheepImg;
    ofPixels sheepPix;
    int sampleCount;
    bool hideGui;
    ofxDatGui* gui;
    ofxDatGuiSlider* numSeedsSlider;
    ofxDatGuiSlider* xNoise;
    ofxDatGuiSlider* yNoise;
    ofxDatGuiSlider* zNoise;
    ofxDatGuiSlider* stepSlider;
    ofxDatGuiSlider* randSlider;
    
    bool useImage;
    ofImage earthImg;
    ofPixels earthPix;
};
