stop();
if(_root.L5VolErupt == true)
{
   _root.L5VolStar = false;
   play();
}
else
{
   _root.L5VolErupt = false;
   if(_root.L5VolStar == true)
   {
      gotoAndStop("End");
   }
   else
   {
      stop();
   }
}
