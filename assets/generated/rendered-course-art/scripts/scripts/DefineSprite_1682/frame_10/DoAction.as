stop();
onEnterFrame = function()
{
   _root.PauseGame = false;
   if(_root.Course.Char.air == false)
   {
      _root.StartNewMusicAndIntroNoVar("InsidetheCastleWalls");
      play();
      onEnterFrame = function()
      {
         _root.PauseGame = false;
      };
   }
};
