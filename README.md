# ofxWidget

A basic widget component that gives host classes superpowers
through composition.

Widget:

+ allows to create parent-child hierarchies for widget draw
  rects.
+ sorts draw/update calls based on widget z-ordering. 
+ re-orders widgets upon mouse click
+ preserves widget hierarchies when re-ordering
+ forwards mouse interactions to the frontmost widget
+ forwards keyboard interactions to the focused widget

If a class has a widget component, this component may
control the way the class is drawn. ofxWidget will call draw
on all widgets in an app, based on their z-order and
visibility. 

By forwarding a widget component's draw call to the host
class draw method, the class is drawn in the correct
z-order.

