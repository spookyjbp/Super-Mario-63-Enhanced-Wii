GoombaSummon = function(X, Y, Xvel, Yvel)
{
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"Goomba"});
};
BobombSummon = function(X, Y, Xvel, Yvel)
{
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"Bobomb"});
};
i = 0;
while(i < 4)
{
   if(random(2) == 0)
   {
      GoombaSummon(0,-600,Math.random() * 8 - 4,11);
   }
   else
   {
      BobombSummon(0,-600,Math.random() * 8 - 4,11);
   }
   i++;
}
