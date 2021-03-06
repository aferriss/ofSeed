#include "ofApp.h"
#include <iostream>
#include <vector>
#include <algorithm>
float theta = 0.0;
float g3 = 150;
//cp -r bin/data "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Resources";  

//--------------------------------------------------------------
void ofApp::setup(){
    w = 1024;
    h = 2048;
    
    //ofSeedRandom(1000);
    
    inc = 0.000001;
    
    ofSetWindowShape(w, w);
    
    //ofSetDataPathRoot("../Resources/data");
        
    noiseShader.load("shaders/noise");
    reposShader.load("shaders/repos");
    emboss.load("shaders/emboss");
    
//    sheepStencil.load("sheepStencil3.png");
//    
//    sheepImg.load("images/sheep4S.jpg");
//    w = sheepImg.getWidth();
//    h = sheepImg.getHeight();
//    sheepPix = sheepImg.getPixels();
    
    
//    sheepImg.resize(w, h);
    
    finishIt = false;
    numRandSeeds = 1;
    step = 0;
    steps = w*h;
    
    ofSetFrameRate(80);
    ofSetBackgroundAuto(false);
    ofEnableAlphaBlending();
    //ofSetBackgroundColor(0);
    ofBackground(0);
    
    
    //noiseFbo.allocate(w,h,GL_RGBA);
    reposFbo.allocate(w,h,GL_RGB);
    //feedback.allocate(w,h,GL_RGBA);
    //embossFbo.allocate(w, h,GL_RGBA);
    
    
    fbPix.allocate(w,h, 4);
    
    noiseFbo.setUseTexture(true);
    reposFbo.setUseTexture(true);
    embossFbo.setUseTexture(true);
    
    screen.grabScreen(0, 0, w, h);
    screen.setUseTexture(true);
    
    traversed = new bool[w*h*4];
    
    
    
    noise = new ofxPerlin();
    noise->noiseDetail(16, 0.15);
	simplex = new ofxSimplex();
    
    //ofSetVerticalSync(true);
    
    //reposFbo.begin();
    //ofClear(255,255,255,0);
    //reposFbo.end();
//    ofSetMinMagFilters(GL_NEAREST,GL_NEAREST);
    
    save = false;
    turnBlack = true;
    rr = 128;
    speed =1.0;
    
    //soundStream.setup(this, 2, 0, 44100, w*h, 4);
    //sampleCount = 100;
    
    pix = new unsigned char[w*h];
    
    vidRecorder.setVideoCodec("mpeg4"); 
    vidRecorder.setVideoBitrate("20000k");
    
    //ofDisableArbTex();
    
    
    stencilFbo.allocate(w, h, GL_RGB);
//    stencilFbo.begin();
//        ofClear(255);
//        for(int i = 0; i<40; i++){
//            float scale = ofRandom(10,200);
//            sheepStencil.draw((int)ofRandom(w), (int)ofRandom(h), -scale, scale);
//        }
//    stencilFbo.end();
//    
//    
//    
//    
//    ofPixels sheepPix ;//= sheepStencil.getPixels();
//    stencilFbo.readToPixels(sheepPix);
//    for (int i = 0; i<stencilFbo.getWidth()*stencilFbo.getHeight(); i++){
//        
//        if(int(sheepPix[i*3]) < 10){
//            int seed = i;
//            seeds.push_back(seed);
//            traversed[seed] = true;
//        }
//        
//        //cout<<ofToString(int(sheepPix[i*4+3]))<<endl;
//    }
    
    useImage = true;
    
    if(useImage){
        earthImg.load("images/mountains2blur2.jpg");
        earthImg.resize(w, h);
        earthPix = earthImg.getPixels();
        
//        vector<int> redPix;
        
//        for(int i = 0; i<w*h; i++){
//            int redPixel = (int)earthPix[i*3];
//            redPix.push_back(redPixel);
//        }
//        
//        std::sort(redPix.begin(), redPix.end());
//        
//        for(int i = 0; i<w*h; i++){
//            earthPix[i*3] = redPix[i];
//        }
        for(int i =0; i<w*h; i++){
            Pixel p;
            p.pixelColor.x = (int)earthPix[i*3];
            p.pixelColor.y = (int)earthPix[i*3+1];
            p.pixelColor.z = (int)earthPix[i*3+2];
            
            p.pixelLoc = i;
            
            p.pixelBri = (p.pixelColor.x + p.pixelColor.y + p.pixelColor.y)/3;
            
            pixelArray.push_back(p);
        }
    }
    
    
    gui = new ofxDatGui(0,0);
    
    numSeedsSlider = gui->addSlider("Num seeds", 0, 1000);
    numSeedsSlider->setPrecision(0);
    
    xNoise = gui->addSlider("X noise", 0, 0.1);
    yNoise = gui->addSlider("Y noise", 0, 0.1);
    zNoise = gui->addSlider("Z noise", 0, 0.1);
    
    xNoise->setPrecision(10);
    yNoise->setPrecision(10);
    zNoise->setPrecision(10);
    
    xNoise->setValue(0.0007);
    yNoise->setValue(0.0009);
    zNoise->setValue(0.001);
    
    
    stepSlider = gui->addSlider("I-Step", 0.001,5);
    stepSlider->setPrecision(4);
    stepSlider->setValue(1);
    
    randSlider= gui->addSlider("Random Amt", 1,1.2);
    randSlider->setPrecision(4);
    randSlider->setValue(1.1888);
    
    hideGui = false;
    ofxDatGuiLog::quiet();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(save == true){
        //sort.grabScreen(0, 0, w, h);
        //saver.addFrame(screen.getPixels());
//        ofPixels savePix;
//        reposFbo.readToPixels(savePix);
//        vidRecorder.addFrame(savePix);
        
        ofPixels savePix;
        reposFbo.readToPixels(savePix);
        ofSaveImage(savePix, "saved/"+ofGetTimestampString()+".png");
        save = false;
        
    }
    
}


