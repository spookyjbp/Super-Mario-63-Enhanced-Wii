stop();
_alpha = 0;
count = 0;
wait = 64;
duration = 640;
onEnterFrame = function()
{
   if(_parent.SwitchPL == true)
   {
      nextFrame();
   }
};
