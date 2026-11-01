if(onlyright == true)
{
   LD = "right";
}
if(onlyleft == true)
{
   LD = "left";
}
if(LD == "right")
{
   _root.Course.FrontGFX.attachMovie("GoldBill","GoldBill" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X + Math.cos(_root.angleToRadians(_rotation)) * 15,_y:_Y + Math.sin(_root.angleToRadians(_rotation)) * 15,_xscale:100,rotation:_rotation,eat:eat,chase:chase,frame:frame,speed:speed,rotdiv:rotdiv,rotmaxspeed:rotmaxspeed,enemyhurt:enemyhurt,parent:this});
}
else
{
   _root.Course.FrontGFX.attachMovie("GoldBill","GoldBill" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X - Math.cos(_root.angleToRadians(_rotation)) * 15,_y:_Y - Math.sin(_root.angleToRadians(_rotation)) * 15,_xscale:-100,rotation:_rotation,eat:eat,chase:chase,frame:frame,speed:speed,rotdiv:rotdiv,rotmaxspeed:rotmaxspeed,enemyhurt:enemyhurt,parent:this});
}
total++;
