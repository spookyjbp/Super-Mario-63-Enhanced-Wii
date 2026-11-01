Re = function()
{
   _alpha = 100;
   _xscale = random(40) + 80;
   _yscale = random(40) + 80;
   _X = _parent.rex;
   _Y = _parent.rey;
   rot = -45 + random(90);
   rotspeed = random(15) / 2;
   speed = (random(5) + 5) / 2;
   xspeed = Math.sin(rot * 0.017453292519943295) * speed;
   yspeed = Math.cos(rot * 0.017453292519943295) * speed;
   count = 0;
   Break = random(3);
   if(Break == true)
   {
      breakcount = 160 + random(40);
   }
   else
   {
      breakcount = 0;
   }
   onEnterFrame = function()
   {
      _rotation = _rotation + rotspeed;
      _X = _X + xspeed;
      _Y = _Y + yspeed;
      count++;
      if(Break == true)
      {
         if(count > breakcount)
         {
            _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y,count:300});
            _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y,count:300});
            _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y,count:300});
            xspeed = 0;
            yspeed = 0;
            onEnterFrame = function()
            {
               _alpha = _alpha - 15;
               if(_alpha <= 0)
               {
                  Re();
               }
            };
         }
      }
      if(count > 470)
      {
         onEnterFrame = function()
         {
            _alpha = _alpha - 15;
            if(_alpha <= 0)
            {
               Re();
            }
         };
      }
   };
};
Re();
count = random(470);
i = 0;
while(i < count)
{
   _X = _X + xspeed;
   _Y = _Y + yspeed;
   i++;
}
