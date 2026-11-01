Move = null;
onEnterFrame = function()
{
   Move();
   if(HL !== undefined)
   {
      Move = function()
      {
         nextFrame();
      };
   }
   else if(_currentframe > 1)
   {
      Move = function()
      {
         prevFrame();
      };
   }
};
stop();
