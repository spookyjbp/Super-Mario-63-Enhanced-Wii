if(_parent._parent.HP == 3)
{
   randomnum = 1 + random(2);
   if(random(2) == 1 && random(2) == 1)
   {
      randomnum++;
   }
}
if(_parent._parent.HP == 2)
{
   randomnum = 1 + random(3);
   if(random(2) == 1 && random(2) == 1)
   {
      randomnum = 3;
   }
}
if(_parent._parent.HP == 1)
{
   randomnum = 2 + random(2);
   if(random(2) == 1 && random(2) == 1)
   {
      randomnum = 1;
   }
}
randomnum = Math.min(randomnum,3);
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
