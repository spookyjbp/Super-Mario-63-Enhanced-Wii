restartvar = false;
onEnterFrame = function()
{
   if(restartvar == true)
   {
      restart();
      onEnterFrame = function()
      {
         _rotation = Number(_root.LDItemArray[num][3]);
      };
   }
   restartvar = true;
};
restart = function()
{
   if(num !== undefined)
   {
      _rotation = Number(_root.LDItemArray[num][3]);
   }
};
