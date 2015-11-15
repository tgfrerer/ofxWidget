#pragma once
#include "ofxWidget.h"
#include <memory>
#include <functional>
#include "ofGraphics.h"
#include "ofColor.h"

class TestGuiElement {

	shared_ptr<ofxWidget> mWidget;
	ofColor				  mColor;
	ofRectangle			  mRect;
public:

	TestGuiElement(float x_, float y_, float w_, float h_ ) {
		mWidget = ofxWidget::make({x_, y_, w_, h_  });

		mRect = { x_, y_, w_, h_ };
		// you can assign your widget pointer here,
		// but don't assign any widget methods like setting mDraw or the mouse responder,
		// since the "this" object is not fully in place right now.
		// do these in setup, when the object is finished with initialising.
	};

	void setup() {

		// tell the widget that we want to draw though it
		// if you don't set it, the widget will not call draw

		// todo: careful with bind: if the underlaying object gets re-allocated,
		// "this" will not point to the correct object anymore!

		mWidget->mDraw = std::bind(&TestGuiElement::draw, this);
		
		mColor = ofFloatColor(ofRandomuf(), ofRandomuf(), ofRandomuf());

	}

	void setParent(TestGuiElement& parent_) {
		mWidget->setParent(parent_.mWidget);
	}

	void update() {};

	// this method will get called through the widget. 
	// you may still call it manually =)
	void draw() {
		
		ofPushStyle();
		ofSetColor(mColor);
		ofDrawRectangle(mWidget->getRect());
		ofPopStyle();
	
	};
};