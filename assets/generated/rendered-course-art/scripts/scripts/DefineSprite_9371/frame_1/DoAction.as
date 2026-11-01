gotoAndStop(_parent.RMP());
onEnterFrame = function()
{
   this._x -= 2;
   if(this._x <= -345)
   {
      _X = _X + 675;
      gotoAndStop(_parent.RMP());
   }
};
