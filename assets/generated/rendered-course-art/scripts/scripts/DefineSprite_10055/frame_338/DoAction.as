onEnterFrame = function()
{
   _root.bgsong.setVolume(Math.max(_root.bgsong.getVolume() - 3,0));
};
