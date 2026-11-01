onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Mario._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Mario._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Mario.yspeed > 0)
   {
      _parent.play();
      _root.Course.Mario.yspeed = -6;
   }
}
