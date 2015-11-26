#pragma once
#include "ofxWidget.h"

#include <memory>
#include <functional>

class Menu
{
	// the container == parent for all other widgets in this class
	std::shared_ptr<ofxWidget> mWiMenuContainer;
	std::shared_ptr<ofxWidget> mWiCloseButton;
	
	// the canvas is where the options will be displayed
	std::shared_ptr<ofxWidget> mWiCanvas;

	std::string mName;
	ofRectangle mRect;

	bool mRectsDirty = true;

	void mouseReponderMenuContainer(const ofMouseEventArgs& args_);
	void calculateRects();

	ofVec2f mLastMouseDown;

public:
	Menu();
	~Menu();

	void setRect(const ofRectangle &rect_);
	const ofRectangle& getRect() const;

	void setName(const std::string& name_);
	const std::string& getName() const;

	std::function<void(Menu* w_)> mRemoveSelf; // a method the menu may call to indicate that it doesn't want to exist anymore.

	void setup();

private:

};

// ---------------------------------------------------------------
// simple getters are inlined

inline const std::string& Menu::getName() const {
	return mName;
}

inline const ofRectangle& Menu::getRect() const {
	return mRect;
}
