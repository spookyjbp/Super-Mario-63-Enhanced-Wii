_alpha = 0;
aa = 0;
txt = "";
txt2 = "";
num = 0;
bz.aa = 0;
bz._alpha = 0;
onEnterFrame = function()
{
   _root.CinemaMessageFinished = false;
   gotoAndStop(_root.CinemaMessageFrame);
   _alpha = _alpha + (aa - _alpha) / 4;
   if(_root.CinemaMessage == "")
   {
      aa = 0;
   }
   else
   {
      aa = 100;
   }
   bz._alpha += (bz.aa - bz._alpha) / 2;
   if(_alpha > 98)
   {
      if(lasttm !== _root.CinemaMessage)
      {
         num = 0;
         txt = "";
         txt2 = "";
         if(_root.CinemaMessage.length < 40)
         {
            txt2 += " \r ";
         }
      }
      i = 0;
      while(i < 2)
      {
         if(num < _root.CinemaMessage.length)
         {
            txt += _root.CinemaMessage.charAt(num);
            txt2 += _root.CinemaMessage.charAt(num);
            num++;
         }
         else
         {
            _root.CinemaMessageFinished = true;
            txt = _root.CinemaMessage;
         }
         i++;
      }
      if(_root.KeyZ() == true && lastframeZ !== true)
      {
         txt = "";
         txt2 = "";
         if(_root.CinemaMessage.length < 40)
         {
            txt2 += " \r ";
         }
         txt = _root.CinemaMessage;
         txt2 += _root.CinemaMessage;
         num = _root.CinemaMessage.length;
      }
      lastframeZ = _root.KeyZ();
      if(num >= _root.CinemaMessage.length)
      {
         bz.aa = 100;
      }
      else
      {
         bz.aa = 0;
      }
      lasttm = _root.CinemaMessage;
   }
   else if(_alpha <= 2)
   {
      txt = "";
   }
};
