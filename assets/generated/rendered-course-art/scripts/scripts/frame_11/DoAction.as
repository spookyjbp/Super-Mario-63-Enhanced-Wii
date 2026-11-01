_root.Restartcoins();
coi = _root["RecordCoin" + _root.Playinglevel];
sta = 0;
starname = "";
stardescription = "";
i = 1;
while(i < 7)
{
   if(_root.StarCoin[_root.Playinglevel * 6 - 6 + i] == true)
   {
      sta++;
   }
   i++;
}
starcount = 0;
i = 0;
while(i < 6)
{
   if(_root.Star[_root.Playinglevel * 5 - 5 + i] == true)
   {
      starcount += 1;
   }
   i++;
}
_root.Fluddpow = "";
_root.StartCourse = false;
_root.Selected = 1;
i = 1;
while(i < 6)
{
   if(_root.Star[_root.Playinglevel * 5 - 5 + i] != true)
   {
      break;
   }
   _root.Selected = i + 1;
   _root.Selected = Math.min(_root.Selected,5);
   i++;
}
count = 0;
if(_root.Playinglevel == 1)
{
   _root.kingBombdefeated = false;
}
if(_root.Playinglevel == 4)
{
   _root.kingBoodefeated = false;
   _root.lvl4GreenSwitch = false;
   _root.lvl4YellowSwitch = false;
   _root.lvl4BooMessage = false;
   _root.lvl4keyA = false;
   _root.lvl4keyB = false;
}
if(_root.Playinglevel == 5)
{
   _root.L5VolErupt = false;
   _root.L5VolStar = false;
}
lestpressed = "";
keytimer = 0;
onEnterFrame = function()
{
   keytimer--;
   keytimer = Math.max(keytimer,0);
   if(lestpressed == "r" && !Key.isDown(39))
   {
      lestpressed = "";
   }
   if(lestpressed == "l" && !Key.isDown(37))
   {
      lestpressed = "";
   }
   if(starcount >= 1 && (keytimer <= 0 || lestpressed == ""))
   {
      if(Key.isDown(39) && !Key.isDown(37))
      {
         keytimer = 15;
         lestpressed = "r";
         if(_root.Selected == 1 && _root.Star[_root.Playinglevel * 5 - 5 + 1] == true)
         {
            _root.Selected = _root.Selected + 1;
         }
         else
         {
            i = _root.Selected + 1;
            while(i < 6)
            {
               if(_root.Star[_root.Playinglevel * 5 - 5 + i] == true || _root.Star[_root.Playinglevel * 5 - 5 + (i - 1)] == true)
               {
                  _root.Selected = i;
                  break;
               }
               i++;
            }
         }
      }
      if(Key.isDown(37) && !Key.isDown(39))
      {
         keytimer = 15;
         lestpressed = "l";
         i = _root.Selected - 1;
         loop1:
         while(i > 0)
         {
            switch(i)
            {
               case 1:
                  _root.Selected = 1;
                  break;
               default:
                  if(_root.Star[_root.Playinglevel * 5 - 5 + i] == true || _root.Star[_root.Playinglevel * 5 - 5 + (i - 1)])
                  {
                     _root.Selected = i;
                     break loop1;
                  }
                  break;
               case _root.Selected:
            }
            i--;
         }
      }
   }
   _root.Selected = Math.max(_root.Selected,1);
   _root.Selected = Math.min(_root.Selected,5);
   _root.Coursename.gotoAndStop(_root.Playinglevel);
   _root.star1.starnum = 1;
   _root.star2.starnum = 2;
   _root.star3.starnum = 3;
   _root.star4.starnum = 4;
   _root.star5.starnum = 5;
   _root.FluddH = true;
   _root.FluddR = true;
   _root.FluddT = true;
   if(_root.StartCourse == true)
   {
      if(count == 0)
      {
         _root.Mariosound = new Sound(this);
         if(_root.CurrentPlayer == "Luigi")
         {
            _root.Mariosound.attachSound("Luigi - Okey");
         }
         if(_root.CurrentPlayer == "Mario")
         {
            _root.Mariosound.attachSound("Mario-LetsGo");
         }
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
         _root.attachMovie("Whiteout","Transition",_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      count++;
      if(count > 32)
      {
         _root.Starnamenum = _root.Playinglevel * 5 - 5 + _root.Starnum;
         _root.Transition.removeMovieClip(_root.Transition);
         _root.Restartcoins();
         _root.StartNewCourse(_root.Playinglevel + "-1",false,"Fromwhite");
      }
   }
};
