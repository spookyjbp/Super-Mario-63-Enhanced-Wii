stop();
currenthp = _root.CharHP;
onEnterFrame = function()
{
   if(currenthp !== _root.CharHP || _root.CharHP < 8)
   {
      if(_root.CharHP < currenthp || _root.CharHP < 7)
      {
         play();
      }
      else
      {
         gotoAndStop("slower");
         play();
      }
      onEnterFrame = null;
   }
   if(_root.WaterHP !== 8)
   {
      gotoAndStop("water");
      play();
      onEnterFrame = null;
   }
   currenthp = _root.CharHP;
};
