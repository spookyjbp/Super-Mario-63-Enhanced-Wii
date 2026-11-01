Re = function()
{
   _alpha = 100;
   _xscale = random(40) + 80;
   _yscale = random(40) + 80;
   _X = _parent.Meteor._x + random(50) - 25;
   _Y = _parent.Meteor._y + random(50) - 25;
   rot = random(360);
   rotspeed = random(15) / 2;
   speed = (random(5) + 5) / 2;
   xspeed = Math.sin(rot) * speed;
   yspeed = Math.cos(rot) * speed;
   count = 0;
   Break = random(3);
   if(Break == true)
   {
      breakcount = 60 + random(40);
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
            _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y});
            _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y});
            _parent.attachMovie("MiniMiniMeteor","MiniMiniMeteor" + _parent.getNextHighestDepth(),_parent.getNextHighestDepth(),{_x:_X,_y:_Y});
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
      if(count > 170)
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
count = random(150);
i = 0;
while(i < count)
{
   _X = _X + xspeed;
   _Y = _Y + yspeed;
   i++;
}
