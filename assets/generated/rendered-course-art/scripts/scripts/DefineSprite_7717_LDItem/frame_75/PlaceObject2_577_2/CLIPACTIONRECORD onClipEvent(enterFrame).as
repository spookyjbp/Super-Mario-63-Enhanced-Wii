onClipEvent(enterFrame){
   _X = _X + movement;
   if(movement > 1)
   {
      moved = "left";
   }
   if(movement < -1)
   {
      moved = "right";
   }
   if(moved == "right")
   {
      movement += 0.3;
   }
   else
   {
      movement -= 0.3;
   }
}
