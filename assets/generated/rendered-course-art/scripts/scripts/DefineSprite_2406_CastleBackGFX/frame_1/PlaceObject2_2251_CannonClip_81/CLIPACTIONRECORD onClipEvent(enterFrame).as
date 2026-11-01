onClipEvent(enterFrame){
   count++;
   if(_root.TotalStars >= 63 && _root.Course.Char.attack == false && count > 12)
   {
      cannonsetup = true;
   }
}
