stop();
onEnterFrame = function()
{
   if(_root.Course.Char.air == false)
   {
      _root.playpause = true;
      onEnterFrame = function()
      {
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "bosscinema";
         _root.Camcenter = _root.Course.FrontGFX.Bowser;
         _root.PauseGame = false;
         count = 0;
         _root.CinemaMessageFrame = 3;
         _root.CinemaMessage = "<b><i>Bowser:</i></b> Gwaahhhhh......";
         onEnterFrame = function()
         {
            _root.playpause = true;
            if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
            {
               _root.CinemaMessage = "";
               count = 0;
               onEnterFrame = function()
               {
                  count++;
                  if(count > 10)
                  {
                     play();
                     onEnterFrame = function()
                     {
                        _root.Camcenter = _root.Course.FrontGFX.Bowser;
                        _root.PauseGame = false;
                     };
                     _root.StopBGsong();
                  }
               };
            }
         };
      };
   }
};
