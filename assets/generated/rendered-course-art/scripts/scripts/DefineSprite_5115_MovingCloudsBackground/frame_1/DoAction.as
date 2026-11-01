X = 0;
onEnterFrame = function()
{
   X--;
   bg1._x = X;
   bg2._x = X + 450;
   if(X <= -450)
   {
      X += 450;
   }
};
