_root.warpframe = false;
if(_root.TotalStarCoins >= 5 || _root.CheatCastleUnlock == true)
{
   stop();
}
else
{
   gotoAndStop(2);
   stop();
}
onEnterFrame = function()
{
   if(SaveGame == true)
   {
      SaveGame = false;
      _root.DoneWithHighScores = false;
      _root.attachMovie("SaveMessage","SaveMessage" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "Idle";
      onEnterFrame = function()
      {
         if(_root.DoneWithHighScores == true)
         {
            _root.Course.Char.attack = false;
            onEnterFrame = null;
         }
      };
   }
};
