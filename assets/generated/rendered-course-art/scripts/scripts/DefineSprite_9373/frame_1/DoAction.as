maxframe = 20;
lastnum = 1;
num = 1;
RMP = function()
{
   last2num = lastnum;
   lastnum = num;
   num = random(maxframe) + 1;
   i = 0;
   while(i < 5)
   {
      if(num == lastnum || num == last2num)
      {
         num = random(maxframe) + 1;
      }
      else
      {
         i = 5;
      }
      i++;
   }
   return num;
};
