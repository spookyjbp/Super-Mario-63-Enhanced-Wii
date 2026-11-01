_root.Course.Char.charClip.frame.gotoAndPlay("start");
if(_root.Course.Char.attack == false || _root.Course.Char.attackFrame !== "SlingStar")
{
   _root.CameraZoom = savecamzoom;
   onEnterFrame = function()
   {
      star.base._rotation += 1;
      star2.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
   };
}
_root.CameraZoom = 75;
