this.pts = [];
var i = 0;
while(i < polygons)
{
   this.pts[i] = {x:i / polygons * WaterLength,y:0,fx:0,fy:0,vx:0,vy:0,restLen:0.5 * (WaterLength / polygons),jy:0};
   i++;
}
onEnterFrame = function()
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
   if(_root.PauseGame == false)
   {
      _loc3_ = 0;
      while(_loc3_ < polygons)
      {
         this.pts[_loc3_].fx = 0;
         if(Math.random() < 0.1)
         {
            this.pts[_loc3_].fy += Math.random() * 2 - 1;
         }
         else
         {
            this.pts[_loc3_].fy = 0;
         }
         _loc3_ = _loc3_ + 1;
      }
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
         marioI = Math.round(X / WaterLength * polygons);
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
      _loc3_ = 0;
      while(_loc3_ < this.pts.length - 1)
      {
         _loc4_ = this.pts[_loc3_];
         _loc5_ = this.pts[_loc3_ + 1];
         _loc7_ = _loc5_.x - _loc4_.x;
         _loc6_ = _loc5_.y - _loc4_.y;
         _loc8_ = Math.sqrt(_loc7_ * _loc7_ + _loc6_ * _loc6_);
         _loc10_ = waterstength * (_loc8_ - _loc4_.restLen);
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
         pt.vx *= 0.85;
         pt.vy *= 0.85;
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
stop();
