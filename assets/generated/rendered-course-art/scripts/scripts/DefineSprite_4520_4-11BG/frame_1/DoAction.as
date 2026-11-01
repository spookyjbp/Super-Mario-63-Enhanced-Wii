m1.gotoAndStop(random(6) + 1);
m2.gotoAndStop(random(6) + 1);
onEnterFrame = function()
{
   if(_root.BGquality == "HIGH" || _root.BGquality == "MEDIUM")
   {
      gotoAndStop(1);
      m1._x += 3;
      m1._rotation += 3;
      if(m1._x >= 320)
      {
         m1._x = -330;
         m1.gotoAndStop(random(6) + 1);
      }
      m2._x -= 3;
      m1._rotation -= 3;
      if(m2._x <= -320)
      {
         m2._x = 330;
         m2.gotoAndStop(random(6) + 1);
      }
   }
   else
   {
      gotoAndStop(2);
   }
};
