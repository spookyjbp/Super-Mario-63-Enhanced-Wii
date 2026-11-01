widt = 300;
heigh = 180;
clip.gotoAndStop(Number(_root.LDItemArray[itemnum][0]));
itemname = _root.ItemNameArray[_root.LDItemArray[itemnum][0]];
mess = unescape(_root.LDItemArray[itemnum][3]);
OEF = function()
{
   _root.LDItemArray[itemnum][3] = escape(mess);
   xnum = Math.round(_root.LDItemArray[itemnum][1]);
   ynum = Math.round(_root.LDItemArray[itemnum][2]);
};
