_alpha = 0;
aa = 0;
txt = "";
txt2 = "";
num = 0;
bz.aa = 0;
bz._alpha = 0;
onEnterFrame = function()
{
   _alpha = _alpha + (aa - _alpha) / 4;
   if(_root.ToadMessage == "")
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
      if(lasttm !== _root.ToadMessage)
      {
         num = 0;
         txt = "";
         txt2 = "";
         if(_root.ToadMessage.length < 40)
         {
            txt2 += " \r ";
         }
      }
      i = 0;
      while(i < 2)
      {
         if(num < _root.ToadMessage.length)
         {
            txt += _root.ToadMessage.charAt(num);
            txt2 += _root.ToadMessage.charAt(num);
            num++;
         }
         else
         {
            txt = _root.ToadMessage;
         }
         i++;
      }
      if(_root.KeyZ() == true)
      {
         txt = "";
         txt2 = "";
         if(_root.ToadMessage.length < 40)
         {
            txt2 += " \r ";
         }
         txt = _root.ToadMessage;
         txt2 += _root.ToadMessage;
         num = _root.ToadMessage.length;
      }
      if(num >= _root.ToadMessage.length)
      {
         bz.aa = 100;
      }
      else
      {
         bz.aa = 0;
      }
      lasttm = _root.ToadMessage;
   }
   else if(_alpha <= 2)
   {
      txt = "";
   }
};
