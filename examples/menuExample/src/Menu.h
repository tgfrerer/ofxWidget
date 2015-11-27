#pragma once
#include "ofxWidget.h"

#include <memory>
#include <functional>

class Menu
{
	class MenuItem {
		std::shared_ptr<ofxWidget> mWiMenuItem;
		std::string mLabel;
		std::string mValue; // what this item will return on click
		MenuItem() = delete; // disable default construc
		
	public:
		
		MenuItem(std::shared_ptr<ofxWidget>& parent_, 
			std::function<void(const std::string& value)>& onClick_,
			const std::string& label_, 
			const std::string& value_);
		void setRect(const ofRectangle& rect_);
		
	};
	
	const std::string mName;
	const std::map<std::string, std::string> mItemLabelsValues; // label, value
	// the container == parent for all other widgets in this class
	std::shared_ptr<ofxWidget> mWiMenuContainer;
	std::shared_ptr<ofxWidget> mWiCloseButton;
	
	// the canvas is where the options will be displayed
	std::shared_ptr<ofxWidget> mWiCanvas;

	ofRectangle mRect;


	Menu() = delete;

public:

	Menu(const std::string& name_, const std::map<std::string, std::string>& itemLabelsValues_);
	~Menu();

	void setRect(const ofRectangle &rect_);
	void setup();

// callbacks 

	std::function<void(Menu* w_)> mRemoveSelf; // a method the menu may call to indicate that it doesn't want to exist anymore.
	std::function<void(const std::string&)> mOnItemClicked;

// getters

	const ofRectangle& getRect() const;
	const std::string& getName() const;

private:
	bool mRectsDirty = true;
	void mouseResponderMenuContainer(const ofMouseEventArgs& args_);
	void calculateRects();
	ofVec2f mLastMouseDown;
	std::list<std::shared_ptr<MenuItem>> mMenuItems;

};

// ---------------------------------------------------------------
// simple getters are inlined

inline const std::string& Menu::getName() const {
	return mName;
}

inline const ofRectangle& Menu::getRect() const {
	return mRect;
}
