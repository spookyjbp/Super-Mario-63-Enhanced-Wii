onClipEvent(load){
   color = Math.round(_root.TotalStars * 1.5) + 25;
   color2 = Math.abs(100 - color);
   myColor = new Color(this);
   colorTrans = new Object();
   colorTrans = {ra:color * 0.5 + 50 - color2 / 3,rb:0,ga:color * 0.5 + 50 - color2 / 3,gb:0,ba:color * 0.5 + 50 - color2 / 3,bb:0,aa:100,ab:0};
   myColor.setTransform(colorTrans);
}
