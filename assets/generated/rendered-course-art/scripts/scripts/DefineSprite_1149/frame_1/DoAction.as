frame = 1;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      frame += _root.Course.Char.xspeed;
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
