stop();
onEnterFrame = null;
_root.bgsong.stop();
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Mario-Fall");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
_root.Course.FrontGFX.attachMovie("MarioDeathGFX","MarioDeathGFX" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_root.Course.Char._x - _root.Course.FrontGFX._x,_y:_root.Course.Char._y - _root.Course.FrontGFX._y});
