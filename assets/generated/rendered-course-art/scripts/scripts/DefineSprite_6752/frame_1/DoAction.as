if(_root.BossCheckpoint !== true)
{
   distance = 0;
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         distance += 0.5;
         if(distance < 1400)
         {
            _X = _X + 0.5;
         }
      }
   };
}
else
{
   _X = _X + 1400;
}
