#include "Menu.h"
#include "ofGraphics.h"

using namespace std;

// ---------------------------------------------------------------

Menu::Menu(const string& name_, const std::map<std::string, std::string>& itemsLabelsValues_)
	: mName(name_)
	, mItemLabelsValues(itemsLabelsValues_)
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

void Menu::setup()
{
	// this is where we create & wire up all our widgets

	mWiMenuContainer = ofxWidget::make({});
	mWiCloseButton = ofxWidget::make({});
	mWiCanvas = ofxWidget::make({});

	mWiCloseButton->setParent(mWiMenuContainer);
	mWiCanvas->setParent(mWiMenuContainer);


	// ---------
	// draw method for the container (that's going to be in the bg)
	//
	mWiMenuContainer->onDraw = [&wi = mWiMenuContainer, &name_ = mName]() {
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


	// ---------
	// draw method for the close button
	//
	mWiCloseButton->onDraw = [&wi = mWiCloseButton]() {
		if (!wi) return;
		ofFill();
		ofSetColor(ofColor::red);
		const auto & rect = wi->getRect();
		ofVec2f circleOrigin(rect.width*0.5, rect.height*0.5);
		ofDrawCircle(rect.position + circleOrigin, rect.width*0.50*.75);
	};

	mWiMenuContainer->onMouse = std::bind(&Menu::mouseResponderMenuContainer, this, std::placeholders::_1);
	// we define the close button responder as a lambda, because it's more 
	// elegant.
	mWiCloseButton->onMouse = [this](ofMouseEventArgs& args_) {
		if (args_.type == ofMouseEventArgs::Released && args_.button == 0) {
			// call an event to tell us that we should close.
			// and that we should destroy this menu.
			// mRemoveSelf is a function provided by the host object,
			// which effectively decrements the shared_ptr
			// to the current Menu, so that it might be eventually destroyed.
			if (mRemoveSelf && mWiCloseButton->getRect().inside(args_)) {
				mRemoveSelf(this);
				return; // after we called removeSelf, we must assume this object is invalid.
			}
		}
	};

	// now add menu items.

	for (auto & itm : mItemLabelsValues) {
		mMenuItems.emplace_back(make_shared<MenuItem>(mWiCanvas, mOnItemClicked, itm.first, itm.second));
	}

	// now do the layout.

	mWiMenuContainer->setRect(mRect);
	mWiCanvas->setRect({ mRect.x + 10, mRect.y + 30.f, mRect.width - 20, mRect.height - 30.f });
	mWiCloseButton->setRect({ mRect.x + mRect.width - 20, mRect.y, 20.f, 20.f });

	int i = 0;
	for (auto &m : mMenuItems) {
		m->setRect({ mRect.x + 10, mRect.y + 30 + (20 + 5) * i, mRect.width - 20, 20 });
		++i;
	}
}

// ---------------------------------------------------------------

void Menu::mouseResponderMenuContainer(const ofMouseEventArgs& args_) {

	if (!mWiMenuContainer)
		return;
	if (args_.type == ofMouseEventArgs::Pressed && args_.button == 0) {
		mLastMouseDown = args_; // args_ is an ofVec2f, so we capture that slice for the position.
	} else if (args_.type == ofMouseEventArgs::Dragged && args_.button == 0) {
		// now we have to calculate the delta to the last 
		// mouse pos, and signal that the rect needs to be recalculated.
		auto delta = args_ - mLastMouseDown;
		mLastMouseDown = args_;
		mWiMenuContainer->moveBy(delta);
	} else if (args_.type == ofMouseEventArgs::Pressed && args_.button == 2) {
		// this should hide all the buttons with the canvas.
		mWiCanvas->setVisibility(mWiCanvas->getVisibility()^1);
	}
}

// ---------------------------------------------------------------

Menu::MenuItem::MenuItem(std::shared_ptr<ofxWidget>& parent_, std::function<void(const std::string&value)>& onClick_, const std::string & label_, const std::string & value_)
	: mLabel(label_)
	, mValue(value_)

{
	mWiMenuItem = ofxWidget::make({});
	mWiMenuItem->setParent(parent_);

	mWiMenuItem->onDraw = [&rect = mWiMenuItem->getRect(), &label = mLabel]() {
		ofFill();
		ofSetColor(ofColor::gray);
		ofDrawRectangle(rect);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(label, rect.x + 10, rect.y + 10);
	};

	// Note that onClick is a local copy of the parameter, and is *not* captured as a reference.
	// otherwise, this will go down very badly.
	// the function object for onClick is captured earlier, in the constructor for MenuItem.
	mWiMenuItem->onMouse = [onClick = onClick_, &value = mValue](const ofMouseEventArgs& args_) {
		if (args_.type == ofMouseEventArgs::Released && args_.button == 0) {
			// this menu has been selected.
			if (onClick)
				onClick(value);
		}
	};

}
// ---------------------------------------------------------------

void Menu::MenuItem::setRect(const ofRectangle & rect_) {
	mWiMenuItem->setRect(rect_);
}
