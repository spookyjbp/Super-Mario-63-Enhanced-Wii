frame = 1;
throwcount = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Course.FrontGFX.Bowser.attack = true;
      _root.Course.FrontGFX.Bowser.attackFrame = "spin";
      throwcount++;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _parent.speed = Math.min(_parent.speed,100);
      _parent.speed = Math.max(_parent.speed,1);
      frame += Math.round(_parent.speed / 10);
      while(frame >= 81)
      {
         frame -= 80;
      }
      gotoAndStop(frame);
      _parent.speed = _root.groundFriction(_parent.speed,0.1,1.01);
      _root.Course.FrontGFX.Bowser.attack = true;
      _root.Course.FrontGFX.Bowser.attackFrame = "spin";
      if(_root.KeySPACE() || Math.abs(_parent.speed) < 1 && throwcount > 32)
      {
         onEnterFrame = null;
         if(frame < 20 || frame > 70)
         {
            if(_root.Course.Char._xscale > 0)
            {
               _root.Course.Char._xscale = -100;
               _root.Course.FrontGFX.Bowser._xscale = 100;
               _root.Course.FrontGFX.Bowser.xspeed = - (_parent.speed / 12 + 3);
            }
            else
            {
               _root.Course.Char._xscale = 100;
               _root.Course.FrontGFX.Bowser._xscale = -100;
               _root.Course.FrontGFX.Bowser.xspeed = _parent.speed / 12 + 3;
            }
         }
         else if(_root.Course.Char._xscale > 0)
         {
            _root.Course.Char._xscale = 100;
            _root.Course.FrontGFX.Bowser._xscale = -100;
            _root.Course.FrontGFX.Bowser.xspeed = _parent.speed / 12 + 3;
         }
         else
         {
            _root.Course.Char._xscale = -100;
            _root.Course.FrontGFX.Bowser._xscale = 100;
            _root.Course.FrontGFX.Bowser.xspeed = - (_parent.speed / 12 + 3);
         }
         _root.Course.FrontGFX.Bowser.yspeed = - _parent.speed / 10 - 3;
         _root.Course.FrontGFX.Bowser.attack = true;
         _root.Course.FrontGFX.Bowser.attackFrame = "tossed";
         _root.Course.FrontGFX.Bowser._x = _root.Course.Char._x - _root.Course.FrontGFX._x - _root.Course.FrontGFX.Bowser._xscale / 100 * (_root.Course.FrontGFX.Bowser.wide / 1.5);
         _root.Course.FrontGFX.Bowser._y = _root.Course.Char._y - _root.Course.FrontGFX._y;
         gotoAndStop("end");
         play();
      }
   }
};
