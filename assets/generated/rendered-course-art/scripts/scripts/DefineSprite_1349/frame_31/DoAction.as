play();
_root.WaterAmount -= 500;
_root.Power = 0;
_root.Course.Char.yspeed -= Math.sin(_root.angleToRadians(_root.DiveRot)) * _root.RocketBlast;
if(_root.Course.Char._xscale > 0)
{
   _root.Course.Char.xspeed -= Math.cos(_root.angleToRadians(_root.DiveRot)) * _root.RocketBlast;
}
else
{
   _root.Course.Char.xspeed += Math.cos(_root.angleToRadians(_root.DiveRot)) * _root.RocketBlast;
}
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Power = 0;
      _root.Course.Char.yspeed -= Math.sin(_root.angleToRadians(_root.DiveRot)) / 3;
      if(_root.Course.Char._xscale > 0)
      {
         _root.Course.Char.xspeed -= Math.cos(_root.angleToRadians(_root.DiveRot)) / 3;
      }
      else
      {
         _root.Course.Char.xspeed += Math.cos(_root.angleToRadians(_root.DiveRot)) / 3;
      }
      _parent._parent.chargeframe = 0;
   }
};
