stop();
onEnterFrame = function()
{
   if(_parent._parent._parent.movedir !== "up")
   {
      play();
      onEnterFrame = null;
   }
};
