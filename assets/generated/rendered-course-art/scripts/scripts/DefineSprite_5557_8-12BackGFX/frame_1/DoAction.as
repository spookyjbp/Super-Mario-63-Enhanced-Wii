stop();
_root.Starnamenum = 36;
_root.CanSwitchFludd = false;
_root.SaveFluddH = true;
_root.SaveFluddR = false;
_root.SaveFluddT = false;
_root.Fluddpow = "H";
_root.Playinglevel = 0;
count = 0;
onEnterFrame = function()
{
   if(_root.Course.FrntGFX.Bowser.hp > 0 && _root.PauseGame == false)
   {
      count++;
      if(count > 288)
      {
         count = 0;
         if(random(2) == 0)
         {
            GoombaSummon(0,-600,Math.random() * 8 - 4,7);
         }
         else
         {
            BobombSummon(0,-600,Math.random() * 8 - 4,7);
         }
      }
   }
};
GoombaSummon = function(X, Y, Xvel, Yvel)
{
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"Goomba"});
   _parent.onscreencount = _parent.onscreencount + 1;
};
BobombSummon = function(X, Y, Xvel, Yvel)
{
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"Bobomb"});
   _parent.onscreencount = _parent.onscreencount + 1;
};
