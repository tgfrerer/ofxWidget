#include "ofApp.h"
#include "ofxWidget.h"

//--------------------------------------------------------------
void ofApp::setup(){

	guiElements.emplace_back( make_shared<TestGuiElement>(10.f, 10.f, 400.f, 400.f) );
	
	guiElements.emplace_back(make_shared<TestGuiElement>(100.f, 30.f, 400.f, 400.f ));
	guiElements.emplace_back(make_shared<TestGuiElement>(130.f, 20.f + 50.f, 100.f, 300.f));
	guiElements.emplace_back(make_shared<TestGuiElement>(130.f, 20.f + 10.f, 100.f, 30.f));
	guiElements.emplace_back(make_shared<TestGuiElement>(130.f, 25.f + 100.f, 30.f, 30.f));

	guiElements[2]->setParent(*guiElements[1]);
	guiElements[3]->setParent(*guiElements[1]);
	guiElements[4]->setParent(*guiElements[0]);

	//it->setParent(guiElements.front()); // now set the parent of element 1 to be element 0

	// it's deleting the wrong widget shared_ptr!

	for (auto&g : guiElements) {
		g->setup();
	}

	


}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofxWidget::draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	guiElements.erase(std::next(guiElements.begin()));
	ofLog();
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
