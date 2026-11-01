stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body) || this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 15) * _root.coursescale / 100 + _root.Course._y,true))
      {
         starfunction();
         play();
      }
      else
      {
         stop();
      }
   }
};
