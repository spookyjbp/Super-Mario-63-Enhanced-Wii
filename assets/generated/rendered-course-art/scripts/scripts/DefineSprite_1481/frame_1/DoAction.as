if(_root.PlayingChar == "Mario")
{
   gotoAndStop("M" + _root.Fluddpow);
}
if(_root.PlayingChar == "Luigi")
{
   gotoAndStop("L" + _root.Fluddpow);
}
onEnterFrame = function()
{
   if(_root.PlayingChar == "Mario")
   {
      gotoAndStop("M" + _root.Fluddpow);
   }
   if(_root.PlayingChar == "Luigi")
   {
      gotoAndStop("L" + _root.Fluddpow);
   }
};
