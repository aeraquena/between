#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    /****************************
     * MARK: Changing variables *
     *****************************/
    
    // Physical setup
    
    // Mode for video on first screen
    
    // 1. Video
    // 2. Depth video
    // 3. Threshold
    // 4. Contour
    videoMode = 4;
    
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
    scaleVal = 2; //2
    
    // Set Kinect depth detection thresholds
    // this should be wider than86the play area
    gui.add(roiX.setup("roi x", 19, 0, 640));
    gui.add(roiY.setup("roi y", 43, 0, 480));
    gui.add(roiW.setup("roi w", 582, 0, 640));
    gui.add(roiH.setup("roi h", 396, 0, 480));
    
    // Bounds parameters
    gui.add(boundsX.setup("bounds x", 432, 0, 500));
    gui.add(boundsY.setup("bounds y", 60, 0, 500));
    gui.add(boundsW.setup("bounds w", 1260, 1200, 1300));
    gui.add(leftBoundsDiff.setup("right bounds diff", 0, -170, 0)); // inverted intentionally
    gui.add(rightBoundsDiff.setup("left bounds diff", 0, -104, 0));
    gui.add(boundsH.setup("bounds h", 924, 220, 1280));
    
    // Depth thresholds
    gui.add(minNearThreshold.setup("min near threshold", 110, 0, 255));
    gui.add(maxNearThreshold.setup("max near threshold", 110, 0, 255));
    gui.add(minFarThreshold.setup("min far threshold", 0, 0, 255));
    gui.add(maxFarThreshold.setup("max far threshold", 0, 0, 255));
    
    // Blob values
    gui.add(minBlobArea.setup("min blob area", 2000, 500, 30000));
    gui.add(maxBlobArea.setup("max blob area", (roiW * roiH) - 4000, 0, roiW * roiH));
    gui.add(maxBlobNum.setup("max blob num", 6, 1, 50));
    
    // Smoothing values for blobs
    gui.add(smoothingSize.setup("smoothing size", 11, 0, 100));
    gui.add(smoothingShape.setup("smoothing shape", 0, 0, 1));
    gui.add(blurValue.setup("blur value", 61, 0, 100)); // must be an odd number
    gui.add(blurThreshold.setup("blur threshold", 177, 0, 255));

    // Position of shape and target FBOs
    // To align silhouettes with bodies
    gui.add(fboLeft.setup("fbo left", -4970, -5000, -2000));
    gui.add(fboTop.setup("fbo top", -1090, -2000, 0));
    gui.add(shapeFboTop.setup("shape fbo top", 187, -400, 1000));
    gui.add(shapeFboLeft.setup("shape fbo left", 488, -200, 1500));
    
    // Shoes position - optional
    gui.add(shoesX.setup("shoes x", -1800, -2500, 0));
    gui.add(shoesY.setup("shoes y", -40, -500, 500));
    gui.add(shoesScale.setup("shoes scale", .44, 0., 1.));
    gui.add(shoesScale2.setup("shoes scale 2", .22, 0., 1.));
    
    // Text position
    gui.add(textX.setup("text x", -3814, -3700, -3900)); // Default text
    gui.add(textX2.setup("text x2", -3789, -3700, -3900)); // Text when inner polygon appears
    gui.add(textY.setup("text y", -2270, -2000, -4000));
    
    // Target rectangle bounds
    gui.add(xOffset.setup("x offset",6,0,20)); // the x value where we should start generating INNER shapes
    gui.add(yOffset.setup("y offset",4,0,12));
    gui.add(xRange.setup("x range",4,2,20)); // the range of the play area x must be at least 3 (min square size)
    gui.add(yRange.setup("y range",4,2,12));
    // Range: 2 means 3x3 (outer)
    
    // Grid square size
    gui.add(GRID_SQUARE_SIZE.setup("grid square size",84,40,200));
    
    // Grid offset
    gui.add(gridXOffset.setup("grid x offset", 0, -200, 200));
    gui.add(gridYOffset.setup("grid y offset", 0, 0, 400));
    
    // Hide controls
    bHide = false;
    
    // Triangulation visible
    triangulationVisible = false;
    
    /***********************
     * MARK: Allocate FBOs *
     ***********************/

    // Projection dimensions
    // TODO: Scale to actual projection dimensions
    PROJECTION_WIDTH = 2880;//1920
    PROJECTION_HEIGHT = 1620;//1080

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
    
    // Change to green (0,255,0) whenever I want to see background color
    ofBackground(0);
    
    ofSetPolyMode(OF_POLY_WINDING_POSITIVE);
    
    ofEnableAntiAliasing();
    
    ofEnableSmoothing();
    
    /*********************
     * MARK: Set up text *
     *********************/
    
    // Font
    ofTrueTypeFont::setGlobalDpi(72);
    
    franklinBook.load("frabk.ttf", 142);
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
    
    /*******************************
     * MARK: Set up target squares *
     *******************************/
    
    updateTargets();

    /**********************
     * MARK: Set up shoes *
     **********************/
    
    // Put this back when drawing shoes
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
            
            // if pix is black, ignore - make it 255?
            // far threshold should be smaller than near threshold
            if(pix[i] == 00) {
                pix[i] = 255; // if black, make white to ignore visual noise
            } else if(pix[i] < thisNearThreshold && pix[i] > thisFarThreshold) {
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
        }
        
        // Invert - for far Kinect
        //grayImage.invert();
        
        // Update the CV images
        grayImage.flagImageChanged();
        
        // Blur image
        grayImage.blurGaussian(blurValue);
        
        grayImage.threshold(blurThreshold);
        
        // Set region of interest
        // Derived from measuring pixels on screen and scaling to Kinect size 640x480 (divide by 2)
        grayImage.setROI(roiX, roiY, roiW, roiH);
        
        // Find contours
        // last params: bFindHoles, bUseApproximation
        contourFinder.findContours(grayImage, minBlobArea, maxBlobArea, maxBlobNum, true, true);
        
        grayImage.resetROI();
        
        // Get pixels from FBO
        ofPixels finalFboPixels;
        finalFbo.readToPixels(finalFboPixels);
        
        ofPixels targetFboPixels;
        targetFbo.readToPixels(targetFboPixels);
        
        /**********************************
         * MARK: Detect number of players *
         **********************************/
        
        // Check every frame
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
        
        // set to false first, to account for no bounding boxes
        polygonIsTouchingRect = false;
        // Compare ALL the current bounding boxes (which are stored in a vector) against the target
        for (int i = 0; i < boundingBoxes.size(); i++) {
            int bbX = boundingBoxes[i].x;
            int bbY = boundingBoxes[i].y;
            int bbW = boundingBoxes[i].width;
            int bbH = boundingBoxes[i].height;
            
            // could replace this with nextTargetRect
            bool isWithinOuterRect = bbX > ((nextTargetRect.x - 1) * GRID_SQUARE_SIZE + boundsX) &&
                                    bbY > ((nextTargetRect.y - 1) * GRID_SQUARE_SIZE  + boundsY) &&
                                    bbW < ((nextTargetRect.width + 2) * GRID_SQUARE_SIZE) &&
                                    bbH < ((nextTargetRect.height + 2) * GRID_SQUARE_SIZE);
            
            bool isOutsideInnerRect = bbX < (nextTargetRect.x * GRID_SQUARE_SIZE + boundsX) &&
                                        bbY < (nextTargetRect.y * GRID_SQUARE_SIZE + boundsY) &&
                                        bbW > (nextTargetRect.width * GRID_SQUARE_SIZE) &&
                                        bbH > (nextTargetRect.height * GRID_SQUARE_SIZE);
            
            polygonIsTouchingRect = isWithinOuterRect && isOutsideInnerRect;
        }
    }
    
    /***************************
     * MARK: Draw texture FBOs *
     ***************************/
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        ofxCvBlob thisBlob = contourFinder.blobs[i];
        
        // Color
        shapeColor thisColor = shapeColors[i % shapeColors.size()];
        ofColor lightColor = thisColor.light;
        ofColor darkColor = thisColor.dark;
        ofColor dotColor = thisColor.dot;
        
        // Begin texture fbos
        textureFbos[i].begin();
        
        // Clear the FBO
        ofClear(0,0,0); // background color
        
        int edgeRange = 5;
        bool isTouchingEdge = blobIsTouchingEdge(thisBlob, roiX, roiY, roiW, roiH);
        
        // Detect if touching the edges
        // thisBlob.hole is true if there is a hole in this blob, but not that this blob is a hole itself
        // thisBlob.hole might have confusing behavior - what if there is no hole in this blob, but it's not a hole itself?
        // If there's a hole in this blob, draw texture background
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
            // This blob is a hole
            ofBackground(0,0,0,0);
        }
        
        ofClearAlpha();
        textureFbos[i].end();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    
    /****************************
     * MARK: Get lerped opacity *
     ****************************/
    
    // Fade target rectangle in and out when no one is playing
    // Get lerped opacity (scale)
    // TODO: Set a variable for lerped opacity range, rather than constants
    int frameNum = ofGetFrameNum() % 120;
    int lerpedOpacity = 190;
    if (contourFinder.nBlobs == 0) {
        if (frameNum < 60) {
            lerpedOpacity =
            ofMap(frameNum,
                    0,
                    60,
                    190,
                    230,
                    true);
        } else {
            lerpedOpacity = ofMap(frameNum,
                    60,
                    120,
                    230,
                    190,
                    true);
        }
    }
    
    /*****************************
     * MARK: Populate shape FBOs *
     *****************************/
    
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
    
    ofBeginShape();
    ofVertex(boundsX - leftBoundsDiff, boundsY);
    ofVertex(boundsX + boundsW + rightBoundsDiff, boundsY);
    ofVertex(boundsX + boundsW, boundsY + boundsH);
    ofVertex(boundsX, boundsY + boundsH);
    ofVertex(boundsX - leftBoundsDiff, boundsY);
    ofEndShape();
    
    ofPopStyle();
    
    /*************************
     * MARK: Draw grid lines *
     *************************/
    
    // Draw grid lines
    // Put back if needed
    // TODO: This is probably causing malloc error... but maybe not since projection width increased
    // Could put min()
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(200,200,200);
    ofSetLineWidth(2);
    for (int i = boundsX; i < boundsX+boundsW; i+=GRID_SQUARE_SIZE) { 
        for (int j = boundsY; j < boundsY+boundsH; j+=GRID_SQUARE_SIZE) {
            ofDrawRectangle(i + gridXOffset, j + gridYOffset, GRID_SQUARE_SIZE, GRID_SQUARE_SIZE);
        }
    }
    ofPopStyle();
    
    /**************************
     * MARK: Draw silhouettes *
     **************************/
    
    // Clear bounding boxes
    boundingBoxes.clear();
    
    // Draw polyline around each blob
    for (int i = 0; i < contourFinder.nBlobs; i++){
        ofxCvBlob thisBlob = contourFinder.blobs[i];
        
        ofPolyline cur;
        cur.addVertices(thisBlob.pts);
        cur.setClosed(true);
        cur = cur.getSmoothed(smoothingSize, smoothingShape);
        
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
               contourPolyline.addVertex(thisPoint); // can do scaleVal here?
            } else {
                // mark as touching edge
                touchingEdge = true;
            }
        }
        ofPopStyle();
        contourPolyline.setClosed(true);
        
        contourPolyline = contourPolyline.getSmoothed(20, smoothingShape);
        
        // Copy polyline into path so it can be filled
        // Commented out: Ignore drawing blob if touching edge (optional)
        if (i < NUM_SHAPE_FBOS) { // && !touchingEdge
            shapeFbos[i].begin();
            
            ofClear(0,0,0,0);
            
            ofFill();
            
            ofBeginShape();
            for( int i = 0; i < cur.getVertices().size(); i++) {
                ofVertex(min(PROJECTION_WIDTH, (int) cur.getVertices().at(i).x * scaleVal + shapeFboLeft),
                         min(PROJECTION_HEIGHT, (int) cur.getVertices().at(i).y * scaleVal + shapeFboTop));
            }
            ofEndShape(true);
            
            shapeFbos[i].end();
            
            // Set texture for shape
            textureFbos[i].getTexture().setAlphaMask(shapeFbos[i].getTexture());
            textureFbos[i].draw(0,0);
        }
        
        // TODO: min(PROJECTION_WIDTH, ... remove this!
        contourPolyline.scale((float) scaleVal, (float) scaleVal); // how do we scale with min?
        ofVec2f myTranslateVector;
        myTranslateVector.x = shapeFboLeft;
        myTranslateVector.y = shapeFboTop;
        contourPolyline.translate(myTranslateVector);
        contourPolyline.setClosed(true); // not sure this does anything
        
        contourPolyline.simplify(3);
        
        // if this is a hole (which program says is NOT a hole, because it doesn't contain a hole), get the centroid and create a triangle
        if (thisBlob.hole == false && !touchingEdge) {
            triangulationVisible = true;
            
            ofPoint centroid = thisBlob.centroid;
            float holeArea = thisBlob.area;
            
            // TODO: Not sure what oldRange and newRange values represent
            // range is 500 - 25000. absolute biggest would be 50000
            int oldRange = 25000-500; // Blob size?
            int newRange = 7; // Resampling count? //10 - 3 = 7. 15 for further Kinect
            // scales one range to another
            int newValue = ceil((((holeArea - 500) * newRange) / oldRange) + 3);
            
            ofPushStyle();
            
            // Draw polyline for triangles
            auto resampledContourPolyline = contourPolyline.getResampledByCount(newValue); // 10
            
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
                ofSetLineWidth(1);
                
                // optional: draw bounding rect
                ofDrawRectangle(boundingBoxPoly);
                
                boundingBoxes.push_back(boundingBoxPoly);
                ofPopStyle();
            }
            ofPopStyle();
        } else {
            triangulationVisible = false;
        }
    }
    
    /******************************
     * MARK: Draw grid line cover *
     ******************************/
    
    // Put back if drawing grid and need to cover up borders
    
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
    
    /********************
     * MARK: Draw shoes *
     ********************/
    
    
    // Draw shoes
    /*
    ofSetColor(255,255,255,255);
    ofPushMatrix();
    ofScale(shoesScale2);
    shoes.draw(shoesX,shoesY);
    ofRotateDeg(180);
    
    // Draw text and arrows
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
    ofPopMatrix();
    */
    
    ofPushMatrix();
    ofScale(shoesScale);
    //shoes.draw(shoesX + 600,shoesY);
    
    ofRotateDeg(180);
    
    // blink
    ofSetColor(255,255,255,ofMap(lerpedOpacity, 140, 180, 180, 255));
    ofSetColor(255,255,255);
    
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
    
    /*****************************
     * MARK: Draw target squares *
     *****************************/
    
    // Draw a shape at target
    if (moveTarget || (polygonIsTouchingRect && targetLerpPercent >= 1.)) {
        // TODO: Print the above variables, figure out how to print bools
        // Calculate new targets
        prevTargetRect = nextTargetRect;
        targetLerpPercent = 0;
        updateTargets();
    }
    
    ofTranslate(boundsX, boundsY);
    
    ofPushMatrix();
    
    // Dashed lines
    // select white for all lines
    glColor3f (1.0, 1.0, 1.0);
    
    glEnable (GL_LINE_STIPPLE);

    // first param is length of each dash
    glLineStipple (20, 0xAAAA);
    glLineWidth(10); // this doesn't seem to do anything
    
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
        
        ofSetLineWidth(4); // this doesn't seem to do anything either
        ofSetColor(255,255,255,255);
        ofNoFill();
        
        // Put back if changing from dashed lines to solid lines
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
        ofSetColor(255,255,255,lerpedOpacity);
        
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
        
        // TODO: Put back if changing from dashed lines to solid lines
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
    ofPushStyle();
    ofNoFill();
    ofSetColor(0,255,0);
    ofDrawRectangle(roiX, roiY, roiW, roiH);
    ofPopStyle();
    
    ofPopMatrix();
    
    /*************************
     * MARK: Draw final FBOs *
     *************************/

    // Draw final FBO on second screen (projection)
    
    ofSetColor(255);
    
    ofPushMatrix();
    
    // Draw shapes
    // Turn around 180
    
    ofRotateDeg(180); // TODO: Do this if Kinect is upside down
    // we need to rotate it in place
    finalFbo.draw(SCREEN_WIDTH + fboLeft, fboTop);
    
    
    // Draw targets
    targetFbo.draw(SCREEN_WIDTH + fboLeft, fboTop); // first one: + GRID_X_OFFSET
    ofPopMatrix();
    
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
    if (!moveTarget) {
        moveTarget = true;
    }
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
            if (!moveTarget) {
                moveTarget = true;
            }
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
void ofApp::updateTargets() {
    int minTargetSize = 1;
    
    int nextX;
    int nextY;
    int nextW;
    int nextH;
    
    // Populate nextTargetRect
    // Check if the targetRect == nextTargetRect. Regenerate until this isn't the case
    do {
        nextX = ofRandom(xOffset, xOffset + xRange - minTargetSize);
        nextY = ofRandom(yOffset, yOffset + yRange - minTargetSize);
        nextW = floor(ofRandom(1, max(1, xRange - nextX + xOffset)));
        nextH = floor(ofRandom(1, max(1, yRange - nextY + yOffset)));
    } while ((xRange > 2 || yRange > 2) && // first check if range is greater than 1x1. If 1x1, it can be the same
             prevTargetRect.x == nextX &&
             prevTargetRect.y == nextY &&
             prevTargetRect.width == nextW &&
             prevTargetRect.height == nextH);
    
    nextTargetRect = ofRectangle(nextX,
                                 nextY,
                                 nextW,
                                 nextH);
    
    // if first time, set targetRect to be nextTargetRect (for animation)
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
