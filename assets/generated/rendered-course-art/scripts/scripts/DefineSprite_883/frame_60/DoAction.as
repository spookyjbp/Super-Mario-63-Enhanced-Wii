onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      _parent._parent._alpha -= 5;
      _root.Camcenter = _parent._parent;
      play();
   }
};
if(_parent._parent.HP == 3 || _parent._parent.HP <= 0)
{
   _root.Course.FrontGFX.gotoAndStop(1);
}
if(_parent._parent.HP == 2)
{
   _root.Course.FrontGFX.gotoAndStop(2);
}
if(_parent._parent.HP == 1)
{
   _root.Course.FrontGFX.gotoAndStop(3);
}
