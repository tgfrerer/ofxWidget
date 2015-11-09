#include "ofxWidget.h"
#include "ofGraphics.h"
#include <algorithm>
// ----------------------------------------------------------------------

// we want to keep track of all widgets that have been created.
std::list<weak_ptr<ofxWidget>> sAllWidgets;

// ----------------------------------------------------------------------

WidgetEventResponder::WidgetEventResponder()
{
	auto listener = this;
	auto prio = OF_EVENT_ORDER_AFTER_APP;

	ofAddListener(ofEvents().mouseDragged, listener, &WidgetEventResponder::mouseEvent, prio);
	ofAddListener(ofEvents().mouseMoved, listener, &WidgetEventResponder::mouseEvent, prio);
	ofAddListener(ofEvents().mousePressed, listener, &WidgetEventResponder::mouseEvent, prio);
	ofAddListener(ofEvents().mouseReleased, listener, &WidgetEventResponder::mouseEvent, prio);
	ofAddListener(ofEvents().mouseScrolled, listener, &WidgetEventResponder::mouseEvent, prio);
	ofAddListener(ofEvents().mouseEntered, listener, &WidgetEventResponder::mouseEvent, prio);
	ofAddListener(ofEvents().mouseExited, listener, &WidgetEventResponder::mouseEvent, prio);

	// now add key event listeners

	ofAddListener(ofEvents().keyPressed, listener, &WidgetEventResponder::keyEvent, prio);
	ofAddListener(ofEvents().keyReleased, listener, &WidgetEventResponder::keyEvent, prio);

	ofLogNotice() << "adding widget listener";
}
// ----------------------------------------------------------------------

WidgetEventResponder::~WidgetEventResponder()
{
	auto listener = this;
	auto prio = OF_EVENT_ORDER_AFTER_APP;

	ofRemoveListener(ofEvents().keyReleased, listener, &WidgetEventResponder::keyEvent, prio);
	ofRemoveListener(ofEvents().keyPressed, listener, &WidgetEventResponder::keyEvent, prio);

	ofRemoveListener(ofEvents().mouseExited, listener, &WidgetEventResponder::mouseEvent, prio);
	ofRemoveListener(ofEvents().mouseEntered, listener, &WidgetEventResponder::mouseEvent, prio);
	ofRemoveListener(ofEvents().mouseScrolled, listener, &WidgetEventResponder::mouseEvent, prio);
	ofRemoveListener(ofEvents().mouseReleased, listener, &WidgetEventResponder::mouseEvent, prio);
	ofRemoveListener(ofEvents().mousePressed, listener, &WidgetEventResponder::mouseEvent, prio);
	ofRemoveListener(ofEvents().mouseMoved, listener, &WidgetEventResponder::mouseEvent, prio);
	ofRemoveListener(ofEvents().mouseDragged, listener, &WidgetEventResponder::mouseEvent, prio);

}

// ----------------------------------------------------------------------

void WidgetEventResponder::mouseEvent(ofMouseEventArgs & args_)
{
	ofxWidget::mouseEvent(args_);
}

// ----------------------------------------------------------------------

void WidgetEventResponder::keyEvent(ofKeyEventArgs & args_)
{
	ofxWidget::keyEvent(args_);
}

// ----------------------------------------------------------------------
// widgets may only be created through this factory function
shared_ptr<ofxWidget> ofxWidget::make(const ofRectangle& rect_) {
	// register for mouse events
	// this happens only when the first widget gets initialised.
	static auto onlyResponder = make_shared<WidgetEventResponder>();

	auto widget = shared_ptr<ofxWidget>(new ofxWidget());
	widget->mRect = rect_;
	widget->mColor = ofFloatColor(ofRandomuf(), ofRandomuf(), ofRandomuf(), 1.0);
	sAllWidgets.emplace_front(widget);  // store a weak pointer to the new object in our list
	return std::move(widget);
}

// ----------------------------------------------------------------------

