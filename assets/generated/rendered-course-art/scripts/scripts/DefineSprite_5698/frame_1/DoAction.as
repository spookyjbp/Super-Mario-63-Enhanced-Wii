_visible = false;
if(_root.CharHP > 0 && !(_root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Respawn"))
{
   _root.Course.Char._xscale = -100;
   _root.Course.Char.attack = true;
   _root.Course.Char.attackFrame = "SlingStar";
   _root.Course.Char._x = _X + _root.Course.BackGFX._x;
   _root.Course.Char._y = _Y + _root.Course.BackGFX._y;
}
else
{
   stop();
}
