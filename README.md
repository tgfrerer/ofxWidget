# ofxWidget

A basic widget component that gives host classes superpowers
through composition.

Ok. I exaggerate. It allows its host to respond to
mouse and keyboard events, though. It also does z-ordering.
And draws the host objects in the correct order.
As if that wasn't enough.

A widget has an mRect, which is the screen-space canvas it 
is allowed to draw into. 

It also (optionally) holds function objects to:

  + draw
  + respond to mouse events
  + respond to keyboard events

which you can bind to from the host. With "host", I mean a 
class that has a widget, holding it with a pointer.

## Caveat

**IMPORTANT**: Once the host object has bound methods, IT MAY 
NOT MOVE ADDRESS (that is, `this` must remain constant). 

If the object is held by a container for example, any re-allocation 
or re-ordering will invalidate any bound function calls, and
lead to segfaults. YOU CAN MAKE SURE THIS DOESN'T HAPPEN BY
HOLDING YOUR OBJECTS USING SHARED_PTRS. THIS WILL GUARANTEE
THE DATA LOCALITY OF THE OBJECT.

ALSO: if the host element is held in a list, or a map, your're
fine, too. The idea is to make sure the underlying references 
and iterators cannot be invalidated, which is the case with
map and list.

See also: http://kera.name/articles/2011/06/iterator-invalidation-rules-c0x/

## How it works

Widgets automatically register for mouse events, and for keyboard
events. Only active widgets will receive event notification. This is 
optimised & very fast.

When the static `Wiget::draw()` method is called, all wigets
which are visible will get drawn, and drawn in the correct 
order, so that the topmost widget will cover any other 
widgets.

If a mouse event is registered, the list of widgets is walked
through, and the widget that is under the mouse will be called
with its (optional) `mouseEvent()`. 

If a keyboard event is registered, the widget currently holding
the focus will receive the keyboard event. Focus is given either
manually, or by retrieving the last clicked widget.

If the widget has no events bound, no events will get called.

Widgets can have exactly one parent, and unlimited children. 
Widget hierarchies are taken into account when deleting widgets, 
and when re-ordering widgets.

## Debug View

Press the right control key on your keyboard to see widget layers 
drawn, with z-order indices.

-------------------------------------------------------------

## INTERNAL WIDGET LIST STORE SYSTEM (`sAllWidgets`)

<pre>
| w0.1 | w0.2 | w0 | w1.0 | w1.1 | w1 | w | ...
\__________________|\_________________|	  |
\_________________________________________|
</pre>

Widgets are stored so that the front most widget is at the front,
all other widgets (including its parent (and its parent parent) are
behind. Widgets have a numChildren count, which is cumulative, so 
the sum total of all generations of children.

This storage method allows for optimisation when walking through 
widget hierarchies.

-------------------------------------------------------------

## DISCUSSION

Q: Can a widget be held by an unique_ptr?

A: No. The book-keeping for Widgets is internally done through
   `weak_ptr's`. There is no way a weak_ptr may reference a 
   unique_ptr, since `lock()` ing the weak_ptr would violate the 
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

------------------------------------------------------------

# MIT Licensed

<pre>
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

</pre>
