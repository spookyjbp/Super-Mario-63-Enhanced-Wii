stop();
onEnterFrame = function()
{
   camCenter._x = (_root.Course.FrontGFX.Bowser._x + (_root.Course.Char._x - _X) * 2) / 3;
   camCenter._y = (_root.Course.FrontGFX.Bowser._y + (_root.Course.Char._y - _Y) * 2) / 3;
   distance = Math.sqrt(Math.pow(_root.Course.FrontGFX.Bowser._x - (_root.Course.Char._x - _X),2) + Math.pow(_root.Course.FrontGFX.Bowser._y - (_root.Course.Char._y - _Y),2));
   if(_root.Course.FrontGFX.defeated == true)
   {
      play();
      onEnterFrame = null;
   }
};
