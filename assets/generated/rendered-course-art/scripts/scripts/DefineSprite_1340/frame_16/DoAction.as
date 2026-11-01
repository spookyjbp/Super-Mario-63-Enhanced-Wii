play();
_root.WaterAmount -= 500;
_root.Power = 0;
_root.Course.Char.yspeed = Math.min(- _root.RocketBlast + Math.max(_root.Course.Char.yspeed / 3,0),_root.Course.Char.yspeed);
onEnterFrame = function()
{
   _root.Power = 0;
   _root.Course.Char.yspeed -= _root.RocketBlast / 50;
   if(_root.Course.Char.Water == true)
   {
      _root.Course.Char.yspeed -= _root.RocketBlast / 45;
   }
   _root.Course.Char.lockon = false;
   if(Key.isDown(39))
   {
      _root.Course.Char.xspeed += 0.7;
      _root.Course.Char._rotation += 5;
   }
   if(Key.isDown(37))
   {
      _root.Course.Char.xspeed -= 0.7;
      _root.Course.Char._rotation -= 5;
   }
   _parent.chargeframe = 0;
};
