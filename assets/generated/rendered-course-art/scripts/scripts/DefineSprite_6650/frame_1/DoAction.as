RF = function()
{
   count = 0;
   waitcount = random(5) + 4 + (_parent._parent.onscreencount - _parent._parent.defeatedcount) * 4;
   gotoAndStop("Rwait");
   play();
};
LF = function()
{
   count = 0;
   waitcount = random(5) + 4 + (_parent._parent.onscreencount - _parent._parent.defeatedcount) * 4;
   gotoAndStop("Lwait");
   play();
};
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
