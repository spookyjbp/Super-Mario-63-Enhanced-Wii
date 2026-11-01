prevFrame();
if(_root.Course.BackGFX._currentframe >= 6)
{
   stop();
   onEnterFrame = null;
}
else
{
   if(_root.playpause == false)
   {
      count++;
   }
   if(count > waittime)
   {
      gotoAndStop("restart");
      play();
   }
}
