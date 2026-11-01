onClipEvent(enterFrame){
   color += Math.round(_root.TotalStars * 1.5 + 25 - color) / 7;
   if(_root.CheatCastleRestore == true)
   {
      color = 100;
   }
   color = Math.max(color,0);
   color = Math.min(color,100);
   color2 = Math.abs(100 - color);
   colorTrans = {ra:color * 0.75 + 25 - color2 / 5,rb:0,ga:color * 0.9 + 10 - color2 / 3,gb:0,ba:color * 0.9 + 10 - color2 / 3,bb:0,aa:100,ab:0};
   myColor.setTransform(colorTrans);
}
