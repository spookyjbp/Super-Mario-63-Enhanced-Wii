stop();
_visible = true;
onEnterFrame = function()
{
   if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
   {
      _visible = true;
      if(this.hitTest(_root.Course.Char.charClip.body) || this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true))
      {
         play();
         _root.WaterAmount += 1500;
         onEnterFrame = null;
      }
   }
   else
   {
      _visible = false;
   }
};
