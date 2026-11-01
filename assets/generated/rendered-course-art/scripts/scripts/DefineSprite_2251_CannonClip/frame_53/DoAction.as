_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("cannon3");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
stop();
savezoom = _root.CameraZoom;
_root.CameraZoom = 50;
