stop();
onEnterFrame = function()
{
   if(removelock == true)
   {
      gotoAndStop(3);
   }
   if(_root.PauseGame == false)
   {
      if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.Course.Char.air == false)
      {
         if(_root.KeyUP() && !_root.KeyRIGHT() && !_root.KeyLEFT())
         {
            doorlockfunction();
         }
      }
   }
};
