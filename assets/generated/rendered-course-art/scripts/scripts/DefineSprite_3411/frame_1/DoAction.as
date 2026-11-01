if(a == undefined)
{
   _alpha = 0;
   a = 0;
   onEnterFrame = function()
   {
      if(_root.Invisible == true && a !== 100)
      {
         a = 100;
         play();
      }
      else if(a !== 0)
      {
         a = 0;
      }
      _alpha = _alpha + (a - _alpha) / 5;
   };
}
