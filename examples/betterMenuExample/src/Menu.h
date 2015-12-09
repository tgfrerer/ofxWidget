#pragma once

#include "ofxWidget.h"
#include "ofRectangle.h"

#include <memory>
#include <functional>

// there are two kinds of menus:
// context menus and pulldown menus.

// this is for context menus.

// these give you a selection of actions to chose.
// only one action may be chosen, 
// or no action.

class MenuItem {
	std::shared_ptr<ofxWidget> mWidget; ///< widget for this item
public:
	struct Settings {
		ofRectangle rect;
		std::string label;
		std::weak_ptr<ofxWidget> weakParent;
		std::function<void()> onClick;
	};
	void setup(const MenuItem::Settings& s_); //< creates widget, sets up Menu Item.
};

class Menu {

	std::shared_ptr<ofxWidget> mCanvas; ///< main canvas for the menu
	std::list<MenuItem> mMenuItems;		

 public:
	void setup();
	const bool shouldClose() const;

public:
	static std::unique_ptr<Menu> make_unique(const ofRectangle& rect_);
};


