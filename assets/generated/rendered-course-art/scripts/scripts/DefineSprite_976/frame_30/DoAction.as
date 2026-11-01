_root.bgSoundFX.stop();
_root.bgSoundFX = new Sound();
_root.bgSoundFX.attachSound("StarAppear");
_root.bgSoundFX.start(0,1);
_root.bgSoundFX.setVolume(100);
alphacount = 0;
onEnterFrame = function()
{
   _root.playpause = true;
   _root.Camcenter = _parent;
   alphacount += 3;
   alphacount = Math.min(alphacount,100);
   this.attachMovie("ChargeLight","ChargeLight" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0,_rotation:random(360),_alpha:alphacount});
   _root.PauseGame = false;
};
