this.createEmptyMovieClip("soundclip",_root.getNextHighestDepth());
distancevolume = 0;
soundclip = new Sound(this);
soundclip.attachSound("VolcanoErupting");
soundclip.start(0,9);
soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
soundclip.setPan(panX);
onEnterFrame = function()
{
   distancevolume += 1;
   distancevolume = Math.min(distancevolume,75);
   soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
};
