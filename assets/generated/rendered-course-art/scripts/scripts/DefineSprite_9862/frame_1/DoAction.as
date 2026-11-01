function camControl()
{
   if(_root.QuakeX == undefined)
   {
      _root.QuakeX = 0;
   }
   if(_root.QuakeY == undefined)
   {
      _root.QuakeY = 0;
   }
   if(_root.RumbleNum == undefined)
   {
      _root.RumbleNum = 0;
   }
   if(_root.BGSpeed == undefined)
   {
      _root.BGSpeed = 0.5;
   }
   if(_root.RumbleNum > 0)
   {
      trumble = Math.round(Math.sqrt(_root.RumbleNum) + 1) * 1.2;
      _root.QuakeX = Math.random() * trumble - trumble / 2;
      _root.QuakeY = Math.random() * trumble - trumble / 2;
   }
   else
   {
      _root.QuakeX = (- _root.QuakeX) * 0.7;
      _root.QuakeY = (- _root.QuakeY) * 0.7;
      if(Math.abs(_root.QuakeX) < 0.01)
      {
         _root.QuakeX = 0;
      }
      if(Math.abs(_root.QuakeY) < 0.01)
      {
         _root.QuakeY = 0;
      }
   }
   parentColor1.setTransform(camColor.getTransform());
   parentColor2.setTransform(camColor.getTransform());
   parentColor3.setTransform(camColor.getTransform());
   var _loc5_ = sX / (Width / 100 * _xscale);
   var _loc4_ = sY / (Height / 100 * _yscale);
   _parent._parent._parent.Course.Stuff._x = - this._x + _root.QuakeX;
   _parent._parent._parent.Course.Stuff._y = - this._y + _root.QuakeY;
   _parent._parent._parent.Course._xscale = 100 * _loc5_;
   _parent._parent._parent.Course._yscale = 100 * _loc4_;
   _parent._parent._parent.BG._xscale = (_parent._parent._parent.Course._xscale - 100) * _root.BGSpeed + 100;
   _parent._parent._parent.BG._yscale = (_parent._parent._parent.Course._yscale - 100) * _root.BGSpeed + 100;
   _parent._parent._parent.BG.Stuff._x = (- this._x) * _root.BGSpeed + _root.QuakeX;
   _parent._parent._parent.BG.Stuff._y = (- this._y) * _root.BGSpeed + _root.QuakeY;
   _parent._parent._parent.BG._rotation = - this._rotation;
   _parent._parent._parent.farBG._rotation = - this._rotation;
   _parent._parent._rotation = - this._rotation;
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
var parentColor1 = new Color(_parent._parent);
var parentColor2 = new Color(_parent._parent._parent.BG);
var parentColor3 = new Color(_parent._parent._parent.farBG);
this.onEnterFrame = camControl;
camControl();
