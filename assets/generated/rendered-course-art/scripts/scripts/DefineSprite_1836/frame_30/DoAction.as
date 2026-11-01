onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.DiveRot = frame._rotation + 90;
      water._rotation = frame._rotation;
      play();
      tellTarget(_root.Course.Char)
      {
         if(_root.KeyDOWN() && Water == false)
         {
            doublejump = 0;
            attack = true;
            attackFrame = "Dive";
            yspeed += 3;
            if(_xscale < 0)
            {
               xspeed -= (xspeed - _root.Divespeed) / 5;
            }
            else
            {
               xspeed -= (xspeed + _root.Divespeed) / 5;
            }
            if(random(2) == 1)
            {
               soundj = "Luigi - Yah!";
            }
            else
            {
               soundj = "Luigi - Yuh!";
            }
            _root.Mariosound = new Sound(this);
            _root.Mariosound.attachSound(soundj);
            _root.Mariosound.start(0,1);
            _root.Mariosound.setVolume(_root.MarioVolume);
         }
         if(_root.KeyZ() && (Water == false || _root.Metal == true))
         {
            attack = true;
            attackFrame = "Ground";
            xspeed = 0;
            yspeed = 0;
         }
         if(air == false && _root.Course.Char.yspeed > -3)
         {
            attack = false;
         }
      }
      if(_root.KeyRIGHT() && _root.Controll == true)
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airwalkspeed) / (_root.airmaxWalkspeed / 3),0) / 1.5;
      }
      if(_root.KeyLEFT() && _root.Controll == true)
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airwalkspeed) / (_root.airmaxWalkspeed / 3),0) / 1.5;
      }
      if(_root.KeyTapRIGHT())
      {
         _root.Course.Char.xspeed -= Math.min((_root.Course.Char.xspeed - _root.airtapspeed) / (_root.airmaxtapWalkspeed / 3),0);
      }
      if(_root.KeyTapLEFT())
      {
         _root.Course.Char.xspeed -= Math.max((_root.Course.Char.xspeed + _root.airtapspeed) / (_root.airmaxtapWalkspeed / 3),0);
      }
   }
   else
   {
      stop();
   }
};
