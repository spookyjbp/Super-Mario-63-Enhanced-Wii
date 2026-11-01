stop();
onEnterFrame = function()
{
   if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 1000 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 1000)
   {
      _visible = true;
      if(this.box.hitTest(_root.Course.Char.charClip.body) || this.box.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true))
      {
         gotoAndStop("collect");
         play();
         _root.CharLives += 1;
         onEnterFrame = null;
      }
   }
   else
   {
      _visible = false;
   }
};
