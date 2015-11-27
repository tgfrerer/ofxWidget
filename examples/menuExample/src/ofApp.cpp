#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	for (int i = 10; i >= 0; --i) {
		string menuName = "Menu " + ofToString(i);
		auto m = make_shared<Menu>(Menu(menuName, {
			{ "Item 0" , "Value 0.Menu " + ofToString(i) },
			{ "Item 1" , "Value 1.Menu " + ofToString(i) },
			{ "Item 2" , "Value 2.Menu " + ofToString(i) },
			{ "Item 3" , "Value 3.Menu " + ofToString(i) },

		}));

		mMenus.emplace_back(m);
		m->setRect({ 10 + i * 30.f, 10.f + i * 30, 200, 300 });
		
		m->mRemoveSelf = [&menuList = mMenus](Menu* m_) {
			// find a shared_ptr in our menu which corresponds to 
			// the menu pointed to m_. If found, remove it form the
			// list. Note that we only capture mMenus from the local
			// stack, and that we capture it by reference.
			auto it = std::find_if(menuList.begin(), menuList.end(), [&m_](const shared_ptr<Menu>& lhs) {
				return lhs.get() == m_;
			});
			if (it != menuList.end())
				menuList.erase(it);
		};
		m->mOnItemClicked = [&menuList = mMenus](const string& value) {
			ofLogNotice() << "Menu item clicked: " << value;
			menuList.clear();
		};
		m->setup();

	}
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	
	// this will draw any menu items.
	ofxWidget::draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
