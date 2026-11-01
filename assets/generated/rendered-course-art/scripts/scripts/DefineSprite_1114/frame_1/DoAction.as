frame = 1;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      frame += Math.min(Math.round(Math.max(_parent.speed,5)),10);
      while(frame >= 41)
      {
         frame -= 40;
      }
      if(_parent.speed > 10)
      {
         gotoAndStop(frame);
      }
      else
      {
         gotoAndStop(frame + 40);
      }
      if(_root.KeyDOWN() && _root.Course.Char.Water == false && _root.Course.Char.air == true)
      {
         _parent.speed = Math.max(10,_parent.speed + 1);
         _root.Course.Char.yspeed += Math.sqrt((_parent.speed + 1) / 50) / 5;
      }
      if(_parent.speed > 7)
      {
         _parent.speed = _root.groundFriction(_parent.speed,0.5,1.1);
      }
      else
      {
         _parent.speed = _root.groundFriction(_parent.speed,0.1,1.03);
      }
      if(_root.Course.Char.Water == true)
      {
         _parent.speed = _root.groundFriction(_parent.speed,0.3,1.1);
      }
      _parent.speed = Math.max(_parent.speed,1);
      if(_parent.speed <= 5 && _root.Course.Char.air == false || _parent.speed <= 3 && _root.Course.Char.air == true)
      {
         if(_root.Course.Char.attackFrame == "Spin" && _root.Course.Char.attack == true)
         {
            _root.Course.Char.attack = false;
            _root.Course.Char.attackFrame = "";
         }
      }
      if(_root.Course.Char.air == true)
      {
         if(!_root.KeyDOWN())
         {
            if(_parent.speed > 7)
            {
               _root.Course.Char.yspeed = _root.groundFriction(_root.Course.Char.yspeed,0.3,1.05);
            }
            else
            {
               _root.Course.Char.yspeed = _root.groundFriction(_root.Course.Char.yspeed,0.1,1.03);
            }
            _root.Course.Char.yspeed -= Math.sqrt((_parent.speed + 1) / 30) / 3;
         }
         if(_root.Course.Char.Water == true)
         {
            _root.Course.Char.yspeed -= Math.sqrt((_parent.speed + 1) / 20) / 2;
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
