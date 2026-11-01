onClipEvent(enterFrame){
   if(firstrun == false)
   {
      _Y = _root["M2-1WaterLevel"] + 420;
      _root.Course.Platforms.cannon.gotoAndStop(2);
      firstrun = true;
   }
}
