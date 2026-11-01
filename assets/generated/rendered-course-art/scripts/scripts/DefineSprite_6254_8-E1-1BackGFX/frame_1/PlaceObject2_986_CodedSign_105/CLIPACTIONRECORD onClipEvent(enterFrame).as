onClipEvent(enterFrame){
   if(_root.PauseGame == false)
   {
      if(_root.TotalStars == 0 && !(_root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Respawn") && _root.Course.Char._x < _X + _root.Course.BackGFX._x + 5)
      {
         if(replacey == false)
         {
            _root.Course.Char._y = 60;
            replacey = true;
         }
         if(read == false)
         {
            if(_root.Course.Char.air == false && waiting < 60)
            {
               _root.Course.Char.yspeed = 0;
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = 1;
            }
            _root.Controll = false;
            if(_root.Course.Char.air == false && waiting++ > 60)
            {
               if(count++ > 12)
               {
                  onEnterFrame = function()
                  {
                     if(_root.PauseGame == false)
                     {
                        _root.Course.Char.attackFrame = "Sign";
                        _root.playpause = true;
                        X = _X + _root.Course.BackGFX._x;
                        _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
                        _root.Camcenter = this;
                     }
                  };
                  _root.Course.Char.yspeed = 0;
                  _root.Course.Char.xspeed = 0;
                  _root.Course.Char.attack = true;
                  _root.Course.Char.charClip.gotoAndStop("Sign");
                  _root.Course.Char.attackFrame = "Sign";
                  play();
                  read = true;
               }
               else
               {
                  _root.Course.Char.yspeed = 0;
                  _root.Course.Char.attack = true;
                  _root.Course.Char.attackFrame = 1;
                  _root.Course.Char.charClip.gotoAndStop(1);
                  _root.Course.Char._xscale = -100;
                  _root.Course.Char.xspeed += 3;
                  _root.Course.Char.xspeed *= 0.6;
               }
            }
            else if(_root.Course.Char.air == true)
            {
               _root.Course.Char.xspeed = 0;
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = 4;
               _root.Course.Char.charClip.gotoAndStop(4);
            }
         }
      }
   }
}
