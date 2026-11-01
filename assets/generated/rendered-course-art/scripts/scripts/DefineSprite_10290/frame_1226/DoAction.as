_root.Restartcoins();
_root.CharXscale = 100;
_root.StartChar("Castle",450,80,450,80,100,_root.CurrentPlayer,"","StarIn");
_root.onEnterFrame = function()
{
   _root.CharCode();
};
_root.gotoAndStop("Courseframe");
