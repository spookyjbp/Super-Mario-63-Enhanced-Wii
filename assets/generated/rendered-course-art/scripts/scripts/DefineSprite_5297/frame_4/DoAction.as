onEnterFrame = function()
{
   X = _X + _parent._x;
   _root.Course.Char._x += (X - _root.Course.Char._x) / 10;
};
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("Door - Creak");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
