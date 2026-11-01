step = 0;
code = "cheat";
Complete = function()
{
   trace("Cheat menu activated!");
   if(_parent.clicked !== true)
   {
      _parent.TransitionF("Cheat");
   }
};
var keyListener = new Object();
keyListener.onKeyDown = function()
{
   if(code.charAt(step).toLowerCase() == chr(Key.getAscii()).toLowerCase())
   {
      step++;
      if(step >= code.length)
      {
         Complete();
         step = 0;
      }
   }
   else
   {
      step = 0;
   }
};
keyListener.onKeyUp = function()
{
};
Key.addListener(keyListener);
