if(rotation == undefined)
{
   rotation = 0;
}
if(rotspeed == undefined)
{
   if(random(2) == 1)
   {
      rotspeed = Math.random() * 8 + 12;
   }
   else
   {
      rotspeed = Math.random() * -8 - 12;
   }
}
if(yspeed == undefined)
{
   yspeed = -4;
}
if(xspeed == undefined)
{
   xspeed = -6;
}
if(explodetype == undefined)
{
   explodetype = "small";
}
if(explodetimer == undefined)
{
   explodetimer = _root.framerate * 3;
}
if(color == undefined)
{
   if(explodetype == "small")
   {
      color = {ra:0,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
   }
   else
   {
      color = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
   }
}
if(explodecolor == undefined)
{
   explodecolor = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
}
bouncecount = 0;
count = 0;
myColor = new Color(this);
colorTrans = new Object();
colorTrans = color;
myColor.setTransform(colorTrans);
_rotation = rotation;
if(xspeed == 0 && yspeed == 0)
{
   speed = 4 + Math.random() * 3;
   xdiff = _root.Course.Char._x - (_X + _parent._x);
   ydiff = _root.Course.Char._y - (_Y + _parent._y);
   rot = Math.atan2(ydiff,xdiff);
   xspeed = Math.cos(rot) * speed;
   yspeed = Math.sin(rot) * speed;
}
