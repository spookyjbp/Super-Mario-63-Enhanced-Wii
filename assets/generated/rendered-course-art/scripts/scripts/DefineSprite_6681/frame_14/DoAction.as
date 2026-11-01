stop();
count = 0;
CheckNextMode = function()
{
   if(_parent.defeatedcount >= 8)
   {
      return true;
   }
   return false;
};
NextMode = function()
{
   if(_parent.defeatedcount >= 8)
   {
      gotoAndStop("m2");
      BlueCoinSummon();
      CheckNextMode = function()
      {
         if(_parent.defeatedcount >= 20)
         {
            return true;
         }
         return false;
      };
      NextMode = function()
      {
         if(_parent.defeatedcount > 20)
         {
            gotoAndStop("m3");
            BlueCoinSummon();
            CheckNextMode = function()
            {
               if(_parent.defeatedcount >= 30)
               {
                  return true;
               }
               return false;
            };
            NextMode = function()
            {
               if(_parent.defeatedcount > 30)
               {
                  gotoAndStop("m5");
                  BlueCoinSummon();
                  CheckNextMode = function()
                  {
                     if(_parent.defeatedcount >= 45)
                     {
                        return true;
                     }
                     return false;
                  };
                  NextMode = function()
                  {
                     if(_parent.defeatedcount > 45)
                     {
                        gotoAndStop("m7");
                        BlueCoinSummon();
                        NextMode = null;
                        CheckNextMode = null;
                     }
                  };
               }
            };
         }
      };
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
BombSummon = function(X, Y, Xvel, Yvel)
{
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"small"});
};
BlueCoinSummon = function()
{
   _root.Course.BackGFX.attachMovie("BlueCoin","BlueCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:425 + random(500),_y:-300 - random(50),bounce:true});
};
