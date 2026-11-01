speed = 15;
if(_root.Course.Char.air == true && _root.Course.Char.yspeed > -3 && _root.Metal == false)
{
   _root.Course.Char.yspeed *= 0.5;
   _root.Course.Char.yspeed -= 5;
}
