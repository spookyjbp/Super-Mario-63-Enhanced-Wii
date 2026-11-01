_root.Course.Char.attack = true;
_root.Course.Char.xspeed = Math.sin(_root.angleToRadians(rotdirection)) * Xpower;
_root.Course.Char.yspeed = (- Math.cos(_root.angleToRadians(rotdirection))) * Ypower;
if(side == "Right")
{
   _root.Course.Char._xscale = -100;
}
else if(side !== "Both")
{
   _root.Course.Char._xscale = 100;
}
_root.Course.Char._x = _X + _parent._x;
_root.Course.Char._y = _Y + _parent._y;
_root.Course.Char._y -= 20;
_root.Course.Char._y -= Math.cos(_root.angleToRadians(rotdirection)) * 30;
_root.Course.Char._x += Math.sin(_root.angleToRadians(rotdirection)) * 30;
_root.Course.Char.air = true;
_root.Course.Char.attackFrame = "Dive";
_root.CameraZoom = savezoom;
onEnterFrame = function()
{
   _root.PauseGame = false;
};
