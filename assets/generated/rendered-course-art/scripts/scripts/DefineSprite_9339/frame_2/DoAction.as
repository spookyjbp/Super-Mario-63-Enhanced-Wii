_root.bgsong.onSoundComplete = null;
_root.bgsong.stop();
_root.bgsong = new Sound(_root);
_root.bgsong.attachSound("TitleScreenTheme");
_root.bgsong.start(0,1);
_root.bgsong.setVolume(100);
starttime = 0;
endtime = _root.bgsong.duration;
frame = 0;
count = 0;
stop();
onEnterFrame = function()
{
   position = _root.bgsong.position - starttime;
   lastframe = frame;
   frame = (1 - (endtime - (starttime + position)) / (endtime - starttime)) * 1106;
   trace("Position " + position);
   trace("Frame " + frame);
   while(_currentframe < frame)
   {
      nextFrame();
   }
   if(lastframe >= frame || isNaN(frame))
   {
      count++;
      if(count > 32)
      {
         nextFrame();
      }
   }
   if(_root.bgsong.position >= endtime)
   {
      play();
      onEnterFrame = null;
   }
};
