stop();
kNbrPts = polygons;
SW = WaterLength;
SH = 0;
MovieClip.prototype.makeWave = function(deep, clr, jit, turbProb)
{
   this.pts = [];
   this.springs = [];
   this.clr = clr;
   this.jit = jit;
   this.turbProb = turbProb;
   this.deep = deep;
   this.stiffK = waterstength;
   this.kDamp = 0.85;
   var _loc3_ = (SW + 100) / kNbrPts;
   var _loc2_ = 0;
   while(_loc2_ < kNbrPts)
   {
      this.pts[_loc2_] = {x:-50 + _loc2_ * _loc3_,y:0,fx:0,fy:0,vx:0,vy:0,restLen:0.5 * _loc3_,jy:0};
      _loc2_ = _loc2_ + 1;
   }
};
MovieClip.prototype.simWave = function()
{
   var _loc3_;
   var _loc4_;
   var _loc5_;
   var _loc7_;
   var _loc6_;
   var _loc8_;
   var _loc10_;
   var _loc11_;
   var _loc9_;
   if(_root.PauseGame == false && Math.abs(_root.Course.Char._y - (_Y + _root.Course.FrontGFX._y)) < 1000)
   {
      _loc3_ = 0;
      while(_loc3_ < this.pts.length)
      {
         this.pts[_loc3_].fy += Math.random() * (randomwave * 2) - randomwave;
         this.pts[_loc3_].fx = 0;
         if(Math.random() >= this.turbProb)
         {
            this.pts[_loc3_].fy = 0;
         }
         _loc3_ = _loc3_ + 1;
      }
      if(graphics == false)
      {
         if(Math.abs(_root.Course.Char.yspeed) > 5 && Math.abs(_root.Course.Char._y - (_Y + _root.Course.FrontGFX._y)) < 20)
         {
            if(rot == undefined || rot == 0 || rot == 180)
            {
               X = _root.Course.Char._x - (_X + _root.Course.FrontGFX._x) + _root.Course.Char.xspeed;
            }
            else
            {
               X = _root.Course.Char._y - (_Y + _root.Course.FrontGFX._y) + _root.Course.Char.yspeed;
            }
            marioI = Math.round(X / SW * kNbrPts);
            if(_root.Course.Char.yspeed > 0)
            {
               this.pts[marioI].fy = _root.Course.Char.yspeed / 2;
            }
            else
            {
               this.pts[marioI].fy = _root.Course.Char.yspeed / 3;
            }
            this.pts[marioI].fx += _root.Course.Char.xspeed / 5;
            _loc3_ = 1;
            while(_loc3_ < 3)
            {
               if(rot == undefined || rot == 0)
               {
                  this.pts[mouseI + _loc3_].fy -= _root.Course.Char.yspeed / 15 / _loc3_;
                  this.pts[mouseI - _loc3_].fy -= _root.Course.Char.yspeed / 15 / _loc3_;
                  this.pts[marioI].fx += _root.Course.Char.xspeed / 10 / _loc3_;
               }
               else if(rot == 90)
               {
                  this.pts[mouseI + _loc3_].fy -= _root.Course.Char.xspeed / 15 / _loc3_;
                  this.pts[mouseI - _loc3_].fy -= _root.Course.Char.xspeed / 15 / _loc3_;
                  this.pts[marioI].fx += _root.Course.Char.yspeed / 10 / _loc3_;
               }
               else if(rot == -90)
               {
                  this.pts[mouseI + _loc3_].fy += _root.Course.Char.xspeed / 15 / _loc3_;
                  this.pts[mouseI - _loc3_].fy += _root.Course.Char.xspeed / 15 / _loc3_;
                  this.pts[marioI].fx -= _root.Course.Char.yspeed / 10 / _loc3_;
               }
               else
               {
                  this.pts[mouseI + _loc3_].fy += _root.Course.Char.yspeed / 15 / _loc3_;
                  this.pts[mouseI - _loc3_].fy += _root.Course.Char.yspeed / 15 / _loc3_;
                  this.pts[marioI].fx -= _root.Course.Char.xspeed / 10 / _loc3_;
               }
               _loc3_ = _loc3_ + 1;
            }
         }
      }
      _loc3_ = 0;
      while(_loc3_ < this.pts.length - 1)
      {
         _loc4_ = this.pts[_loc3_];
         _loc5_ = this.pts[_loc3_ + 1];
         _loc7_ = _loc5_.x - _loc4_.x;
         _loc6_ = _loc5_.y - _loc4_.y;
         _loc8_ = Math.sqrt(_loc7_ * _loc7_ + _loc6_ * _loc6_);
         _loc10_ = this.stiffK * (_loc8_ - _loc4_.restLen);
         _loc11_ = _loc10_ * _loc7_ / _loc8_;
         _loc9_ = _loc10_ * _loc6_ / _loc8_;
         if(_loc3_ != 0)
         {
            _loc4_.fx += _loc11_;
            _loc4_.fy += _loc9_;
         }
         if(_loc3_ != this.pts.length - 2)
         {
            _loc5_.fx -= _loc11_;
            _loc5_.fy -= _loc9_;
         }
         _loc3_ = _loc3_ + 1;
      }
      this.clear();
      r = Math.floor(redcolor);
      g = Math.floor(greencolor);
      b = Math.floor(bluecolor);
      this.clr = r << 16 | g << 8 | b;
      this.beginFill(this.clr,alpha);
      this.moveTo(this.pts[0].x,this.pts[0].y);
      _loc3_ = 1;
      while(_loc3_ < this.pts.length - 1)
      {
         pt = this.pts[_loc3_];
         pt.vx *= this.kDamp;
         pt.vy *= this.kDamp;
         pt.vx += pt.fx;
         pt.vy += pt.fy;
         pt.x += pt.vx;
         pt.y += pt.vy;
         this.lineTo(pt.x,pt.y);
         _loc3_ = _loc3_ + 1;
      }
      _loc3_ = this.pts.length - 1;
      this.lineTo(this.pts[_loc3_].x,this.pts[_loc3_].y);
      this.lineTo(this.pts[_loc3_].x,WaterDeepness);
      this.lineTo(this.pts[0].x,WaterDeepness);
      this.lineTo(this.pts[0].x,this.pts[0].y);
      this.endFill();
   }
};
var d = 0;
var vd = 4;
kNbrLines = 1;
var i = 1;
while(i < kNbrLines + 1)
{
   var r = i / (kNbrLines - 1);
   var ir = 1 - r;
   var mc = this.createEmptyMovieClip("mc" + i,kNbrLines * 2 + 1 - i * 2);
   mc._x = 0;
   mc._y = SH / 2 - d;
   var deep;
   if(i == 0)
   {
      deep = SH / 2;
   }
   else
   {
      deep = d * 2;
   }
   mc.makeWave(deep,mergeColor(3356487,14079449,r * 0.9),0.1 + 1 * ir * ir,0.1);
   mc.onEnterFrame = mc.simWave;
   d += vd;
   vd /= 2;
   i++;
}
SS = 10;
var mc = "";
mc.cy = SH / 2.8;
mc._x = - SW;
mc._y = SH / 4;
