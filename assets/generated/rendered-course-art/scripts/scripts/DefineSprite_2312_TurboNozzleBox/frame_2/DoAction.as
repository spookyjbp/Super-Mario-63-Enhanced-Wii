_root.FluddArray[_root.Playinglevel][3] = true;
_root.SaveFluddT = true;
_parent.attachMovie("Tnozzle","Hnozzle" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y + 10});
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Nozzle Box Break");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
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
