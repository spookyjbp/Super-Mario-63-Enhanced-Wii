stop();
if(scale == undefined)
{
   scale = 1;
}
gotoAndStop(Math.round(scale));
depth = _root.Course.BPlatforms.getNextHighestDepth();
_root.Course.BPlatforms.attachMovie("LogPL","LogPL" + depth,depth,{_x:_X,_y:_Y,speedY:speedY,speedX:speedX,landedon:false,_xscale:_xscale});
rotspeed = 0;
land = false;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false && Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
   {
      rotspeed = _root.groundFriction(rotspeed,0.15,1.05);
      distance = (_root.Course.Char._x - (_X + _parent._x)) / this.scale;
      tellTarget(_root.Course.BPlatforms["LogPL" + depth])
      {
         if(this.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _root.changestep = 3;
            _root.StepSound = "step-wetwood";
            land = true;
            _root.Course.Char._x += _rotation / 7;
            this.rotspeed += this.distance / 5 / ((this.scale - 1) / 2 + 1);
            this.rotspeed -= _rotation - _root.groundFriction(_rotation,0.3 / this.scale,0.07 / this.scale + 1);
         }
         else
         {
            if(land == true)
            {
               _root.Course.Char.xspeed += (_rotation / 5 - _root.Course.Char.xspeed) / 3;
               if(_root.Course.Char.yspeed < -3)
               {
                  this.rotspeed += this.distance * 2 / ((this.scale - 1) / 2 + 1);
               }
            }
            land = false;
            this.rotspeed -= _rotation - _root.groundFriction(_rotation,1 / this.scale,0.12 / this.scale + 1);
         }
         _rotation = _rotation + this.rotspeed / 20;
         if(_rotation >= 80 || _rotation <= -80)
         {
            this.rotspeed *= -0.7;
         }
         _rotation = Math.max(_rotation,-80);
         _rotation = Math.min(_rotation,80);
         _xscale = this.scale * 100;
         this._rotation = _rotation;
      }
   }
};
