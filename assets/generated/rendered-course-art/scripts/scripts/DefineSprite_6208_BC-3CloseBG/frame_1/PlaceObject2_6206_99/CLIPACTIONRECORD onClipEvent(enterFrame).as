onClipEvent(enterFrame){
   if(_root.Course.Char._x - _root.Course.BackGFX._x < -1600 && g !== true)
   {
      play();
      g = true;
      onEnterFrame = function()
      {
         if(_root.PauseGame == false)
         {
            play();
         }
         else
         {
            stop();
         }
      };
   }
}
