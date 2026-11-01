_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("ItemAppears");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
_root["coindata" + _root.playingcourse][coinnum] = 1;
