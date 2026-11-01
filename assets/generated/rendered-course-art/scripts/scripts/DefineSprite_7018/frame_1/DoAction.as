count = 0;
if(ty == 2)
{
   speed = -1.1;
}
else if(ty == 3)
{
   speed = 0.5;
}
else
{
   speed = 1;
}
onEnterFrame = function()
{
   _X = _X - speed;
   count -= speed;
   if(speed > 0)
   {
      if(count <= -48)
      {
         count += 48;
         _X = _X + 48;
      }
   }
   else if(count >= 48)
   {
      count -= 48;
      _X = _X - 48;
   }
};
