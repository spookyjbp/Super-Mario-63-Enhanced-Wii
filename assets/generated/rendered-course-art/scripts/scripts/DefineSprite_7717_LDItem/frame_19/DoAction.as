restartvar = false;
onEnterFrame = function()
{
   if(restartvar == true)
   {
      restart();
      onEnterFrame = function()
      {
         rot += 1;
         _rotation = Number(_root.LDItemArray[num][4]) + rot * Number(_root.LDItemArray[num][3]);
      };
   }
   restartvar = true;
};
restart = function()
{
   if(num !== undefined)
   {
      trace("Spinning PL Animate");
      rot = 0;
      _rotation = 0;
   }
};