//--------------------------------------------------------------
void ofApp::draw(){


    
    if(step == 0){
        for(int i = 0; i<=numRandSeeds; i++){
            int seed = int(ofRandom(w*h));
            seeds.push_back(seed);
            traversed[seed] = true;
        }
    }
    else{
        updateSeeds();
    }
    
    vector<int> redPix (seeds.size(), 0);
    vector<int> greenPix (seeds.size(), 0);
    vector<int> bluePix (seeds.size(), 0);
    
    vector<Pixel> pixpix;
    
    
    auto screenPix = screen.getPixels();
    
    for (int i = seeds.size()-1; i>= 0; i--){
//        pixpix [i] = pixelArray[ seeds[i] ].pixelBri;
        pixpix.push_back( pixelArray[seeds[i]]);
        //pixpix[i] = pixelArray[ seeds[i] ];
        
//        redPix[ i ] = (int)earthPix[ seeds[i] *3 ];
//        greenPix[ i ] = (int)earthPix[ seeds[i] * 3 +1];
//        bluePix[ i ] = (int)earthPix[ seeds[i] *3 +2];
    }
    
//    std::sort(redPix.begin(), redPix.end());
//    std::sort(greenPix.begin(), greenPix.end());
//    std::sort(bluePix.begin(), bluePix.end());
    std::sort(pixpix.begin(), pixpix.end());
    
    
    
    for(int i = seeds.size()-1; i>=0; i--){
        int loc = i;
//        if( (int)sheepPix[ seeds[i]*3 ] <= 30){
        screenPix[ seeds[i] *3] = pixpix[loc].pixelColor.x;
        screenPix[ seeds[i] *3+1] = pixpix[loc].pixelColor.y;
        screenPix[ seeds[i] *3+2] = pixpix[loc].pixelColor.z;
//        }
//        screenPix[loc+1] = 0;
//        screenPix[loc+2] = 0;
    }
    
//    for(int i = 0; i<w*h; i++){
//        int loc = i*3;
//        screenPix[loc] = 255;
//        screenPix[loc+1] = 0;
//        screenPix[loc+2] = 0;
//    }
    
    screen.setFromPixels(screenPix);
    screen.update();
    
    if(step < steps){
        step++;
        //cout<<ofToString(step)+"/"+ofToString(steps)<<endl;
        //screen.update();
    }
    
    else{
        cout<<"done"<<endl;
    }
    
    /*
    if(step%1000 == true){
        numRandSeeds = int(ofRandom(5));
        //for(int i=0; i<numRandSeeds; i++){
            int seed = (int)ofRandom(w*h);
            seeds.push_back(seed);
            traversed = new bool[w*h];
            noise->noiseSeed(ofRandom(10000000));
        //}
    }
    */
    //ofPopStyle();
    
    if(step%1000 == true){
        //noise->noiseSeed(ofRandom(100000));
    }
        

    
//    stencilFbo.begin();
//    if(ofGetMousePressed()){
//        sheepStencil.draw(ofGetMouseX(), ofGetMouseY());
//    }
//    stencilFbo.end();
    

    
    reposFbo.begin();
    /*
        reposShader.begin();
            reposShader.setUniformTexture("tex0", screen.getTexture(), 0);
            reposShader.setUniformTexture("tex1", stencilFbo.getTexture(), 1);
            reposShader.setUniformTexture("tex2", stencilFbo.getTexture(), 2);
        
            reposShader.setUniform2f("params", ofMap(ofGetMouseX(),0,800,-2,2), ofMap(ofGetMouseY(),0,800,0.5,50) );
            reposShader.setUniform2f("res", w, h);

        screen.draw(0,0);
    
    reposShader.end();
    */
    
    screen.draw(0,0);
        
//    
    reposFbo.end();
//
    reposFbo.draw(0,0, w, h);
//  stenTex.draw(0,0);

    
}

