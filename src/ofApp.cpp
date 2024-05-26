#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    /****************************
     * MARK: Changing variables *
     ****************************/
    
    // Physical setup
    
    // 0 = Dual screen desktop
    // 1 = Bedroom wall
    // 2 = Single screen desktop
    // 3 = Church floor, single projection (presentation mode)
    // 4 = Church floor, dual screen and projection
    roomMode = 4;
    
    // Mode for video on first screen
    // Mode for video on first screen
    
    // 1. Video
    // 2. Depth video
    // 3. Threshold
    // 4. Contour
    videoMode = 3;
    
    // Set frame rate
    ofSetFrameRate(60);
    
    /****************
     * MARK: Kinect *
     ****************/
    
    // Enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();
    
    kinect.open();
    
    // Allocate images
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    /************************
     * MARK: Initialize GUI *
     ************************/
    
    gui.setup();
    
    // Scale value of projection resolution to Kinect resolution
    scaleVal = 2;
    
    // Set Kinect depth detection thresholds
    if (roomMode == 0) {
        // Desktop
        // 4 feet away
        
        //gui.add(nearThreshold.setup("near threshold", 177, 0, 255));
        
        gui.add(roiX.setup("roi x", 48, 0, 640));
        gui.add(roiY.setup("roi y", 50, 0, 480));
        gui.add(roiW.setup("roi w", 550, 0, 640));
        gui.add(roiH.setup("roi h", 376, 0, 480));
        
    } else if (roomMode == 1) {
        // Bedroom wall
        // 8 feet away
        
        //gui.add(nearThreshold.setup("near threshold", 177, 0, 255));
        
        gui.add(roiX.setup("roi x", 163, 0, 640));
        gui.add(roiY.setup("roi y", 136, 0, 480));
        gui.add(roiW.setup("roi w", 294, 0, 640));
        gui.add(roiH.setup("roi h", 182, 0, 480));
        
    } else if (roomMode == 2) {
        // Desktop
        // 4 feet away
        
        //gui.add(nearThreshold.setup("near threshold", 242, 0, 255));
        
        gui.add(roiX.setup("roi x", 137, 0, 640));
        gui.add(roiY.setup("roi y", 35, 0, 480));
        gui.add(roiW.setup("roi w", 501, 0, 640));
        gui.add(roiH.setup("roi h", 388, 0, 480));
        
        scaleVal = 1;
    } else if (roomMode == 3 || roomMode == 4) {
        // THIS IS THE ONE TO MODIFY
        // Church floor
        // 10.5 feet away
        
        //gui.add(nearThreshold.setup("near threshold", 135, 0, 255));
        
        // Home
        gui.add(roiX.setup("roi x", 30, 0, 640));
        gui.add(roiY.setup("roi y", 50, 0, 480));
        gui.add(roiW.setup("roi w", 556, 0, 640));
        gui.add(roiH.setup("roi h", 326, 0, 480));
        
        scaleVal = 2;
    }
    
    // Bounds parameters
    gui.add(boundsX.setup("bounds x", 250, -500, 500)); // 263
    gui.add(boundsY.setup("bounds y", 40, 0, 500));
    gui.add(boundsW.setup("bounds w", 1360, 400, 2000)); //802
    gui.add(leftBoundsDiff.setup("right bounds diff", 0, -170, 0)); // inverted intentionally
    gui.add(rightBoundsDiff.setup("left bounds diff", 0, -104, 0));
    gui.add(boundsH.setup("bounds h", 1005, 220, 1280));
    
    // Set blob parameters
    if (roomMode == 2) {
        // Minimum blob area
        gui.add(minBlobArea.setup("min blob area", 22000, 1000, 30000));
        
        gui.add(minFarThreshold.setup("min far threshold", 0, 0, 70));
        gui.add(maxFarThreshold.setup("max far threshold", 0, 0, 70));
    } else {
        // Minimum blob area
        gui.add(minBlobArea.setup("min blob area", 500, 500, 30000));
        
        // Depth thresholds
        gui.add(minFarThreshold.setup("min far threshold", 0, 0, 255));
        gui.add(maxFarThreshold.setup("max far threshold", 0, 0, 255));
    }
    
    gui.add(minNearThreshold.setup("min near threshold", 90, 0, 255)); // For NYU: 114
    gui.add(maxNearThreshold.setup("max near threshold", 90, 0, 255)); // For NYU: 141
    
    // Blob values
    gui.add(maxBlobArea.setup("max blob area", (roiW * roiH) - 4000, 0, roiW * roiH));
    gui.add(maxBlobNum.setup("max blob num", 6, 1, 50));
    
    // Smoothing values for blobs
    gui.add(smoothingSize.setup("smoothing size", 11, 0, 100));
    gui.add(smoothingShape.setup("smoothing shape", 0, 0, 1));
    gui.add(blurValue.setup("blur value", 25, 0, 100)); // must be odd. was 7
    gui.add(blurThreshold.setup("blur threshold", 247, 0, 255)); //20

    // Position of shape and target FBOs
    // To align silhouettes with bodies
    
    if (roomMode == 2) {
        gui.add(fboLeft.setup("fbo left", -300, -400, 300));
        gui.add(fboTop.setup("fbo top", 0, -200, 400));
        gui.add(shapeFboTop.setup("shape fbo top", 280, -332, 0));
        gui.add(shapeFboLeft.setup("shape fbo left", 7, -200, 200));
    } else {
        // THIS IS THE ONE TO MODIFY
        gui.add(fboLeft.setup("fbo left", 88, -300, 400));
        gui.add(fboTop.setup("fbo top", 0, -200, 300));
        gui.add(shapeFboTop.setup("shape fbo top", 271, -400, 400));
        gui.add(shapeFboLeft.setup("shape fbo left", 450, -200, 700));
    }
    
    // Seat circles
    /*gui.add(circle1X.setup("circle R x", 214, 205, 245));
    gui.add(circle1Y.setup("circle R y", 472, 455, 495));
    gui.add(circle1Radius.setup("circle R radius", 0, 87, 127));
    gui.add(circle2X.setup("circle L x", 1100, 875, 1315));
    gui.add(circle2Y.setup("circle L y", 488, 475, 515));
    gui.add(circle2Radius.setup("circle L radius", 0, 90, 130));*/
    
    gui.add(shoesX.setup("shoes x", -1800, -2500, 0));
    gui.add(shoesY.setup("shoes y", -40, -500, 500));
    gui.add(shoesScale.setup("shoes scale", .44, 0., 1.));
    gui.add(shoesScale2.setup("shoes scale 2", .22, 0., 1.));
    
    gui.add(textX.setup("text x", -3620, -4000, -3000));
    gui.add(textX2.setup("text x2", -3575, -4000, -3000));
    gui.add(textY.setup("text y", -2440, -4000, 2000));
    
    gui.add(xOffset.setup("x offset",4,0,20)); // the x value where we should start generating INNER shapes
    gui.add(yOffset.setup("y offset",3,0,12));
    gui.add(xRange.setup("x range",3,0,20)); // the range of the play area x must be at least 3 (min square size)
    gui.add(yRange.setup("y range",4,0,12));
    gui.add(GRID_SQUARE_SIZE.setup("grid square size",125,40,200));
    
    // Hide controls
    bHide = false;
    
    if (roomMode == 3) {
        bHide = true;
    }
    
    // Triangulation visible
    triangulationVisible = false;
    
    /***********************
     * MARK: Allocate FBOs *
     ***********************/
    
    if (roomMode == 3 || roomMode == 4) {
        PROJECTION_WIDTH = 1920;
        PROJECTION_HEIGHT = 1200; //1080
    } else if (roomMode == 2) {
        PROJECTION_WIDTH = 640;
        PROJECTION_HEIGHT = 480;
    } else {
        PROJECTION_WIDTH = 1280;
        PROJECTION_HEIGHT = 960;
    }

    finalFbo.allocate(PROJECTION_WIDTH,PROJECTION_HEIGHT);
    targetFbo.allocate(PROJECTION_WIDTH,PROJECTION_HEIGHT);
    
    for (int i = 0; i < NUM_SHAPE_FBOS; i++) {
        ofFbo shapeFbo;
        shapeFbo.allocate(PROJECTION_WIDTH,PROJECTION_HEIGHT);
        shapeFbos.push_back(shapeFbo);
        ofFbo textureFbo;
        textureFbo.allocate(PROJECTION_WIDTH,PROJECTION_HEIGHT);
        textureFbos.push_back(textureFbo);
    }
    
    /*************************
     * MARK: Set up graphics *
     *************************/
    
    // TODO: Change to green whenever I want to see bg color
    ofBackground(0);
    
    ofSetPolyMode(OF_POLY_WINDING_POSITIVE);
    
    ofEnableAntiAliasing();
    
    ofEnableSmoothing();
    
    /*********************
     * MARK: Set up text *
     *********************/
    
    // Font
    ofTrueTypeFont::setGlobalDpi(72);
    
    franklinBook.load("frabk.ttf", 150);
    franklinBook.setLineHeight(18.0f);
    franklinBook.setLetterSpacing(1.037);
    
    /**********************
     * MARK: Shape colors *
     **********************/
    
    // Shape colors
    // Gradients between neighboring colors
    
    // Red -> orange
    shapeColor redOrangeColor = {
        ofColor(219,0,67),
        ofColor(245,131,87),
        ofColor(161, 22, 18)
    };
    shapeColors.push_back(redOrangeColor);
    
    // Orange -> yellow
    shapeColor orangeYellowColor = {
        ofColor(217,43,0),
        ofColor(246,240,36),
        ofColor(208,29,23)
    };
    shapeColors.push_back(orangeYellowColor);
    
    // Yellow -> green
    shapeColor yellowGreenColor = {
        ofColor(246,240,36),
        ofColor(42,185,94),
        ofColor(4,64,50)
    };
    shapeColors.push_back(yellowGreenColor);
    
    // Green -> blue
    shapeColor greenBlueColor = {
        ofColor(42,185,94),
        ofColor(55,2,243),
        ofColor(36,0,168)
    };
    shapeColors.push_back(greenBlueColor);
    
    // Blue -> purple
    shapeColor bluePurpleColor = {
        ofColor(55,2,243),
        ofColor(187,115,239),
        ofColor(58,0,99)
    };
    shapeColors.push_back(bluePurpleColor);
    
    // Pink -> red
    shapeColor pinkRedColor = {
        ofColor(221,66,236),
        ofColor(219,0,67),
        ofColor(179,0,0)
    };
    shapeColors.push_back(pinkRedColor);
    
    /*************************
     * MARK: Set up target squares *
     *************************/
    
    //moveTarget = true;
    updateTargets();

    /*************************
     * MARK: Set up image *
     *************************/
    //shoes.load("shoes.png");
}

