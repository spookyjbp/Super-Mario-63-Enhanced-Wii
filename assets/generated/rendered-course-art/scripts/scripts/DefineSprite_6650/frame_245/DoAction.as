prevFrame();
count++;
if(count > waitcount || this.Lbox.hitTest(_root.Course.Char.charClip.body))
{
   if(_parent.CheckNextMode() == false)
   {
      num = random(6);
      if(this.Lbox.hitTest(_root.Course.Char.charClip.body))
      {
         num = 5;
      }
      if(num == 0 || num == 1)
      {
         gotoAndStop("Lgoomba");
         play();
      }
      else if(num == 2 || num == 3)
      {
         gotoAndStop("Lbobomb");
         play();
      }
      else if(num == 4)
      {
         gotoAndStop("Lbomb");
         play();
      }
      else
      {
         gotoAndStop("Lwarp");
         play();
      }
   }
   else
   {
      gotoAndStop("Lend");
      play();
   }
}
