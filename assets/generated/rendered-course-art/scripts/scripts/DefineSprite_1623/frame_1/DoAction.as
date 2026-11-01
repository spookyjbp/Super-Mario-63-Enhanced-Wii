stop();
_root.CameraZoom = 100;
_root.GetKeyMusic();
count = 0;
othertime = 0;
onEnterFrame = function()
{
   _root.PauseGame = false;
   timeinsong = _root.bgsong.position / 1000;
   othertime++;
   if(timeinsong > 1.3 || othertime > 92)
   {
      _root.nullCam = false;
      _root.Camspeed = 1;
      play();
   }
};
