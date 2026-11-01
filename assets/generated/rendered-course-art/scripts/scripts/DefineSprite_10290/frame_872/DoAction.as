_root.bgsong.onSoundComplete = null;
_root.bgsong.stop();
_root.bgsong = new Sound(_root);
_root.bgsong.attachSound("Airship Theme Repeat");
_root.bgsong.start(0,1);
_root.bgsong.setVolume(100);
_root.bgsong.onSoundComplete = function()
{
   _root.bgsong.attachSound("Airship Theme Repeat");
   _root.bgsong.start(0,999);
   _root.bgsong.setVolume(100);
   _root.bgsong.onSoundComplete = null;
};
