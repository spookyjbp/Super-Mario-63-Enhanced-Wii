if(mid == undefined)
{
   _xscale = 75;
   _yscale = 75;
   onEnterFrame = function()
   {
      this._x -= 0.375;
      if(this._x <= -225)
      {
         _X = _X + 225;
      }
   };
}
else
{
   _xscale = 100;
   _yscale = 100;
   onEnterFrame = function()
   {
      this._x -= 0.75;
      if(this._x <= -300)
      {
         _X = _X + 300;
      }
   };
}
