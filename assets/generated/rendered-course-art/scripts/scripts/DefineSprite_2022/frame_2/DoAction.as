stop();
_root.CameraZoom = 100;
onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.ZoomoutCam();
   timeinsong = _root.bgsong.position / 1000;
   lengthofsong = _root.bgsong.duration / 1000;
   if(timeinsong > lengthofsong - 0.1)
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
