stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(count > 3)
      {
         if(this.body.hitTest(_root.Course.Char.charClip.body))
         {
            nextFrame();
         }
      }
      else
      {
         count++;
      }
   }
};
