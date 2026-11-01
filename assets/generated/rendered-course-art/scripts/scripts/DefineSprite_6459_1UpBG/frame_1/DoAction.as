onEnterFrame = function()
{
   if(_root.BGquality == "HIGH" || _root.BGquality == "MEDIUM")
   {
      gotoAndStop(1);
      m1._x += 3;
      if(m1._x >= 320)
      {
         m1._x = -330;
      }
      m2._x -= 3;
      if(m2._x <= -320)
      {
         m2._x = 330;
      }
   }
   else
   {
      gotoAndStop(2);
   }
};
