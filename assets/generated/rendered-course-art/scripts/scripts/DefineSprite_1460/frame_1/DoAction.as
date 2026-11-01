_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("mario-fire");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
count = 0;
groundcount = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Course.Char.hurtfallcount = 0;
      play();
      if(_root.Course.Char._xscale < 0)
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airtapspeed) / (_root.airmaxtapWalkspeed / 2),0) / 2;
      }
      else
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airtapspeed) / (_root.airmaxtapWalkspeed / 2),0) / 2;
      }
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
      if(_root.Course.Char.yspeed < 0 && groundcount <= 0 && _root.Course.Char.yspeed < 0)
      {
         _root.Course.Char.yspeed -= _root.gravity / 2;
      }
      else if(groundcount < 2 && _root.Course.Char.yspeed < 0)
      {
         _root.Course.Char.yspeed -= _root.gravity / 3;
      }
      if(_root.Course.Char.air == false && _root.Course.Char.yspeed > -3)
      {
         groundcount++;
         if(_root.Course.Char._xscale < 0)
         {
            _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airwalkspeed) / (_root.airmaxtapWalkspeed * 3),0) * 4;
         }
         else
         {
            _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airwalkspeed) / (_root.airmaxtapWalkspeed * 3),0) * 4;
         }
         _root.Course.Char.yspeed /= 3;
         _root.Course.Char.yspeed -= 7 - groundcount * 1.5;
      }
      if(count++ > 300 || groundcount >= 2 && _root.Course.Char.yspeed > -3 || _root.Course.Char.Water == true)
      {
         _root.Course.Char.attack = false;
      }
   }
   else
   {
      stop();
   }
};
