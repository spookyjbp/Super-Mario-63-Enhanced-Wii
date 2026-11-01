stop();
_parent.inv = true;
_parent.yspeed = -15;
onEnterFrame = function()
{
   _parent.yspeed *= 0.98;
   if(_parent.action == "Fall")
   {
      nextFrame();
   }
};
