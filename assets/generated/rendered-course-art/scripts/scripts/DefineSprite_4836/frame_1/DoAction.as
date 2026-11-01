count = 0;
if(_root.CharHP <= 0)
{
   _root.CharHP = 8;
   _root.Course.Char.attack = false;
}
onEnterFrame = function()
{
   _root.Course.BackGFX.StartClip._y = -50;
   if(count < 128 || _root.PauseGame == false)
   {
      count++;
   }
   if(count > 96)
   {
      if(_root.PauseGame == false)
      {
         if(count == 128)
         {
            _root.StartBossMusic();
         }
         if(count > 128)
         {
            diff = _root.currentCamY - (_Y + _parent._y);
            diff = Math.min(0,diff);
            diff = Math.max(-1000,diff);
            _Y = _Y - (0.3 - diff / 100);
            _Y = Math.max(_Y,-1100);
         }
         else if(count % 2 == 0)
         {
            _root.MoveAllPlats(3,0);
         }
         else
         {
            _root.MoveAllPlats(-3,0);
         }
      }
   }
   else
   {
      _root.Controll = false;
      _root.PauseGame = false;
      if(count > 32)
      {
         _root.Camspeed = 15;
         _root.MaxCamspeed = 25;
         _root.Camcenter = _root.Course.BackGFX.Target1;
      }
   }
   if(count == 32)
   {
      _root.StopBGsong();
   }
   if(count == 128)
   {
      _root.Controll = true;
      _root.Camcenter = "none";
   }
};
