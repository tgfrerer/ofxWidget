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

Widgets automatically register for mouse eventes

When the static Wiget::draw() method is called, all wigets
which are visible will get drawn, and drawn in the correct 
order, so that the topmost widget will cover any other 
widgets.

If a mouse event is registered, the list of widgets is walked
through, and the widget that is under the mouse will be called
with its (optional) mouseEvent(). 

If the widget has no events bound, no events will get called.

-------------------------------------------------------------

INTERNAL WIDGET LIST STORE SYSTEM (`sAllWidgets`)

| w0.1 | w0.2 | w0 | w1.0 | w1.1 | w1 | w | ...
\__________________|\_________________|	  |
\_________________________________________|

Widgets are stored so that the front most widget is at the front,
all other widgets (including its parent (and its parent parent) are
behind. Widgets have a numChildren count, which is cumulative, so 
the sum total of all generations of children.

-------------------------------------------------------------

ISSUES

Q: Can a widget be held by an unique_ptr?

A: No. The book-keeping for Widgets is internally done through
   weak_ptr's. There is no way a weak_ptr may reference a 
   unique_ptr, since lock() ing the weak_ptr would violate the 
   invariant that unique_ptrs are unique.

Q: How do we deal with parent widgets? Should these receive 
   user input from their children? 

A: No. User input is routed exclusively to the widget in focus,
   Unless there is a "mouse down" event, which means that 
   the focus may move to another widget.
								 
   Children may, however, bubble user input to their parents.

Q: Do we want to accelerate the hit testing for widgets any 
   further?

A: At the moment hit testing only happens when we click, and 
   appears to be fast enough. If we wanted to send enter/exit
   events to wigets which have not yet received the focus, 
   we would have to do hit-testing for every position change.
   At this point it will become interesting to think about 
   spatial hashes, etc.

Q: Is there a way to stop events form being further processed?

A: Yes. If the eventResponder uses the bool return type overload,
   the return value controls whether an event will be notified 
   further. Currently, all events that are caught by any widget
   will not further be notified. This means, if you interact with
   a widget, the event will only be notified on the widget.

Q: Is there a way to bubble events from child widgets to their
   parents?

A: Yes. Look at parentExample. The idea is to grab the parent,
   And then call the parent's event responder with the current
   event arguments. It is for the parent to decide what to do 
   with the event.

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
	bool mouseEvent(ofMouseEventArgs& args);
	bool keyEvent(ofKeyEventArgs& args);

public:
	WidgetEventResponder();
	~WidgetEventResponder();
};

// -------------------------------------------------------------

class ofxWidget 
{
	// we keep track of all widget rectangles.
	// everytime a new widget is created or destroyed,
	// we update our widgetRects.
	friend class WidgetEventResponder;

	static bool mouseEvent(ofMouseEventArgs& args);
	static bool keyEvent(ofKeyEventArgs& args);
	static void bringToFront(std::list<weak_ptr<ofxWidget>>::iterator it_);
	static void updateVisibleWidgetsList();
	static bool bVisibleListDirty;

	static ofVec2f sLastMousePos;

	ofxWidget();
	ofRectangle mRect; // widget rect on screen

	bool mVisible = true; // layer visiblity
	bool mHover = false;  // mouse-over detected?

	size_t mNumChildren = 0;	 // number of children for this widget.
	std::weak_ptr<ofxWidget> mParent; // parent widget for this.
	std::weak_ptr<ofxWidget> mThis; // weak ptr to self

public:
	~ofxWidget();

	const ofRectangle& getRect()const {
		return mRect;
	};

	// set the rect for a widget. absolute coordinates.
	void setRect(const ofRectangle& rect_) {
		mRect = rect_;
	};

	void moveBy(const ofVec2f& delta_); //< Move this widget (and any children) by an offset
	void moveTo(const ofVec2f& pos_);

	void setVisibility(bool visible_) {
		if (visible_ != mVisible)
			bVisibleListDirty = true;
		mVisible = visible_;
	}

	const bool getVisibility() const {
		return mVisible;
	};

	const bool getHover() const {
		return mHover;
	}

	std::function<void(ofMouseEventArgs&)> onMouse; // this method be called on mouse event
	std::function<void(ofKeyEventArgs&)> onKey; // this method be called on mouse event
	
	std::function<void()> onActivate;			// called when this widget gets activated
	std::function<void()> onDeactivate;			// called when this widget gets deactivated
	std::function<void()> onMouseEnter;			// called when mouse enters this widget
	std::function<void()> onMouseLeave;			// called when mouse exits  this widget

	std::function<void()> onUpdate; // update method for the widget.	Only called on visible widgets.
	std::function<void()> onDraw;   // draw method for the widget. Only called on visible widgets.
	
	void setParent(std::shared_ptr<ofxWidget>& p_); // set a widget's parent, this will update the children list, by calling a method over all widgets.

	std::weak_ptr<ofxWidget>& getParent() {
		return mParent;
	};

	static void draw();			// draw all widgets
	static void update();		// update all widgets

public: // widget logic functions
	bool isAtFront(); //< returns whether this widget is at the front
	bool isActivated();  //< returns whether this widget has the focus
public: // factory function
	static shared_ptr<ofxWidget> make(const ofRectangle& rect_);

};