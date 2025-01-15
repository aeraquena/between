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
    // min and max threshold to account for sloped surface (angled camera)
    ofxIntSlider minNearThreshold;
    ofxIntSlider maxNearThreshold;
    ofxIntSlider minFarThreshold;
    ofxIntSlider maxFarThreshold;
    
    // Blob properties
    ofxIntSlider minBlobArea;
    ofxIntSlider maxBlobArea;
    ofxIntSlider maxBlobNum;
    
    // Number of blobs in previous frame
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
    
    // Scale value of projection resolution to Kinect resolution
    ofxFloatSlider scaleVal;
    
    /******************
     * MARK: Graphics *
     ******************/
    
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
    ofRectangle prevTargetRect; // fixed target rectangle of previous turn
    ofRectangle nextTargetRect; // fixed target rectangle of next turn
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
    
    // differences in bound width - create a trapezoid to account for keystoned projections
    ofxIntSlider leftBoundsDiff;
    ofxIntSlider rightBoundsDiff;
    
    // Grid offset - so it matches target
    ofxIntSlider gridXOffset;
    ofxIntSlider gridYOffset;
    
    /**************
     * MARK: Text *
     **************/
    
    // Font
    ofTrueTypeFont franklinBook;
    
    /*************
     * MARK: GUI *
     *************/
    
    // Display modes
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
    
    ofImage shoes;
    ofxIntSlider shoesX;
    ofxIntSlider shoesY;
    ofxFloatSlider shoesScale;
    ofxFloatSlider shoesScale2;

    ofxIntSlider textX;
    ofxIntSlider textX2;
    ofxIntSlider textY;
    
    // Note: These are all in square units... should be multiplied by GRID_SQUARE_SIZE
    int targetRectX;
    int targetRectY;
    int targetRectW;
    int targetRectH;
    ofRectangle targetRect; // animated: lerps between prevTargetRect and nextTargetRect
    
    vector<ofRectangle> boundingBoxes;
    
    int BOUNDING_BOX_MARGIN = 8;
    
    int MIN_TARGET_SIZE = 1;
    
    ofxIntSlider xOffset;
    ofxIntSlider yOffset;
    ofxIntSlider xRange;
    ofxIntSlider yRange;
    ofxIntSlider GRID_SQUARE_SIZE;
    ofxToggle showTargetRange;
    
};
