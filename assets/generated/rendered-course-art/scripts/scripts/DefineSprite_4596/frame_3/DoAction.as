stop();
saveY = _Y;
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("SinkingLavaGround","SinkingLavaGround" + depth,depth,{_x:_X,_y:_Y,landedon:false});
startcount = 0;
sink = false;
currentspeedY = 0;
offcount = sinktime;
clip.gotoAndStop(frame);
_root.Course.Platforms["SinkingLavaGround" + depth].gotoAndStop(frame);
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(startcount > 5)
      {
         distance = (_root.Course.Char._x - (_X + _parent._x)) / this.scale;
         if(sink == true)
         {
            offcount = 0;
         }
         else
         {
            offcount++;
         }
         if(sink == true || offcount <= sinktime)
         {
            if(_Y > saveY + deepness - currentspeedY * 3 + 3)
            {
               currentspeedY = (saveY + deepness - _Y) / 25;
            }
            else
            {
               currentspeedY += fallspeed1;
               currentspeedY *= fallspeed2;
            }
         }
         else if(_Y > saveY + 5)
         {
            currentspeedY -= risespeed1;
            currentspeedY *= risespeed2;
         }
         else
         {
            currentspeedY = (saveY - _Y) / 15;
         }
      }
      else
      {
         startcount++;
      }
      _Y = _Y + currentspeedY;
      tellTarget(_root.Course.Platforms["SinkingLavaGround" + depth])
      {
         hit = false;
         if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.currentspeedY + 3) * _root.coursescale / 100 + _root.Course._y,false))
         {
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.currentspeedY + 3) * _root.coursescale / 100 + _root.Course._y,true))
            {
               hit = true;
            }
         }
         if(hit == true && _root.Course.Char.yspeed > -3)
         {
            _root.Course.Char._y += this.currentspeedY;
            if(landedon == false)
            {
               currentspeedY += _root.Course.Char.yspeed / 2;
               this.sink = true;
            }
            landedon = true;
         }
         else
         {
            if(landedon == true)
            {
               this.sink = false;
               currentspeedY += (- _root.Course.Char.yspeed) / 1.5;
               _root.Course.Char.yspeed += this.currentspeedY / 2;
            }
            landedon = false;
         }
         _Y = this._y;
         _X = this._x;
      }
   }
};
