stop();
sinwave = 0;
yspeed = 0;
xspeed = 0;
frame = "Idle";
aiwait = 0;
aicount = 0;
targety = _Y;
targetx = _X;
attack = false;
diffcounter = 0;
diffcounter2 = true;
hp = 3;
OEF = function()
{
   _root.Camcenter = _root.Course.FrontGFX.Bowser;
   _root.PauseGame = false;
   _root.Camcenter = _root.Course.BackGFX.box3;
};
Message1 = function()
{
   clip.gotoAndStop("Talk");
   _root.CinemaMessageFrame = 3;
   _root.CinemaMessage = "<b><i>Bowser:</i></b> Hahaha!! Mario, You think that you can actually stop my Master Plan!!";
};
Message2 = function()
{
   _root.CinemaMessage = "<b><i>Bowser:</i></b> The Meteor is almost ready to launch.";
};
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
if(_root.lvl8keyE == false)
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         _root.Controll = false;
         if(_root.Course.Char.air == false)
         {
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
}
else
{
   count = 0;
   onEnterFrame = function()
   {
      count++;
      if(count == 3)
      {
         _Y = _Y + 1500;
         _root.Course.BackGFX.nextFrame();
         _root.Course.BackGFX.nextFrame();
         _root.Course.FrontGFX.nextFrame();
         _root.Course.Platforms.nextFrame();
      }
   };
}
end = function()
{
   _root.Camcenter = "None";
   count = 0;
   onEnterFrame = function()
   {
      count++;
      if(count > 10)
      {
         clip.gotoAndStop("Idle");
         _root.Course.Char.attack = false;
         _root.playpause = false;
         onEnterFrame = null;
         _root.Camcenter = "none";
         _root.StopBGsong();
         _root.StartBossMusic();
         gotoAndStop(3);
      }
   };
};
