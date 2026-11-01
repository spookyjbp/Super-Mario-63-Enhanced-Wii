frame = 1;
onEnterFrame = function()
{
   var _loc3_;
   if(_root.PauseGame == false)
   {
      tellTarget(_root.Course.Char)
      {
         if(air == false)
         {
            if(_root.KeyRIGHT() && _xscale == -100)
            {
               if(Ice == true)
               {
                  xspeed += _root.walkspeed * 1.5 / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed += _root.walkspeed * 1.5 / _root.PuddleAccel;
               }
               else
               {
                  xspeed += _root.walkspeed * 1.5;
               }
            }
            if(_root.KeyLEFT() && _xscale == 100)
            {
               if(Ice == true)
               {
                  xspeed -= _root.walkspeed * 1.5 / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed -= _root.walkspeed * 1.5 / _root.PuddleAccel;
               }
               else
               {
                  xspeed -= _root.walkspeed * 1.5;
               }
               _xscale = 100;
            }
            if(_root.KeyRIGHT() && _xscale == 100 || _root.KeyLEFT() && _xscale == -100 || !_root.KeyRIGHT() && _xscale == -100 || !_root.KeyLEFT() && _xscale == 100 || _root.KeyUP() || _root.KeyDOWN() || _root.KeySPACE())
            {
               attack = false;
            }
         }
      }
      oldframe = frame;
      if(_root.Course.Char.air == true)
      {
         frame += Math.round(_root.Course.Char.xspeed / count) - 1.5 * _root.Course.Char._xscale / 100;
         count += 0.2;
         if(count > 3)
         {
            _root.Course.Char.attack = false;
         }
      }
      else
      {
         frame += Math.round(_root.Course.Char.xspeed * 0.7) - 1.5 * _root.Course.Char._xscale / 100;
         count = 1;
      }
      if(frame > 10 && frame <= 20 && !(oldframe > 10 && oldframe <= 20) || frame > 50 && frame <= 60 && !(oldframe > 50 && oldframe <= 60))
      {
         _loc3_ = new Sound(this);
         _loc3_.attachSound(_root.StepSound);
         _loc3_.start(0,1);
         _loc3_.setVolume(_root.MarioVolume);
         panX = _root.Course.Char._x;
         panX *= 1;
         panX = Math.min(100,panX);
         panX = Math.max(-100,panX);
         _loc3_.setPan(panX);
      }
      if(_root.Course.Char.Ice == true)
      {
         frame -= _root.Course.Char.xspeed / 2;
         if(_root.KeyRIGHT())
         {
            frame += 5;
         }
         if(_root.KeyLEFT())
         {
            frame -= 5;
         }
      }
      if(frame >= 80)
      {
         frame = 1;
      }
      if(frame <= 0)
      {
         frame = 80;
      }
      gotoAndStop(Math.round(frame) + 7);
   }
   else
   {
      stop();
   }
};
