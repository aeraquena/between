#include "ofMain.h"
#include "ofApp.h"

int main() {
    // Fullscreen one window
    //ofSetupOpenGL(1920,1080,OF_FULLSCREEN);
    //ofRunApp(new ofApp());
    
    // Fullscreen spans multiple windows
    ofGLFWWindowSettings settings;
    settings.multiMonitorFullScreen = true;
    settings.windowMode = OF_FULLSCREEN;
    ofCreateWindow(settings);
    ofRunApp(new ofApp);
    
    // Windowed
    //ofSetupOpenGL(2560, 1024, OF_WINDOW);
    //ofRunApp(new ofApp());
}