//--------------------------------------------------------------
void ofApp::update() {
    
    /****************
     * MARK: Kinect *
     ****************/
    
    kinect.update();
    
    // There is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        int w = kinect.getWidth();
        int h = kinect.getHeight();
        
        // Load grayscale depth image from the Kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());
        
        // Threshold Kinect pixels
        ofPixels & pix = grayImage.getPixels();
        int numPixels = pix.size();
        for(int i = 0; i < numPixels; i++) {
            // Calculate row, then calculate far threshold based on row
            int row = i / kinect.width;
            int thisFarThreshold = ofMap(row, roiY, roiY+roiH, minFarThreshold, maxFarThreshold, true);
            
            // Calculate row, then calculate near threshold based on row
            int thisNearThreshold = ofMap(row, roiY, roiY+roiH, minNearThreshold, maxNearThreshold, true);
            
            if(pix[i] < thisNearThreshold && pix[i] > thisFarThreshold) {
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
        }
        
        // Update the CV images
        grayImage.flagImageChanged();
        
        // Blur image
        grayImage.blurGaussian(blurValue);
        
        grayImage.threshold(blurThreshold);
        
        // Set region of interest
        // Derived from measuring pixels on screen and scaling to Kinect size 640x480 (divide by 2)
        grayImage.setROI(roiX, roiY, roiW, roiH);
        
        // Find contours
        contourFinder.findContours(grayImage, minBlobArea, maxBlobArea, maxBlobNum, true, true);
        // last params: bFindHoles, bUseApproximation
        
        grayImage.resetROI();
        
        // Get pixels from FBO
        ofPixels finalFboPixels;
        finalFbo.readToPixels(finalFboPixels);
        
        ofPixels targetFboPixels;
        targetFbo.readToPixels(targetFboPixels);
        
        /**********************************
         * MARK: Detect number of players *
         **********************************/
        
        //blobArea = 0;
        /*for (int i = 0; i < contourFinder.nBlobs; i++) {
            blobArea += contourFinder.blobs[i].area;
        }*/
        
        // Check every second
        if (ofGetFrameNum()) {
            if (prevNumBlobs != contourFinder.nBlobs) {
                // Shuffle colors
                ofRandomize(shapeColors);
            }
            
            prevNumBlobs = contourFinder.nBlobs;
        }
        
        /*****************
         * MARK: Targets *
         *****************/
        
        // Compare ALL the current bounding boxes (which should be stored in a vector) against hte target
        for (int i = 0; i < boundingBoxes.size(); i++) {
            int bbX = boundingBoxes[i].x;
            int bbY = boundingBoxes[i].y;
            int bbW = boundingBoxes[i].width;
            int bbH = boundingBoxes[i].height;
            
            int bbXScaled = bbX;// * scaleVal + shapeFboLeft;
            int bbYScaled = bbY;// * scaleVal + shapeFboTop;
            int bbWScaled = bbW;// * scaleVal;
            int bbHScaled = bbH;// * scaleVal;
            
            // scale all targets by boundsX, bounds Y
            
            // could replace this with nextTargetRect
            bool isWithinOuterRect = bbXScaled > ((nextTargetRect.x - 1) * GRID_SQUARE_SIZE + boundsX) &&
                                    bbYScaled > ((nextTargetRect.y - 1) * GRID_SQUARE_SIZE  + boundsY) &&
                                    bbWScaled < ((nextTargetRect.width + 2) * GRID_SQUARE_SIZE) && // might have to subtract x and y
                                    bbHScaled < ((nextTargetRect.height + 2) * GRID_SQUARE_SIZE);
            
            bool isOutsideInnerRect = bbXScaled < (nextTargetRect.x * GRID_SQUARE_SIZE + boundsX) &&
                                        bbYScaled < (nextTargetRect.y * GRID_SQUARE_SIZE + boundsY) &&
                                        bbWScaled > (nextTargetRect.width * GRID_SQUARE_SIZE) && // might have to subtract x and y - but prob not
                                        bbHScaled > (nextTargetRect.height * GRID_SQUARE_SIZE);
            
            polygonIsTouchingRect = isWithinOuterRect && isOutsideInnerRect;
            // && isOutsideInnerRect
            
            // print out ... is within outer rect, is outside inner rect
            /*if (moveTarget ||
                (polygonIsTouchingRect && targetLerpPercent >= 1.)) {
                prevTargetRect = nextTargetRect;
                targetLerpPercent = 0;
                updateTargets();
            }*/
        }
    }
    
    /***************************
     * MARK: Draw texture FBOs *
     ***************************/
    
    shapeColor yellowColor = {
        ofColor(245, 239, 66),
        ofColor(245, 239, 66),
        ofColor(245, 239, 66)
    };
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        ofxCvBlob thisBlob = contourFinder.blobs[i];
        
        // Color
        shapeColor thisColor = shapeColors[i % shapeColors.size()];
        ofColor lightColor = thisColor.light;
        ofColor darkColor = thisColor.dark;
        ofColor dotColor = thisColor.dot;
        
        // Begin texture fbos
        textureFbos[i].begin();
        
        ofClear(0,0,0); // background
        // clear the fbo
        
        int edgeRange = 5;
        bool isTouchingEdge = blobIsTouchingEdge(thisBlob, roiX, roiY, roiW, roiH);
        
        // if there is NOT a hole, draw texture background
        // detect if touching the edges
        if (thisBlob.hole == true || isTouchingEdge) {
            ofBackground(lightColor);
            
            ofFill();
            
            // Gradient
            glBegin(GL_QUADS);
            glColor4f((float) lightColor.r/255.,
                      (float) lightColor.g/255.,
                      (float) lightColor.b/255.,
                      1);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(PROJECTION_WIDTH, 0.0f, 0.0f);
            glColor4f((float) darkColor.r/255.,
                      (float) darkColor.g/255.,
                      (float) darkColor.b/255.,
                      1);
            glVertex3f(PROJECTION_WIDTH, PROJECTION_HEIGHT, 0.0f);
            glVertex3f(0.0f, PROJECTION_HEIGHT, 0.0f);
            glEnd();
            
            // Dots
            ofSetColor(dotColor);
            for (int x = 10; x < PROJECTION_WIDTH; x += 10) {
                for (int y = 10; y < PROJECTION_HEIGHT; y += 10) {
                    float n = ofNoise(x * 0.01, y * 0.01, ofGetFrameNum() * 0.02);
                    float colorN = ofNoise(x * 0.05, y * 0.05, ofGetFrameNum() * 0.1);
                    ofPushMatrix();
                    ofTranslate(x, y);
                    ofScale(7 * n);
                    
                    ofColor lerpedColor = dotColor.getLerped(ofColor(255), colorN);
                    ofSetColor(lerpedColor.r, lerpedColor.g, lerpedColor.b, colorN * 255.);
                    
                    ofDrawCircle(0, 0, 1);
                    ofPopMatrix();
                }
            }
        } else {
            ofBackground(0,0,0,0);
        }
        
        ofClearAlpha();
        textureFbos[i].end();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    /*****************************
     * MARK: Populate shape FBOs *
     *****************************/
    
    // Get lerped opacity (scale)
    int frameNum = ofGetFrameNum() % 120;
    int lerpedOpacity = 140;
    if (contourFinder.nBlobs == 0) {
        if (frameNum < 60) {
            lerpedOpacity =
            ofMap(frameNum,
                                      0,
                                      60,
                                        140,
                                      180,
                                      true);
        } else {
            lerpedOpacity = ofMap(frameNum,
                                      60,
                                      120,
                                      180,
                                      140,
                                      true);
        }
    }
    
    // Draw to final FBO
    
    finalFbo.begin();
    
    ofSetColor(255);
    ofClear(0,0,0);
    ofClearAlpha();
    ofBackground(0,0,0,0);
    
    // Draw bounds
    ofPushStyle();
    ofSetColor(255,255,255);
    ofNoFill();
    ofSetLineWidth(4);
    
    /*ofDrawRectangle(
                    boundsX,
                    boundsY,
                    boundsW,
                    boundsH);*/
    
    ofBeginShape();
    ofVertex(boundsX - leftBoundsDiff, boundsY);
    ofVertex(boundsX + boundsW + rightBoundsDiff, boundsY);
    ofVertex(boundsX + boundsW, boundsY + boundsH);
    ofVertex(boundsX, boundsY + boundsH);
    ofVertex(boundsX - leftBoundsDiff, boundsY);
    
    ofEndShape();
    
    ofPopStyle();
    
    // Draw grid lines
    // TODO: Put back if needed
    ofPushStyle();
    ofNoFill();
    ofSetColor(255,255,255);
    ofSetLineWidth(1);
    for (int i = boundsX; i < boundsW+GRID_SQUARE_SIZE*2; i+=GRID_SQUARE_SIZE) {
        for (int j = boundsY; j < boundsH; j+=GRID_SQUARE_SIZE) {
            ofDrawRectangle(i, j, GRID_SQUARE_SIZE, GRID_SQUARE_SIZE);
        }
    }
    ofPopStyle();
    
    // Seat circles
    /*ofPushStyle();
    
    // Black bg
    ofFill();
    ofSetColor(0,0,0,255);
    ofDrawCircle(circle1X, circle1Y, circle1Radius);
    ofDrawCircle(circle2X, circle2Y, circle2Radius);
    
    // White outline
    ofNoFill();
    ofSetLineWidth(4);
    ofSetColor(255, 255, 255, 255);
    ofDrawCircle(circle1X, circle1Y, circle1Radius);
    ofDrawCircle(circle2X, circle2Y, circle2Radius);
    ofPopStyle();*/
    
    /**************************
     * MARK: Draw silhouettes *
     **************************/
    
    // Clear bounding boxes
    boundingBoxes.clear();
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        ofxCvBlob thisBlob = contourFinder.blobs[i];
        
        ofPolyline cur;
        cur.addVertices(thisBlob.pts);
        cur.setClosed(true);
        //cur.close();
        cur = cur.getSmoothed(smoothingSize, smoothingShape);
        
        /*************************
         * MARK: Box2d *
         *************************/
        
        int edgeRange = 5;
         
        ofPolyline contourPolyline;
        bool touchingEdge = false;
        
        ofPushStyle();
        ofSetColor(255,0,0);
        for (int j = 0; j < thisBlob.pts.size(); j++) {
            ofPoint thisPoint = thisBlob.pts[j];
            
            if (!((thisPoint.x >= 0 && thisPoint.x <= edgeRange) ||
                (thisPoint.x >= roiW - edgeRange && thisPoint.x <= roiW + edgeRange) ||
                (thisPoint.y >= 0 && thisPoint.y <= edgeRange) ||
                (thisPoint.y >= roiH - edgeRange && thisPoint.y <= roiH + edgeRange))) {
               contourPolyline.addVertex(thisPoint);
                // maybe: draw a point here!
            } else {
                // mark as touching edge
                touchingEdge = true;
            }
        }
        ofPopStyle();
        contourPolyline.setClosed(true);
        
        contourPolyline = contourPolyline.getSmoothed(20, smoothingShape);
        
        // Copy polyline into path so it can be filled
        
        if (i < NUM_SHAPE_FBOS) { // && !touchingEdge
            shapeFbos[i].begin();
            
            ofClear(0,0,0,0);
            
            ofFill();
            
            ofBeginShape();
            for( int i = 0; i < cur.getVertices().size(); i++) {
                ofVertex(cur.getVertices().at(i).x * scaleVal + shapeFboLeft,
                         cur.getVertices().at(i).y * scaleVal + shapeFboTop);
            }
            ofEndShape(true);
            
            shapeFbos[i].end();
            
            // Set texture for shape
            // TODO: If it's NOT a hole!
            // and the rest have to be transparent
            textureFbos[i].getTexture().setAlphaMask(shapeFbos[i].getTexture());
            //} else {
                //textureFbos[i].getTexture().setAlphaMask(shapeFbos[i].getTexture()); //getTexture().setAlphaMask(shapeFbos[i].getTexture());
                // we need an fbo with a clear
                
                // JUST simply draw the shapefbo
                //shapeFbos[i].draw(0,0);
            //}
            textureFbos[i].draw(0,0);
            //textureFbos[i].getTexture().setAlphaMask(shapeFbos[i].getTexture());
        }
        
        // TODO: Nice: cur and contourPolyline are the same? or at least start off the same
        contourPolyline.scale((float) scaleVal, (float) scaleVal);
        ofVec2f myTranslateVector;
        myTranslateVector.x = shapeFboLeft;
        myTranslateVector.y = shapeFboTop;
        contourPolyline.translate(myTranslateVector);
        contourPolyline.setClosed(true); // not sure this does anything
        
        contourPolyline.simplify(3);
        
        // if this is a hole (which program says is NOT a hole), get the centroid and create a triangle
        if (thisBlob.hole == false && !touchingEdge) {
            triangulationVisible = true;
            
            ofPoint centroid = thisBlob.centroid;
            //ofRectangle boundingRect = thisBlob.boundingRect;
            // range is 500 - 25000. absolute biggest would be 50000
            float holeArea = thisBlob.area;
            
            int oldRange = 25000-500;
            int newRange = 10-3;
            int newValue = ceil((((holeArea - 500) * newRange) / oldRange) + 3);
            
            ofPushStyle();
            
            // Draw polyline for triangles
            auto resampledContourPolyline = contourPolyline.getResampledByCount(newValue); //10
        
            // Print out points, and get the bounds of the points
            
            // is there one???
            
            ofSetLineWidth(4);
            
            if (resampledContourPolyline.getVertices().size() > 2) {
                // Delaunay triangulation
                // Add points
                // Triangulate them
                // Draw the triangles
                triangulation.reset();
                
                for (int i = 0; i < resampledContourPolyline.getVertices().size(); i++) {
                    triangulation.addPoint(resampledContourPolyline.getVertices()[i]);
                }
                
                triangulation.triangulate();
                
                ofRectangle boundingBoxPoly = contourPolyline.getBoundingBox();
                
                ofSetColor(255,255,255,255);
                ofNoFill();
                triangulation.draw();
                
                ofPushStyle();
                ofSetColor(255,255,255,255);
                //ofFill();
                ofSetLineWidth(1);
                /*ofRectangle smallerBoundingBox = ofRectangle(boundingBoxPoly.x + BOUNDING_BOX_MARGIN,
                                                             boundingBoxPoly.y  + BOUNDING_BOX_MARGIN,
                                                             boundingBoxPoly.width - 2*BOUNDING_BOX_MARGIN,
                                                             boundingBoxPoly.height - 2*BOUNDING_BOX_MARGIN);
                ofDrawRectangle(smallerBoundingBox.x * scaleVal + shapeFboLeft,
                                smallerBoundingBox.y * scaleVal + shapeFboTop,
                                smallerBoundingBox.width * scaleVal,
                                smallerBoundingBox.height * scaleVal);*/
                /*ofDrawRectangle(boundingBoxPoly.x * scaleVal + shapeFboLeft,
                                boundingBoxPoly.y * scaleVal + shapeFboTop,
                                boundingBoxPoly.width * scaleVal,
                                boundingBoxPoly.height * scaleVal);*/
                
                // optional: draw bounding rect
                ofDrawRectangle(boundingBoxPoly);
                
                boundingBoxes.push_back(boundingBoxPoly); // need to modify it here...
                ofPopStyle();
            }
            ofPopStyle();
        } else {
            triangulationVisible = false;
        }
    }
    
    
    
    // grid line cover
    
    // top
    /*ofSetColor(0,255,0,255); // will be black
    ofFill();
    ofDrawRectangle(boundsX,
                    824+142+10, //(boundsY+boundsH)
                    PROJECTION_WIDTH,//boundsW*GRID_SQUARE_SIZE,
                    200);*/
    
    // left
    /*ofSetColor(0,0,0,255);
    ofDrawRectangle(boundsW, fboTop, 105, PROJECTION_HEIGHT);*/
    
    ofSetColor(255,255,255,255);
    
    // Draw shoes
    /*ofPushMatrix();
    ofScale(shoesScale2);
    //shoes.draw(shoesX,shoesY);
    ofRotateDeg(180);
    
    
    ofFill();
    ofTranslate(shoesX, shoesY);
    ofRotateDeg(180);
    ofDrawTriangle(50,10,10,40,90,40);
    ofTranslate(-1800,0);
    ofDrawTriangle(50,10,10,40,90,40);
    ofTranslate(1800,0);
    ofRotateDeg(180);
        franklinBook.drawString("PLEASE REMOVE SHOES", 50,30);
    
    ofNoFill();
    ofPopMatrix();*/
    
    ofPushMatrix();
    ofScale(shoesScale);
    //shoes.draw(shoesX + 600,shoesY);
    
    ofRotateDeg(180);
    
    // blink
    ofSetColor(255,255,255,ofMap(lerpedOpacity, 140, 180, 180, 255));
    ofSetColor(255,255,255);
    
    // draw big black rect
    if (triangulationVisible) {
        franklinBook.drawString("NOW FIT THE POLYGON INTO THE GRAY AREA", textX2, textY);
    } else {
        franklinBook.drawString("KNEEL FACE TO FACE AND TOUCH BOTH HANDS", textX-50, textY);
    }
    ofPopMatrix();
    
    finalFbo.end();
    
    /*************************
     * MARK: Draw target FBO *
     *************************/
    
    targetFbo.begin();
    ofClear(0,0,0,0);
    ofBackground(0,0,0,0);
    
    /*******************************
     * MARK: Draw target squares *
     *******************************/
    
    // TODO: Lerp this based on percent
    
    // Draw a shape at target
    if (moveTarget || (polygonIsTouchingRect && targetLerpPercent >= 1.)) {
        // Calculate new targets
        prevTargetRect = nextTargetRect;
        targetLerpPercent = 0;
        updateTargets();
    }
    
    ofTranslate(boundsX, boundsY);
    
    ofPushMatrix();
    /*ofTranslate(-2*lerpedOpacity,
                -2*lerpedOpacity);
    ofScale((nextTargetRect.width + lerpedOpacity)/nextTargetRect.width,
            (nextTargetRect.height + lerpedOpacity)/nextTargetRect.height);*/
    
    // Dashed lines
    /* select white for all lines  */
    glColor3f (1.0, 1.0, 1.0);
    
    glEnable (GL_LINE_STIPPLE);

    glLineStipple (20, 0xAAAA); // first param is length of each dash
    glLineWidth(10);
    
    if (targetLerpPercent < 1.) {
        // If transition is in progress,
        // draw interpolated rectangle between prev and next
        
        targetRect.x = lerp(targetLerpPercent,
                            prevTargetRect.x,
                            nextTargetRect.x);
        targetRect.y = lerp(targetLerpPercent,
                            prevTargetRect.y,
                            nextTargetRect.y);
        targetRect.width = lerp(targetLerpPercent,
                            prevTargetRect.width,
                            nextTargetRect.width);
        targetRect.height = lerp(targetLerpPercent,
                            prevTargetRect.height,
                            nextTargetRect.height);
        
        // Inner shading rectangles
        
        ofFill();
        ofSetColor(255,255,255,160);
        
        // top
        ofDrawRectangle((targetRect.x - GRID_SQUARE_SIZE),
                        (targetRect.y - GRID_SQUARE_SIZE),
                        (targetRect.width + 2*GRID_SQUARE_SIZE),
                        GRID_SQUARE_SIZE);
        // bottom
        ofDrawRectangle((targetRect.x - GRID_SQUARE_SIZE),
                        (targetRect.y + targetRect.height),
                        (targetRect.width + 2*GRID_SQUARE_SIZE),
                        GRID_SQUARE_SIZE);
        // left
        ofDrawRectangle((targetRect.x - GRID_SQUARE_SIZE),
                        targetRect.y,
                        GRID_SQUARE_SIZE,
                        targetRect.height);
        // right
        ofDrawRectangle((targetRect.x + targetRect.width),
                        targetRect.y,
                        GRID_SQUARE_SIZE,
                        targetRect.height);
        
        // Dashed lines
        
        // Inner rect
        
        ofSetLineWidth(4);
        ofSetColor(255,255,255,255);
        ofNoFill();
        //ofDrawRectangle(targetRect);
        
        // Outer rect
        /*ofDrawRectangle(targetRect.x - GRID_SQUARE_SIZE,
                        targetRect.y - GRID_SQUARE_SIZE,
                        targetRect.width + 2*GRID_SQUARE_SIZE,
                        targetRect.height + 2*GRID_SQUARE_SIZE);*/
        
        // Inner rect
        
        // Top
        glBegin(GL_LINES);
        glVertex2f (targetRect.x,
                    targetRect.y);
        glVertex2f (targetRect.x + targetRect.width,
                    targetRect.y);
        glEnd();
        
        // Right
        glBegin(GL_LINES);
        glVertex2f (targetRect.x + targetRect.width,
                    targetRect.y);
        glVertex2f (targetRect.x + targetRect.width,
                    targetRect.y + targetRect.height);
        glEnd();
        
        // Bottom
        glBegin(GL_LINES);
        glVertex2f (targetRect.x + targetRect.width,
                    targetRect.y  + targetRect.height);
        glVertex2f (targetRect.x,
                    targetRect.y + targetRect.height);
        glEnd();
        
        // Left
        glBegin(GL_LINES);
        glVertex2f (targetRect.x,
                    targetRect.y + targetRect.height);
        glVertex2f (targetRect.x,
                    targetRect.y);
        glEnd();
        
        // Outer rect
        
        // Top
        glBegin(GL_LINES);
        glVertex2f (targetRect.x - GRID_SQUARE_SIZE,
                    targetRect.y - GRID_SQUARE_SIZE);
        glVertex2f (targetRect.x + targetRect.width + GRID_SQUARE_SIZE,
                    targetRect.y - GRID_SQUARE_SIZE);
        glEnd();
        
        // Right
        glBegin(GL_LINES);
        glVertex2f (targetRect.x + targetRect.width + GRID_SQUARE_SIZE,
                    targetRect.y - GRID_SQUARE_SIZE);
        glVertex2f (targetRect.x + targetRect.width + GRID_SQUARE_SIZE,
                    targetRect.y + targetRect.height + GRID_SQUARE_SIZE);
        glEnd();
        
        // Bottom
        glBegin(GL_LINES);
        glVertex2f (targetRect.x + targetRect.width + GRID_SQUARE_SIZE,
                    targetRect.y + targetRect.height + GRID_SQUARE_SIZE);
        glVertex2f (targetRect.x - GRID_SQUARE_SIZE,
                    targetRect.y + targetRect.height + GRID_SQUARE_SIZE);
        glEnd();
        
        // Left
        glBegin(GL_LINES);
        glVertex2f (targetRect.x - GRID_SQUARE_SIZE,
                    targetRect.y + targetRect.height + GRID_SQUARE_SIZE);
        glVertex2f (targetRect.x - GRID_SQUARE_SIZE,
                    targetRect.y - GRID_SQUARE_SIZE);
        glEnd();
        
        targetLerpPercent += .125;
    } else {
        // If transition is complete,
        // Draw the static target rect
        ofFill();
        ofSetColor(255,255,255,lerpedOpacity); // weirdly - go down to 150 and its almost invisible?
        
        // Blink
        
        ofPushMatrix();
        ofScale(GRID_SQUARE_SIZE);
        
        // Inner shading rectangles
        
        // top
        ofDrawRectangle((nextTargetRect.x - 1),
                        (nextTargetRect.y - 1),
                        (nextTargetRect.width + 2),
                        1);
        // bottom
        ofDrawRectangle((nextTargetRect.x - 1),
                        (nextTargetRect.y + nextTargetRect.height),
                        (nextTargetRect.width + 2),
                        1);
        // left
        ofDrawRectangle((nextTargetRect.x - 1),
                        nextTargetRect.y,
                        1,
                        nextTargetRect.height);
        // right
        ofDrawRectangle((nextTargetRect.x + nextTargetRect.width),
                        nextTargetRect.y,
                        1,
                        nextTargetRect.height);
        
        // Dashed lines
        
        // Inner rect
        
        // TODO: Put back?
        ofSetLineWidth(4);
        ofSetColor(255,255,255,255);
        ofNoFill();
        //ofDrawRectangle(nextTargetRect);
        
        // Outer rect
        /*ofDrawRectangle(nextTargetRect.x - 1,
                        nextTargetRect.y - 1,
                        nextTargetRect.width + 2,
                        nextTargetRect.height + 2);*/
        
        // Top
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x,
                    nextTargetRect.y);
        glVertex2f (nextTargetRect.x + nextTargetRect.width,
                    nextTargetRect.y);
        glEnd();
        
        // Right
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x + nextTargetRect.width,
                    nextTargetRect.y);
        glVertex2f (nextTargetRect.x + nextTargetRect.width,
                    nextTargetRect.y + nextTargetRect.height);
        glEnd();
        
        // Bottom
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x + nextTargetRect.width,
                    nextTargetRect.y  + nextTargetRect.height);
        glVertex2f (nextTargetRect.x,
                    nextTargetRect.y + nextTargetRect.height);
        glEnd();
        
        // Left
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x,
                    nextTargetRect.y + nextTargetRect.height);
        glVertex2f (nextTargetRect.x,
                    nextTargetRect.y);
        glEnd();
        
        // Outer rect
        
        // Top
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x - 1,
                    nextTargetRect.y - 1);
        glVertex2f (nextTargetRect.x + nextTargetRect.width + 1,
                    nextTargetRect.y - 1);
        glEnd();
        
        // Right
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x + nextTargetRect.width + 1,
                    nextTargetRect.y - 1);
        glVertex2f (nextTargetRect.x + nextTargetRect.width + 1,
                    nextTargetRect.y + nextTargetRect.height + 1);
        glEnd();
        
        // Bottom
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x + nextTargetRect.width + 1,
                    nextTargetRect.y + nextTargetRect.height + 1);
        glVertex2f (nextTargetRect.x - 1,
                    nextTargetRect.y + nextTargetRect.height + 1);
        glEnd();
        
        // Left
        glBegin(GL_LINES);
        glVertex2f (nextTargetRect.x - 1,
                    nextTargetRect.y + nextTargetRect.height + 1);
        glVertex2f (nextTargetRect.x - 1,
                    nextTargetRect.y - 1);
        glEnd();
        
        ofPopStyle();
        
        ofPopMatrix();
    }
    
    glDisable (GL_LINE_STIPPLE);
    glFlush();
    
    glLineWidth(1);
    
    ofPopMatrix();
    
    targetFbo.end();
    
    /**************************
     * MARK: Draw from Kinect *
     **************************/
    
    ofPushMatrix();
    
    ofSetColor(255,255,255,255);

    // Full screen
    ofTranslate(220,0); // accommodate UI menu

    int drawVideoX = 0;
    int drawVideoY = 0;
    int drawVideoWidth = 640;
    int drawVideoHeight = 480;

    // Kinect video on first screen
    if (videoMode == 1) {
        // Kinect video image
        kinect.draw(drawVideoX, drawVideoY, drawVideoWidth, drawVideoHeight);
    } else if (videoMode == 2) {
        // Depth image
        kinect.drawDepth(drawVideoX, drawVideoY, drawVideoWidth, drawVideoHeight);
    } else if (videoMode == 3) {
        // Threshold image
        grayImage.draw(drawVideoX, drawVideoY, drawVideoWidth, drawVideoHeight);
    } else if (videoMode == 4) {
        // Contours
        ofPushMatrix();
        ofTranslate(roiX, roiY);
        contourFinder.draw(drawVideoX, drawVideoY, drawVideoWidth, drawVideoHeight);
        ofPopMatrix();
    }

    // Draw ROI on 1st screen
    if (roomMode != 3) {
        ofPushStyle();
        ofNoFill();
        ofSetColor(0,255,0);
        ofDrawRectangle(roiX, roiY, roiW, roiH);
        ofPopStyle();
    }
    
    ofPopMatrix();
    
    /*************************
     * MARK: Draw final FBOs *
     *************************/

    // Draw final FBO on second screen (projection)
    
    ofSetColor(255);
    
    ofPushMatrix();
    
    if (roomMode == 2 || roomMode == 3) {
        // Draw shapes
        finalFbo.draw(fboLeft, fboTop);
        
        // Draw targets
        targetFbo.draw(fboLeft, fboTop);
    } else {
        // Draw shapes
        finalFbo.draw(SCREEN_WIDTH + fboLeft, fboTop);
        
        // Draw targets
        targetFbo.draw(SCREEN_WIDTH + fboLeft, fboTop);
    }
    
    ofPopMatrix();
    
    /*************************
     * MARK: Draw debug text *
     *************************/
    
    if (roomMode != 3) {
        ofPushStyle();
        
        /*stringstream reportStream;
        
        reportStream << "Num blobs: " << contourFinder.nBlobs << endl;
        
        reportStream << endl;

        franklinBook.drawString("Target X: " +
                                to_string(nextTargetRect.x * GRID_SQUARE_SIZE + boundsX)
                                + " Y: " +
                                to_string(nextTargetRect.y * GRID_SQUARE_SIZE + boundsY), 200, 500);*/
        
        /*for (int i = 0; i < boundingBoxes.size(); i++) {
            int bbX = boundingBoxes[i].x;
            int bbY = boundingBoxes[i].y;
            int bbW = boundingBoxes[i].width;
            int bbH = boundingBoxes[i].height;
            
            int bbXScaled = bbX; //bbX * scaleVal + shapeFboLeft;
            int bbYScaled = bbY; // * scaleVal + shapeFboTop;
            int bbWScaled = bbW; // * scaleVal;
            int bbHScaled = bbH; // * scaleVal;
            
            franklinBook.drawString("Bounding box X: " +
                                    to_string(bbXScaled)
                                    + " Y: " +
                                    to_string(bbYScaled)
                                    + " W: " +
                                    to_string(bbWScaled)
                                    + " H: " +
                                    to_string(bbHScaled), 200, 570 + i*70);
        }*/
        
        
        /*franklinBook.drawString(to_string(moveTarget), 200, 500);
        franklinBook.drawString(to_string(targetLerpPercent), 200, 430);
        

        for (int i = 0; i < contourFinder.blobs.size(); i++) {
            franklinBook.drawString(to_string(contourFinder.blobs[i].area), 200, 570 + i*70);
        }*/

        
        //ofDrawBitmapString(reportStream.str(), 220, 550);
        
        ofPopStyle();
    } else {
        ofHideCursor();
    }
    
    
    // Draw GUI
    if(!bHide){
        gui.draw();
    }
    
}

