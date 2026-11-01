_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i> ....I can settle our bitter rivalry once and for all!!!";
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
