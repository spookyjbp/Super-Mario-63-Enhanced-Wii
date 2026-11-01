if(_parent._parent.bounce == true)
{
   _root.Course.Char._y -= 5 * _parent._parent.scale;
}
if(!(_root.Course.Char.landtime == 1 && _parent._parent.bounce == true))
{
   if(_root.KeyUP())
   {
      _root.Course.Char.landtime = 2;
   }
}
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Goomba-Die");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
