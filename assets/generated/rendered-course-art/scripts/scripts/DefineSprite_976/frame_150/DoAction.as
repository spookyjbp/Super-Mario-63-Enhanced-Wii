if(_root.Star[_parent.starnum] == true)
{
   if(_parent.alternateoldframe !== true)
   {
      play();
   }
   else
   {
      _parent.gotoAndStop(1);
      _parent._alpha = 50;
   }
}
else
{
   _parent.gotoAndStop(1);
}
