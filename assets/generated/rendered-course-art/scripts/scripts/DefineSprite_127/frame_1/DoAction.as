this._alpha = 50;
wgy = 0;
ty = this._y;
onEnterFrame = function()
{
   wgy += 0.05;
   yh = Math.cos(wgy) * 5;
   if(_root.Fluddpow !== "")
   {
      gotoAndStop(_root.Fluddpow);
   }
   else
   {
      gotoAndStop(1);
   }
   this._y = ty + yh;
};
