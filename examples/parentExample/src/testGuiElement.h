#pragma once
#include "ofxWidget.h"
#include <memory>
#include <functional>
#include "ofGraphics.h"
#include "ofColor.h"
#include "ofEvents.h"

class TestGuiElement {

	shared_ptr<ofxWidget> mWidget;
	ofColor				  mColor;
	ofRectangle			  mRect;
public:

	TestGuiElement(float x_, float y_, float w_, float h_ ) {
		mWidget = ofxWidget::make({x_, y_, w_, h_  });

		mRect = { x_, y_, w_, h_ };
		// you can assign your widget pointer here,
		// but don't assign any widget methods like setting onDraw or the mouse responder,
		// since the "this" object is not fully in place right now.
		// do these in setup, when the object is finished with initialising.
	};

	void setup() {

		// tell the widget that we want to draw though it
		// if you don't set it, the widget will not call draw

		// todo: careful with bind: if the underlaying object gets re-allocated,
		// "this" will not point to the correct object anymore!

		mWidget->onDraw = std::bind(&TestGuiElement::draw, this);
		mWidget->onKey = std::bind(&TestGuiElement::keyResponder, this, std::placeholders::_1);

		mColor = ofFloatColor(ofRandomuf(), ofRandomuf(), ofRandomuf());

	}

	void setParent(TestGuiElement& parent_) {
		mWidget->setParent(parent_.mWidget);
	}

	void keyResponder(ofKeyEventArgs& args_) {
		// change color on key events
		if (args_.key == ' ') {
			mColor = ofFloatColor(ofRandomuf(), ofRandomuf(), ofRandomuf());
		}
		
		if (auto p = mWidget->getParent().lock()) {
			// Our widget has parent.
			// now we need to find out if it also has a method that responds to key presses.
			if (p->onKey) {
				p->onKey(args_); // forward this event.
			}
		}
	}

	void update() {};

	// this method will get called through the widget. 
	// you may still call it manually =)
	void draw() {
		
		ofPushStyle();
		ofSetColor(mColor);
		ofDrawRectangle(mWidget->getRect());
		
		if (mWidget->getHover()) {
			ofNoFill();
			ofSetLineWidth(2);
			ofSetColor(ofColor::black);
			ofDrawRectangle(mWidget->getRect());
		}
		ofPopStyle();
	
	};
};