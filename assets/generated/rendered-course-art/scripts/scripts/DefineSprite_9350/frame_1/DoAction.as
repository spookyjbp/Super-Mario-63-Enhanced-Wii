onEnterFrame = function()
{
   this._x -= 0.75;
   if(this._x <= -1000)
   {
      _X = _X + 1000;
   }
};
