_root.StartCreditsCourse("1-1",0,0,0,0,100);
this.swapDepths(_root.getNextHighestDepth());
starttime = _root.bgsong.position;
endtime = _root.bgsong.duration;
frame = 0;
count = 0;
stop();
onEnterFrame = function()
{
   position = _root.bgsong.position - starttime;
   lastframe = frame;
   frame = (1 - (endtime - (starttime + position)) / (endtime - starttime)) * 4562;
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
