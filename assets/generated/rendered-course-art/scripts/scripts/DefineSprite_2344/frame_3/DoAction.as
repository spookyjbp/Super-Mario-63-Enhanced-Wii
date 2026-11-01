stop();
onEnterFrame = function()
{
   if(_parent._parent.yspeed > 0)
   {
      nextFrame();
      onEnterFrame = null;
   }
};
