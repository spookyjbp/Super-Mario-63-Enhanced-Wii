stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(graphic == false)
      {
         if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
         {
            _visible = true;
            if(this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Metal !== true && _root.Invincible !== true || _root.PowerTimer <= 0))
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
                  else
                  {
                     _root.Course.Char.attack = true;
                     _root.Course.Char.attackFrame = "Faint";
                  }
               }
            }
         }
         else
         {
            _visible = false;
         }
      }
      if(OnWait !== "endless")
      {
         count++;
         if(count > OnWait)
         {
            play();
            onEnterFrame = function()
            {
               if(_root.PauseGame == false)
               {
                  play();
               }
               else
               {
                  stop();
               }
            };
         }
      }
   }
};
