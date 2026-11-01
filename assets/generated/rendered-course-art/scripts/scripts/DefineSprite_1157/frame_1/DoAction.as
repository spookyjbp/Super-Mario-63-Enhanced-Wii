stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      _yscale = _yscale * -1;
      if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0)
      {
         nextFrame();
      }
      else
      {
         prevFrame();
      }
      _root.Fluddaniout = _currentframe;
   }
};
if(_root.Fluddaniout !== undefined && _root.KeySPACE())
{
   gotoAndStop(_root.Fluddaniout);
}
