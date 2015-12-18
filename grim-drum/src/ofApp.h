/*
 * Grim-sounding digital drum machine
 *
 * Oliver Reynolds, 2015
 *
 * <oli_kester@zoho.com>
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"

//--- my enums ---
enum SeqToEdit {KICK,TOM,SNARE,CLAP,OPENHH,CLOSEDHH};

//--- my classes ---
class grimClap{
public:
    grimClap();
    double play();
    void setFat(double fat);
    void setRelease(double releaseD);
    void trigger();
    double output = 0 ;
    double envOut1,envOut2,envOut3;
    bool inverse = false;
    double fatness;
    double gain = 1;
    maxiOsc tone1,mod1,tone2;
    maxiEnv envelope1,envelope2,envelope3;
};

class grimTom{
public:
    grimTom();
    double play();
    void setPitch(double pitch);
    void setRelease(double releaseD);
    void trigger();
    double pitch;
    double output = 0 ;
    double outputD = 0 ;
    double envOut1,envOut2;
    bool inverse = false;
    double gain = 1;
    maxiOsc tone1,tone2;
    maxiEnv envelope1,envelope2;
};

class ofApp : public ofBaseApp{

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
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void audioOut(float * output,int bufferSize, int nChannels);
    void audioIn(float * output,int bufferSize, int nChannels);

    //--- my methods ---
    //parameter listeners
    void tempoChanged(double & val);
    void tog1Changed(bool & val);
    void tog2Changed(bool & val);
    void tog3Changed(bool & val);
    void tog4Changed(bool & val);
    void tog5Changed(bool & val);
    void tog6Changed(bool & val);
    void tog7Changed(bool & val);
    void tog8Changed(bool & val);
    void tog9Changed(bool & val);
    void tog10Changed(bool & val);
    void tog11Changed(bool & val);
    void tog12Changed(bool & val);
    void tog13Changed(bool & val);
    void tog14Changed(bool & val);
    void tog15Changed(bool & val);
    void tog16Changed(bool & val);

    //sequence management
    void setStep(SeqToEdit seqEditing, int index, bool newVal);
    bool getStep(SeqToEdit seq, int index);
    void loadSeq(SeqToEdit newSelectedSeq);
    void redrawSteps();

    //audio settings
    int bufferSize;
    int sampleRate;

    //sequencing
    int kickSeq[16],tomSeq[16],snareSeq[16],clapSeq[16],openHHSeq[16],closedHHSeq[16];
    SeqToEdit editingThisSeq;

    //control params
    int seqIndex,bufferCount;
    bool playing;
    double rmsOut,outSum;

    //sound outputs
    double kickOut,tomOut,snareOut,clapOut,openHHOut,closedHHOut,finalOut;

    //maxi objects
    maxiClock myClock;
    maxiKick myKick;
    maxiSnare mySnare;
    maxiHats myOpenHH,myClosedHH;
    maxiOsc mySine;

    //custom sound objects
    grimTom myTom;
    grimClap myClap;

    //colours
    ofColor colourBackHigh,colourBackLow,colourTopBar,colourFont,colourLedOff,colourLedOn,colourLedGreen,colourVU;

    //ofxGui objects
    ofTrueTypeFont fontMono,fontSerif;
    ofBitmapFont fontBitmap;
    ofxGuiGroup guiMaster,guiKick,guiTom,guiSnare,guiClap,guiOpenHH,guiClosedHH;
    ofxToggle togSteps[16],play;
    ofParameter<double> paramMastTempo,paramMastVol,paramKickVol,paramKickRel,paramTomVol,paramTomPitch,paramSnareVol,paramSnareRel,paramClapVol,paramClapFat,paramClosedHHVol,paramClosedHHDist,paramOpenHHVol,paramOpenHHRel;
    ofParameter<bool> paramMastPlay,paramKickMute,paramTomMute,paramSnareMute,paramClapMute,paramOpenHHMute,paramClosedHHMute;
};
