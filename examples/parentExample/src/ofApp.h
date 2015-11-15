#pragma once

#include "ofMain.h"
#include "testGuiElement.h"


class ofApp : public ofBaseApp{

	// the problem is probably here: since the objects 
	// may move, the bind position is not guaranteed to 
	// resolve to the correct object.

	vector<shared_ptr<TestGuiElement>> guiElements;	// a list is ok, a vector not. // a map is ok, too.

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
		
};
