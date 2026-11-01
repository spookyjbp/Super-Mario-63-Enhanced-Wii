_root.bgsong.onSoundComplete = null;
_root.bgsong.stop();
_root.bgsong = new Sound(_root);
_root.bgsong.attachSound("PrincessPeachesCastle-Intro");
_root.bgsong.start(0,1);
_root.bgsong.setVolume(100);
_root.bgsong.onSoundComplete = function()
{
   _root.bgsong.attachSound("PrincessPeachesCastle-Repeat");
   _root.bgsong.start(0,999);
   _root.bgsong.setVolume(100);
   _root.bgsong.onSoundComplete = null;
};
_root.BGSpeed = 0.1;
Skip = function()
{
   gotoAndStop("scene3");
};