ofxWidget::ofxWidget() {
	;
}

// ----------------------------------------------------------------------

ofxWidget::~ofxWidget() {

	// now we remove any expired widgets from our list.
	sAllWidgets.remove_if([this](auto &lhs)->bool {
		if (lhs.expired()) {
			// that widget has been expired.
			return true;
		} else {
			return false;
		}
	});

}

// ----------------------------------------------------------------------

void ofxWidget::draw() {

	// make sure to draw last to first,
	// since we don't do z-testing.

	auto it = sAllWidgets.crbegin();

	while (it != sAllWidgets.crend()) {
		auto p = it->lock();
		if (p) {
			if (p->mDraw && p->mVisible) {
				// TODO: we could set up a clip rect for the widget here...
				p->mDraw(); // call the widget
			}
			++it;
		}
	}
}

// ----------------------------------------------------------------------
void ofxWidget::update() {

	// make sure to draw last to first,
	// since we don't do z-testing.

	auto it = sAllWidgets.crbegin();

	while (it != sAllWidgets.crend()) {
		auto p = it->lock();
		if (p) {
			if (p->mUpdate && p->mVisible) {
				// TODO: we could set up a clip rect for the widget here...
				p->mUpdate(); // call the widget
			}
			++it;
		}
	}
}

// ----------------------------------------------------------------------
// static method - called once for all widgets
void ofxWidget::mouseEvent(ofMouseEventArgs& args_) {
	// Check whether the mouse is inside, otherwise discard mouse event,
	// or do all other sorts of things with this widget.

	// all visible widgets need to be sorted front to back
	// then we can do a hit test, and trigger the first hit widget.

	if (sAllWidgets.empty()) return;

	// ---------| invariant: there are some widgets flying around.

	float mx = args_.x;
	float my = args_.y;

	//if (args_.type == ofMouseEventArgs::Scrolled) {
	//	// scrolling is annoying, since the args.x and .y actually refer 
	//	// to the scroll delta, and not the mouse position.
	//	// so we need extract the mouse pos separately...
	//	mx = ofGetMouseX();
	//	my = ofGetMouseY();
	//}

	// find the first widget that is under the mouse, that is also visible
	auto it = std::find_if(sAllWidgets.begin(), sAllWidgets.end(), [&mx, &my](std::weak_ptr<ofxWidget>& w) ->bool {
		auto p = w.lock();
		if (p && p->mVisible && p->mRect.inside(mx, my)) {
			return true;
		} else {
			return false;
		}
	});

	if (it != sAllWidgets.end()) {
		// we are over a widget.

		// if the event is a mouse down, then we need to make sure that the 
		// widget is moved to the front of the widget list.

		auto p = it->lock();
		if (p) {
			if (p->mMouseResponder) {
				p->mMouseResponder(args_); // call the mouse event on the widget
			}
		}
	}

	// if we have a click, we want to make sure the widget gets to be the topmost widget.
	if (args_.type == ofMouseEventArgs::Pressed && it != sAllWidgets.end() && it != sAllWidgets.begin()) {

		// ---------| the widget clicked is not yet the first in our list of widgets

		// we need to put the widget into the first spot in our list
		// for this, we use the splice operation, which just moves elements around in our list,
		// but does not involve any creation/desctruction of elements.

		// transfers element in range [it,std::next(it)) to position
		// sAllWidgets.begin()
		sAllWidgets.splice(sAllWidgets.begin(), sAllWidgets, it, std::next(it));
	}
}

// ----------------------------------------------------------------------
// static method - called once for all widgets
void ofxWidget::keyEvent(ofKeyEventArgs& args_) {

	if (sAllWidgets.empty()) return;

	// only the frontmost widget will receive the event.
	// but only if it is visible

	auto p = sAllWidgets.front().lock();

	if (p->mKeyResponder && p->mVisible) {
		p->mKeyResponder(args_);
	}

}
// ----------------------------------------------------------------------
