onEnterFrame = function()
{
   distancevolume -= 5;
   soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
};
