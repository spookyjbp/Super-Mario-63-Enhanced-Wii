onClipEvent(enterFrame){
   if(_root.KeyTapLEFT())
   {
      rotspeed -= 0.5;
   }
   if(_root.KeyTapRIGHT())
   {
      rotspeed += 0.5;
   }
   if(_root.KeyLEFT())
   {
      rotspeed -= 1;
   }
   if(_root.KeyRIGHT())
   {
      rotspeed += 1;
   }
   if(_root.KeySPACE())
   {
      _parent.play();
      _parent.rotdirection = _rotation;
   }
   rotation += rotspeed;
   _rotation = rotation;
   if(_parent.side == "Right")
   {
      _rotation = Math.max(0,_rotation);
      _rotation = Math.min(75,_rotation);
   }
   else if(_parent.side !== "Both")
   {
      _rotation = Math.min(0,_rotation);
      _rotation = Math.max(-75,_rotation);
   }
   else
   {
      _rotation = Math.max(-75,_rotation);
      _rotation = Math.min(75,_rotation);
   }
   rotation = _rotation;
   rotspeed = _root.groundFriction(rotspeed,0.5,1.1);
}
