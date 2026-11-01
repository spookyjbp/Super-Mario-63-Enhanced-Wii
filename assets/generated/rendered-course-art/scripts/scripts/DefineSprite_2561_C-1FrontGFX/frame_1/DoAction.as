heightlevel = 0;
onEnterFrame = function()
{
   if(_root.Course.Char._y - _Y > -34)
   {
      heightlevel = 0;
   }
   if((_root.Course.Char._y - _Y < -57 || _root.Course.Char._y - _Y < -30 && _root.Course.Char.air == false) && heightlevel < 1)
   {
      heightlevel = 1;
   }
   if((_root.Course.Char._y - _Y < -82 || _root.Course.Char._y - _Y < -55 && _root.Course.Char.air == false) && heightlevel < 2)
   {
      heightlevel = 2;
   }
   if(heightlevel == 2 && _root.Course.Char._y - _Y > -50)
   {
      heightlevel = 1;
   }
   gotoAndStop(heightlevel + 1);
};
