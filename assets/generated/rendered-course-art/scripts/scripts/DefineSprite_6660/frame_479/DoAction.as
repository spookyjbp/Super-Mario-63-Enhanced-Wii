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
      gotoAndStop("RS1");
      play();
   }
   else if(randomnum == 1)
   {
      gotoAndStop("RS2");
      play();
   }
   else if(randomnum == 2)
   {
      gotoAndStop("RS3");
      play();
   }
   else
   {
      gotoAndStop("RA1");
      play();
   }
   if(_parent.CheckNextMode())
   {
      _parent.NextMode();
   }
}
