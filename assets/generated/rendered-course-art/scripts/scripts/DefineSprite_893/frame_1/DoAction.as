stop();
onEnterFrame = function()
{
   if(_parent._parent._parent.movedir !== "down")
   {
      play();
      onEnterFrame = null;
   }
};
