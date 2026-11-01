vcount = 0;
_root.Course.FrontGFX.nextFrame();
Volcano = function()
{
   if(vcount > 5)
   {
      vcount = 0;
      ty = random(4) + 1;
      _root.Course.FrontGFX.attachMovie("5-4VolFire" + ty,"5-4VolFire" + ty + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:-100 + random(250),_y:-730 - random(25)});
   }
   else if(vcount == 0)
   {
      ty = random(2) + 1;
      _root.Course.FrontGFX.attachMovie("5-4VolFire" + ty,"5-4VolFire" + ty + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:-100 + random(250),_y:-730 - random(25)});
   }
   if(_root._quality == "HIGH" || _root._quality == "BEST")
   {
      vcount += 3;
   }
   else if(_root._quality == "MEDIUM")
   {
      vcount += 2;
   }
   else
   {
      vcount += 1;
   }
};
_root.Course.Char.yspeed = -17;
onEnterFrame = function()
{
   Volcano();
   _root.PauseGame = false;
   _root.Controll = false;
};
