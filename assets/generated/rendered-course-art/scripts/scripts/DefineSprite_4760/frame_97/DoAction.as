onEnterFrame = function()
{
   Volcano();
   _root.PauseGame = false;
   _root.Controll = false;
   if(_root.Course.Char.air == false)
   {
      _root.L5VolStar = true;
      _root.L5VolErupt = false;
      onEnterFrame = function()
      {
         Volcano();
      };
   }
};
