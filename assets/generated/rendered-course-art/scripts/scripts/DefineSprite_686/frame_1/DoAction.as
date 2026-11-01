if(shine == true)
{
   gotoAndStop(4);
}
else if(_root.Star[_name] == true)
{
   gotoAndStop(3);
}
else
{
   gotoAndStop(2);
}
if(_root.CheatStarToggle == true)
{
   onMouseDown = function()
   {
      if(Math.abs(_xmouse) < 10 && Math.abs(_ymouse) < 10)
      {
         _root.Star[_name] = !_root.Star[_name];
         _root.CalculateStars();
         if(_root.Star[_name] == true)
         {
            gotoAndStop(3);
         }
         else
         {
            gotoAndStop(2);
         }
      }
   };
}
