_root.Restartcoins();
coi = _root["RecordCoin" + (_root.Playinglevelnum + 8)];
sta = 0;
starname = "";
stardescription = "";
i = 1;
while(i < 4)
{
   if(_root.StarCoin[_root.Playinglevelnum * 3 - 3 + i + 49] == true)
   {
      sta++;
   }
   i++;
}
starcount = 0;
i = 0;
while(i < 4)
{
   if(_root.Star[_root.Playinglevelnum * 3 + 51 + i] == true)
   {
      starcount += 1;
   }
   i++;
}
_root.Fluddpow = "";
_root.StartCourse = false;
_root.Selected = 1;
i = 1;
while(i < 4)
{
   if(_root.Star[_root.Playinglevelnum * 3 + 51 + i] != true)
   {
      break;
   }
   _root.Selected = i + 1;
   _root.Selected = Math.min(_root.Selected,3);
   i++;
}
count = 0;
if(_root.Playinglevelnum == 1)
{
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
         if(_root.Selected == 1 && _root.Star[_root.Playinglevelnum * 3 + 51 + 1] == true)
         {
            _root.Selected = _root.Selected + 1;
         }
         else
         {
            i = _root.Selected + 1;
            while(i < 4)
            {
               if(_root.Star[_root.Playinglevelnum * 3 + 51 + i] == true || _root.Star[_root.Playinglevelnum * 3 + 51 + (i - 1)] == true)
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
                  if(_root.Star[_root.Playinglevelnum * 3 + 51 + i] == true || _root.Star[_root.Playinglevelnum * 3 + 51 + (i - 1)])
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
   _root.Selected = Math.min(_root.Selected,3);
   _root.Coursename.gotoAndStop(_root.Playinglevelnum);
   _root.star1.starnum = 1;
   _root.star2.starnum = 2;
   _root.star3.starnum = 3;
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
         _root.Starnamenum = _root.Playinglevelnum * 3 + 51 + _root.Starnum;
         _root.Transition.removeMovieClip(_root.Transition);
         _root.Restartcoins();
         if(_root.Playinglevelnum == 2)
         {
            _root["M2-1WaterLevel"] = -200;
         }
         _root.StartNewCourse("M" + _root.Playinglevelnum + "-1",false,"Fromwhite");
      }
   }
};
