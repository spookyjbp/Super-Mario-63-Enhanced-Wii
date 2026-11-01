if(rotation == undefined)
{
   rotation = 0;
}
if(speed == undefined)
{
   speed = 5 + Math.random() * 5;
}
if(explodesize == undefined)
{
   explodesize = "small";
}
if(color == undefined)
{
   color = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
}
if(explodecolor == undefined)
{
   explodecolor = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
}
if(bounce == undefined)
{
   bounce = false;
}
if(bounceboost == undefined)
{
   bounceboost = 0;
}
if(follow == undefined)
{
   follow = false;
}
if(bouncemax == undefined)
{
   bouncemax = 3;
}
if(frame == undefined)
{
   frame = 1;
}
if(lifetime == undefined)
{
   lifetime = _root.framerate * 3;
}
clip.gotoAndStop(frame);
bouncecount = 0;
count = 0;
myColor = new Color(this);
colorTrans = new Object();
colorTrans = color;
myColor.setTransform(colorTrans);
_rotation = rotation;
yspeed = (- Math.sin(_root.angleToRadians(rotation))) * speed;
xspeed = (- Math.cos(_root.angleToRadians(rotation))) * speed;
