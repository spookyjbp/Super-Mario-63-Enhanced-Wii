stop();
_root.bgsong.onSoundComplete = null;
_root.bgsong.stop();
_root.bgsong = new Sound(this);
_root.bgsong.attachSound("Melty Molten Galaxy - Intro");
_root.bgsong.start(0,1);
_root.bgsong.setVolume(100);
_root.bgsong.onSoundComplete = function()
{
   _root.bgsong.attachSound("Melty Molten Galaxy - Repeat");
   _root.bgsong.start(0,999);
   _root.bgsong.setVolume(100);
   _root.bgsong.onSoundComplete = null;
};
