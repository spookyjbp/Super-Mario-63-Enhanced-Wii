bnum = random(30) + 30;
bcount = 0;
btype = random(6) + 1;
this.createEmptyMovieClip("birdsound",_root.getNextHighestDepth());
onEnterFrame = function()
{
   birdvolume = _root.TotalStars / 64;
   if(_root.CheatCastleRestore == true)
   {
      birdvolume = 1;
   }
   bcount++;
   if(bcount > bnum)
   {
      birdsound = new Sound(this);
      birdsound.attachSound("Birdy" + btype);
      birdsound.start(0,1);
      birdsound.setVolume(Math.round(_root.MarioVolume / 5) * birdvolume);
      bcount = 0;
      btype = random(6) + 1;
      bnum = random(50) + 15;
   }
};
