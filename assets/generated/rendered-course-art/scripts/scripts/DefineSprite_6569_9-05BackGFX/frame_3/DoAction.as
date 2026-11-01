stop();
_root.AreaTextClipF("Castle Secret Star:\r<i>Secret of the Sky</i>",45);
onEnterFrame = function()
{
   if(_root.Course.Char.attackFrame == "Star")
   {
      nextFrame();
      onEnterFrame = null;
   }
};
