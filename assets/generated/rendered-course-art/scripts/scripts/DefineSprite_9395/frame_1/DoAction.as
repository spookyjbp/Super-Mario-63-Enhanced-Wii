tens = Math.floor(_root["File" + _parent._parent.file + "TotalStars"] / 10);
ones = _root["File" + _parent._parent.file + "TotalStars"] - tens * 10;
if(tens == 0)
{
   t.gotoAndStop(11);
}
else
{
   t.gotoAndStop(tens + 1);
}
o.gotoAndStop(ones + 1);
