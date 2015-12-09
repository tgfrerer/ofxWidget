#pragma once

#include "ofxWidget.h"
#include "ofRectangle.h"

#include <memory>
#include <functional>
#include <tuple>
// there are two kinds of menus:
// context menus and pulldown menus.

// this is for context menus.

// these give you a selection of actions to chose.
// only one action may be chosen, 
// or no action.

struct BaseGuiProperties {
	virtual ~BaseGuiProperties() {}; // make this polymorphic
	ofRectangle rect;
};

struct GuiButtonProperties : public BaseGuiProperties {
	string label;
};


class MenuItem {
	std::shared_ptr<ofxWidget> mWidget; ///< widget for this item

public:
	// because settings are not stored within the menu item,
	// we must make local copies of this to be able to draw.
	// also, we cannot change any properties once
	// they are set.
	// this is too clever.
	struct Settings {
		GuiButtonProperties properties;
		std::weak_ptr<ofxWidget> weakParent;
		std::function<void()> onClick;
	};
	void setup(const MenuItem::Settings& s_); //< creates widget, sets up Menu Item.
};

class Menu {
	struct Item {
		std::string label;
		std::function<void()> fn;
	};
	std::shared_ptr<ofxWidget> mCanvas; ///< main canvas for the menu
	std::list<MenuItem> mMenuItems;
	std::vector<Item> mItems;

	bool mWantsClose = false;

	void onItemClick(int itemIndex);

public:
	void setItems(std::vector<Item> && itm) {
		mItems = itm;
	};

	void setup();
	const bool shouldClose() const;
public:
	static std::unique_ptr<Menu> make_unique(const ofRectangle& rect_);
};