//--------------------------------------------------------------
void ofApp::exit() {
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //updateTargets();
    moveTarget = true;
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    switch (key) {
            
        case 'o':
            kinect.open();
            break;
            
        case 'c':
            kinect.close();
            break;
            
        case '0':
            // No video
            videoMode = 0;
            break;
            
        case '1':
            // Kinect video
            videoMode = 1;
            break;
            
        case '2':
            // Depth image
            videoMode = 2;
            break;
            
        case '3':
            // Threshold image
            videoMode = 3;
            break;
            
        case '4':
            // Contours
            videoMode = 4;
            break;
            
        case 'h':
            // Press h to toggle show/hide GUI
            bHide = !bHide;
            break;
            
        case ' ':
            // Press space to manually move target
            moveTarget = true;
            //updateTargets();
            break;
    }
}

//--------------------------------------------------------------
bool ofApp::blobIsTouchingEdge(ofxCvBlob thisBlob, int roiX, int roiY, int roiW, int roiH){
    int edgeRange = 5;
    for (int j = 0; j < thisBlob.pts.size(); j++) {
        ofPoint thisPoint = thisBlob.pts[j];
        
        if ((thisPoint.x >= 0 && thisPoint.x <= edgeRange) ||
            (thisPoint.x >= roiW - edgeRange && thisPoint.x <= roiW + edgeRange) ||
            (thisPoint.y >= 0 && thisPoint.y <= edgeRange) ||
            (thisPoint.y >= roiH - edgeRange && thisPoint.y <= roiH + edgeRange)) {
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
// Custom function that takes an index and returns the coordinates of the triangle we refer to
/*vector <ofPoint> ofApp::getTriangle(int i, ofxDelaunay thisTriangulation){
    int pA = thisTriangulation.triangleMesh.getIndex(i*3);
    int pB = thisTriangulation.triangleMesh.getIndex(i*3+1);
    int pC = thisTriangulation.triangleMesh.getIndex(i*3+2);
    
    ofPoint pointA = thisTriangulation.triangleMesh.getVertex(pA);
    ofPoint pointB = thisTriangulation.triangleMesh.getVertex(pB);
    ofPoint pointC = thisTriangulation.triangleMesh.getVertex(pC);
    
    vector <ofPoint> points;
    points.push_back(pointA);
    points.push_back(pointB);
    points.push_back(pointC);
    return points;
}*/

//--------------------------------------------------------------
void ofApp::updateTargets() {
    int minTargetSize = 1; //3
    
    int nextX = ofRandom(xOffset, xOffset + xRange - minTargetSize);
    int nextY = ofRandom(yOffset, yOffset + yRange - minTargetSize);
    
    // populate nextTargetRect
    nextTargetRect = ofRectangle(nextX,
                                 nextY,
                                 floor(ofRandom(1, max(1, xRange - nextX + xOffset))),
                                 floor(ofRandom(1, max(1, yRange - nextY + yOffset))));
    // if first time, set targetRect to be nextTargetRect
    
    if (firstTime) {
        prevTargetRect = nextTargetRect;
        targetRect.x = nextTargetRect.x;
        targetRect.y = nextTargetRect.y;
        targetRect.width = nextTargetRect.width;
        targetRect.height = nextTargetRect.height;
        
        firstTime = false;
    }
    
    moveTarget = false;
}

//--------------------------------------------------------------
int ofApp::lerp(float targetPercent, int prev, int next) {
    return (int) ofMap(targetPercent, 0., 1., prev * GRID_SQUARE_SIZE, next * GRID_SQUARE_SIZE, true);
}
