#include "ofxWidget.h"
#include "ofGraphics.h"
#include "ofUtils.h"
#include <algorithm>
// ----------------------------------------------------------------------

// we want to keep track of all widgets that have been created.

// this is a "flattened" version of our widget scene graph. 
std::list<weak_ptr<ofxWidget>> sAllWidgets;

// the widget that is in focus and will receive interactions.
weak_ptr<ofxWidget>			   sFocusedWidget;

ofVec2f ofxWidget::sLastMousePos{ 0.f,0.f };

// ----------------------------------------------------------------------

bool isSame(weak_ptr<ofxWidget> &lhs, weak_ptr<ofxWidget>&rhs) {
	return (!lhs.expired() && !rhs.expired() && (!rhs.owner_before(lhs) && !lhs.owner_before(rhs)));
}

// ----------------------------------------------------------------------

auto findIt(weak_ptr<ofxWidget>& needle_, list<weak_ptr<ofxWidget>>::iterator start_ = sAllWidgets.begin()) {
	// find needle in widget list haystack
	return find_if(start_, sAllWidgets.end(), [needle = needle_](weak_ptr<ofxWidget>&w) {
		// finds if it two shared ptrs are the same.
		return (!w.owner_before(needle) && !needle.owner_before(w));
	});
}

// ----------------------------------------------------------------------

auto findIt(shared_ptr<ofxWidget>& needle_) {
	// find needle in widget list haystack
	weak_ptr<ofxWidget> wkP = needle_;
	return findIt(wkP);
}


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
	widget->mThis = widget; // widget keeps weak store to self - will this make it leak?
	// it should not, since we're creating the widget using new(), and not make_shared

	sAllWidgets.emplace_back(widget);  // store a weak pointer to the new object in our list
	return std::move(widget);
}

// ----------------------------------------------------------------------

ofxWidget::ofxWidget() {
}

// ----------------------------------------------------------------------

ofxWidget::~ofxWidget() {

	// Q: what if the parent gets destroyed first?

	// A: We should be fine.

	// the parent will remove elements from sAllWidgets,
	// possibly already having deleted our widget.
	// if that is the case, we can't find ourselves 
	// in the list of sAllWidgets, and we won't delete
	// anything from that list.

	auto it = findIt(mThis);

	if (it != sAllWidgets.end()) {

		// we have found ourselves. now, we need to delete our object range from 
		// the global list of widgets.

		sAllWidgets.erase(std::prev(it, mNumChildren), std::next(it)); // we delete the children, too.

		// let's see if we have a parent
		if (auto parent = mParent.lock()) {
			// decrement the parent's child number by (mNumChildren+1) ...
			parent->mNumChildren -= (mNumChildren + 1);
			// ... recursively
			while (parent = parent->mParent.lock()) {
				parent->mNumChildren -= (mNumChildren + 1);
			}
		};

	} // end if (it != sAllWidgets.end()) 

}

// ----------------------------------------------------------------------

void ofxWidget::setParent(std::shared_ptr<ofxWidget>& p_)
{

	if (auto p = mParent.lock()) {

		// TODO: 

		// how weird! this widget has a parent already.
		// delete the widgets from the parent's child list
		ofLogWarning() << "Widget has parent already!";
		return;
	}

	// find ourselves in widget list
	auto itMe = findIt(mThis);

	// find parent in widget list

	auto itParent = findIt(p_);

	/*

	When an object gets a parent,

	1. move it to the beginning of the parent's child range.
	2. set its parent pointer
	3. increase parent's child count by number of (own children + 1 ), recursively.

	*/

	if (itParent != sAllWidgets.end()) {
		// move current element and its children to the front of the new parent's child range
		if (auto parent = itParent->lock()) {

			sAllWidgets.splice(
				std::prev(itParent, parent->mNumChildren), 				// where to move elements to -> front of parent range
				sAllWidgets, 											// where to take elements from
				std::prev(itMe, mNumChildren), std::next(itMe));		// range of elements to move -> range of current element and its children

			mParent = parent; // set current widget's new parent
			// now increase the parents child count by (1+mNumChildren), recursively

			parent->mNumChildren += (1 + mNumChildren);

			while (parent = parent->mParent.lock()) {
				// travel up parent hierarchy and increase child count for all ancestors
				parent->mNumChildren += (1 + mNumChildren);
			}

		}
	}

}

// ----------------------------------------------------------------------

weak_ptr<ofxWidget>& ofxWidget::getParent()
{
	return mParent;
}

// ----------------------------------------------------------------------

