_root.Course.FrontGFX.attachMovie("GroundShockwave","Shockwave" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_parent._parent._x + 10,_y:_parent._parent._y,_xscale:100});
_root.Course.FrontGFX.attachMovie("GroundShockwave","Shockwave" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_parent._parent._x - 10,_y:_parent._parent._y,_xscale:-100});
_root.Course.FrontGFX.attachMovie("GroundShockwaveFGFX","GroundShockwaveFGFX" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_parent._parent._x,_y:_parent._parent._y});
_root.Course.BackGFX.attachMovie("GroundShockwaveBGFX","GroundShockwaveBGFX" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_parent._parent._x,_y:_parent._parent._y});
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("BowserStep");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
