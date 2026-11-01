FN = function()
{
   onEnterFrame = function()
   {
      if(saveto._alpha > 0)
      {
         saveto._alpha -= 4;
      }
      else
      {
         onEnterFrame = function()
         {
            if(_currentframe < 10)
            {
               nextFrame();
            }
            yes._alpha += 4;
            no._alpha += 4;
            if(yes._alpha >= 100 && yes._alpha !== undefined)
            {
               yes._alpha = 100;
               no._alpha = 100;
               onEnterFrame = null;
            }
         };
      }
   };
};
No = function()
{
   onEnterFrame = function()
   {
      if(yes._alpha > 0)
      {
         yes._alpha -= 4;
         no._alpha -= 4;
      }
      else
      {
         onEnterFrame = function()
         {
            prevFrame();
            saveto._alpha += 4;
            if(saveto._alpha >= 100)
            {
               saveto._alpha = 100;
               onEnterFrame = null;
            }
         };
      }
   };
};
Yes = function()
{
   _parent.FSave(filenum);
   onEnterFrame = function()
   {
      if(yes._alpha > 0)
      {
         yes._alpha -= 4;
         no._alpha -= 4;
      }
      else
      {
         onEnterFrame = function()
         {
            nextFrame();
            if(_currentframe >= 20)
            {
               onEnterFrame = null;
            }
         };
      }
   };
};
stop();
