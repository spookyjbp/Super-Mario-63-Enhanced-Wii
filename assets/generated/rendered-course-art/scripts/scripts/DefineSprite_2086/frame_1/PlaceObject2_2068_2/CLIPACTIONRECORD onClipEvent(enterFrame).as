onClipEvent(enterFrame){
   if(_X > 330 && _X < 370 && _root.TotalCoins >= 90)
   {
      _X = _X + (350 - _X) / 5;
   }
   else
   {
      _X = _X + 4;
      if(_X > 500)
      {
         _X = -50;
      }
   }
   if(_root.TotalCoins >= 25 && _currentframe <= 15 && _root.block1 == false && _X > 80 && _X < 100)
   {
      _root.block1 = true;
      _parent.block1.gotoAndPlay(2);
      gotoAndStop("jump");
      play();
   }
   if(_root.TotalCoins >= 50 && _currentframe <= 15 && _root.block2 == false && _X > 205 && _X < 225)
   {
      _root.block2 = true;
      _parent.block2.play();
      gotoAndStop("jump");
      play();
   }
   if(_root.TotalCoins >= 75 && _currentframe <= 15 && _root.block3 == false && _X > 330 && _X < 350)
   {
      _root.block3 = true;
      _parent.block3.gotoAndPlay(2);
      gotoAndStop("jump");
      play();
   }
   if(_root.block3 == true && _root.TotalCoins >= 90 && _X > 330 && _X < 370 && _currentframe <= 17)
   {
      gotoAndStop("end");
   }
   if(_root.TotalCoins >= 90 && _root.percentloaded - _root.TotalCoins >= 0 && _root.block3count < 10 && _X > 330 && _X < 370 && (_currentframe <= 17 || _currentframe >= 32))
   {
      _root.block3count += 1;
      _parent.block3.gotoAndPlay(2);
      gotoAndStop("jump");
      play();
   }
}
