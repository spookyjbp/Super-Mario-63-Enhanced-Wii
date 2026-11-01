onEnterFrame = function()
{
   if(_root.Course.Char._y - _Y > -250)
   {
      gotoAndStop(2);
      onEnterFrame = function()
      {
         _root.CamYchange = 20;
      };
   }
   else
   {
      gotoAndStop(1);
   }
};
