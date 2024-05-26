#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxDelaunay.h"

class ofApp : public ofBaseApp {
public:
    
    /*******************
     * MARK: Functions *
     *******************/
    
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void updateTargets();
    bool blobIsTouchingEdge(ofxCvBlob blob, int roiX, int roiY, int roiW, int roiH);
    int lerp(float targetPercent, int prev, int next);
    
    /*****************
     * MARK: Structs *
     *****************/
    
    // Shape color
    typedef struct {
        ofColor light;
        ofColor dark;
        ofColor dot;
    } shapeColor;
    
    // Target square
    typedef struct {
        int x;
        int y;
        bool touching;
    } targetSquare;
    // x and y are in terms of raw size, before multiplying
    
    /********************
     * MARK: Resolution *
     ********************/
    
    // Projection
    int PROJECTION_WIDTH;
    int PROJECTION_HEIGHT;
    
    // Laptop screen
    int SCREEN_WIDTH = 1440; // 1280
    int SCREEN_HEIGHT = 900; // 800
    
    // Scale value of projection resolution to Kinect resolution
    int scaleVal;
    
    /****************
     * MARK: Kinect *
     ****************/
    
    ofxKinect kinect;
    
    // Kinect images
    ofxCvColorImage colorImg; // color video image
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    ofxCvContourFinder contourFinder;
    
    // Movable rectangle to calculate region of interest / subsection of original image
    ofxIntSlider roiX;
    ofxIntSlider roiY;
    ofxIntSlider roiW;
    ofxIntSlider roiH;
    
    // Depth thresholds
    //ofxIntSlider nearThreshold;
    ofxIntSlider minNearThreshold;
    ofxIntSlider maxNearThreshold;
    ofxIntSlider minFarThreshold;
    ofxIntSlider maxFarThreshold;
    
    // Blob properties
    ofxIntSlider minBlobArea;
    ofxIntSlider maxBlobArea;
    ofxIntSlider maxBlobNum;
    
    int prevNumBlobs = 0;
    
    /**************
     * MARK: FBOs *
     **************/
    
    // Final FBO combining all body shapes to output onto projection
    ofFbo finalFbo;
    
    // Shape FBOs
    vector<ofFbo> shapeFbos;
    
    // Texture FBOs
    vector<ofFbo> textureFbos;
    
    // Target FBO
    ofFbo targetFbo;
    
    // Max number of shape FBOs
    const int NUM_SHAPE_FBOS = 6; //3
    
    // Position of shape and target FBOs
    ofxIntSlider fboLeft;
    ofxIntSlider fboTop;
    
    // Additional position of shape FBO
    ofxIntSlider shapeFboTop;
    ofxIntSlider shapeFboLeft;
    
    /******************
     * MARK: Graphics *
     ******************/
    
    // Target triangle line width
    
    // Smoothing values for blobs
    ofxIntSlider smoothingSize;
    ofxFloatSlider smoothingShape;
    ofxIntSlider blurValue;
    ofxIntSlider blurThreshold;
    
    // Colors
    vector<shapeColor> shapeColors;
    
    /*****************
     * MARK: Targets *
     *****************/
    
    // Rectangles of target points
    ofRectangle prevTargetRect;
    ofRectangle nextTargetRect;
    float targetLerpPercent = 1.;
    bool firstTime = true;
    
    bool polygonIsTouchingRect;
    
    // Target values
    bool moveTarget;
    
    // Bounds dimensions
    ofxIntSlider boundsX;
    ofxIntSlider boundsY;
    ofxIntSlider boundsW;
    ofxIntSlider boundsH;
    //ofxIntSlider boundsW2;
    ofxIntSlider leftBoundsDiff;
    ofxIntSlider rightBoundsDiff;
    
    
    // Circles for seat cushions
    ofxIntSlider circle1X;
    ofxIntSlider circle1Y;
    ofxIntSlider circle2X;
    ofxIntSlider circle2Y;
    ofxIntSlider circle1Radius;
    ofxIntSlider circle2Radius;
    
    /**************
     * MARK: Text *
     **************/
    
    // Font
    ofTrueTypeFont franklinBook;
    
    /*************
     * MARK: GUI *
     *************/
    
    // Display modes
    int roomMode;
    int videoMode;
    
    // GUI
    ofxPanel gui;
    bool bHide;
    
    // Delaunay triangulation
    ofxDelaunay triangulation;
    
    bool triangulationVisible;
    
    /**********************
     * MARK: Target rectangle *
     **********************/
    
    //int GRID_SQUARE_SIZE;
    
    ofImage shoes;
    ofxIntSlider shoesX;
    ofxIntSlider shoesY;
    ofxFloatSlider shoesScale;
    ofxFloatSlider shoesScale2;

    ofxIntSlider textX;
    ofxIntSlider textX2;
    ofxIntSlider textY;
    
    // Note: These are all in square units... should be multiplied by grid square size
    int targetRectX;
    int targetRectY;
    int targetRectW;
    int targetRectH;
    ofRectangle targetRect;
    
    vector<ofRectangle> boundingBoxes;
    
    int BOUNDING_BOX_MARGIN = 8;
    
    ofxIntSlider xOffset;
    ofxIntSlider yOffset;
    ofxIntSlider xRange;
    ofxIntSlider yRange;
    ofxIntSlider GRID_SQUARE_SIZE;
    
};
