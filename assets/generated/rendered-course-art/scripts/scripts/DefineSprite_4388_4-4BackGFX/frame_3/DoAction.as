stop();
if(_root.Course.Char._y - _Y < -500)
{
   _root.soundcrank = new Sound(this);
   _root.soundcrank.attachSound("crank");
   _root.soundcrank.start(0,1);
   _root.soundcrank.setVolume(_root.BgVolume * 0.3);
}
_root.soundcrank.onSoundComplete = function()
{
   if(_root.Course.Char._y - _Y < -500)
   {
      _root.soundcrank.attachSound("crank");
      _root.soundcrank.start(0,1);
      _root.soundcrank.setVolume(_root.BgVolume * 0.3);
   }
};
stop();
_root.warpframe = false;
onEnterFrame = function()
{
   if(this.warp1.hitTest(_root.Course.Char.charClip.feet))
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.attackFrame = "none";
      warp = true;
      warpnum = 1;
   }
   if(warp == true && _root.warpframe == false)
   {
      if(warpnum == 1)
      {
         _root.attachMovie("downtransition","downtransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      warp = false;
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.changecourse("downtransition2","4-10",-1350,-600,-1350,-600);
      }
   }
};
