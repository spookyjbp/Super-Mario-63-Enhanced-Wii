stop();
if(picture == false)
{
   _root.PlayMessage(signmessage);
}
else
{
   _root.PlayPicture(signmessage);
}
readingfunction();
onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.playpause = true;
   X = _X + _parent._x;
   _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
   if(othercamcenter != undefined)
   {
      _root.Camcenter = othercamcenter;
   }
   if(!_root.KeyZ())
   {
      SPACE = false;
   }
   if(_root.KeyZ() && SPACE == false)
   {
      if(picture == false)
      {
         _root.StopMessage();
      }
      else
      {
         _root.StopPicture();
      }
      play();
   }
};
