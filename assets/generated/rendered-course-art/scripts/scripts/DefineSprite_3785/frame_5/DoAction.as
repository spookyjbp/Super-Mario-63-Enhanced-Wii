fail = true;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
      if(fail == true && this.box.hitTest(_root.Course.Char.charClip.body))
      {
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "grabbed";
         fail = false;
      }
   }
   else
   {
      stop();
   }
};
