onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char.blink <= 0)
         {
            if(_parent._parent._xscale < 0)
            {
               _root.MarioHit(1,90,0,3,20,"none");
            }
            else
            {
               _root.MarioHit(1,90,0,-3,20,"none");
            }
            if(_root.CharHP >= 0)
            {
               _root.Course.Char.yspeed = Math.min(-16,_root.Course.Char.yspeed);
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = "Lava";
            }
            else
            {
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = "Faint";
            }
         }
      }
      play();
   }
   else
   {
      stop();
   }
};
