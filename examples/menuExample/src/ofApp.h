#pragma once

#include "ofMain.h"
#include "Menu.h"
#include "ofxWidget.h"

class ofApp : public ofBaseApp{

	std::list<std::shared_ptr<Menu>> mMenus;

	public:
	
		void removeMenu(Menu* m_);
		
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
