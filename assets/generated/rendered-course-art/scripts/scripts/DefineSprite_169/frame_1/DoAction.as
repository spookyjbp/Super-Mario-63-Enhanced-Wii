onEnterFrame = function()
{
   if(_root._quality == "LOW")
   {
      _parent.qnum = 0;
   }
   else if(_root._quality == "MEDIUM")
   {
      _parent.qnum = 1;
   }
   else if(_root._quality == "HIGH")
   {
      _parent.qnum = 2;
   }
   else if(_root._quality == "BEST")
   {
      _parent.qnum = 3;
   }
   gotoAndStop(_parent.qnum + 1);
};
