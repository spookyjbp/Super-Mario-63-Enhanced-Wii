color = Math.round(_root.TotalStars * 1.5) + 25;
color2 = Math.abs(100 - color);
myColor = new Color(this);
colorTrans = new Object();
colorTrans = {ra:color * 0.5 + 50 - color2 / 3,rb:0,ga:color * 0.5 + 50 - color2 / 3,gb:0,ba:color * 0.5 + 50 - color2 / 3,bb:0,aa:100,ab:0};
myColor.setTransform(colorTrans);
onEnterFrame = function()
{
   color += Math.round(_root.TotalStars * 1.5 + 25 - color) / 7;
   if(_root.CheatCastleRestore == true)
   {
      color = 100;
   }
   if(color > 100)
   {
      oldcolor = color;
      color += Math.abs(100 - color) * 2.5;
   }
   color2 = Math.abs(100 - color);
   colorTrans = {ra:color * 0.5 + 50 - color2 / 3,rb:0,ga:color * 0.5 + 50 - color2 / 3,gb:0,ba:color * 0.5 + 50 - color2 / 3,bb:0,aa:100,ab:0};
   myColor.setTransform(colorTrans);
   if(color > 100)
   {
      color = oldcolor;
   }
   if(_root.BGquality == "LOW")
   {
      gotoAndStop(3);
   }
   else if(_root.BGquality == "MEDIUM")
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
