stop();
_alpha = 0;
count = 0;
wait = 32;
durations = 1000;
onEnterFrame = function()
{
   if(_parent.SwitchPL == true)
   {
      nextFrame();
   }
};
