#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
   //setup TCP Server
    
    //ofSetVerticalSync(true);
    ofSetVerticalSync(false);
    
    ofSetFrameRate(1000);
    
    TCP.setup(11999);
    
    TCP.setMessageDelimiter("\n");
    
    //Setup osc server
    sender.setup(HOST, PORTOUT);
    
    receiver.setup(PORTIN);
    
    //AddListenerInterface for osc messages
    //Este listener usa una version antigua de ofxOSC
    ofAddListener(receiver.onMessageReceived, this, &ofApp::onMessageReceived);
    

    //Setup PD
   
    // setup OF sound stream & Pd patch
    int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
    int numInputs = 0;
    
    ofSoundStreamSetup(2, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);
    if(!pdObject.init(2, numInputs, 44100, ticksPerBuffer)) {
        OF_EXIT_APP(1);
    }
    
    //Subscribe for receiving pure data events when sequencing
    pdObject.subscribe("TriggerKick");
    pdObject.subscribe("TriggerSnare");
    pdObject.subscribe("TriggerHats");
    pdObject.subscribe("TriggerCymbal");
    pdObject.subscribe("TriggerPerc");
    
    pdObject.subscribe("pos");
    pdObject.addReceiver(*this);   // automatically receives from all subscribed sources
    
   
    patch = pdObject.openPatch("patchesPd/1Secuencer.pd");
    pdObject.start();
   
   
    //Setup Midi
    
    // print the available output ports to the console
    midiOut.listOutPorts(); // via instance
    //ofxMidiOut::listPorts(); // via static too
    
    // connect
    midiOut.openPort(0); // by number
    //midiOut.openPort("IAC Driver Pure Data In"); // by name
    //midiOut.openVirtualPort("ofxMidiOut"); // open a virtual port
    
    channel = 1;
    currentPgm = 0;
    note = 0;
    velocity = 0;
    pan = 0;
    bend = 0;
    touch = 0;
    polytouch = 0;
    
    kickNote=36;
    snareNote = 38;
    hatsNote = 40;
    cymbalNote =41;
    percNote = 42;
    
    //ofSetBackgroundAuto(false);
    
    myImage.loadImage("GUI/mdmLogo.png"); //allocate space in ram, decode jpg, load pixels.
    
    myImage.draw(0,0,ofGetWidth(),ofGetHeight());

}

//--------------------------------------------------------------
void ofApp::update(){
    
for(unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++){
    
    if( TCP.isClientConnected(i) ){
    
        string str = TCP.receive(i);
        
        tcpMessageReceived = str;
        
        vector<string> splitString = ofSplitString( str, ";");
        
        //ofLogNotice() << "Tcp message arrived: " ;
        ofLogNotice() << str;
        
        if (str.length() > 0 ){
        
        for(int i=0; i<splitString.size(); i++){
            
            printf( "element %i is %s\n", i, splitString[i].c_str() );
           
        }
            
        }
        

        if(splitString[0] == "M"){
            mode = ofToInt(splitString[1]);
            ofLogNotice() << "llego mensaje mode  " << mode;

        }
        
        if(splitString[0] == "T"){
            
            tempo = ofToInt(splitString[1]);
            
            pdObject.sendFloat("metro", tempo);
            
            ofxOscMessage m;
            m.setAddress("/live/tempo");
            m.addFloatArg(tempo);
            sender.sendMessage(m);
        }
        
        
       
        if(splitString[0] == "P"){
            
            
            int st = ofToInt(splitString[1]);
            if (mode==1){
            
            if (st == 0){
                ofxOscMessage m;
                m.setAddress("/live/stop");
                sender.sendMessage(m);
                pdObject.sendFloat("st",1);
            }
            if (st == 1){
                pdObject.sendFloat("st",2);
                pdObject.sendFloat("st",2);
                ofxOscMessage m;
                m.setAddress("/live/play");
                sender.sendMessage(m);
                
            }
            
            }
            
            ofLogNotice() << "llego mensaje START " << endl;
            
        }
        
        if(splitString[0] == "S"){
            
            int sound = ofToInt(splitString[1]);
            
            int pos = ofToInt(splitString[3]);
            
            ofLogNotice() << "llego instrumento " << sound << "posicion " <<  pos << endl;
            
            pdObject << StartMessage() << sound << pos << FinishList("list");
    
        }
        
        }
       
    }
  
}

