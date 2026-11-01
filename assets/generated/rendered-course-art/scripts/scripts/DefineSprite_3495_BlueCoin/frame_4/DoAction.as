_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("CoinJingle");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
if(bounce == false)
{
   _root["coindata" + _root.playingcourse][coinnum] = 1;
}
