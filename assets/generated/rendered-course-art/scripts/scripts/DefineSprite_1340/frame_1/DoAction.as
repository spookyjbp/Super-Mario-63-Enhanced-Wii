stop();
gotoAndStop(_parent.chargeframe);
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(_root.waterblast == true)
      {
         nextFrame();
         this.attachMovie("ChargeLine","ChargeLine" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0});
      }
      else
      {
         prevFrame();
      }
      _parent.chargeframe = _currentframe;
   }
   else
   {
      stop();
   }
};