//--------------------------------------------------------------
void ofApp::updateSeeds(){
    float xOff = 0.0;
    float zOff = 0.0;
    
    
    //float xOff = ofGetElapsedTimef()*0.9;
    //float zOff = ofGetElapsedTimef()*0.8;
    

    
    
    /*
    float inc  = 0;
    float z = 0;
    z+=0.5;
    if(z == w){
        z = 0;
    }
    inc+=0.0000000000000005;
     */
    
    auto pixPtr = screen.getPixels();
//    auto sheepPtr = sheepImg.getPixels();
    
    for(int i = seeds.size()-1; i>=0; i--){
        float yOff = 0.0;
        //float yOff = ofGetElapsedTimef()*0.4;
        
        
        int x = int(seeds[i] % w);
        int y = int(seeds[i] / w);
        
        
        
        //float r = 255-ofSignedNoise(zOff,xOff,yOff,ofGetElapsedTimef()/30)*512; //red
        //float g = 255-ofSignedNoise(xOff,yOff,zOff,ofGetElapsedTimef()/30)*512; //green
        //float b = 255-ofSignedNoise(yOff,zOff,yOff,ofGetElapsedTimef()/30)*512; //blue
        
        float r2 = 255 - noise->noiseuf(xOff, yOff, zOff) * 512;
        float g2 = 255 - noise->noiseuf(yOff, zOff, xOff) * 512;
        float b2 = 255 - noise->noiseuf(zOff, xOff, yOff) * 512;//(noise->noise(zOff));
        
        float briMod = r2;
        briMod = int(ofMap(briMod,0,255,0,3));
        briMod = ofClamp(briMod, 0, 2);
        
        
        //screen.setColor(x, y, c);
        
        int loc = (y*w+ x)*3;
        
        if(useImage){
//            pixPtr[loc] = earthPix[loc];
//            pixPtr[loc+1] = earthPix[loc+1];
//            pixPtr[loc+2] = earthPix[loc+2];
        } else{
            pixPtr[loc] = r2;//sheepPtr[loc] * (r2*0.025);
            pixPtr[loc+1] = g2;//sheepPtr[loc] * (r2*0.025);
            pixPtr[loc+2] = b2;//sheepPtr[loc] * (r2*0.025);
        }
        //screen.setFromPixels(pixPtr, w, h, OF_IMAGE_COLOR);
        //screen.setFromPixels();
        
        int n = int(abs(noise->noise(zOff, yOff, xOff))*tan(sin(ofRandom(ofRandom(ofGetFrameNum()*0.9))*3.141592)*randSlider->getValue()))%256;
        
        //cout<<ofToString(c)<<endl;
        briMod = int(ofRandom(0,2));
        
        //---------Up, Right, Down, Left
        int upIndex = seeds[i] - w ;//+ briMod;// (int)ofMap(ofNoise(xOff,yOff),0,1,0,int(ofRandom(2)));
        int rightIndex = seeds[i] + 1 ;//- briMod;// (int)ofMap(ofNoise(xOff,yOff),0,1,0,int(ofRandom(2)));
        int downIndex = seeds[i] + w ;//- briMod;// (int)ofMap(ofNoise(xOff,yOff),0,1,0,int(ofRandom(2)));
        int leftIndex = seeds[i] - 1 ;//+ briMod;// (int)ofMap(ofNoise(xOff,yOff),0,1,0,int(ofRandom(2)));
        
        //ofSetColor(c);
        
        //ofRect(0, 0, 100, 100);
        //inc+=0.001;
        int iStep = i / stepSlider->getValue();
        
        if(y-iStep>0 && !traversed[upIndex]){
            seeds.push_back(upIndex);
            traversed[upIndex] = true;
        }
        
        if(x < w-1-iStep && !traversed[rightIndex]){
            seeds.push_back(rightIndex);
            traversed[rightIndex] = true;
        }
        
        if(y< h-1-iStep && !traversed[downIndex]){
            seeds.push_back(downIndex);
            traversed[downIndex] = true;
        }
        
        if(x-i > 0 && !traversed[leftIndex]){
            seeds.push_back(leftIndex);
            traversed[leftIndex] = true;
        }
        
        
        //---------Upper Left, Upper Right, Lower Left, Lower Right 
        int ulIndex = seeds[i] - w - 1 - int(ofRandom(-2,2));//int(ofMap(noise->noise(xOff, yOff), -1,1,-2,2));
        int urIndex = seeds[i] - w + 1 + int(ofRandom(-2,2));//int(ofMap(noise->noise(xOff, yOff), -1,1,-2,2));
        int llIndex = seeds[i] + w - 1 - int(ofRandom(-2,2));//int(ofMap(noise->noise(xOff, yOff), -1,1,-2,2));
        int lrIndex = seeds[i] + w + 1 + int(ofRandom(-2,2));//int(ofMap(noise->noise(xOff, yOff), -1,1,-2,2));
    
        if(x-i>0 && y-i>0 && !traversed[ulIndex]){
            seeds.push_back(ulIndex);
            traversed[ulIndex] = true;
        }
        
        if( y-i>0 && !traversed[urIndex]){
            seeds.push_back(urIndex);
            traversed[urIndex] = true;
        }
        
        if(y < h-1-i &&  !traversed[llIndex]){
            seeds.push_back(llIndex);
            traversed[llIndex] = true;
        }
        
        if(x< w-1-i && y< h-1-i && !traversed[lrIndex]){
            seeds.push_back(lrIndex);
            traversed[lrIndex] = true;
        }
        
        

        seeds.erase(seeds.begin()+i);
        
        xOff+=xNoise->getValue();
        yOff+=yNoise->getValue();
        
    }
//    cout<<ofToString(xOff)<<endl;
    zOff+= zNoise->getValue();
    
    //if(ofGetFrameNum()%300 == 0){
//        screen.setFromPixels(pixPtr);
    //}
    
    }

