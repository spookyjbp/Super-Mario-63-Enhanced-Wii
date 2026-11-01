stop();
if(firstrun == undefined)
{
   xspeed = _root.Course.Char.xspeed;
   yspeed = _root.Course.Char.yspeed;
   speed = Math.sqrt(xspeed * xspeed + yspeed * yspeed);
   frame = 1;
   firstrun = true;
}
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(_root.CameraZoom > 75)
      {
         _root.CameraZoom -= 5;
      }
      _root.Course.Char.xspeed = xspeed;
      _root.Course.Char.yspeed = yspeed;
      turnspeed = 1 / (speed / 1.5);
      turnspeed = Math.max(turnspeed,0.001);
      turnspeed = Math.min(turnspeed,1);
      if(_root.Course.Char._xscale < 0)
      {
         rot = Math.atan2(yspeed,xspeed) + 1.5707963267948966;
      }
      else
      {
         rot = Math.atan2(yspeed,- xspeed) + 1.5707963267948966;
      }
      if(_root.KeyUP())
      {
         rot += Math.max(rot * 0.92 - rot,- turnspeed);
      }
      if(_root.KeyDOWN())
      {
         rot += Math.min((rot - 3.141592653589793) * 0.92 + 3.141592653589793 - rot,turnspeed);
      }
      if(frame <= 3)
      {
         frame = 1;
         if(rot < 1.0471975511965976)
         {
            frame = 2;
         }
         if(rot > 2.0943951023931953)
         {
            frame = 3;
         }
      }
      if(_root.Course.Char._xscale < 0)
      {
         xspeed = Math.cos(rot - 1.5707963267948966) * speed;
      }
      else
      {
         xspeed = (- Math.cos(rot - 1.5707963267948966)) * speed;
      }
      yspeed = Math.sin(rot - 1.5707963267948966) * speed;
      if((rot < 0.4487989505128276 || rot > 2.827433388230814) && (_root.KeyLEFT() && _root.Course.Char._xscale < 0 || _root.KeyRIGHT() && _root.Course.Char._xscale > 0))
      {
         xspeed *= -0.9;
         _root.Course.Char._xscale *= -1;
         frame = 4;
      }
      yspeed += 0.2;
      xspeed *= 0.999;
      yspeed *= 0.999;
      speed = Math.sqrt(xspeed * xspeed + yspeed * yspeed);
      speed = Math.max(speed,0.3);
      _rotation = - _root.radiansToAngle(rot);
      gotoAndStop(frame);
      if(_root.Course.Char.air == false || _root.WingCap == false)
      {
         _root.Course.Char.attackFrame = "Dive";
      }
      if(_root.Course.Char.air == true && _root.KeyZ())
      {
         _root.Course.Char.attackFrame = "Ground";
      }
      if(_root.Course.Char.Water == true)
      {
         _root.Course.Char.attack = false;
      }
   }
};
