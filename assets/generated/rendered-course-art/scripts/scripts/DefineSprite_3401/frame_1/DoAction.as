frame = 1;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      frame += Math.abs(_parent._parent.xspeed);
      while(frame > 20)
      {
         frame -= 20;
      }
      frame = Math.max(frame,1);
      gotoAndStop(Math.round(frame));
   }
   else
   {
      stop();
   }
};
