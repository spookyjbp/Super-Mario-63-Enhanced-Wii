if(_parent._parent.bounce == true)
{
   _root.Course.Char._y -= 5;
}
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("LargeGoombaDie");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
