movedir = "up";
frame = "move";
HP = 3;
clip.gotoAndStop(frame);
yspeed = 0;
xspeed = 0;
rotmovement = 0;
rotspeed = 2;
attack = true;
attackFrame = "shine";
count = 0;
if(_root.kingBoodefeated == true)
{
   stop();
   onEnterFrame = null;
   frame = "none";
   clip.gotoAndStop(frame);
}
