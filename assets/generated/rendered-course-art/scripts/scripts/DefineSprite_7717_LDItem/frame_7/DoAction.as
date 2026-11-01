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
      clip.DirectionX = String(_root.LDItemArray[num][9]);
      clip.DirectionY = String(_root.LDItemArray[num][10]);
      clip.distanceX = Number(_root.LDItemArray[num][5]);
      clip.distanceY = Number(_root.LDItemArray[num][6]);
      clip.accelX = Number(_root.LDItemArray[num][11]);
      clip.accelY = Number(_root.LDItemArray[num][11]);
      clip.speedX = Number(_root.LDItemArray[num][3]);
      clip.speedY = Number(_root.LDItemArray[num][4]);
      clip.Xoffstart = Number(_root.LDItemArray[num][7]);
      clip.Yoffstart = Number(_root.LDItemArray[num][8]);
      clip.scale = Number(_root.LDItemArray[num][12]);
      clip.touchngo = Boolean(Number(_root.LDItemArray[num][13]));
      clip.disapearcount = Number(_root.LDItemArray[num][14]);
      clip.Start2();
      clip2.gotoAndStop(Number(_root.LDItemArray[num][12]));
   }
};
