onEnterFrame = function()
{
   gotoAndStop(_parent.fra);
   if(_parent.chase == true)
   {
      clip.play();
   }
   else
   {
      clip.gotoAndStop(1);
   }
};
