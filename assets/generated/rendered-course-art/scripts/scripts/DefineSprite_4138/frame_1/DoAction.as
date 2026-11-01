frame = 1;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      frame += Math.round(Math.abs(_parent._parent.xspeed / 2)) + 3;
      if(frame >= 40)
      {
         frame -= 40;
      }
      gotoAndStop(frame);
   }
   else
   {
      stop();
   }
};
