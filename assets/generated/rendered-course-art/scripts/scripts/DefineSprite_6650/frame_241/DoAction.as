prevFrame();
count++;
if(count > waitcount || this.Rbox.hitTest(_root.Course.Char.charClip.body))
{
   if(_parent.CheckNextMode() == false)
   {
      num = random(6);
      if(this.Rbox.hitTest(_root.Course.Char.charClip.body))
      {
         num = 5;
      }
      if(num == 0 || num == 1)
      {
         gotoAndStop("Rgoomba");
         play();
      }
      else if(num == 2 || num == 3)
      {
         gotoAndStop("Rbobomb");
         play();
      }
      else if(num == 4)
      {
         gotoAndStop("Rbomb");
         play();
      }
      else
      {
         gotoAndStop("Rwarp");
         play();
      }
   }
   else
   {
      gotoAndStop("Rend");
      play();
   }
}
