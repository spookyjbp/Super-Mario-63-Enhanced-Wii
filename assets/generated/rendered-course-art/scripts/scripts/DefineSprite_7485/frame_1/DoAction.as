hi = "Highscore:  " + _root["RecordCoin" + _root.lastPlayinglevel];
stop();
count = 0;
maxnum = _root.TotalCoins;
n = "x 0";
coinsound = new Sound();
coinsound.attachSound("Coin (Small)");
onEnterFrame = function()
{
   if(_parent.stopnum == _parent._currentframe)
   {
      if(count < maxnum)
      {
         count++;
         if(count % 2 == 0)
         {
            coinsound.start();
         }
         n = "x " + count;
      }
      else
      {
         coinsound2 = new Sound();
         coinsound2.attachSound("CoinJingle");
         coinsound2.start();
         count = maxnum;
         n = "x " + count;
         if(_root.TotalCoins > _root["RecordCoin" + _root.lastPlayinglevel] && _parent.level !== "Secret Course")
         {
            this.play();
         }
         else
         {
            _parent.play();
         }
         onEnterFrame = null;
      }
   }
};
if(_parent.level == "Secret Course")
{
   gotoAndStop("nohighscore");
}
