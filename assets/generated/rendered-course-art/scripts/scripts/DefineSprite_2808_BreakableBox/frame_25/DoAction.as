stop();
onEnterFrame = function()
{
   if((Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.Course.Char._x - (_X + _parent._x)) > 600 || Math.abs(_root.Course.Char._y - (_Y + _parent._y)) > 600))
   {
      if(reappear == true)
      {
         gotoAndStop(3);
      }
   }
};
