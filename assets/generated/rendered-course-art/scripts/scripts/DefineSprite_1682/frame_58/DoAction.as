if(_root["StarMessage" + _root.TotalStars] !== undefined)
{
   _root.StopBGsong();
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound("mario-goodnews");
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
   _root.PlayMessage(_root["StarMessage" + _root.TotalStars]);
}
else
{
   gotoAndStop("end");
   play();
}
