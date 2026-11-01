stop();
count = 0;
currenthp = _root.CharHP;
onEnterFrame = function()
{
   if(currenthp == _root.CharHP && _root.CharHP == 8)
   {
      count++;
   }
   else
   {
      currenthp = _root.CharHP;
      count = 0;
   }
   if(_root.WaterHP <= 7)
   {
      gotoAndStop("hurtwater");
      play();
      onEnterFrame = null;
   }
   if(count > 160)
   {
      play();
      onEnterFrame = null;
   }
};
if(_root.WaterHP <= 7 && _root.CharHP >= 8)
{
   gotoAndStop(45);
}
if(_root.WaterHP <= 7 && _root.CharHP <= 7)
{
   gotoAndStop("both");
}
