onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Char.yspeed > 0)
   {
      _parent.play();
      _root.Course.Char.yspeed = -6;
   }
}
