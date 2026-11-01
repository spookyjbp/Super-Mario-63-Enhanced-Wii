count = 1;
onEnterFrame = function()
{
   _root.QuakeX = (random(16) - 8) * count;
   _root.QuakeY = (random(20) - 10) * count;
   _root.bgsong.setVolume(Math.min(_root.bgsong.getVolume() + 3,100));
   count -= 0.01;
   if(count <= 0)
   {
      onEnterFrame = null;
   }
};
