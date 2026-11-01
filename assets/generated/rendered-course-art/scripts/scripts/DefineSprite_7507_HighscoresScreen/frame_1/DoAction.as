key = false;
num = 0;
level = "Secret Course";
if(_root.lastPlayinglevel == 1)
{
   level = "Bob-omb Battle Field";
}
if(_root.lastPlayinglevel == 2)
{
   level = "Snowmans Land";
}
if(_root.lastPlayinglevel == 3)
{
   level = "Hazy Maze Cave";
}
if(_root.lastPlayinglevel == 4)
{
   level = "Boo\'s Mansion";
}
if(_root.lastPlayinglevel == 5)
{
   level = "Lethal Lava Land";
}
if(_root.lastPlayinglevel == 6)
{
   level = "Tall Tall Mountain";
}
if(_root.lastPlayinglevel == 7)
{
   level = "Rainbow Ride";
}
if(_root.lastPlayinglevel == 8)
{
   level = "Bowsers Castle";
}
if(_root.lastPlayinglevel == 9)
{
   level = "Sinking Sand Land";
}
if(_root.lastPlayinglevel == 10)
{
   level = "Wet Dry World";
}
if(_root.lastPlayinglevel == 11)
{
   level = "Tick Tock Clock";
}
star = _root.StarNameArray[_root.lastStarnamenum];
sta = star;
cou = level;
SAVE = function()
{
   _root.SaveFile();
   if(_root.TotalCoins > _root["RecordCoin" + _root.lastPlayinglevel])
   {
      _root["RecordCoin" + _root.lastPlayinglevel] = _root.TotalCoins;
      _root.SaveHighScores();
   }
   count = 0;
   onEnterFrame = function()
   {
      count++;
      _alpha = _alpha - 5;
      _Y = _Y - 3;
      if(count >= 20)
      {
         _root.DoneWithHighScores = true;
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
      _alpha = _alpha - 5;
      _Y = _Y - 3;
      if(count >= 20)
      {
         _root.DoneWithHighScores = true;
         this.removeMovieClip();
      }
   };
};
SAVEXIT = function()
{
   _root.newstar = false;
   _root.SaveFile();
   _root.starwarpframe = false;
   _root.Blackout();
   onEnterFrame = function()
   {
      if(_root.starwarpframe == true)
      {
         _root.ReturnToTitle();
         this.removeMovieClip();
      }
   };
};
ChangeHL = function(a, hl)
{
   hl = String(Number(hl) + a);
   if(Number(hl) > 3)
   {
      hl = "1";
   }
   else if(Number(hl) < 1)
   {
      hl = "3";
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
