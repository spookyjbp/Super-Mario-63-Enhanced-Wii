if(_parent._parent.bounce == true)
{
   _root.Course.Char._y -= 5;
}
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("LargeGoombaDie");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
