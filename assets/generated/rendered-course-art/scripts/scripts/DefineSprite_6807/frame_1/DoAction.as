HitTest = function(ground, X, Y)
{
   if(_root.Course[ground].hitTest(X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
   {
      return true;
   }
   return false;
};
yspeed = 0;
maxY = _Y;
onEnterFrame = function()
{
   yspeed *= 0.7;
   if(HitTest("Water",_X,_Y))
   {
      count = 0;
      i = 0;
      while(i < 7)
      {
         if(HitTest("Water",_X,_Y - i * 2))
         {
            count++;
         }
         i++;
      }
      yspeed -= count / 10;
   }
   else
   {
      yspeed += 1;
   }
   _Y = _Y + yspeed;
   _Y = Math.min(_Y,maxY);
   _root.Course.BackGFX.cannon2._y = _Y;
   _root.Course.Platforms.cannon._y = _Y;
};