void ofApp::draw(){
    
    myImage.draw(0,0,ofGetWidth(),ofGetHeight());
    
    for(unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++){
        
        if( TCP.isClientConnected(i) ){

    string port = ofToString( TCP.getClientPort(i) );
    string ip   = TCP.getClientIP(i);
    info = "client "+ofToString(i)+" -connected from "+ip+" on port: "+port;
        
        }
    
    }
    
    ofDrawBitmapString(info, ofGetWidth()/10, ofGetHeight()/20);
    
    //ofDrawBitmapString(" fps: " + ofToString(ofGetFrameRate()), 50, 50);
    
    ofDrawBitmapString(" TCP Rx: " +  tcpMessageReceived, 50, 50);

}


//--------------------------------------------------------------

//--------------------------------------------------------------


void ofApp::receiveBang(const std::string& dest) {
    
    
if (mode==1) {
    
    ofLog() << "OF: bang ";
    
   velocity = 127;
    if (dest == "TriggerKick"){
    midiOut.sendNoteOn(channel, kickNote,  velocity);
    
    midiOut.sendNoteOff(channel, kickNote);

    cout << " midi note " << kickNote << endl;
        
    } else
      if (dest == "TriggerSnare"){
            midiOut.sendNoteOn(channel, snareNote,  velocity);
            midiOut.sendNoteOff(channel,  snareNote);
        }
    
    if (dest == "TriggerHats"){
        
        
        midiOut.sendNoteOn(channel, hatsNote,  velocity);
        
        midiOut.sendNoteOff(channel, hatsNote);
     //   cout << " midi note " << hatsNote << endl;
        
    }
    
    if (dest == "TriggerCymbal"){
        
       
        midiOut.sendNoteOn(channel, cymbalNote,  velocity);
        
        midiOut.sendNoteOff(channel, cymbalNote);
      //  cout << " midi note " << cymbalNote << endl;
        
    }
    
    if (dest == "TriggerPerc"){
        
       
        midiOut.sendNoteOn(channel, cymbalNote,  velocity);
        //cout << " midi note " << cymbalNote<< endl;
        midiOut.sendNoteOff(channel,  cymbalNote);
        
    }
        
}

}

//--------------------------------------------------------------

void ofApp::onMessageReceived(ofxOscMessage &msg)
{
    cout << "received mesage"<<endl;
    
    string addr = msg.getAddress();
    
    cout << "address : "<< addr << endl;
    
    if(addr == "/live/tempo"){
        int tempo = msg.getArgAsInt32(0);
        pdObject.sendFloat("metro", tempo);
        cout << "Llego bpm "<< tempo  << endl;
        msgTx = "T;"+ofToString(tempo);
        TCP.sendToAll(msgTx);
    }
   
    
    if(addr == "/live/play"){
        int st = msg.getArgAsInt32(0);
        //pdObject.sendFloat("st",st);
        cout << "Llego play "<< st  << endl;
        msgTx = "P;"+ofToString(st);
        TCP.sendToAll(msgTx);
    }
    
}


//--------------------------------------------------------------


void ofApp::receiveFloat(const std::string& dest, float value) {
    
    cout << "OF: float " << dest << ": " << value << endl;
}

//--------------------------------------------------------------

void ofApp::receiveMessage(const std::string& dest, const std::string& msg, const List& list) {
    cout << "OF: message " << dest << ": " << msg << " " << list.toString() << list.types() << endl;
}


//--------------------------------------------------------------

void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
     pdObject.audioOut(output, bufferSize, nChannels);
}


//--------------------------------------------------------------

void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
    pdObject.audioIn(input, bufferSize, nChannels);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
