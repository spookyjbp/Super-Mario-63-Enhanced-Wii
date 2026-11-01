aa = 0;
_alpha = 0;
onEnterFrame = function()
{
   _alpha = _alpha + (aa - _alpha) / 4;
};
