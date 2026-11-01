if(!(_root.Course.Char.landtime == 1 && _parent._parent.bounce == true))
{
   if(_root.KeyUP())
   {
      _root.Course.Char.landtime = 2;
   }
   else
   {
      nextFrame();
      play();
   }
}
