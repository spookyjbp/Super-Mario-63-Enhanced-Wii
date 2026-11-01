Moveright = function()
{
   offground = true;
   if(offground == false)
   {
   }
   Xspot2 = 15;
   while(Xspot2 < 30)
   {
      Xspot = Xspot2 + xspeed * 7;
      adding = 3;
      Yspot = -5;
      while(Yspot < 30)
      {
         adding = Math.min(adding,5);
         if(!_root.Course.Lava.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(_root.Course.Platforms.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               offground = false;
               break;
            }
            if(_root.Course.BPlatforms.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               offground = false;
               break;
            }
            if(_root.Course.Water.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               offground = true;
               break;
            }
         }
         Yspot += adding += 0.5;
      }
      Xspot2 += 10;
   }
   if(offground == false)
   {
      xspeed += walkspeed;
   }
   else
   {
      randomnum = 0;
      movedirection = "left";
      if(air == false)
      {
         frame = "Idle";
      }
   }
};
Moveleft = function()
{
   offground = true;
   if(offground == false)
   {
   }
   Xspot2 = 15;
   while(Xspot2 < 30)
   {
      Xspot = - Xspot2 + xspeed * 7;
      adding = 3;
      Yspot = -5;
      while(Yspot < 30)
      {
         adding = Math.min(adding,5);
         if(!_root.Course.Lava.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(_root.Course.Platforms.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               offground = false;
               break;
            }
            if(_root.Course.BPlatforms.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               offground = false;
               break;
            }
            if(_root.Course.Water.hitTest((_X + Xspot) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + Yspot) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               offground = true;
               break;
            }
         }
         Yspot += adding += 0.5;
      }
      Xspot2 += 10;
   }
   if(offground == false)
   {
      xspeed -= walkspeed;
   }
   else
   {
      randomnum = 0;
      movedirection = "right";
      if(air == false && !frame == "tumble" && !frame == "Squish")
      {
         frame = "Idle";
      }
   }
};
