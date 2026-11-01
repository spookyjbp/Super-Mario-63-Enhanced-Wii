_root.StopBGsong();
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Boo");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
_root.Course.BackGFX.gotoAndStop(3);
