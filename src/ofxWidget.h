#pragma once
#include <functional>
#include <memory>
#include "ofEvents.h"
#include "ofRectangle.h"
/*
   _____    ___
  /    /   /  /     ofxWidget
 /  __/ * /  /__    (c) ponies & light ltd., 2015-2016.
/__/     /_____/    poniesandlight.co.uk

ofxWidget
Created by @tgfrerer 2015.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/
// ----------------------------------------------------------------------

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
	ofRectangle mRect;					// widget rect on screen

	bool mVisible = true;				// layer visiblity
	bool mHover = false;				// mouse-over detected?

	size_t mNumChildren = 0;			// number of children for this widget.
	std::weak_ptr<ofxWidget> mParent;	// parent widget for this.
	std::weak_ptr<ofxWidget> mThis;		// weak ptr to self

public:
	~ofxWidget();

	void setRect(const ofRectangle& rect_); //< set the widget rect in absolute coordinates.
	const ofRectangle& getRect() const;	    //< return the widget's rect in absolute coordinates.

	void moveBy(const ofVec2f& delta_); //< Move this widget (and any children) by an offset
	void moveTo(const ofVec2f& pos_);   //< Move this widget (and any children) to an absolute position

	void setVisibility(bool visible_);	//< Set this widget's visibility. Children of invisible widgets will not be drawn nor updated.
	const bool getVisibility() const ;  //< Get this widget's visibility

	const bool getHover() const;		//< Return whether the mouse is currently over this widget

	std::function<void(ofMouseEventArgs&)> onMouse; //< Mouse event callback
	std::function<void(ofKeyEventArgs&)> onKey;		//< Keyboard event callback
	
	std::function<void()> onFocusEnter;	//< Activation callback	(when widget receives focus)
	std::function<void()> onFocusLeave;	//< Deactivation callback (when widget loses focus)
	std::function<void()> onMouseEnter;	//< Mouse enter callback
	std::function<void()> onMouseLeave;	//< Mouse exit callback

	std::function<void()> onUpdate; //<Once-per frame update callback for widget. Only called if widget is visible.	Update callbacks will be issued based on z-order, back to front.
	std::function<void()> onDraw;   //<Once-per frame draw callback for widget. Only called if widget is visible. Draw callbacks will be issued over based on z-order, back to front.
	
	void setParent(std::shared_ptr<ofxWidget>& p_); //< set a widget's parent, this will update the children list, by calling a method over all widgets.
	std::weak_ptr<ofxWidget>& getParent();

	static void update();		//< Trigger update callbacks for all widgets. The callbacks will be issued in the correct z-order, back to front.
	static void draw();			//< Trigger draw callbacks for all widgets. The callbacks will be issued in the correct z-order, back to front.
	

public: // widget logic functions

	void setFocus(bool focus_);	 // manually give widget focus

	const bool isAtFront() const;		//< returns whether this widget as far to the front as possible
	const bool isActivated() const;		//< returns whether this widget has the focus
	const bool containsFocus() const;	//< returns whether this widget or one of its children the focus
public: // factory function
	static shared_ptr<ofxWidget> make(const ofRectangle& rect_);
};

// ----------------------------------------------------------------------
 
inline std::weak_ptr<ofxWidget>& ofxWidget::getParent() {
		return mParent;
};

inline const bool ofxWidget::getHover() const {
	return mHover;
}

inline const bool ofxWidget::getVisibility() const {
	return mVisible;
};

inline void ofxWidget::setVisibility(bool visible_) {
	if (visible_ != mVisible)
		bVisibleListDirty = true;
	mVisible = visible_;
};

inline void ofxWidget::setRect(const ofRectangle& rect_) {
	mRect = rect_;
}

inline const ofRectangle& ofxWidget::getRect() const {
	return mRect;
};

// ----------------------------------------------------------------------
