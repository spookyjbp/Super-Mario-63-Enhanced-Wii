stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
      {
         _visible = true;
         if(bounce == true)
         {
            if(_root.PauseGame == false)
            {
               bouncefunction();
            }
         }
         if((this.hitTest(_root.Course.Char.charClip.body) || this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true)) && hitcoin == true)
         {
            _root.WaterHP = 8;
            _root.Course.Char.fallcount = 0;
            _root.hpcoincount += 15;
            _root.coincount += 5;
            _root.Coins += 5;
            _root.Power = Math.min(_root.Power + 50,100);
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
   }
};
