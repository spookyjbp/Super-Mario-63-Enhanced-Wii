_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i></b> and has kicked me out of his high-ranking circle! After I raised him from a baby, <b>this is how he treats me!?!?!</b>";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         _root.Camcenter = _parent.Target1;
         _root.PauseGame = false;
      };
      play();
   }
   lastframeZ = _root.KeyZ();
   _root.Camcenter = _parent.Target1;
   _root.PauseGame = false;
};
