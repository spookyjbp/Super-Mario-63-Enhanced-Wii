_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad to Left:</i>  ......!!!!... You look <i>exhausted</i>!!!";
lastframeZ = _root.KeyZ();
toadrun = false;
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         if(toadrun == true)
         {
            onEnterFrame = null;
            play();
         }
      };
   }
   lastframeZ = _root.KeyZ();
};
