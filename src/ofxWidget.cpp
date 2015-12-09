#include "ofxWidget.h"
#include "ofGraphics.h"
#include "ofUtils.h"
#include <algorithm>
// ----------------------------------------------------------------------

// we want to keep track of all widgets that have been created.

// this is a "flattened" version of our widget scene graph. 
std::list<weak_ptr<ofxWidget>> sAllWidgets;

// a list of visible widgets
list<weak_ptr<ofxWidget>>	   sVisibleWidgets;
bool ofxWidget::bVisibleListDirty = true; // whether the cache needs to be re-built upon update.

// the widget that is in focus and will receive interactions.
weak_ptr<ofxWidget>			   sFocusedWidget;
weak_ptr<ofxWidget>			   sWidgetUnderMouse;

ofVec2f ofxWidget::sLastMousePos{ 0.f,0.f };

// ----------------------------------------------------------------------

bool isSame(const weak_ptr<ofxWidget> &lhs, const weak_ptr<ofxWidget>&rhs) {
	return (!lhs.expired() && !rhs.expired() && (!rhs.owner_before(lhs) && !lhs.owner_before(rhs)));
}

// ----------------------------------------------------------------------

auto findIt(const weak_ptr<ofxWidget>& needle_,
	const list<weak_ptr<ofxWidget>>::iterator start_,
	const list<weak_ptr<ofxWidget>>::iterator end_) {
	// find needle in widget list haystack
	return find_if(start_, end_, [needle = needle_](weak_ptr<ofxWidget>&w) {
		// finds if it two shared ptrs are the same.
		return (!w.owner_before(needle) && !needle.owner_before(w));
	});
}

// ----------------------------------------------------------------------

//auto findIt(shared_ptr<ofxWidget>& needle_) {
//	// find needle in widget list haystack
//	weak_ptr<ofxWidget> wkP = needle_;
//	return findIt(wkP, sAllWidgets.begin(), sAllWidgets.end());
//}

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

	ofLogVerbose() << "adding widget listener";
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

bool WidgetEventResponder::mouseEvent(ofMouseEventArgs & args_)
{
	return ofxWidget::mouseEvent(args_);
}

// ----------------------------------------------------------------------

bool WidgetEventResponder::keyEvent(ofKeyEventArgs & args_)
{
	return ofxWidget::keyEvent(args_);
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

	sAllWidgets.emplace_front(widget);  // store a weak pointer to the new object in our list
	ofxWidget::bVisibleListDirty = true;
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

	auto it = findIt(mThis, sAllWidgets.begin(), sAllWidgets.end());

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
		ofxWidget::bVisibleListDirty = true;
	} // end if (it != sAllWidgets.end()) 
}

// ----------------------------------------------------------------------

void ofxWidget::setParent(std::shared_ptr<ofxWidget>& p_)
{
	if (auto p = mParent.lock()) {
		// TODO: 
		// how weird! this widget has a parent already.
		// delete the widgets from the parent's child list
		ofLogWarning() << "Widget already has parent!";
		return;
	}

	// find ourselves in widget list
	auto itMe = findIt(mThis, sAllWidgets.begin(), sAllWidgets.end());
	// find parent in widget list
	weak_ptr<ofxWidget> tmpParent = p_;
	auto itParent = findIt(tmpParent, sAllWidgets.begin(), sAllWidgets.end());

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

	ofxWidget::bVisibleListDirty = true;
}

// ----------------------------------------------------------------------

void ofxWidget::updateVisibleWidgetsList() {

	if (!ofxWidget::bVisibleListDirty)
		return;

	//ofLogNotice() << "vList Update cache miss";

	// build a list of visible widgets based on all widgets
	// TODO: only rebuild this list if visiblility list on adding/removing and 
	// parenting widgets, and on changing widget visiblity, of course.
	sVisibleWidgets.clear();

	for (auto it = sAllWidgets.crbegin(); it != sAllWidgets.crend(); ++it) {
		if (auto p = it->lock()) {
			if (p->mVisible) {
				sVisibleWidgets.emplace_front(*it);
			} else {
				std::advance(it, p->mNumChildren);
			}
		}
	}
	ofxWidget::bVisibleListDirty = false;
}

// ----------------------------------------------------------------------

