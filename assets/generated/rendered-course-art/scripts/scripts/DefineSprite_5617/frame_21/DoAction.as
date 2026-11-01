i = 0;
while(i < 2)
{
   if(random(2) == 0)
   {
      GoombaSummon(0,-600,Math.random() * 8 - 4,9);
   }
   else
   {
      BobombSummon(0,-600,Math.random() * 8 - 4,9);
   }
   i++;
}
BlueCoinSummon = function()
{
   _root.Course.BackGFX.attachMovie("BlueCoin","BlueCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:random(600) - 300,_y:-300 - random(50),bounce:true});
};
BlueCoinSummon();
