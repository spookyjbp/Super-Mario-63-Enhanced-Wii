onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.Course.Char.yspeed = Math.max(0,_root.Course.Char.yspeed);
};
if(_parent.Skipturn == true)
{
   gotoAndPlay(5);
   _parent.Skipturn = false;
}
