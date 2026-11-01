stop();
othertime = 0;
onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.ZoomoutCam();
   timeinsong = _root.bgsong.position / 1000;
   lengthofsong = _root.bgsong.duration / 1000;
   othertime++;
   if(timeinsong > lengthofsong - 0.5 || othertime > 276)
   {
      _root.Blackout();
      play();
      onEnterFrame = function()
      {
         _root.PauseGame = false;
         _root.ZoomoutCam();
      };
   }
};
