if(doortype == 4)
{
   doorsound = "door-close";
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound(doorsound);
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
}
