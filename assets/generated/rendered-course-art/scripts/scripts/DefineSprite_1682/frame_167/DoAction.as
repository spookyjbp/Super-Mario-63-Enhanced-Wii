stop();
_root.DoneWithHighScores = false;
_root.attachMovie("SaveMessage","SaveMessage" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
onEnterFrame = function()
{
   _root.PauseGame = false;
   if(_root.DoneWithHighScores == true)
   {
      play();
      onEnterFrame = function()
      {
         _root.PauseGame = false;
      };
   }
};
