onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _rotation = _rotation - rotspeed;
      rotspeed *= 0.8;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed *= 0.95;
      if(_root.Course.Char.yspeed > -1)
      {
         _root.Course.Char.yspeed *= 0.9;
      }
      else
      {
         _root.Course.Char.yspeed -= _root.gravity * 0.9;
      }
      _root.Course.FrontGFX.Bowser.attack = true;
      _root.Course.FrontGFX.Bowser.attackFrame = "spin";
      play();
   }
   else
   {
      stop();
   }
};
