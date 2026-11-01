_root.TotalStars = 0;
_root.Cinema = false;
_root.CheatCastleRestore = true;
_root.StartChar("8-E1-1",0,0,0,0,100,_root.CurrentPlayer,"","StarIn",true);
_root.onEnterFrame = function()
{
   _root.CharCode();
};
_root.gotoAndStop("Courseframe");
