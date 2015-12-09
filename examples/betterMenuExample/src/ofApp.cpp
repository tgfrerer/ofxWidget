#include "ofApp.h"
#include "ofxWidget.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetBackgroundColor(ofColor::darkGray);
	ofLog() << "setup";
}

//--------------------------------------------------------------
void ofApp::update() {

	if (mContextMenu && mContextMenu->shouldClose())
		mContextMenu.reset();

	ofxWidget::update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofPushStyle();
	ofxWidget::draw();
	ofPopStyle();
}

//--------------------------------------------------------------

void ofApp::createContextMenu(int x_, int y_) {
	mContextMenu = Menu::make_unique({ float(x_), float(y_), 200.f, 200.f });
	mContextMenu->setup();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (button == 2) {
		createContextMenu(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
