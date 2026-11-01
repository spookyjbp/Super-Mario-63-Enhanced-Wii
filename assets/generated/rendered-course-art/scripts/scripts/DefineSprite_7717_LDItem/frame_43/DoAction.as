onEnterFrame = function()
{
   _root.LDItemArray[num][1] = Math.round(Number(_root.LDItemArray[num][1]) / 32) * 32;
   _X = _root.LDItemArray[num][1];
   i = 0;
   while(i < LevelItems.length)
   {
      if(LevelItems[i][0] == "43")
      {
      }
      i++;
   }
};
