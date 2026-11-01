Y = 0;
count = 0;
onEnterFrame = function()
{
   if(_root.Course.BackGFX.Switched == "gone")
   {
      _Y = _Y + 160;
      onEnterFrame = null;
   }
   if(_root.Course.BackGFX.Switched == true)
   {
      count++;
      _root.playpause = true;
      if(Y < 120)
      {
         _root.Camcenter = this;
      }
      if(count > 32)
      {
         if(sounded !== true)
         {
            sounded = true;
            _root.Mariosound = new Sound(this);
            _root.Mariosound.attachSound("WaterGurgle");
            _root.Mariosound.start(0,1);
            _root.Mariosound.setVolume(_root.MarioVolume);
         }
         if(count % 2 == 0)
         {
            _X = _X + 2;
         }
         else
         {
            _X = _X - 2;
         }
         _Y = _Y + 3;
         Y += 3;
         if(Y > 160)
         {
            _root.playpause = false;
            onEnterFrame = null;
         }
      }
   }
};
