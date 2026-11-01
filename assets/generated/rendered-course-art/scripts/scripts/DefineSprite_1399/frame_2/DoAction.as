stop();
_root.GetStarMusic2();
othertime = 0;
onEnterFrame = function()
{
   othertime++;
   _root.PauseGame = false;
   timeinsong = _root.bgsong.position / 1000;
   if(timeinsong > 2.2 || othertime > 96)
   {
      play();
   }
};
