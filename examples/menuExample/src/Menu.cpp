#include "Menu.h"
#include "ofGraphics.h"

using namespace std;

// ---------------------------------------------------------------

Menu::Menu()
{
	ofLogNotice() << "created menu";
}

Menu::~Menu() {
	ofLogNotice() << "destroyed menu";
}

// ---------------------------------------------------------------

void Menu::setRect(const ofRectangle& rect_) {
	mRect = rect_;
}

// ---------------------------------------------------------------

void Menu::setName(const string& name_) {
	mName = name_;
}

// ---------------------------------------------------------------

void Menu::setup()
{
	// this is where we create & wire up all our widgets

	mWiMenuContainer = ofxWidget::make({});
	mWiCloseButton = ofxWidget::make({});

	mWiCloseButton->setParent(mWiMenuContainer);

	calculateRects();

	// ---------
	// draw method for the container (that's going to be in the bg)
	//
	mWiMenuContainer->mDraw = [&wi = mWiMenuContainer, &name_ = mName]() {
		if (!wi) return;
		const auto & rect = wi->getRect();
		ofSetColor(ofColor::white);
		ofFill();
		ofDrawRectangle(rect);
		ofSetColor(ofColor::red);
		ofNoFill();
		ofDrawRectangle(rect);
		ofSetColor(ofColor::black);
		ofDrawBitmapString(name_, rect.x + 10, rect.y + 12);
	};

	mWiMenuContainer->mMouseResponder = std::bind(&Menu::mouseReponderMenuContainer, this, std::placeholders::_1);

	// ---------
	// draw method for the close button
	//
	mWiCloseButton->mDraw = [&wi = mWiCloseButton]() {
		if (!wi) return;
		ofFill();
		ofSetColor(ofColor::red);
		ofDrawRectangle(wi->getRect());
	};

	// we define the close button responder as a lambda, because it's more 
	// elegant.
	mWiCloseButton->mMouseResponder = [this](ofMouseEventArgs& args_) {
		if (args_.type == ofMouseEventArgs::Released && args_.button == 0) {
			// call an event to tell us that we should close.
			// and that we should destroy this menu.
			// mRemoveSelf is a function provided by the host object,
			// which effectively decrements the shared_ptr
			// to the current Menu, so that it might be eventually destroyed.
			if (mRemoveSelf)
				mRemoveSelf(this);
		}
	};

}

// ---------------------------------------------------------------

void Menu::calculateRects() {
	if (!mWiMenuContainer) return;

	mWiMenuContainer->setRect(mRect);
	mWiCloseButton->setRect({ mRect.x + mRect.width - 20, mRect.y, 20.f, 20.f });


}
// ---------------------------------------------------------------

void Menu::mouseReponderMenuContainer(const ofMouseEventArgs& args_) {
	
		if (!mWiMenuContainer)
			return;
		if (args_.type == ofMouseEventArgs::Pressed && args_.button == 0) {
			mLastMouseDown = args_; // args_ is an ofVec2f, so we capture that slice for the position.
		} else if (args_.type == ofMouseEventArgs::Dragged && args_.button == 0) {
			// now we have to calculate the delta to the last 
			// mouse pos, and signal that the rect needs to be recalculated.
			auto delta = args_ - mLastMouseDown;
			mRect.x += delta.x;
			mRect.y += delta.y;
			mLastMouseDown = args_;
			calculateRects();
		}
	
}

// ---------------------------------------------------------------



