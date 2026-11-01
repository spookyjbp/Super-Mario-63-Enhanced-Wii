stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char.yspeed > -7)
      {
         if(Math.abs(_root.Course.Char._rotation) > 3)
         {
            play();
         }
         else
         {
            _root.Course.Char.attack = false;
            onEnterFrame = null;
            gotoAndPlay(1);
         }
      }
   }
};
