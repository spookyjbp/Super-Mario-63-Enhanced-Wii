onEnterFrame = function()
{
   if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 1000)
   {
      _visible = true;
      if(_root.PauseGame == false)
      {
         if(_root.playpause == false && (this.hitTest(_root.Course.Char.charClip.body) || this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true)))
         {
            _root.WaterAmount += 50;
         }
      }
   }
   else
   {
      _visible = false;
   }
};
