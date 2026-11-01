stop();
oldpage = _root.Page;
_root.Page = Math.min(_parent.TotalPages - 1,_root.Page);
if(oldpage != _root.Page)
{
   if(isNaN(_root.Page))
   {
      _root.Page = 1;
   }
   prevFrame();
}
pagebox.text = _root.Page + 1 + "/" + _parent.TotalPages;
highlighted = "";
_parent.Selected = "";
onEnterFrame = function()
{
   i = 0;
   while(i < Results.length - 1)
   {
      this["Row" + i].gotoAndStop(1);
      i++;
   }
   highlighted = Math.round(_ymouse / 20);
   if(highlighted >= 0 && highlighted <= Results.length - 1)
   {
      this["Row" + highlighted].gotoAndStop(2);
   }
   if(_parent.Selected >= 0 && _parent.Selected <= Results.length - 1)
   {
      this["Row" + _parent.Selected].gotoAndStop(3);
   }
};
onMouseDown = function()
{
   _parent.Selected = "";
   select = Math.round(_ymouse / 20);
   if(select >= 0 && select <= Results.length - 2)
   {
      _parent.Selected = Math.round(_ymouse / 20);
   }
};
Refresh = function()
{
   _root.Page = Math.max(_root.Page,0);
   i = 0;
   while(i < Results.length - 1)
   {
      this["Row" + i].removeMovieClip();
      i++;
   }
   prevFrame();
};
