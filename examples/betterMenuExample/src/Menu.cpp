#include "Menu.h"
#include "ofGraphics.h"
#include "ofUtils.h"

using namespace std;


// ----------------------------------------------------------------------
// static draw method - 
void drawButtonBackground(const ofRectangle& rect_, bool hover_ = false) {
	if (hover_) {
		ofSetColor(ofFloatColor(0.4));
	} else {
		ofSetColor(ofFloatColor(0.2));
	}
	ofFill();
	ofDrawRectangle(rect_);
}

// ----------------------------------------------------------------------
// adds draw method based on detected property type.
void applySkin(shared_ptr<ofxWidget>& w_, const BaseGuiProperties* props_) {

	if (auto p = dynamic_cast<const GuiButtonProperties*>(props_)) {
		// make a local copy of the button properties for drawing
		w_->onDraw = [&w = w_, cachedProps = *p]() {
			drawButtonBackground(w->getRect(), w->getHover());
			ofSetColor(ofColor::white);
			auto rect = w->getRect();
			ofDrawBitmapString(cachedProps.label, rect.x +20, rect.y + 20);
		};
	}
}

// ----------------------------------------------------------------------

void MenuItem::setup(const MenuItem::Settings& s_) {
	if (auto p = s_.weakParent.lock()) {
		auto &rect = s_.properties.rect;
		mWidget = ofxWidget::make(ofRectangle(rect.x, rect.y, rect.width, rect.height));
		mWidget->setParent(s_.weakParent.lock());

		applySkin(mWidget, &s_.properties); // makes the widget draw like a button
		// note that we make local copies of the settings, 
		// since settings is a temporary object.
		mWidget->onMouse = [rect = rect, clickFn = s_.onClick](ofMouseEventArgs& args_) {
			if (args_.type == ofMouseEventArgs::Released && rect.inside(args_)) {
				if (clickFn)
					clickFn();
			}
		};
	} else {
		// could not setup menu item
		ofLogWarning() << "Could not setup menu item. Parent widget not available.";
	}
}

// ----------------------------------------------------------------------

void Menu::setup() {

	mCanvas->onDraw = [&w = mCanvas]() {
		ofSetColor(ofColor::lightGrey);
		ofFill();
		ofDrawRectangle(w->getRect());
	};

	const auto & rect = mCanvas->getRect();

	mMenuItems.clear();

	for (int i = 0; i < mItems.size(); i++) {
		MenuItem::Settings s;
		s.weakParent = mCanvas;
		s.properties.rect = ofRectangle(rect.position + ofVec2f(0.f, i * 30.f),  rect.width, 30.f );
		s.properties.label = mItems[i].label;
		s.onClick = [num=i, this]() {
			onItemClick(num);
		};
		mMenuItems.emplace_back();
		mMenuItems.back().setup(s);
	}

	mCanvas->setRect({ rect.position, rect.width, (mItems.size()) * 30.f });
	
	// -------

	// put the menu in focus.
	mCanvas->setFocus(true);
}

// ----------------------------------------------------------------------

const bool Menu::shouldClose() const {
	if (mWantsClose)
		return true;
	if (mCanvas) {
		return (mCanvas->containsFocus() == false);
	} else {
		return true;
	}
}

// ----------------------------------------------------------------------
// responder for all menu items
void Menu::onItemClick(int index_) {
	if (index_ < int(mItems.size())) {
		mItems[index_].fn();
	}
	mWantsClose = true;
}

// ----------------------------------------------------------------------
// static factory to create menu
unique_ptr<Menu> Menu::make_unique(const ofRectangle& rect_) {
	unique_ptr<Menu> m = unique_ptr<Menu>(new Menu);
	m->mCanvas = ofxWidget::make(rect_);
	return std::move(m);
};