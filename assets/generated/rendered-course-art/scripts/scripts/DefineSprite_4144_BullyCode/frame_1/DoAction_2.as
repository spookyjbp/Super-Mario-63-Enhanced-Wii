Moveright = function()
{
   offground = true;
   if(offground == false)
   {
   }
   Xspot2 = 5;
   while(Xspot2 < 25)
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
               break;
            }
         }
         Yspot += adding += 0.5;
      }
      Xspot2 += 7;
   }
   if(offground == false || AiGround == false)
   {
      return true;
   }
   randomnum = 0;
   return false;
};
Moveleft = function()
{
   offground = true;
   if(offground == false)
   {
   }
   Xspot2 = 5;
   while(Xspot2 < 25)
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
               break;
            }
         }
         Yspot += adding += 0.5;
      }
      Xspot2 += 7;
   }
   if(offground == false || AiGround == false)
   {
      return true;
   }
   randomnum = 0;
   return false;
};
MakeSound = function()
{
   randomnum = random(2) + 1;
   _root.bgsoundFX = new Sound(this);
   _root.bgsoundFX.attachSound("thwomp-" + randomnum);
   _root.bgsoundFX.start(0,1);
   _root.bgsoundFX.setVolume(_root.BgVolume);
};
