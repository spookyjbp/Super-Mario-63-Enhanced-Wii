frame = 1;
onEnterFrame = function()
{
   var _loc3_;
   if(_root.PauseGame == false)
   {
      oldframe = frame;
      frame += _root.Course.Char.xspeed * 1.5;
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
         if(Key.isDown(39))
         {
            frame += 3;
         }
         if(Key.isDown(37))
         {
            frame -= 3;
         }
      }
      gotoAndStop(Math.round(frame) * 1 / 1);
      if(frame >= 80)
      {
         frame = 1;
      }
      if(frame <= 0)
      {
         frame = 80;
      }
   }
   else
   {
      stop();
   }
};
