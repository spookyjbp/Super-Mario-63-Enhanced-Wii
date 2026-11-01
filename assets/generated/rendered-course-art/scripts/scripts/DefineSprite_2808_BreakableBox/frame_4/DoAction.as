if(blocknum == 1)
{
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound("boxbreak");
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
}
else if(blocknum == 2)
{
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound("GlassShatter");
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
}
else if(blocknum == 3)
{
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound("GlassShatter");
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
   _root.Course.FrontGFX.attachMovie("InvertWave","InvertWave" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X,_y:_Y - 30});
}
else
{
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound("boxbreak");
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
}
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
