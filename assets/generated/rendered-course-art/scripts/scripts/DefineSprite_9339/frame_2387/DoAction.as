gotoAndStop("repeat");
play();
_root.bgsong.stop();
_root.bgsong = new Sound(_root);
_root.bgsong.attachSound("TitleScreenTheme");
_root.bgsong.start(2.58,1);
_root.bgsong.setVolume(100);
