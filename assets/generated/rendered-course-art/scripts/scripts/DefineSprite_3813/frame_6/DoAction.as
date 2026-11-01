stop();
onEnterFrame = function()
{
   if(_parent._parent.yspeed > 2)
   {
      nextFrame();
      onEnterFrame = function()
      {
         if(_root.PauseGame == false)
         {
            play();
         }
         else
         {
            stop();
         }
      };
   }
};
