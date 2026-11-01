stop();
onEnterFrame = function()
{
   distance = Math.abs(_X + _parent._x - _root.Course.Char._x);
   distance = Math.max(distance,0);
   distance = Math.min(distance,100);
   if(_root.Course.Char._x > _X + _parent._x)
   {
      _xscale = -100;
   }
   else
   {
      _xscale = 100;
   }
   if(distance < 100)
   {
      play();
      if(this.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char.blink <= 0)
         {
            if(_root.Course.Char._x > _X + _parent._x)
            {
               _root.MarioHit(2,90,0,3,20,"none");
            }
            else
            {
               _root.MarioHit(2,90,0,-3,20,"none");
            }
            if(_root.CharHP >= 0)
            {
               _root.Course.Char.yspeed = Math.min(-16,_root.Course.Char.yspeed);
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = "Lava";
            }
         }
      }
   }
};
