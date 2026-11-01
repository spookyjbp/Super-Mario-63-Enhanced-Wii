prevFrame();
count++;
if(count > waittime)
{
   lastnum = randomnum;
   randomnum = random(4);
   if(lastnum == randomnum)
   {
      randomnum = random(3);
   }
   if(randomnum == 0)
   {
      gotoAndStop("LS1");
      play();
   }
   else if(randomnum == 1)
   {
      gotoAndStop("LS2");
      play();
   }
   else if(randomnum == 2)
   {
      gotoAndStop("LS3");
      play();
   }
   else
   {
      gotoAndStop("LA1");
      play();
   }
   if(_parent.CheckNextMode())
   {
      _parent.NextMode();
   }
}
