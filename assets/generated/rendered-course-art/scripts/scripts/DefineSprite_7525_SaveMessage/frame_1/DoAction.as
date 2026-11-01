SAVE = function()
{
   _root.SaveFile();
   count = 0;
   onEnterFrame = function()
   {
      count++;
      _alpha = _alpha - 10;
      if(count >= 10)
      {
         _root.DoneWithHighScores = true;
         _root.PauseGame = false;
         this.removeMovieClip();
      }
   };
};
NOSAVE = function()
{
   count = 0;
   onEnterFrame = function()
   {
      count++;
      _alpha = _alpha - 10;
      if(count >= 10)
      {
         _root.DoneWithHighScores = true;
         _root.PauseGame = false;
         this.removeMovieClip();
      }
   };
};
ChangeHL = function(a, hl)
{
   hl = String(Number(hl) + a);
   if(Number(hl) > 2)
   {
      hl = "1";
   }
   else if(Number(hl) < 1)
   {
      hl = "2";
   }
   return hl;
};
CheckChange = function(a)
{
   if(lastkey == false)
   {
      if(hl == undefined)
      {
         hl = "1";
      }
      else
      {
         hl = ChangeHL(a,hl);
      }
   }
};
onEnterFrame = function()
{
   _root.PauseGame = true;
   _root.Stats.Pause.gotoAndStop(1);
   if(clicked == false)
   {
      lastkey = key;
      if(_root.KeyRIGHT() == true || _root.KeyDOWN() == true)
      {
         key = true;
         CheckChange(1);
      }
      else if(_root.KeyLEFT() == true || _root.KeyUP() == true)
      {
         key = true;
         CheckChange(-1);
      }
      else
      {
         key = false;
      }
      if(_xmouse !== lastxmouse || _ymouse !== lastymouse)
      {
         mousemove = true;
      }
      else
      {
         mousemove = false;
      }
      lastxmouse = _xmouse;
      lastymouse = _ymouse;
   }
};
