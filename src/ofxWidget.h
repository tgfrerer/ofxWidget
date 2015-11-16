#pragma once
#include <functional>
#include <memory>
#include "ofEvents.h"
#include "ofRectangle.h"
/*

A widget is a component which gives its host superpowers.
Nawww. I exaggerate. It allows its host to respond to
mouse and keyboard events, though. It also does z-ordering.
And draws the host objects in the correct order.
As if that wasn't enough.

A widget has an mRect, which is the screen-space canvas it 
is allowed to draw into. 

It also (optionally) holds function objects to

	+ draw
	+ respond to mouse events

which you can bind to from the host. With "host", I mean a 
class that has a widget, holding it with a pointer.

IMPORTANT: Once the host object has bound methods, IT MAY 
NOT MOVE ADDRESS ('this' must remain constant). If the object
is held by a container for example, any re-allocation or
re-ordering will invalidate any bound function calls, and
lead to segfaults. YOU CAN MAKE SURE THIS DOESN'T HAPPEN BY
HOLDING YOUR OBJECTS USING SHARED_PTRS. THIS WILL GUARANTEE
THE DATA LOCALITY OF THE OBJECT.

ALSO: if the host element is held in a list, or a map, your're
fine, too. The idea is to make sure the underlying references 
and iterators cannot be invalidated, which is the case with
map and list.

see also: http://kera.name/articles/2011/06/iterator-invalidation-rules-c0x/

The above is because the host needs to re-assign all bindings
everytime its data address changes - and the widget has no way
of knowing that the host address has changed.

TODO: the class that has a widget should really hold this 
widget using a unique_ptr<> since it may only hold a single
widget.

Widgets automatically register for mouse eventes

When the static Wiget::draw() method is called, all wigets
which are visible will get drawn, and drawn in the correct 
order, so that the topmost widget will cover any other 
widgets.

If a mouse event is registered, the list of widgets is walked
through, and the widget that is under the mouse will be called
with its (optional) mouseEvent(). 

If the widget has no events bound, no events will get called.


*/

class ofxWidget;

class WidgetEventResponder {

	/*
	
	This class is automatically registered 
	as the sole event responder, once and only once 
	the first widget is initialised.

	From this class the Widget static member function
	is called, which will re-distribute the event
	to the topmost Widget that has passed the hit test.

	*/
	void mouseEvent(ofMouseEventArgs& args);
	void keyEvent(ofKeyEventArgs& args);

public:
	WidgetEventResponder();
	~WidgetEventResponder();
};

class ofxWidget 
{
	// we keep track of all widget rectangles.
	// everytime a new widget is created or destroyed,
	// we update our widgetRects.
	friend class WidgetEventResponder;

	static void mouseEvent(ofMouseEventArgs& args);
	static void keyEvent(ofKeyEventArgs& args);
	static void bringToFront(std::list<weak_ptr<ofxWidget>>::iterator it_);
	
	static ofVec2f sLastMousePos;

	ofxWidget();
	ofRectangle mRect; // widget rect on screen

	bool mVisible = true; // layer visiblity

	size_t mNumChildren = 0;	 // number of children for this widget.
	std::weak_ptr<ofxWidget> mParent; // parent widget for this.
	std::weak_ptr<ofxWidget> mThis; // weak ptr to self

public:
	~ofxWidget();

	const ofRectangle& getRect()const {
		return mRect;
	};

	void setRect(const ofRectangle& rect_) {
		mRect = rect_;
	};

	void setVisibility(bool visible_) {
		mVisible = visible_;
	}

	const bool getVisibility() const {
		return mVisible;
	};

	std::function<void(ofMouseEventArgs&)> mMouseResponder; // this method be called on mouse event
	std::function<void(ofKeyEventArgs&)> mKeyResponder; // this method be called on mouse event
	std::function<void()> mUpdate; // update method for the widget.
	std::function<void()> mDraw;   // draw method for the widget.
	
	void setParent(std::shared_ptr<ofxWidget>& p_); // set a widget's parent, this will update the children list, by calling a method over all widgets.

	static void draw(); // draw widgets rect.
	static void update();

	bool isAtFront(); // returns whether this wiget is at the front (and receiving events)

public: // factory function
	static shared_ptr<ofxWidget> make(const ofRectangle& rect_);

};