restartvar = false;
onEnterFrame = function()
{
   if(restartvar == true)
   {
      restart();
      onEnterFrame = null;
   }
   restartvar = true;
};
restart = function()
{
   if(num !== undefined)
   {
      trace("Green PL Animate");
      clip.speed = Number(_root.LDItemArray[num][3]);
      clip.num = Number(_root.LDItemArray[num][4]);
      clip.distance = Number(_root.LDItemArray[num][5]);
      clip.angle = Number(_root.LDItemArray[num][6]);
      clip.scale = Number(_root.LDItemArray[num][7]);
      clip.Start();
      clip._alpha = 50;
   }
};