void ofxWidget::bringToFront(std::list<weak_ptr<ofxWidget>>::iterator it_)
{
	if (it_->expired())
		return;

	// ---------| invariant: element is valid

	auto element = it_->lock();

	// We're conservative with re-ordering.
	// Let's move the iterator backward to see if we are actually 
	// already sorted.
	// If the list were already sorted, then moving back from the current
	// iterator by the number of its children would bring us 
	// to the beginning of sAllWidgets. Then, there is no need to re-order.
	if (std::prev(it_, element->mNumChildren) == sAllWidgets.begin())
		return;

	// ----------| invariant: element (range) not yet at front.

	/*
	Algorithm:

	while current object range has a parent, put current object
	range to the front of parent range make parent range current
	object range.

	As soon as there is no parent anymore, put last object range
	to the front of the list

	Heuristic: parent iterator's position always to be found
	after current iterator.
	*/

	auto parent = element->mParent.lock();
	auto elementIt = it_;

	while (parent) {

		auto itParent = findIt(element->mParent, std::next(elementIt), sAllWidgets.end()); // start our search for parent after current element.

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

	ofxWidget::bVisibleListDirty = true;
}

// ----------------------------------------------------------------------

void ofxWidget::draw() {
	int zOrder = 0;
	updateVisibleWidgetsList();
	// note the reverse iterators: we are drawing back to front.
	for (auto it = sVisibleWidgets.crbegin(); it != sVisibleWidgets.crend(); ++it) {
		if (auto p = it->lock()) {
			if (p->onDraw) {
				p->onDraw(); // call the widget
				if (ofGetKeyPressed(OF_KEY_RIGHT_CONTROL)) {
					ofPushStyle();
					ofFill();
					ofSetColor(ofColor::red, 64);
					ofDrawRectangle(p->getRect());
					ofDrawBitmapStringHighlight(ofToString(zOrder), p->mRect.x, p->mRect.y + 10);
					ofPopStyle();
				}
				zOrder++;
			}
		}
	}
}

// ----------------------------------------------------------------------

void ofxWidget::update() {
	updateVisibleWidgetsList();
	// make sure to update last to first,
	// just to stay consistent with draw order.
	for (auto it = sVisibleWidgets.crbegin(); it != sVisibleWidgets.crend(); ++it) {
		if (auto p = it->lock()) {
			if (p->onUpdate)
				p->onUpdate(); // call the widget
		}
	}
}

// ----------------------------------------------------------------------
// static method - called once for all widgets by the
//                 WidgetEventResponder, which self-
//                 registers to all events upon creation of the first widget.
//
bool ofxWidget::mouseEvent(ofMouseEventArgs& args_) {
	// If we register a mouse down event, we do a hit test over
	// all visible widgets, and re-order if necessary.
	// Then, and in all other cases, we do a hit-test on the 
	// frontmost widget and, if positive, forward the event to this 
	// widget.

	updateVisibleWidgetsList();

	if (sVisibleWidgets.empty()) return false;

	// ---------| invariant: there are some widgets flying around.

	bool eventAttended = false;

	float mx = args_.x;
	float my = args_.y;

	// if we have a mouse down on a widget, we need to check which 
	// widget was hit and potentially re-order widgets.

	// find the first widget that is under the mouse, that is also visible
	// if it is not yet up front, bring it to the front.

	// hit-test only visible widgets - this makes sure to only evaluate 
	// the widgets which are visible, and whose parents are visible, too.
	auto itUnderMouse = std::find_if(sVisibleWidgets.begin(), sVisibleWidgets.end(), [&mx, &my](std::weak_ptr<ofxWidget>& w) ->bool {
		auto p = w.lock();
		if (p && p->mVisible && p->mRect.inside(mx, my)) {
			return true;
		} else {
			return false;
		}
	});

	// if we have a click, we want to make sure the widget gets to be the topmost widget.
	if (args_.type == ofMouseEventArgs::Pressed) {

		// --- now iterate over sAllWidgets instead of just the visible widgets.
		// we need to do this, because otherwise the reorder check won't be safe 
		// as the number of children in sVisibleWidgets is potentially incorrect,
		// as the number of children there refers to all children of a widget,
		// and not just the visible children of the widget.
		auto itPressedWidget = (itUnderMouse == sVisibleWidgets.end() ?
			sAllWidgets.end() :
			findIt(*itUnderMouse, sAllWidgets.begin(), sAllWidgets.end()));

		if (itPressedWidget != sAllWidgets.end()) {
			if (!isSame(*itPressedWidget, sFocusedWidget)) {
				// change in focus detected.
				// first, let the first element know that it is losing focus
				if (auto previousElementInFocus = sFocusedWidget.lock())
					if (previousElementInFocus->onFocusLost)
						previousElementInFocus->onFocusLost();

				sFocusedWidget = *itPressedWidget;

				// now that the new wiget is at the front, send an activate callback.
				if (auto nextFocusedWidget = sFocusedWidget.lock())
					if (nextFocusedWidget->onFocusReceived)
						nextFocusedWidget->onFocusReceived();
			}
			bringToFront(itPressedWidget); // reorder widgets
		} else {
			// hit test was not successful, no wigets found.
			if (auto previousElementInFocus = sFocusedWidget.lock())
				if (previousElementInFocus->onFocusLost)
					previousElementInFocus->onFocusLost();

			sFocusedWidget.reset(); // no widget gets the focus, then.
		}
	} // end if (args_.type == ofMouseEventArgs::Pressed)

	// now, we will attempt to send the mouse event to the widget that 
	// is in focus.

	if (itUnderMouse != sVisibleWidgets.end()) {
			// a widget is under the mouse.
			// is it the same as the current widget under the mouse?
		if (!isSame(*itUnderMouse, sWidgetUnderMouse)) {
			if (auto nU = itUnderMouse->lock())
			{
				// there is a new widget under the mouse
				if (auto w = sWidgetUnderMouse.lock()) {
					// there was an old widget under the mouse
					if (w->onMouseLeave)
						w->onMouseLeave();
					w->mHover = false;
				}
				if (nU->onMouseEnter)
					nU->onMouseEnter();
				nU->mHover = true;
				sWidgetUnderMouse = *itUnderMouse;
			}
		}
	} else {
		if (auto w = sWidgetUnderMouse.lock()) {
			// there was a widget under mouse,
			// but now there is none.
			if (w->onMouseLeave)
				w->onMouseLeave();
			w->mHover = false;
			sWidgetUnderMouse.reset();
		}
	}

	if (auto w = sFocusedWidget.lock()) {
		if (w->onMouse) {
			w->onMouse(args_);
			eventAttended = true;
		}
	}

	// store last mouse position last thing, so that 
	// we are able to calculate a difference.
	sLastMousePos.set(mx, my);
	return eventAttended;
}

// ----------------------------------------------------------------------
// static method - called once on the widget having the focus
bool ofxWidget::keyEvent(ofKeyEventArgs& args_) {

	if (sAllWidgets.empty()) return false;

	if (auto w = sFocusedWidget.lock()) {
		if (w->onKey)
			w->onKey(args_);
	}
	return false;
}

// ----------------------------------------------------------------------

void ofxWidget::setFocus(bool focus_) {

	if (focus_ == isActivated())
		return;

	// callback previous widget telling it that it 
	// loses focus
	if (auto previousElementInFocus = sFocusedWidget.lock())
		if (previousElementInFocus->onFocusLost)
			previousElementInFocus->onFocusLost();

	sFocusedWidget = mThis;

	// callback this widget telling it that it 
	// receives focus
	if (auto nextFocusedWidget = sFocusedWidget.lock())
		if (nextFocusedWidget->onFocusReceived)
			nextFocusedWidget->onFocusReceived();
}

// ----------------------------------------------------------------------

const bool ofxWidget::isAtFront() const {
	if (sAllWidgets.empty())
		return false;
	auto it = findIt(mThis, sAllWidgets.begin(), sAllWidgets.end());
	return (std::prev(it, mNumChildren) == sAllWidgets.begin());
}

// ----------------------------------------------------------------------

const bool ofxWidget::isActivated() const {
	return (sAllWidgets.empty()) ? false : isSame(mThis, sFocusedWidget);
}

// ----------------------------------------------------------------------

const bool ofxWidget::containsFocus() const {
	if (isSame(sFocusedWidget, mThis))
		return true;

	auto itThis = findIt(mThis, sAllWidgets.begin(), sAllWidgets.end());

	for (int i = 0; i != mNumChildren; ++i) {
		if (isSame(sFocusedWidget, *--itThis))
			return true;
	}
	return false;
}

// ----------------------------------------------------------------------

void ofxWidget::moveBy(const ofVec2f & delta_) {

	// find the iterator to the current widget
	auto it = findIt(mThis, sAllWidgets.begin(), sAllWidgets.end());
	// convert to a reverse iterator
	auto rIt = std::reverse_iterator<decltype(it)>(it);

	mRect.position += delta_;

	// all children (if any) are lined up *before* a parent in sAllWidgets.
	// we're using a reverse iterator so that we don't get a problem 
	// at the very physically first element.
	for (int i = 0; i < int(mNumChildren) && rIt != sAllWidgets.rend(); i++, rIt++) {
		if (auto w = rIt->lock()) {
			w->mRect.position += delta_;
		}
	}
}

// ----------------------------------------------------------------------

void ofxWidget::moveTo(const ofVec2f& pos_) {
	moveBy(pos_ - mRect.position);
}

// ----------------------------------------------------------------------
