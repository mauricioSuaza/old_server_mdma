#pragma once

#include "ofMain.h"
#include "ofxPd.h"
#include "ofxMidi.h"
#include "ofxOsc.h"
#include "ofxNetwork.h"


#define HOST "127.0.0.1"
#define PORTOUT 9000
#define PORTIN 9001


const int port = 50000;


using namespace pd;


class ofApp : public ofBaseApp, public PdReceiver{

	public:
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
    
        void audioReceived(float * input, int bufferSize, int nChannels);
        void audioRequested(float * output, int bufferSize, int nChannels);

    
        void receiveBang(const std::string& dest);
        void receiveFloat(const std::string& dest, float value);
        void receiveMessage(const std::string& dest, const std::string& msg, const List& list);
    
        void onMessageReceived(ofxOscMessage &msg); 
    
    
private:
   
    ofxPd           pdObject;
    Patch           patch;
    bool            playToggle=false;
       
    ofxMidiOut midiOut;
    int channel;
    
    unsigned int currentPgm;
    int note, velocity;
    int pan, bend, touch, polytouch;
    
    
    int kickNote, snareNote, hatsNote, cymbalNote, percNote;
    
    int mode = 0;
    float tempo = 120;
    
    ofImage myImage; //allocate space for variable

    ofxTCPServer TCP;
    
    string info;
    
   
    ofxOscSender sender;
    
    ofxOscReceiver receiver;
    
    string 			msgTx, msgRx;
    
    string tcpMessageReceived;
  
};
