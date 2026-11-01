if(_root.StarCoin[_name] == true)
{
   gotoAndStop(2);
}
else
{
   gotoAndStop(1);
}
if(_root.CheatStarToggle == true)
{
   onMouseDown = function()
   {
      if(Math.abs(_xmouse) < 10 && Math.abs(_ymouse) < 10)
      {
         _root.StarCoin[_name] = !_root.StarCoin[_name];
         _root.CalculateStarCoins();
         if(_root.StarCoin[_name] == true)
         {
            gotoAndStop(2);
         }
         else
         {
            gotoAndStop(1);
         }
      }
   };
}
