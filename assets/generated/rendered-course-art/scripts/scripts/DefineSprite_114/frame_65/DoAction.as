stop();
currenthp = _root.CharHP;
onEnterFrame = function()
{
   if(_root.CharHP >= 8)
   {
      gotoAndStop("hpaway");
      play();
      onEnterFrame = null;
   }
   if(_root.WaterHP >= 8)
   {
      gotoAndStop("wateraway");
      play();
      onEnterFrame = null;
   }
   currenthp = _root.CharHP;
};
