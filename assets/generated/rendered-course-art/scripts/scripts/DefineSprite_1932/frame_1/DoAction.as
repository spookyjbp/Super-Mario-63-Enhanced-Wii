_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Luigi - Cry");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
count = 0;
slowdowncount = 1;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char._xscale < 0)
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airtapspeed) / (_root.airmaxtapWalkspeed / 2),0) / 2;
      }
      else
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airtapspeed) / (_root.airmaxtapWalkspeed / 2),0) / 2;
      }
      if(_root.Course.Char.air == true)
      {
         if(_root.KeyRIGHT() && _root.Controll == true)
         {
            _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airwalkspeed) / _root.airmaxWalkspeed,0);
            _root.Course.Char._xscale = -100;
         }
         if(_root.KeyLEFT() && _root.Controll == true)
         {
            _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airwalkspeed) / _root.airmaxWalkspeed,0);
            _root.Course.Char._xscale = 100;
         }
         if(_root.KeyTapRIGHT() && !_root.KeyRIGHT() && _root.Controll == true)
         {
            _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airtapspeed) / _root.airmaxtapWalkspeed,0) / 2;
            _root.Course.Char._xscale = -100;
         }
         if(_root.KeyTapLEFT() && !_root.KeyLEFT() && _root.Controll == true)
         {
            _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airtapspeed) / _root.airmaxtapWalkspeed,0) / 2;
            _root.Course.Char._xscale = 100;
         }
      }
      if(_root.Course.Char.yspeed < 0 && _root.Course.Char.air == true)
      {
         if(_currentframe > 4)
         {
            gotoAndStop(4);
         }
         _root.Course.Char.yspeed -= _root.gravity / 2;
      }
      else if(_currentframe > 4 && _root.Course.Char.air == true)
      {
         gotoAndStop(5);
      }
      if(_currentframe < 4)
      {
         play();
      }
      if(_root.Course.Char.air == false && _root.Course.Char.yspeed > -3)
      {
         if(_currentframe < 6)
         {
            gotoAndPlay(6);
         }
         else
         {
            play();
         }
         if((_root.KeyRIGHT() || _root.KeyTapRIGHT()) && _root.Controll == true)
         {
            _root.Course.Char._xscale = -100;
         }
         if((_root.KeyLEFT() || _root.KeyTapLEFT()) && _root.Controll == true)
         {
            _root.Course.Char._xscale = 100;
         }
         if(_root.KeyUP() && _root.Controll == true)
         {
            _root.Course.Char.yspeed /= 2;
            _root.Course.Char.yspeed -= _root.jumpspeed / 1.5;
         }
         tellTarget(_root.Course.Char)
         {
            slowdowncount += 0.05;
            if(_xscale < 0)
            {
               if(Ice == true)
               {
                  xspeed += _root.walkspeed * 1.2 / this.slowdowncount / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed += _root.walkspeed * 1.2 / this.slowdowncount / _root.PuddleAccel;
               }
               else
               {
                  xspeed += _root.walkspeed * 1.2;
               }
            }
            else if(_xscale > 0)
            {
               if(Ice == true)
               {
                  xspeed -= _root.walkspeed * 1.2 / this.slowdowncount / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed -= _root.walkspeed * 1.2 / this.slowdowncount / _root.PuddleAccel;
               }
               else
               {
                  xspeed -= _root.walkspeed * 1.2 / this.slowdowncount;
               }
            }
         }
      }
      if(count++ > 112 || _root.Course.Char.Water == true || _root.Course.Char.Puddle == true)
      {
         _root.Course.Char.attack = false;
      }
   }
   else
   {
      stop();
   }
};