//--------------------------------------------------------------
/*
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    
    for (int i = 0; i < bufferSize; i++){
        float val = ofMap(pix[sampleCount],0,255,-1,1);
        output[i*nChannels    ] = sin(val) * 0.9;
        output[i*nChannels + 1] = sin(val) * 0.9;
        
        sampleCount++;
        sampleCount %= w*w;
    }
    
    
}
*/

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == '='){
        
        
        for(int i=0; i<numRandSeeds; i++){
            int seed = (int)ofRandom(w*h);
            seeds.push_back(seed);
            traversed = new bool[w*h];
            noise->noiseSeed(ofRandom(10000000));
            inc+=1;
        }
    }
    
    if(key == 's'){
        //saver.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_HIGH);
        //saver.setup(w, h, "nike"+ofToString(ofGetTimestampString())+".mov");
        
        //vidRecorder.setup("sheep"+ofToString(ofGetTimestampString())+".mov", w, h, 30);
        //vidRecorder.start();
        save = !save;
    }
    
    if(key == 'f'){
        ofSetFullscreen(true);
    }
    
    if(key == 'u'){
        reposFbo.draw(0,0,w/4,h/4);
    }
    
    if(key == ' '){
        seeds.clear();
        numRandSeeds = numSeedsSlider->getValue();
        for(int i = 0; i<numRandSeeds; i++){
            int seed = (int)ofRandom(w*h);
            seeds.push_back(seed);
            traversed = new bool[w*h];
        }
        noise->noiseSeed(ofRandom(10000000));
    }
    
    if(key == 'h'){
        hideGui = !hideGui;
        gui->setVisible(hideGui);
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

void ofApp::swap(unsigned char a[], int rx, int ry, int gx, int gy, int bx, int by ) {
    // if(a[rx-3]>a[rx]){ //swap reds
    int tempR = a[rx];
    a[rx] = a[ry];
    a[ry] = tempR;
    // }
    // if(a[gx-3]>a[gx]){ //swap greens
    int tempG = a[gx];
    a[gx] = a[gy];
    a[gy] = tempG;
    // }
    // if(a[bx-3]>a[bx]){ //swap blues
    int tempB = a[bx];
    a[bx] = a[by];
    a[by] = tempB;
    // }
}


void ofApp::exit(){
    //saver.finishMovie();
    vidRecorder.close();
}
