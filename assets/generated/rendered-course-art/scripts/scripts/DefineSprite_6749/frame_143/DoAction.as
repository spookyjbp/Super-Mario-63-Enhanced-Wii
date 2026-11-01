stop();
_Y = _Y + 200;
_X = -600;
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count += 1;
      _X = _X + 0.5;
      _X = Math.min(_X,2340);
   }
};
count = 0;
CheckNextMode = function()
{
   if(count >= 2400)
   {
      return true;
   }
   return false;
};
NextMode = function()
{
   _root.Course.FrontGFX.gotoAndStop(2);
   _root.Course.BackGFX.gotoAndStop(2);
   CheckNextMode = function()
   {
      if(count >= 3000)
      {
         return true;
      }
      return false;
   };
   NextMode = function()
   {
      nextFrame();
   };
};
GoombaSummon = function(X, Y, Xvel, Yvel)
{
   Xvel = (- (X - (_root.Course.Char._x - _root.Course.BackGFX._x))) / 48;
   Yvel = (- (Y - (_root.Course.Char._y - _root.Course.BackGFX._y))) / 48;
   Xvel += Math.random() * 3 - 1.5;
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"Goomba"});
   _parent.onscreencount = _parent.onscreencount + 1;
};
BobombSummon = function(X, Y, Xvel, Yvel)
{
   Xvel = (- (X - (_root.Course.Char._x - _root.Course.BackGFX._x))) / 48;
   Yvel = (- (Y - (_root.Course.Char._y - _root.Course.BackGFX._y))) / 48;
   Xvel += Math.random() * 4 - 2;
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"Bobomb"});
   _parent.onscreencount = _parent.onscreencount + 1;
};
BombSummon = function(X, Y, Xvel, Yvel)
{
   _root.Course.FrontGFX.attachMovie("KamekTriangleAttack","Triangle" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:X,_y:Y,yspeed:Yvel,xspeed:Xvel,explodetype:"small"});
};
BlueCoinSummon = function()
{
   _root.Course.BackGFX.attachMovie("BlueCoin","BlueCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_root.Course.Char._x - _root.Course.BackGFX._x + random(500) - 250,_y:-300 - random(50),bounce:true});
};
