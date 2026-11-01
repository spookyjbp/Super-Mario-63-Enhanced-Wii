_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Shy-Guy");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
if(!(_root.Course.Char.landtime == 1 && _parent._parent.bounce == true))
{
   if(_root.KeyUP())
   {
      _root.Course.Char.landtime = 2;
   }
   else
   {
      nextFrame();
      play();
   }
}
