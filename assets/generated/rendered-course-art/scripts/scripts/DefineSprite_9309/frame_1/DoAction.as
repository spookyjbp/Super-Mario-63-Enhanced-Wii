function camControl()
{
   parentColor1.setTransform(camColor.getTransform());
   parentColor2.setTransform(camColor.getTransform());
   parentColor3.setTransform(camColor.getTransform());
   parentColor4.setTransform(camColor.getTransform());
   parentColor5.setTransform(camColor.getTransform());
   var _loc4_ = sX / (Width / 100 * _xscale);
   var _loc3_ = sY / (Height / 100 * _yscale);
   if(_root.CreditsCamFirstFrame < 2)
   {
      _root.CreditsCourseX = - this._x;
      _root.CreditsCourseY = - this._y;
      _root.CreditsCourseScaleX = 100 * _loc4_;
      _root.CreditsCourseScaleY = 100 * _loc3_;
   }
   else
   {
      _root.CreditsCourseX += (- this._x - _root.CreditsCourseX) / 5;
      _root.CreditsCourseY += (- this._y - _root.CreditsCourseY) / 5;
      _root.CreditsCourseScaleX += (100 * _loc4_ - _root.CreditsCourseScaleX) / 5;
      _root.CreditsCourseScaleY += (100 * _loc3_ - _root.CreditsCourseScaleY) / 5;
   }
   _root.CreditsCamFirstFrame = _root.CreditsCamFirstFrame + 1;
   _root.CreditsCourseRot = - this._rotation;
}
this._visible = false;
var oldMode = Stage.scaleMode;
var cX = Stage.width / 2;
var cY = Stage.height / 2;
var sX = Stage.width;
var sY = Stage.height;
var Width = 720;
var Height = 480;
var camColor = new Color(this);
var parentColor1 = new Color(_root.FarBackground);
var parentColor2 = new Color(_root.Course);
var parentColor3 = new Color(_root.BGCourse);
var parentColor4 = new Color(_root.MidBackground);
var parentColor5 = new Color(_root.BGCloseCourse);
_root.CreditsCamFirstFrame = 0;
this.onEnterFrame = camControl;
camControl();
