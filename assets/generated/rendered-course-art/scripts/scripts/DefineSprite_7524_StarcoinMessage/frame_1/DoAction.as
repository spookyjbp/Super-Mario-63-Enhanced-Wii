mess = "You have collected " + _root.TotalStarCoins + " Star Coins.";
if(_root.TotalStarCoins == 1)
{
   mess = "You have collected your First Star Coin!!";
}
else if(_root.TotalStarCoins == 5)
{
   mess = "With 5 Star Coins, you have just unlocked the <b>LEVEL DESIGNER!!</b>  Go check it out any time soon.  It\'s on the Right Wing of the Castle.";
}
else if(_root.TotalStarCoins == 10)
{
   mess = "Yay! Double Digits! You\'ve just unlocked the <b>Grass Tileset</b> for the Level Designer!";
}
else if(_root.TotalStarCoins == 13)
{
   mess = "By collecting 13 Star Coins,  a Time Paradox appeared;  you now have -3 Shine Sprites...  Just kidding... Instead, you\'ve unlocked the <b>Cave Tileset</b>!!";
}
else if(_root.TotalStarCoins == 16)
{
   mess = "You\'ve collected 16 Star Coins.  You win a <b>special prize</b>... come over to the hallway right of the Level Designer to pick it up! ";
}
else if(_root.TotalStarCoins == 21)
{
   mess = "The 21st Star Coin comes with a bonus!  You have unlocked the <b>Snow Tilesets</b> for the Level Designer!!";
}
else if(_root.TotalStarCoins == 25)
{
   mess = "Merry Christmas!!!  You have 25 Star Coins now, and with it, comes laughter, joy, and contentment.  But most importantly, the <b>Volcano Tileset!!</b>...";
}
else if(_root.TotalStarCoins == 28)
{
   mess = "With 28 Star Coins, you have unlocked the Tileset titled <b>Desert</b> for the Level Designer!!";
}
else if(_root.TotalStarCoins == 32)
{
   mess = "You now have collected half the Star Coins in the game!  For it, you receive a <b><FONT COLOR=\"#00FF00\">Special Prize!!</FONT></b>  Check out the hallway to the right of the Level Designer to see what <i>it</i> is.";
}
else if(_root.TotalStarCoins == 38)
{
   mess = "With 38 Star Coins, you have unlocked the <b>Castle Tileset</b> for the Level Designer!!";
}
else if(_root.TotalStarCoins == 43)
{
   mess = "43 Star Coins lets you use the <b>Other Tileset</b> in the Level Designer!";
}
else if(_root.TotalStarCoins == 48)
{
   mess = "With 48 Star Coins, you receive a <b>third prize!!!</b>  ";
}
else if(_root.TotalStarCoins == 55)
{
   mess = "You have collected 55 Star Coins!!  Less than 10 left until you have every single one!!";
}
else if(_root.TotalStarCoins == 63)
{
   mess = "You have collected 63 Star Coins!!  <b>One More!!</b>";
}
else if(_root.TotalStarCoins == 64)
{
   mess = "Oh Dear!! It appears as if you have <b>all 64 Star Coins!!! </b>  Every single one!!!  For this, you have unlocked your <b>final special prize!!</b>  You can collect it in the hallway to the right of the Level Designer!!";
}
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
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("ItemAppears");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
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
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("ItemAppears");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
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
