if(_root.lastcoinsound <= 0 || _root.lastcoinsound == undefined)
{
   _root.bgsoundFX = new Sound(this);
   _root.bgsoundFX.attachSound("CoinJingle");
   _root.bgsoundFX.start(0,1);
   _root.bgsoundFX.setVolume(_root.BgVolume);
   _root.lastcoinsound = 4;
}
if(bounce == false)
{
   _root["coindata" + _root.playingcourse][coinnum] = 1;
}