void ofxWidget::bringToFront(std::list<weak_ptr<ofxWidget>>::iterator it_)
{
	// reorders widgets, bringing the widget pointed to by the iterator it_ to the front of the widget list.
	ofLog() << "reorder";
	if (it_ == sAllWidgets.begin())
		return;

	// ----------| invariant: element not yet at front.

	auto element = it_->lock();

	if (element == nullptr)
		return;

	// ---------| invariant: element is valid

	/*

	Algorithm:

	recursively: while current object range has a parent, put current object range to the front of parent range
	make parent range current object range. repeat until there is no parent range.

	as soon as there is no parent anymore, put last object range to the front of the list

	Heuristic: parent iterator's position always to be found after current iterator.

	*/


	auto parent = element->mParent.lock();
	auto elementIt = it_;

	while (parent) {

		auto itParent = findIt(element->mParent, std::next(elementIt)); // start our search for parent after current element.

		// if element has parent, bring element range to front of parent range.
		if (std::prev(elementIt, element->mNumChildren) != std::prev(itParent, parent->mNumChildren)) {
			sAllWidgets.splice(
				std::prev(itParent, parent->mNumChildren), 					// where to move elements to -> front of parent range
				sAllWidgets, 												// where to take elements from
				std::prev(elementIt, element->mNumChildren),
				std::next(elementIt));		// range of elements to move -> range of current element and its children
		}

		// because sAllWidgets is a list, splice will only invalidate iterators 
		// before our next search range.

		elementIt = itParent;
		element = elementIt->lock();
		parent = element->mParent.lock();
	}

	// now move the element range (which is now our most senior parent element range) to the front fo the list.

	if (std::prev(elementIt, element->mNumChildren) != sAllWidgets.begin()) {
		sAllWidgets.splice(
			sAllWidgets.begin(),
			sAllWidgets,
			std::prev(elementIt, element->mNumChildren), // from the beginning of our now most senior parent element range
			std::next(elementIt));						 // to the end of our now most senior parent element range
	}


}

// ----------------------------------------------------------------------

void ofxWidget::draw() {
	// make sure to draw last to first,
	// since we don't do z-testing.
	int zOrder = 0;
	for (auto it = sAllWidgets.crbegin(); it != sAllWidgets.crend(); ++it) {
		if (auto p = it->lock()) {
			if (p->mDraw && p->mVisible) {
				// TODO: we could set up a clip rect for the widget here...
				p->mDraw(); // call the widget
				if (ofGetKeyPressed(OF_KEY_RIGHT_CONTROL)) {
					ofPushStyle();
					ofFill();
					ofSetColor(ofColor::red, 64);
					ofDrawRectangle(p->getRect());
					ofDrawBitmapStringHighlight(ofToString(zOrder), p->mRect.x, p->mRect.y + 10);

					ofPopStyle();
				}
				zOrder++;
			} else {
				// we can jump ahead by the number of children,
				// since if a parent layer is invisible, 
				// this means that its children must be 
				// invisible, too.
				std::advance(it, p->mNumChildren);
			}
		}
	}
}

// ----------------------------------------------------------------------

void ofxWidget::update() {
	// make sure to update last to first,
	// just to stay consistent with draw order.
	for (auto it = sAllWidgets.crbegin(); it != sAllWidgets.crend(); ++it) {
		if (auto p = it->lock()) {
			if (p->mUpdate && p->mVisible) {
				// TODO: we could set up a clip rect for the widget here...
				p->mUpdate(); // call the widget
			}
		}
	}
}

// ----------------------------------------------------------------------
// static method - called once for all widgets by the
//                 WidgetEventResponder, which self-
//                 registers to all events upon creation.
//
void ofxWidget::mouseEvent(ofMouseEventArgs& args_) {

	// If we register a mouse down event, we do a hit test over
	// all visible widgets, and re-order if necessary.
	// Then, and in all other cases, we do a hit-test on the 
	// frontmost widget and, if positive, forward the event to this 
	// widget.

	if (sAllWidgets.empty()) return;

	// ---------| invariant: there are some widgets flying around.

	float mx = args_.x;
	float my = args_.y;

	// if we have a mouse down, we re-order widgets.

	// if we have a click, we want to make sure the widget gets to be the topmost widget.
	if (args_.type == ofMouseEventArgs::Pressed) {

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
			if (!isSame(*it, sFocusedWidget)) {
				// change in focus detected.
				// first, let the first element know that it is losing focus
				if (auto previousElementInFocus = sFocusedWidget.lock())
					if (previousElementInFocus->mExitFocus)
						previousElementInFocus->mExitFocus();

				sFocusedWidget = *it;

				// now that the new wiget is at the front, send an activate callback.
				if (auto nextFocusedWidget = it->lock())
					if (nextFocusedWidget->mEnterFocus)
						nextFocusedWidget->mEnterFocus();
			}
			if (auto w = it->lock()) {
				// We're conservative with re-ordering.
				// Let's move the iterator forward to see if we are actually 
				// already sorted.
				// If the list were already sorted, then moving back from the current
				// iterator by the number of its children would bring us 
				// to the beginning of sAllWidgets. Then, there is no need to re-order.
				if (std::prev(it, w->mNumChildren) != sAllWidgets.begin()) {
					
					bringToFront(it); // reorder widgets
				}
			}
		}
	}

	// now, we will attempt to send the mouse event to the widget that 
	// is in focus.

	if (auto w = sFocusedWidget.lock()) {
		if (w->mMouseResponder)
			w->mMouseResponder(args_);
		// TODO: we should send a "mouse left" event if the last mousePos was inside -
		// and a "mouse enter" event if the last mousePos was outside.

		//if (w->getRect().inside({ mx, my, 0.f })) {
		//} else {
		//}
	}

	// store last mouse position last thing, so that 
	// we are able to calculate a difference.
	sLastMousePos.set(mx, my);
}

// ----------------------------------------------------------------------
// static method - called once for any widgets
void ofxWidget::keyEvent(ofKeyEventArgs& args_) {

	if (sAllWidgets.empty()) return;

	if (auto w = sFocusedWidget.lock()) {
		if (w->mKeyResponder)
			w->mKeyResponder(args_);
	}

}
// ----------------------------------------------------------------------

bool ofxWidget::isAtFront() {
	return (sAllWidgets.empty()) ? false : isSame(mThis, sAllWidgets.front());
}

// ----------------------------------------------------------------------