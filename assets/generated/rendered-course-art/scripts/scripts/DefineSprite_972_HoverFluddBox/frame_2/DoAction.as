_root.FluddArray[_root.Playinglevel][1] = true;
_root.SaveFluddH = true;
_parent.attachMovie("Hnozzle","Hnozzle" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y + 10});
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
