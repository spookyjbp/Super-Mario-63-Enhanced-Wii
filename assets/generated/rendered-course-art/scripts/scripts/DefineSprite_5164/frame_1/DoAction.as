if(fallspeed == undefined)
{
   fallspeed = _root.gravity / 5;
}
if(fallamount == undefined)
{
   fallamount = 192;
}
if(fallwait == undefined)
{
   fallwait = 10;
}
if(quakeamount == undefined)
{
   quakeamount = 2;
}
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("FallingblockCode","FallingblockCode" + depth,depth,{_x:_X,_y:_Y,fallspeed:fallspeed,fallamount:fallamount,fallwait:fallwait,quakeamount:quakeamount,yspeed:0,fall:false});
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      tellTarget(_root.Course.Platforms["FallingblockCode" + depth])
      {
         if(fall == false)
         {
            this._alpha += 10;
            this._alpha = Math.min(this._alpha,100);
            if(this._alpha >= 100 && hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               returnX = _X;
               returnY = _Y;
               timefall = 0;
               fallcount = 0;
               fall = true;
            }
         }
         if(fall == true)
         {
            timefall++;
            if(timefall > fallwait)
            {
               fallcount++;
               yspeed += fallspeed;
               yspeed = _root.groundFriction(yspeed,0.01,1.25);
            }
            else
            {
               _Y = returnY + (Math.random() * quakeamount - Math.random() * quakeamount);
               if(timefall == fallwait)
               {
                  _Y = returnY;
                  _X = returnX;
               }
            }
            if(yspeed < 0)
            {
               _Y = _Y - yspeed;
            }
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _root.Course.Char._y += yspeed;
               if(landedon == false)
               {
               }
               landedon = true;
            }
            else
            {
               if(landedon == true)
               {
                  _root.Course.Char.yspeed += yspeed;
               }
               landedon = false;
            }
            if(yspeed < 0)
            {
               _Y = _Y + yspeed;
            }
            _Y = _Y + yspeed;
            if(fallcount > fallamount - 10)
            {
               this._alpha -= 10;
               this._alpha = Math.max(this._alpha,0);
            }
            if(fallcount > fallamount)
            {
               fallcount = 0;
               yspeed = 0;
               _X = returnX;
               _Y = returnY;
               fall = false;
            }
         }
         this._y = _Y;
         this._x = _X;
      }
   }
};
