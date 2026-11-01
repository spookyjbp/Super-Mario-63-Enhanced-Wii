stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      stop();
      _root.Course.Char.yspeed += _root.gravity / 2;
      if(_root.Course.Char.yspeed < 3)
      {
         _root.Course.Char.yspeed += _root.gravity / 2;
      }
      _root.Course.Char.xspeed *= 0.95;
      if(_root.KeyRIGHT() && _root.Controll == true)
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airwalkspeed) / _root.airmaxWalkspeed,0);
      }
      if(_root.KeyLEFT() && _root.Controll == true)
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airwalkspeed) / _root.airmaxWalkspeed,0);
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
      if(_root.Course.Char.Water == true && _root.Metal !== true || _root.Course.Char.yspeed < 1)
      {
         gotoAndStop("end");
         play();
      }
      if(_root.Course.Char.air == false)
      {
         gotoAndStop("hit");
         play();
      }
   }
   else
   {
      stop();
   }
};
