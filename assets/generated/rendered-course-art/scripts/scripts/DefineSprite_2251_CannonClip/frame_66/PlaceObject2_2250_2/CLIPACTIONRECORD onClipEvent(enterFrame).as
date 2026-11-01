onClipEvent(enterFrame){
   _parent.rotdirection = _root.groundFriction(_parent.rotdirection,3,1.2);
   _rotation = _parent.rotdirection;
}
