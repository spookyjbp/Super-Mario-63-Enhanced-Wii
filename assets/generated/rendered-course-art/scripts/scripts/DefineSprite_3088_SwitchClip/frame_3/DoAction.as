stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.box.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
      {
         play();
         onEnterFrame = function()
         {
            _root.PauseGame = false;
            _root.Course.Char.yspeed = 0;
            _root.Course.Char.xspeed = 0;
            Y = _Y + _parent._y;
            _root.Course.Char._y += (Y - _root.Course.Char._y) / 5;
            X = _X + _parent._x;
            _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
         };
      }
   }
};
