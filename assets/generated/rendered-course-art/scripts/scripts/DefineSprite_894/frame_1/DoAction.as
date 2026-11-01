stop();
onEnterFrame = function()
{
   if(_parent._parent._parent.movedir !== "none")
   {
      play();
      onEnterFrame = null;
   }
};
