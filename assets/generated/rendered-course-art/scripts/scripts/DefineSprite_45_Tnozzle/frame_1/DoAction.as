yspeed = -3;
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _root.Fluddpow = "T";
         _root.WaterAmount = _root.TotalWater;
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("Fludd Nozzle");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
         this.removeMovieClip(this);
      }
      if(!_root.Course.Platforms.hitTest((_X + _parent._x) * _root.coursescale / 100 + _root.Course._x,(_Y + _parent._y + 7) * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest((_X + _parent._x) * _root.coursescale / 100 + _root.Course._x,(_Y + _parent._y + 7) * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y = _Y + yspeed;
         yspeed += 0.5;
         yspeed *= 0.95;
      }
      while(_root.Course.Platforms.hitTest((_X + _parent._x) * _root.coursescale / 100 + _root.Course._x,(_Y + _parent._y + 5) * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest((_X + _parent._x) * _root.coursescale / 100 + _root.Course._x,(_Y + _parent._y + 5) * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y--;
         yspeed = Math.min(0,yspeed);
      }
      count++;
      if(count > 900)
      {
         _alpha = random(100);
      }
      if(count > 1000)
      {
         this.removeMovieClip(this);
      }
   }
};
