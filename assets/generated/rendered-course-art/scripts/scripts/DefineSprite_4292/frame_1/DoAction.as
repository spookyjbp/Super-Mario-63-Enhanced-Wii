count = 0;
hit = false;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.box.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 7) * _root.coursescale / 100 + _root.Course._y,true) && _root.Metal !== true)
      {
         if(_root.Metal == false && _root.Invincible == false && _root.Invisible == false)
         {
            if(hit == false)
            {
            }
            hit = true;
         }
         _root.Coure.Char.xspeed *= 0.97;
         _root.Coure.Char.yspeed *= 0.95;
         _root.Course.Char.fallcount = 0;
         count++;
         if(count > 48)
         {
            _root.Mariosound = new Sound(this);
            _root.Mariosound.attachSound("sound - Drain");
            _root.Mariosound.start(0,1);
            _root.Mariosound.setVolume(_root.MarioVolume);
            _root.CharHP = _root.CharHP - 1;
            if(_root.CharHP <= 0)
            {
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = "Drown";
            }
            count = 0;
         }
      }
      else
      {
         if(_root.Metal == false && _root.Invincible == false && _root.Invisible == false)
         {
            if(hit == true)
            {
            }
            hit = false;
         }
         count = 0;
      }
   }
};
