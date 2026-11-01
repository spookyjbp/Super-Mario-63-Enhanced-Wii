stop();
count = 0;
onEnterFrame = function()
{
   if(Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600 || count++ > 960)
   {
      gotoAndPlay(1);
      onEnterFrame = null;
   }
};
