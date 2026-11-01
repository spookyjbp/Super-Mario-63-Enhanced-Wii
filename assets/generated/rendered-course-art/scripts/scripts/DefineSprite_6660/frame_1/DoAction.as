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
RF = function()
{
   randomnum = random(5);
   if(randomnum == 0 || randomnum == 1)
   {
      _parent.GoombaSummon(_parent._x + box._x,_parent._y + box._y,-2 - Math.random() * 5,3 + Math.random() * 7);
   }
   else if(randomnum == 2 || randomnum == 3)
   {
      _parent.BobombSummon(_parent._x + box._x,_parent._y + box._y,-2 - Math.random() * 5,3 + Math.random() * 7);
   }
   else
   {
      _parent.BombSummon(_parent._x + box._x,_parent._y + box._y,0,0);
   }
};
count = 0;
waittime = 15 + random(9) + (_parent._parent.onscreencount - _parent._parent.defeatedcount) * 2;
if(_parent.side == "R")
{
   gotoAndStop("Rwait");
   play();
}
else
{
   gotoAndStop("Lwait");
   play();
}
