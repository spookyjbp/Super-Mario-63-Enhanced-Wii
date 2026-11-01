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
         _root.RedCoins = _root.RedCoins + 1;
         _root.WaterHP += 2;
         _root.WaterHP = Math.min(_root.WaterHP,8);
         cointext = _root.RedCoins;
         _root.Course.Char.fallcount = 0;
         _root.hpcoincount += 5;
         _root.coincount += 2;
         _root.Coins += 2;
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
