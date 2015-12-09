#include "Menu.h"
#include "ofGraphics.h"

using namespace std;

// factory method to create menu
unique_ptr<Menu> Menu::make_unique(const ofRectangle& rect_) {
	unique_ptr<Menu> m = unique_ptr<Menu>(new Menu);
	m->mCanvas = ofxWidget::make(rect_);
	return std::move(m);
};

// ----------------------------------------------------------------------
// static draw method - 
void drawButton(const ofRectangle& rect_, bool hover_ = false) {
	if (hover_) {
		ofSetColor(ofColor::lightCoral);
	} else {
		ofSetColor(ofColor::coral);
	}
	ofFill();
	ofDrawRectangle(rect_);
}

// ----------------------------------------------------------------------

void applyButtonBehaviour(shared_ptr<ofxWidget>& button_) {

	button_->onDraw = [&w = button_]() {
		drawButton(w->getRect(), w->getHover());
	};

	button_->onMouse = [&w = button_](ofMouseEventArgs& args) {
		if (args.type == ofMouseEventArgs::Released && w->getRect().inside(args)) {
			// mouse released.
			ofLogNotice() << "Button clicked!";
		}
	};
}

// ----------------------------------------------------------------------

void MenuItem::setup(const MenuItem::Settings& s_) {
	if (auto p = s_.weakParent.lock()) {
		mWidget = ofxWidget::make(ofRectangle(s_.rect.x, s_.rect.y, s_.rect.width, s_.rect.height));
		mWidget->setParent(s_.weakParent.lock());

		// this is an interesting pattern: 
		// it is a bit like strategy, but maybe also a bit like decorator
		applyButtonBehaviour(mWidget); // makes the widget behave like a button
		// override default click behaviour
		mWidget->onMouse = [rect = s_.rect, clickFn = s_.onClick](ofMouseEventArgs& args_) {
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

	// all our elements need to have these two methods bound
	// so that as long as one element has the focus, we're golden.

	const auto & rect = mCanvas->getRect();

	for (int i = 0; i < 5; i++) {
		MenuItem::Settings s;
		s.weakParent = mCanvas;
		s.rect = ofRectangle(rect.position + ofVec2f(0.f, i * 30.f),  rect.width, 30.f );
		s.onClick = [num=i]() {
			ofLogNotice() << "Menu item " << num << " clicked.";
		};
		mMenuItems.emplace_back();
		mMenuItems.back().setup(s);
	}

	mCanvas->setRect({ rect.position, rect.width, (5) * 30.f });

	// -------

	// put the menu in focus.
	mCanvas->setFocus(true);
}

// ----------------------------------------------------------------------

const bool Menu::shouldClose() const {
	if (mCanvas) {
		return (mCanvas->containsFocus() == false);
	} else {
		// canvas does not exist.
		// should close.
		return true;
	}
}

// ----------------------------------------------------------------------

