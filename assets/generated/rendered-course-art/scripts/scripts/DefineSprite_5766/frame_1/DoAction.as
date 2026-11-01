frame = 1;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_parent._parent.HP >= 3)
      {
         frame += 2;
      }
      if(_parent._parent.HP == 2)
      {
         frame += 3;
      }
      if(_parent._parent.HP <= 1)
      {
         frame += 5;
      }
      gotoAndStop(Math.round(frame) * 1 / 1);
      if(frame >= 70)
      {
         frame = 1;
      }
      if(frame <= 0)
      {
         frame = 70;
      }
   }
   else
   {
      stop();
   }
};
