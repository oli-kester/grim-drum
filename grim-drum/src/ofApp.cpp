/*
 * Grim-sounding digital drum machine
 *
 * Oliver Reynolds, 2015
 *
 * <oli_kester@zoho.com>
 *
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //audio settings
    sampleRate = 44100;
    bufferSize = 512;
    bufferCount=0;

    //clear sequences
    for (int c=0;c<16;c++){
        kickSeq[c]=0;
        tomSeq[c]=0;
        snareSeq[c]=0;
        clapSeq[c]=0;
        openHHSeq[c]=0;
        closedHHSeq[c]=0;
    }

    //init control values
    seqIndex=0;
    outSum=0;
    myClock.setTempo(paramMastTempo.get());
    myClock.setTicksPerBeat(4);
    //start playhead from -1 to ensure step 1 plays on first pass
    myClock.playHead=-1;
    //start with editing the kick
    editingThisSeq=KICK;

    //setup sounds
    myKick.envelope.attack=0.2;
    mySnare.useFilter=false;
    mySnare.setPitch(500);
    mySnare.envelope.setAttack(5);
    myOpenHH.envelope.setDecay(100);
    myOpenHH.setPitch(50);
    myClosedHH.envelope.setDecay(50);
    myClosedHH.envelope.setDecay(50);
    myClosedHH.setPitch(10000);

    //set colour palette
    colourBackHigh.set(ofColor::grey);
    colourBackLow.set(ofColor::darkGray);
    colourTopBar.set(ofColor::black);
    colourFont.set(ofColor::honeyDew);
    colourLedOff.set(ofColor::honeyDew);
    colourLedOn.set(ofColor::red);
    colourLedGreen.set(ofColor::green);
    colourVU.set(100);

    //set fonts
    fontMono.load(OF_TTF_MONO,33);
    fontMono.setLetterSpacing(2);
    fontSerif.load(OF_TTF_SERIF,10);

    //GUI setup
    ofSetWindowTitle("Grim Drum");
    ofEnableAlphaBlending();

    //add parameter listeners
    paramMastTempo.addListener(this,&ofApp::tempoChanged);

    //attach a listener to every toggle
    togSteps[0].addListener(this,&ofApp::tog1Changed);
    togSteps[1].addListener(this,&ofApp::tog2Changed);
    togSteps[2].addListener(this,&ofApp::tog3Changed);
    togSteps[3].addListener(this,&ofApp::tog4Changed);
    togSteps[4].addListener(this,&ofApp::tog5Changed);
    togSteps[5].addListener(this,&ofApp::tog6Changed);
    togSteps[6].addListener(this,&ofApp::tog7Changed);
    togSteps[7].addListener(this,&ofApp::tog8Changed);
    togSteps[8].addListener(this,&ofApp::tog9Changed);
    togSteps[9].addListener(this,&ofApp::tog10Changed);
    togSteps[10].addListener(this,&ofApp::tog11Changed);
    togSteps[11].addListener(this,&ofApp::tog12Changed);
    togSteps[12].addListener(this,&ofApp::tog13Changed);
    togSteps[13].addListener(this,&ofApp::tog14Changed);
    togSteps[14].addListener(this,&ofApp::tog15Changed);
    togSteps[15].addListener(this,&ofApp::tog16Changed);

    //control panel
    guiMaster.setup("Master","settings.xml",ofGetWidth()-250,ofGetHeight()-205);
    guiMaster.add(paramMastVol.set("Volume",100,0,100));
    guiMaster.add(paramMastTempo.set("Tempo",120,50,300));
    guiMaster.add(paramMastPlay.set("Play (Space)",false));

    //draw sequencer
    redrawSteps();

    //kick parameters
    guiKick.setup("Bass Drum","settings.xml",15,65);
    guiKick.add(paramKickVol.set("Volume",80,0,100));
    guiKick.add(paramKickRel.set("Release",25,0,100));
    guiKick.add(paramKickMute.set("Mute (Q)",false));

    //tom parameters
    guiTom.setup("Tom Drum","settings.xml",15,175);
    guiTom.add(paramTomVol.set("Volume",80,0,100));
    guiTom.add(paramTomPitch.set("Pitch",50,0,100));
    guiTom.add(paramTomMute.set("Mute (A)",false));

    //snare parameters
    guiSnare.setup("Snare Drum","settings.xml",265,65);
    guiSnare.add(paramSnareVol.set("Volume",80,0,100));
    guiSnare.add(paramSnareRel.set("Release",20,0,100));
    guiSnare.add(paramSnareMute.set("Mute (W)",false));

    //clap parameters
    guiClap.setup("Hand Clap","settings.xml",265,175);
    guiClap.add(paramClapVol.set("Volume",80,0,100));
    guiClap.add(paramClapFat.set("Fat",1,1,100));
    guiClap.add(paramClapMute.set("Mute (S)",false));

    //closed HH parameters
    guiClosedHH.setup("Closed HH","settings.xml",515,65);
    guiClosedHH.add(paramClosedHHVol.set("Volume",80,0,100));
    guiClosedHH.add(paramClosedHHDist.set("Distortion",0,0,100));
    guiClosedHH.add(paramClosedHHMute.set("Mute (E)",false));

    //open HH parameters
    guiOpenHH.setup("Open HH","settings.xml",515,175);
    guiOpenHH.add(paramOpenHHVol.set("Volume",80,0,100));
    guiOpenHH.add(paramOpenHHRel.set("Release",30,0,100));
    guiOpenHH.add(paramOpenHHMute.set("Mute (D)",false));

    ofSoundStreamSetup(2,2,this,sampleRate,bufferSize,4);
}

//--------------------------------------------------------------
void ofApp::update(){
    //DEBUG uncomment to output playhead value
    //    cout << "playhead - " << myClock.playHead << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
    //background
    ofBackgroundGradient(colourBackHigh,colourBackLow,OF_GRADIENT_LINEAR);
    //top backing bar
    ofSetColor(colourTopBar,190);
    ofDrawRectangle(0,0,ofGetWidth(),55);
    //bottom backing bar
    ofSetColor(colourTopBar,90);
    ofDrawRectangle(0,ofGetHeight()-90,ofGetWidth(),90);
    //title
    ofSetColor(colourFont,230);
    fontMono.drawString("GRIM DRUM",10,45);

    //step lights
    for (int c=0;c<16;c++){
        //use the playhead to determine current step LED to blink
        if (myClock.playHead%16==c)
            ofSetColor(colourLedOn);
        else
            ofSetColor(colourLedOff,200);

        //draw this LED
        ofDrawCircle(5*(12.5*c+1)+20,ofGetHeight()-80,5);
    }

    //hotkey labels
    ofSetColor(colourFont);
    ofDrawBitmapString("(R)",togSteps[0].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(T)",togSteps[1].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(Y)",togSteps[2].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(U)",togSteps[3].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(I)",togSteps[4].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(O)",togSteps[5].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(P)",togSteps[6].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("([)",togSteps[7].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(F)",togSteps[8].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(G)",togSteps[9].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(H)",togSteps[10].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(J)",togSteps[11].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(K)",togSteps[12].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(L)",togSteps[13].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(;)",togSteps[14].getPosition().x+7,ofGetHeight()-15);
    ofDrawBitmapString("(')",togSteps[15].getPosition().x+7,ofGetHeight()-15);

    //draw box around GUI of currently selected instrument
    ofSetColor(colourFont,150);
    switch (editingThisSeq){
    case KICK:
        ofDrawRectRounded(guiKick.getPosition().x-5,guiKick.getPosition().y-1,guiKick.getWidth()+10,guiKick.getHeight()+10,10);
        break;
    case TOM:
        ofDrawRectRounded(guiTom.getPosition().x-5,guiTom.getPosition().y-1,guiTom.getWidth()+10,guiTom.getHeight()+10,10);
        break;
    case SNARE:
        ofDrawRectRounded(guiSnare.getPosition().x-5,guiSnare.getPosition().y-1,guiSnare.getWidth()+10,guiSnare.getHeight()+10,10);
        break;
    case CLAP:
        ofDrawRectRounded(guiClap.getPosition().x-5,guiClap.getPosition().y-1,guiClap.getWidth()+10,guiClap.getHeight()+10,10);
        break;
    case CLOSEDHH:
        ofDrawRectRounded(guiClosedHH.getPosition().x-5,guiClosedHH.getPosition().y-1,guiClosedHH.getWidth()+10,guiClosedHH.getHeight()+10,10);
        break;
    case OPENHH:
        ofDrawRectRounded(guiOpenHH.getPosition().x-5,guiOpenHH.getPosition().y-1,guiOpenHH.getWidth()+10,guiOpenHH.getHeight()+10,10);
        break;
    }
    //draw GUI
    guiKick.draw();
    guiTom.draw();
    guiSnare.draw();
    guiClap.draw();
    guiOpenHH.draw();
    guiClosedHH.draw();
    guiMaster.draw();

    //draw step toggle switches
    for (int c=0;c<16;c++){
        togSteps[c].draw();
    }

    //blink tempo LED every 4 ticks
    if (myClock.playHead%4==0){
        ofSetColor(colourLedOn);
    } else ofSetColor(colourLedOff);
    ofDrawCircle(guiMaster.getPosition().x+190,guiMaster.getPosition().y+12,5);

    //level meter backing
    ofSetColor(colourVU);
    ofDrawRectRounded(ofGetWidth()-30,10,20,270,50);

    //DEBUG print output level to console
    //    cout << rmsOut << endl;

    //LEDs for level meter - start off green
    ofSetColor(colourLedGreen,200);
    if (rmsOut<0.1)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-110,5);
    if (rmsOut<0.2)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-130,5);
    if (rmsOut<0.3)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-150,5);
    if (rmsOut<0.4)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-170,5);
    if (rmsOut<0.5)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-190,5);
    if (rmsOut<0.6)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-210,5);
    if (rmsOut<0.7)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-230,5);
    if (rmsOut<0.8)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-250,5);
    if (rmsOut<0.9)
        ofSetColor(colourLedOff);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-270,5);
    if (rmsOut<1)
        ofSetColor(colourLedOff);
    else
        //red LED for volume clipping
        ofSetColor(colourLedOn);
    ofDrawCircle(ofGetWidth()-20,ofGetHeight()-290,5);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //DEBUG uncomment to output keys to console
//    cout << "Key pressed, ASCII - " << key << endl;

    //space toggles play && resets clock
    if (key==32){
        paramMastPlay.set(!paramMastPlay.get());
        myClock.playHead=-1;
    }

    //component mute hotkeys
    if (key==113){
        paramKickMute.set(!paramKickMute.get());
    }
    if (key==97){
        paramTomMute.set(!paramTomMute.get());
    }
    if (key==119){
        paramSnareMute.set(!paramSnareMute.get());
    }
    if (key==115){
        paramClapMute.set(!paramClapMute.get());
    }
    if (key==100){
        paramOpenHHMute.set(!paramOpenHHMute.get());
    }
    if (key==101){
        paramClosedHHMute.set(!paramClosedHHMute.get());
    }

    //seq toggle hotkeys
    if (key==114){
        setStep(editingThisSeq,0,!getStep(editingThisSeq,0));
        redrawSteps();
    }
    if (key==116){
        setStep(editingThisSeq,1,!getStep(editingThisSeq,1));
        redrawSteps();
    }
    if (key==121){
        setStep(editingThisSeq,2,!getStep(editingThisSeq,2));
        redrawSteps();
    }
    if (key==117){
        setStep(editingThisSeq,3,!getStep(editingThisSeq,3));
        redrawSteps();
    }
    if (key==105){
        setStep(editingThisSeq,4,!getStep(editingThisSeq,4));
        redrawSteps();
    }
    if (key==111){
        setStep(editingThisSeq,5,!getStep(editingThisSeq,5));
        redrawSteps();
    }
    if (key==112){
        setStep(editingThisSeq,6,!getStep(editingThisSeq,6));
        redrawSteps();
    }
    if (key==91){
        setStep(editingThisSeq,7,!getStep(editingThisSeq,7));
        redrawSteps();
    }
    if (key==102){
        setStep(editingThisSeq,8,!getStep(editingThisSeq,8));
        redrawSteps();
    }
    if (key==103){
        setStep(editingThisSeq,9,!getStep(editingThisSeq,9));
        redrawSteps();
    }
    if (key==104){
        setStep(editingThisSeq,10,!getStep(editingThisSeq,10));
        redrawSteps();
    }
    if (key==106){
        setStep(editingThisSeq,11,!getStep(editingThisSeq,11));
        redrawSteps();
    }
    if (key==107){
        setStep(editingThisSeq,12,!getStep(editingThisSeq,12));
        redrawSteps();
    }
    if (key==108){
        setStep(editingThisSeq,13,!getStep(editingThisSeq,13));
        redrawSteps();
    }
    if (key==59){
        setStep(editingThisSeq,14,!getStep(editingThisSeq,14));
        redrawSteps();
    }
    if (key==39){
        setStep(editingThisSeq,15,!getStep(editingThisSeq,15));
        redrawSteps();
    }
}


//--------------------------------------------------------------
void ofApp::audioOut(float * output,int bufferSize, int nChannels){
    //count buffers for metering
    bufferCount++;

    for (int i=0;i<bufferSize;i++){
        //only tick clock if we're playing
        if (paramMastPlay.get()){
            myClock.ticker();
        }
        //on each clock tick
        if (myClock.tick) {
            //check each pattern array to see if it's time to trigger the sound. Don't add new triggers if we're muted
            if (kickSeq[myClock.playHead%(sizeof(kickSeq)/sizeof(*kickSeq))] && !paramKickMute){
                //grab sound parameters from GUI
                myKick.setRelease(paramKickRel*10);
                myKick.trigger();
            }
            if (tomSeq[myClock.playHead%(sizeof(tomSeq)/sizeof(*tomSeq))] && !paramTomMute){
                myTom.setPitch(paramTomPitch*8+100);
                myTom.trigger();
            }
            if (snareSeq[myClock.playHead%(sizeof(snareSeq)/sizeof(*snareSeq))] && !paramSnareMute){
                mySnare.setRelease(paramSnareRel*5);
                mySnare.trigger();
            }
            if (clapSeq[myClock.playHead%(sizeof(clapSeq)/sizeof(*clapSeq))] && !paramClapMute){
                myClap.setFat(paramClapFat);
                myClap.trigger();
            }
            if (openHHSeq[myClock.playHead%(sizeof(openHHSeq)/sizeof(*openHHSeq))] && !paramOpenHHMute){
                myOpenHH.setRelease(paramOpenHHRel*20);
                myOpenHH.trigger();
            }
            if (closedHHSeq[myClock.playHead%(sizeof(closedHHSeq)/sizeof(*closedHHSeq))] && !paramClosedHHMute){
                myClosedHH.distortion=paramClosedHHDist*0.1;
                myClosedHH.trigger();
            }
        }

        //DEBUG sine for testing audio
        //        kickOut=mySine.sinewave(440);

        //--- mixing ---
        //only play if the instument is not muted
        if (!paramKickMute)
            kickOut=myKick.play()*paramKickVol*0.01;
        if (!paramTomMute)
            tomOut=myTom.play()*paramTomVol*0.003;
        if (!paramSnareMute)
            snareOut=mySnare.play()*paramSnareVol*0.005;
        if (!paramClapMute)
            clapOut=myClap.play()*paramClapVol*0.02;
        if (!paramOpenHHMute)
            openHHOut=myOpenHH.play()*paramOpenHHVol*0.001;
        if (!paramClosedHHMute)
            closedHHOut=myClosedHH.play()*paramClosedHHVol*0.001;

        //distortion for closed HH. is actually amplitude modulation
        //slider is dry/wet.
        closedHHOut=(closedHHOut*(1-paramClosedHHDist*0.01))+(closedHHOut*mySine.sinewave(30)*paramClosedHHDist*0.01);

        //add together, and control with master vol
        finalOut=(kickOut+tomOut+snareOut+clapOut+openHHOut+closedHHOut)*paramMastVol*0.01;

        //add to sum for metering
        outSum+=fabs(finalOut);

        //send to buffers
        output[i*nChannels]=finalOut;
        output[i*nChannels+1]=finalOut;
    }
    //reset output sum every 2 buffers
    if (bufferCount>2){
        //calculate RMS for metering and scale
        rmsOut=sqrt(outSum/(bufferSize*2))*1.2;
        bufferCount=0;
        outSum=0;
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(float * output,int bufferSize, int nChannels){
}

//--- my methods ---
//parameter listeners
void ofApp::tempoChanged(double & val){
    myClock.setTempo(val);
    //DEBUG uncomment to print tempo changes
    //   cout << "New tempo - " << val << endl;
}

void ofApp::tog1Changed(bool & val){
    setStep(editingThisSeq,0,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 1 - " << val << endl;
}
void ofApp::tog2Changed(bool & val){
    setStep(editingThisSeq,1,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 2 - " << val << endl;
}
void ofApp::tog3Changed(bool & val){
    setStep(editingThisSeq,2,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 3 - " << val << endl;
}
void ofApp::tog4Changed(bool & val){
    setStep(editingThisSeq,3,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 4 - " << val << endl;
}
void ofApp::tog5Changed(bool & val){
    setStep(editingThisSeq,4,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 5 - " << val << endl;
}
void ofApp::tog6Changed(bool & val){
    setStep(editingThisSeq,5,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 6 - " << val << endl;
}
void ofApp::tog7Changed(bool & val){
    setStep(editingThisSeq,6,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 7 - " << val << endl;
}
void ofApp::tog8Changed(bool & val){
    setStep(editingThisSeq,7,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 8 - " << val << endl;
}
void ofApp::tog9Changed(bool & val){
    setStep(editingThisSeq,8,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 9 - " << val << endl;
}
void ofApp::tog10Changed(bool & val){
    setStep(editingThisSeq,9,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 10 - " << val << endl;
}
void ofApp::tog11Changed(bool & val){
    setStep(editingThisSeq,10,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 11 - " << val << endl;
}
void ofApp::tog12Changed(bool & val){
    setStep(editingThisSeq,11,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 12 - " << val << endl;
}
void ofApp::tog13Changed(bool & val){
    setStep(editingThisSeq,12,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 13 - " << val << endl;
}
void ofApp::tog14Changed(bool & val){
    setStep(editingThisSeq,13,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 14 - " << val << endl;
}
void ofApp::tog15Changed(bool & val){
    setStep(editingThisSeq,14,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 15 - " << val << endl;
}
void ofApp::tog16Changed(bool & val){
    setStep(editingThisSeq,15,val);
    //DEBUG uncomment to print sequence changes
//    cout << "Change step 16 - " << val << endl;
}

//set step "index" of the sequence determined by "seqEditing" with the new value "newVal"
void ofApp::setStep(SeqToEdit seqEditing, int index, bool newVal){
    switch (seqEditing){
    case KICK:
        kickSeq[index]=newVal;
        break;
    case SNARE:
        snareSeq[index]=newVal;
        break;
    case TOM:
        tomSeq[index]=newVal;
        break;
    case CLAP:
        clapSeq[index]=newVal;
        break;
    case OPENHH:
        openHHSeq[index]=newVal;
        break;
    case CLOSEDHH:
        closedHHSeq[index]=newVal;
        break;
    default:
        cout << "Broken enum value. Exiting - " << seqEditing << endl;
        ofExit(1);
    }
}

//get step of sequence "seq" at index "index"
bool ofApp::getStep(SeqToEdit seq, int index){
    switch (seq){
    case KICK:
        return kickSeq[index];
        break;
    case SNARE:
        return snareSeq[index];
        break;
    case TOM:
        return tomSeq[index];
        break;
    case CLAP:
        return clapSeq[index];
        break;
    case OPENHH:
        return openHHSeq[index];
        break;
    case CLOSEDHH:
        return closedHHSeq[index];
        break;
    default:
        cout << "Broken enum value. Exiting - " << seq << endl;
        ofExit(1);
    }
}

//load sequence from array and onto GUI toggles
void ofApp::loadSeq(SeqToEdit newSelectedSeq){
    editingThisSeq = newSelectedSeq;
    redrawSteps();
}

//redraw sequence steps
void ofApp::redrawSteps(){
    for (int c=0;c<16;c++){
        switch (editingThisSeq){
        case KICK:
            togSteps[c].setup(std::to_string(c+1),kickSeq[c],ofGetWidth()*(1./25.),ofGetWidth()*(1./25.));
            togSteps[c].setPosition(5*(12.5*c+1),ofGetHeight()-70);
            break;
        case SNARE:
            togSteps[c].setup(std::to_string(c+1),snareSeq[c],ofGetWidth()*(1./25.),ofGetWidth()*(1./25.));
            togSteps[c].setPosition(5*(12.5*c+1),ofGetHeight()-70);
            break;
        case TOM:
            togSteps[c].setup(std::to_string(c+1),tomSeq[c],ofGetWidth()*(1./25.),ofGetWidth()*(1./25.));
            togSteps[c].setPosition(5*(12.5*c+1),ofGetHeight()-70);
            break;
        case CLAP:
            togSteps[c].setup(std::to_string(c+1),clapSeq[c],ofGetWidth()*(1./25.),ofGetWidth()*(1./25.));
            togSteps[c].setPosition(5*(12.5*c+1),ofGetHeight()-70);
            break;
        case OPENHH:
            togSteps[c].setup(std::to_string(c+1),openHHSeq[c],ofGetWidth()*(1./25.),ofGetWidth()*(1./25.));
            togSteps[c].setPosition(5*(12.5*c+1),ofGetHeight()-70);
            break;
        case CLOSEDHH:
            togSteps[c].setup(std::to_string(c+1),closedHHSeq[c],ofGetWidth()*(1./25.),ofGetWidth()*(1./25.));
            togSteps[c].setPosition(5*(12.5*c+1),ofGetHeight()-70);
            break;
        default:
            cout << "Broken enum value. Exiting - " << editingThisSeq << endl;
            ofExit(1);
        }
    }
}

//--- my classes ---
grimClap::grimClap(){
    grimClap::gain=1;
    grimClap::setFat(0);
    //master envelope
    grimClap::envelope1.setAttack(0);
    grimClap::envelope1.setDecay(170);
    grimClap::envelope1.setSustain(0.05);
    grimClap::envelope1.setRelease(80);
    grimClap::envelope1.holdtime=1;
    grimClap::envelope1.trigger=0;
    //noise envelope
    grimClap::envelope2.setAttack(5);
    grimClap::envelope2.setDecay(50);
    grimClap::envelope2.setSustain(0.05);
    grimClap::envelope2.setRelease(5);
    grimClap::envelope2.holdtime=1;
    grimClap::envelope2.trigger=0;
    //modulation envelope
    grimClap::envelope3.setAttack(15);
    grimClap::envelope3.setDecay(1000);
    grimClap::envelope3.setSustain(0.8);
    grimClap::envelope3.setRelease(10);
    grimClap::envelope3.holdtime=1;
    grimClap::envelope3.trigger=0;
}

//a rough clappy sound based on noise
double grimClap::play(){
    envOut1=envelope1.adsr(1.,envelope1.trigger);
    envOut2=envelope2.adsr(1.,envelope2.trigger);
    envOut3=envelope3.adsr(1.,envelope3.trigger);

    if (inverse) {

        envOut1=fabs(1-envOut1);
        envOut2=fabs(1-envOut2);
        envOut3=fabs(1-envOut3);

    }
    //the starting tone is just noise
    output=tone1.noise();

    //sine for filtering and low end snap
    output+=tone2.sinewave(mod1.sinewave(80)*fatness+1)*0.2+envOut2*fatness*0.01;

    //fast phasor for stutter effect
    output*=mod1.phasor(envOut1)*envOut1*0.1;

    if (envelope1.trigger==1) {
        envelope1.trigger=0;
        envelope2.trigger=0;
        envelope3.trigger=0;
    }

    return output*gain;
}

//fatness increases the low end of the clap slightly
void grimClap::setFat(double fat){
    fatness=fat;
}

void grimClap::trigger() {

    envelope1.trigger=1;
    envelope2.trigger=1;
    envelope3.trigger=1;

}

//A long tom sound (descending sine)
grimTom::grimTom(){

    grimTom::envelope1.setAttack(10);
    grimTom::setPitch(800);
    grimTom::envelope1.setDecay(30);
    grimTom::envelope1.setSustain(5);
    grimTom::envelope1.setRelease(800);
    grimTom::envelope1.holdtime=1;
    grimTom::envelope1.trigger=0;

    grimTom::envelope2.setAttack(5);
    grimTom::envelope2.setDecay(100);
    grimTom::envelope2.setSustain(10);
    grimTom::envelope2.setRelease(10);
    grimTom::envelope2.holdtime=100;
    grimTom::envelope2.trigger=0;

};

double grimTom::play(){

    envOut1=envelope1.adsr(1.,envelope1.trigger);
    envOut2=envelope2.adsr(1.,envelope2.trigger);

    if (inverse) {

        envOut1=fabs(1-envOut1);

    }
    //main descending tone
    output=(tone1.triangle(pitch*(0.1+(envOut1*0.35))))*envOut1*0.9;
    //add a bit of sine punch
    output+=tone2.sinewave(pitch*0.5)*envOut2*0.7;

    if (envelope1.trigger==1) {
        envelope1.trigger=0;
    }

    if (envelope2.trigger==1) {
        envelope2.trigger=0;
    }

    return output*gain;
};

void grimTom::setRelease(double release) {

    envelope1.setRelease(release);

}

void grimTom::setPitch(double newPitch) {

    pitch=newPitch;

}

void grimTom::trigger() {

    envelope1.trigger=1;
    envelope2.trigger=1;

}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //check if click is in the title bar of each instrument
    if (x>(guiKick.getPosition().x) && x<((guiKick.getPosition().x)+guiKick.getWidth()) && y>(guiKick.getPosition().y) && y<((guiKick.getPosition().y)+guiKick.getHeight())){
        //DEBUG print message
//        cout << "guiKick Click" << endl;
        loadSeq(KICK);
    }
    if (x>(guiTom.getPosition().x) && x<((guiTom.getPosition().x)+guiTom.getWidth()) && y>(guiTom.getPosition().y) && y<((guiTom.getPosition().y)+guiTom.getHeight())){
        //DEBUG print message
//        cout << "guiTom Click" << endl;
        loadSeq(TOM);
    }
    if (x>(guiSnare.getPosition().x) && x<((guiSnare.getPosition().x)+guiSnare.getWidth()) && y>(guiSnare.getPosition().y) && y<((guiSnare.getPosition().y)+guiSnare.getHeight())){
        //DEBUG print message
//        cout << "guiSnare Click" << endl;
        loadSeq(SNARE);
    }
    if (x>(guiClap.getPosition().x) && x<((guiClap.getPosition().x)+guiClap.getWidth()) && y>(guiClap.getPosition().y) && y<((guiClap.getPosition().y)+guiClap.getHeight())){
        //DEBUG print message
//        cout << "guiClap Click" << endl;
        loadSeq(CLAP);
    }
    if (x>(guiOpenHH.getPosition().x) && x<((guiOpenHH.getPosition().x)+guiOpenHH.getWidth()) && y>(guiOpenHH.getPosition().y) && y<((guiOpenHH.getPosition().y)+guiOpenHH.getHeight())){
        //DEBUG print message
//        cout << "guiOpenHH Click" << endl;
        loadSeq(OPENHH);
    }
    if (x>(guiClosedHH.getPosition().x) && x<((guiClosedHH.getPosition().x)+guiClosedHH.getWidth()) && y>(guiClosedHH.getPosition().y) && y<((guiClosedHH.getPosition().y)+guiClosedHH.getHeight())){
        //DEBUG print message
//        cout << "guiClosedHH Click" << endl;
        loadSeq(CLOSEDHH);
    }
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
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
