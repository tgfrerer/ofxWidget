#pragma once
#include "ofxWidget.h"
#include <memory>
#include <functional>
#include "ofGraphics.h"
#include "ofColor.h"

class TestGuiElement {

	shared_ptr<ofxWidget> mWidget;
	ofColor				  mColor;
public:

	TestGuiElement() {
		mWidget = std::move(ofxWidget::makeWidget({ (rand() % 10) * 10 + 10.f, (rand() % 10) * 10 + 10.f, 200.f, 200.f }));

		// you can assign your widget pointer here,
		// but don't assign any widget methods like setting mDraw or the mouse responder,
		// since the "this" object is not fully in place right now.
		// do these in setup, when the object is finished with initialising.
	};

	void setup() {

		// tell the widget that we want to draw though it
		// if you don't set it, the widget will not call draw
		mWidget->mDraw = std::bind(&TestGuiElement::draw, this);
		mColor = ofFloatColor(ofRandomuf(), ofRandomuf(), ofRandomuf());

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