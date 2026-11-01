onClipEvent(load){
   color = Math.round(_root.TotalStars * 1.5) + 25;
   color2 = Math.abs(100 - color);
   myColor = new Color(this);
   colorTrans = new Object();
   colorTrans = {ra:100 - (color2 / 2 + Math.max(color2 - 50,0)) - color2 / 5,rb:0,ga:color,gb:0,ba:color,bb:0,aa:100,ab:0};
   myColor.setTransform(colorTrans);
}
