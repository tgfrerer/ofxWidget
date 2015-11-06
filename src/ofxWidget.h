#pragma once
#include <functional>
#include <memory>
#include "ofEvents.h"
#include "ofRectangle.h"
/*

A widget is a component which gives its host superpowers.
Nawww. I exaggerate. It allows its host to respond to
mouse events, though. As if that wasn't enough.

A widget has an mRect, which is the screen-space canvas it 
is allowed to draw into. 

It also (optionally) holds function objects to

	+ draw
	+ respond to mouse events

which you can bind to from the host. With "host", I mean a 
class that has a widget, holding it with a pointer.

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

	ofxWidget();
	ofRectangle mRect; // widget rect on screen
	ofColor		mColor;

public:
	~ofxWidget();

	const ofRectangle& getRect()const {
		return mRect;
	};

	void setRect(const ofRectangle& rect_) {
		mRect = rect_;
	};

	std::function<void(ofMouseEventArgs&)> mMouseResponder; // this method be called on mouse event
	std::function<void(ofKeyEventArgs&)> mKeyResponder; // this method be called on mouse event
	std::function<void()> mDraw; // draw method for the widget.
	
	static void draw(); // draw widgets rect.

public: // factory function
	static shared_ptr<ofxWidget> make(const ofRectangle& rect_);

};