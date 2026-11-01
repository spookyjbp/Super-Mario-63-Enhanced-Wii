stop();
onEnterFrame = function()
{
   frame = Math.round((- (_root.Course.Char._y - _root.Course.BackGFX._y)) / 4);
   frame = Math.min(frame,300);
   frame = Math.max(frame,1);
   gotoAndStop(frame);
};
