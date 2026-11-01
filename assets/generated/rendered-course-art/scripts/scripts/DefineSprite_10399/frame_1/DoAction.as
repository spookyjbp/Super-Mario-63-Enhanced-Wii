count = 100;
onEnterFrame = function()
{
   count -= 5;
   count = Math.max(count,0);
   _root.bgsong.setVolume(count);
};
