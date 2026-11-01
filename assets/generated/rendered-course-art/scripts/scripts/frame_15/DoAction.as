stop();
if(_root.LDMenuFrame == "nosong")
{
   _root.LDMenuFrame = 1;
}
else
{
   _root.SongIntro = "FileSelectSong-Intro";
   _root.SongRepeat = "FileSelectSong-Repeat";
   if(_root.MuteBGMusic != true)
   {
      _root.bgsong.stop();
      _root.bgsong = new Sound(this);
      _root.bgsong.attachSound("FileSelectSong-Intro");
      _root.bgsong.start(0,1);
      _root.bgsong.setVolume(_root.BgVolume);
      _root.bgsong.onSoundComplete = function()
      {
         _root.bgsong.attachSound("FileSelectSong-Repeat");
         _root.bgsong.start(0,999);
         _root.bgsong.setVolume(_root.BgVolume);
      };
   }
}
