ywave = random(90);
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
if(_root.MaxStarCoins < 5 && false)
{
   gotoAndStop(3);
   locked = true;
}
else
{
   gotoAndStop(2);
   locked = false;
}
onEnterFrame = function()
{
   if(bg.hitTest(_root._xmouse,_root._ymouse,true) && locked == false && _parent.clicked !== true)
   {
      sc = 120;
   }
   else
   {
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 4;
   _yscale = _xscale;
   txt._xscale += (sc - txt._xscale) / 4;
   txt._yscale = txt._xscale;
   ywave += 0.035;
   rwave += 0.025;
   this._y = Y + Math.sin(ywave) * 6;
   this._rotation = R + Math.sin(rwave) * _xscale / 20;
};
onMouseDown = function()
{
   if(sc == 120 && _parent.clicked !== true)
   {
      _root.LDMenuFrame = "nosong";
      _root.gotoAndStop("LDMenu");
      _parent.clicked = true;
      sc = 120;
      _xscale = 150;
      _yscale = 150;
      r = 0;
      rspeed = 12;
      onEnterFrame = function()
      {
         r += rspeed;
         rspeed *= 0.97;
         if(sc > 10)
         {
            sc -= 10;
         }
         _xscale = _xscale + (sc - _xscale) / 4;
         _yscale = _xscale;
         ywave += 0.035;
         rwave += 0.025;
         this._y = Y + Math.sin(ywave) * 6;
         this._rotation = R + r + Math.sin(rwave) * _xscale / 20;
      };
      onMouseDown = null;
   }
};
