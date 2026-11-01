onEnterFrame = function()
{
   frame = _root.hpcoincount + 1;
   if(_root.CharHP == 8)
   {
      frame = 6;
   }
   gotoAndStop(frame);
   if(_root.hpcoinplay == true)
   {
      animation.play();
      _root.hpcoinplay = false;
   }
};
