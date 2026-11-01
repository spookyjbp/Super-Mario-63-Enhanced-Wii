stop();
onEnterFrame = function()
{
   if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
   {
      _visible = true;
      if(this.box.hitTest(_root.Course.Char.charClip.body))
      {
         _root["coindata" + _root.playingcourse][coinnum] = 1;
         play();
         _root.WaterHP += 3;
         _root.WaterHP = Math.min(_root.WaterHP,8);
         _root.SilverStars = _root.SilverStars + 1;
         cointext = _root.SilverStars;
         onEnterFrame = function()
         {
            if(_root.PauseGame == false)
            {
               play();
            }
            else
            {
               stop();
            }
         };
      }
      if(_root.PauseGame == false)
      {
         clip.play();
      }
      else
      {
         clip.stop();
      }
   }
   else
   {
      _visible = false;
   }
};
