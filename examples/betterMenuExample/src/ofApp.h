#pragma once

#include "ofMain.h"
#include "Menu.h"

class ofApp : public ofBaseApp{

	std::unique_ptr<Menu> mContextMenu;
	

	public:
		void setup();
		void update();
		void draw();
		void createContextMenu(int x_, int y_);

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