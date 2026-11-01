stop();
currenthp = _root.CharHP;
onEnterFrame = function()
{
   if(_root.CharHP < 7)
   {
      gotoAndStop("hurtwater");
      play();
      onEnterFrame = null;
   }
   if(_root.WaterHP >= 8)
   {
      play();
      onEnterFrame = null;
   }
   currenthp = _root.CharHP;
};
