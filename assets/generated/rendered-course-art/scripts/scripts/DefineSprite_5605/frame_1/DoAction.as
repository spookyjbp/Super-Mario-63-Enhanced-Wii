rotto = Math.atan2(_parent._parent._y - (_root.Course.Char._y - _root.Course.BackGFX._y),_parent._parent._x - (_root.Course.Char._x - _root.Course.BackGFX._x)) * 180 / 3.141592653589793;
if(_parent._parent._xscale > 0)
{
   rotto = Math.min(rotto,45);
   rotto = Math.max(rotto,-45);
}
else
{
   if(rotto > 0)
   {
      rotto = 180 - rotto;
   }
   else
   {
      rotto = -180 - rotto;
   }
   rotto = Math.min(rotto,45);
   rotto = Math.max(rotto,-45);
}
speed = 10;
if(fireball == undefined)
{
   fireball = [0,0,0];
}
if(loops == undefined)
{
   loops = fireball.length;
}
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _rotation = _rotation - (_rotation - rotto) / speed;
      play();
   }
   else
   {
      stop();
   }
};
