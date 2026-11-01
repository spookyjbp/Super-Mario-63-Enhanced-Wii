stop();
_root.CharHP = 8;
_root.WaterHP = 8;
_root.StartChar("GameOver",0,0,0,0,100,_root.CurrentPlayer,_root.Fluddpow,"StarIn");
onEnterFrame = function()
{
   _root.nullCam = true;
   _root.CharCode();
};
