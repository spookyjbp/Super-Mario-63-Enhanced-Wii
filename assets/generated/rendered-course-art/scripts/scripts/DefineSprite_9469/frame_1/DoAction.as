Song1Intro = "FileSelectSong-Intro";
Song1Repeat = "FileSelectSong-Repeat";
Song1Name = "File Select";
Song1Info = "Remixed by HouseMasta (see credits)";
Song2Intro = "PrincessPeachesCastle-Intro";
Song2Repeat = "PrincessPeachesCastle-Repeat";
Song2Name = "Castle Grounds";
Song2Info = "Ripped from Super Smash Bros. Melee";
Song3Repeat = "InsidetheCastleWalls";
Song3Name = "Inside the Castle Walls";
Song3Info = "Ripped from Super Mario 64";
Song4Intro = "Bombomb Battlefield Intro";
Song4Repeat = "Bombomb Battlefield Repeat";
Song4Name = "Bobomb Battlefield";
Song4Info = "Ripped from Super Mario 64";
Song5Intro = "Gritzy Desert (SSBB) Intro";
Song5Repeat = "Gritzy Desert (SSBB) Repeat";
Song5Name = "Sinking Sand Land";
Song5Info = "Ripped from Super Smash Bros. Brawl";
Song6Intro = "Snowmans Land - Intro";
Song6Repeat = "Snowmans Land - Repeat";
Song6Name = "Snowmans Land";
Song6Info = "Ripped from Super Mario 64";
Song7Intro = "HazyMazeCave-Intro";
Song7Repeat = "HazyMazeCave-Rock";
Song7Name = "Hazy Maze Cave";
Song7Info = "Ripped from Super Mario 64";
Song8Repeat = "HazyMazeCave-Haze";
Song8Name = "Hazy Maze Cave - Haze";
Song8Info = "Ripped from Super Mario 64";
Song9Intro = "Boos Mansion - Intro";
Song9Repeat = "Boos Mansion - Repeat";
Song9Name = "Boos Mansion";
Song9Info = "Ripped from Super Mario Galaxy";
Song10Intro = "Melty Molten Galaxy - Intro";
Song10Repeat = "Melty Molten Galaxy - Repeat";
Song10Name = "Lethal Lava Land";
Song10Info = "Ripped from Super Mario Galaxy";
Song11Intro = "Rainbow Ride - Intro";
Song11Repeat = "Rainbow Ride - Repeat";
Song11Name = "Rainbow Ride";
Song11Info = "Ripped from Super Smash Bros Melee";
Song12Intro = "Secret Course - Intro";
Song12Repeat = "Secret Course - Repeat";
Song12Name = "Secret Course";
Song12Info = "Remixed by m477zorz (see Credits)";
Song13Intro = "SuperMarioGalaxyPurpleCoin-Intro";
Song13Repeat = "SuperMarioGalaxyPurpleCoin-Repeat";
Song13Name = "Purple Coin Theme";
Song13Info = "Ripped from Super Mario Galaxy";
Song14Intro = "Boss Music 64 - Intro";
Song14Repeat = "Boss Music 64 - Repeat";
Song14Name = "Boss Theme";
Song14Info = "Ripped from Super Mario 64";
Song15Repeat = "Boss Music";
Song15Name = "Run! Run... Run!!!!";
Song15Info = "Ripped from Super Mario Galaxy";
Song16Intro = "WingCap - Intro";
Song16Repeat = "WingCap - Repeat";
Song16Name = "Invisiblility";
Song16Info = "Ripped from Super Mario 64";
Song17Intro = "MetalCap - Intro";
Song17Repeat = "MetalCap - Repeat";
Song17Name = "Metal Cap Theme";
Song17Info = "Ripped from Super Mario 64";
Song18Intro = "WingCap - Intro";
Song18Repeat = "WingCap - Repeat";
Song18Name = "Invincibility";
Song18Info = "Ripped from Super Mario 64";
Song19Repeat = "EndlessStairs-Repeat";
Song19Name = "Endless Stairs";
Song19Info = "Ripped from Super Mario 64";
Song20Intro = "BowserPlan-Intro";
Song20Repeat = "BowserPlan-Repeat";
Song20Name = "Bowsers Plan";
Song20Info = "Ripped from Super Mario Galaxy";
Song21Intro = "BowserTrapIntro";
Song21Repeat = "BowserTrapRepeat";
Song21Name = "Bowsers Trap";
Song21Info = "Remixed by ParagonX9  (see Credits)";
Song22Intro = "Bowsers Theme - Intro";
Song22Repeat = "Bowsers Theme - Repeat";
Song22Name = "Bowsers Theme";
Song22Info = "Ripped from Super Mario 64";
Song23Intro = "BuoyBaseGalaxy(Dry)-Intro";
Song23Repeat = "BuoyBaseGalaxy(Dry)-Repeat";
Song23Name = "Floater Land";
Song23Info = "Ripped from Super Mario Galaxy";
Song24Intro = "Airship Theme Intro";
Song24Repeat = "Airship Theme Repeat";
Song24Name = "Terror Theme";
Song24Info = "Ripped from Super Smash Bros Brawl";
Song25Intro = "Super Mario Galaxy Bowser Battle 2 - Intro";
Song25Repeat = "Super Mario Galaxy Bowser Battle 2 - Repeat";
Song25Name = "The Final Battle";
Song25Info = "Ripped from Super Mario Galaxy";
Song26Intro = "Super Mario Galaxy Bowser Battle - Intro";
Song26Repeat = "Super Mario Galaxy Bowser Battle - Repeat";
Song26Name = "The Meteor";
Song26Info = "Ripped from Super Mario Galaxy";
Song27Intro = "Super Mario 64 Ending Theme";
Song27Repeat = "";
Song27Name = "Credits";
Song27Info = "Ripped from Super Mario 64";
if(_parent.Songnum == undefined)
{
   _parent.Songnum = 1;
}
Mode = "none";
PlaySong = function()
{
   a = this["Song" + _parent.Songnum + "Intro"];
   b = this["Song" + _parent.Songnum + "Repeat"];
   if(a == undefined)
   {
      a = b;
   }
   _root.StartNewMusicAndIntroNoVar(a,b);
};
StopSong = function()
{
   _root.StopBGsong();
};
onEnterFrame = function()
{
   Songname = _parent.Songnum + ": " + this["Song" + _parent.Songnum + "Name"];
   Songinfo = this["Song" + _parent.Songnum + "Info"];
};
