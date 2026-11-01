frame = 1;
attackFrame = "";
attack = false;
air = false;
idlecount = 0;
HP = 3;
tall = 60;
wide = 60;
yspeed = 0;
xspeed = 0;
blink = 0;
gettingbackoncourse = false;
OEF = function()
{
   _root.Camcenter = _root.Course.FrontGFX.Bowser;
   _root.PauseGame = false;
};
if(difficulty == 1)
{
   Message1 = function()
   {
      _root.CinemaMessageFrame = 3;
      _root.CinemaMessage = "<b><i>Bowser:</i></b> Tough Luck! Looks like the Princess isn\'t here... Gwa ha ha!! Go ahead! Just try to grab me by the tail!";
   };
   Message2 = function()
   {
      _root.CinemaMessage = "<b><i>Bowser:</i></b> A wimp like you could never dive into my tail to grab it and swing me around by pressing Right and Left repeatedly!";
   };
}
else if(difficulty == 2)
{
   Message1 = function()
   {
      _root.CinemaMessageFrame = 3;
      _root.CinemaMessage = "<b><i>Bowser:</i></b> Hahaha!! Mario, You think by appearing here that you\'d be able to stop my Master Plan!!";
   };
   Message2 = function()
   {
      clip.gotoAndStop("Orb");
      _root.CinemaMessage = "<b><i>Bowser:</i></b> I\'ve retrieved what I needed down here, and was just about to leave when you showed up.";
   };
   Message3 = function()
   {
      _root.CinemaMessage = "<b><i>Bowser:</i></b> I was getting worried you caught onto my plan, and was trying to stop me;  but of course ";
   };
   Message4 = function()
   {
      _root.CinemaMessage = "<b><i>Bowser:</i></b> your ignorance has lead you straight into another trap of mine.  Now, with you here,";
   };
   Message5 = function()
   {
      clip.frame.gotoAndPlay("end");
      _root.CinemaMessage = "<b><i>Bowser:</i></b> I can go straight ahead to the next step in my Master Plan... which is to ..... DESTROY YOU!";
   };
}
messcount = 0;
loopcode = function()
{
   messcount++;
   if(this["Message" + messcount] !== undefined)
   {
      this["Message" + messcount]();
      lastframeZ = _root.KeyZ();
      onEnterFrame = function()
      {
         OEF();
         if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
         {
            _root.CinemaMessage = "";
            loopcode();
         }
         lastframeZ = _root.KeyZ();
      };
   }
   else
   {
      loopcode = null;
      end();
   }
};
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Controll = false;
      if(_root.Course.Char.air == false)
      {
         gotoAndStop(3);
         onEnterFrame = function()
         {
            if(_root.Course.Char.air == false)
            {
               _root.Course.Char.attack = true;
               _root.Course.Char.attackFrame = "bosscinema";
               _root.Course.Char._xscale = -100;
               _root.playpause = true;
               loopcode();
            }
         };
      }
   }
};
end = function()
{
   if(difficulty == 2)
   {
      clip.frame.gotoAndPlay("end");
   }
   count = 0;
   onEnterFrame = function()
   {
      count++;
      if(count > 10)
      {
         clip.gotoAndStop(1);
         _root.Course.Char.attack = false;
         _root.playpause = false;
         onEnterFrame = null;
         _root.Camcenter = "none";
         nextFrame();
      }
   };
};
