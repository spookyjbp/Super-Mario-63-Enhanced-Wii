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
      clip.blocktype = Number(_root.LDItemArray[num][13]);
      clip.rotdirection = String(_root.LDItemArray[num][14]);
      clip.rotspeed = Number(_root.LDItemArray[num][15]);
      clip.rotwait = Number(_root.LDItemArray[num][16]);
      clip.quake = Boolean(_root.LDItemArray[num][17]);
      clip.Start2();
      clip2.gotoAndStop(Number(_root.LDItemArray[num][13]));
      clip2._xscale = Number(_root.LDItemArray[num][12]);
      clip2._yscale = Number(_root.LDItemArray[num][12]);
      clip.gotoAndStop(Number(_root.LDItemArray[num][13]));
      clip._xscale = Number(_root.LDItemArray[num][12]);
      clip._yscale = Number(_root.LDItemArray[num][12]);
   }
};
