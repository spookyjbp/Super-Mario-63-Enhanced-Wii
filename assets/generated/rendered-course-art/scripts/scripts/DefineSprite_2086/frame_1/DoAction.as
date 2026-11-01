stop();
lastcoin = 0;
percent = 0;
onEnterFrame = function()
{
   lastcoin++;
   if(percent < _root.percentloaded && percent < 87 && lastcoin >= 2)
   {
      lastcoin = 0;
      percent++;
      depth = _root.percentloaded;
      attachMovie("PreloaderCoin","Coin" + _root.percentloaded,this.getNextHighestDepth(),{_x:500,_y:210});
   }
   if(_root.percentloaded >= 100 && _root.TotalCoins >= 100)
   {
      onEnterFrame = null;
      play();
   }
};
Skip = function()
{
   if(_root.percentloaded >= 100)
   {
      onEnterFrame = null;
      _root.play();
      Skip = null;
   }
};
