onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(Start == true)
      {
         _root.Course.BackGFX.fall0.Start = true;
         _rotation = _rotation + speed;
         _Y = _Y + fallspeed;
         fallspeed += 0.005;
         _X = _X - 0.4;
      }
   }
}
