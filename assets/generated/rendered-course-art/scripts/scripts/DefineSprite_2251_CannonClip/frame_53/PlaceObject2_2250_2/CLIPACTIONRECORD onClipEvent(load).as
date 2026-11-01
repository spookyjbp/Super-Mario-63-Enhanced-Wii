onClipEvent(load){
   if(_parent.side == "Right")
   {
      rotation = 45;
   }
   else if(_parent.side !== "Both")
   {
      rotation = -45;
   }
   else
   {
      rotation = 22.5;
   }
   _rotation = rotation;
   rotspeed = 0;
}
