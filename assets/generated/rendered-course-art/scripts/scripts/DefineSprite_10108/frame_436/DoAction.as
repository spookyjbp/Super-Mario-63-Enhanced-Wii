_root.Restartcoins();
_root.CharXscale = 100;
_root.CheatCastleRestore = false;
_root.StartChar("Castle",80,60,80,60,100,_root.CurrentPlayer,"","StarIn");
_root.onEnterFrame = function()
{
   _root.CharCode();
};
_root.gotoAndStop("Courseframe");
