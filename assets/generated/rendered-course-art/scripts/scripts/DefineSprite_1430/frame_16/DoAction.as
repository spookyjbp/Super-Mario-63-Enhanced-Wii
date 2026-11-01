stop();
if(_root.CharHP > 0)
{
   onEnterFrame = function()
   {
      stop();
      if(_root.PauseGame == false)
      {
         if(_root.KeyUP() || _root.KeyRIGHT() || _root.KeyLEFT() || _root.KeyDOWN() || _root.KeySPACE())
         {
            play();
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
   };
}
else
{
   gotoAndStop("end");
   play();
}
