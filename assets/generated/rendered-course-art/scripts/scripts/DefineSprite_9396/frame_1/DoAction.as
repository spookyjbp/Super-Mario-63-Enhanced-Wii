tens = Math.floor(_root["File" + _parent._parent.file + "TotalStarCoins"] / 10);
ones = _root["File" + _parent._parent.file + "TotalStarCoins"] - tens * 10;
if(tens == 0)
{
   t.gotoAndStop(11);
}
else
{
   t.gotoAndStop(tens + 1);
}
o.gotoAndStop(ones + 1);
