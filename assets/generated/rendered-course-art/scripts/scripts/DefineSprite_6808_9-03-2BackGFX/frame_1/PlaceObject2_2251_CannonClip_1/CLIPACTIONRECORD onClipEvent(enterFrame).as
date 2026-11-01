onClipEvent(enterFrame){
   count++;
   if(_root.Course.BackGFX.Cannonvar == true && _root.Course.Char.attack == false && count > 12)
   {
      cannonsetup = true;
      _root.Course.Platforms.cannon.gotoAndStop(2);
   }
}
