_root.PlayLevelFromCode = function(code)
{
   _root.TileArray = [];
   _root.GridXSize = 50;
   _root.GridYSize = 30;
   _root.LDItemArray = [];
   avar = "";
   code = code.split("~");
   _root.GridXSize = Number(code[0].split("x")[0]);
   _root.GridYSize = Number(code[0].split("x")[1]);
   _root.LDMusic = Number(code[3]);
   _root.LDCourseBG = Number(code[4]);
   _root.LDCourseName = unescape(String(code[5]));
   _root.TileArray = _root.ExpandNumbers(code[1]);
   _root.LDItemArray = code[2].split("|");
   i = 0;
   while(i < _root.LDItemArray.length)
   {
      _root.LDItemArray[i] = _root.LDItemArray[i].split(",");
      i++;
   }
   MaxX = _root.GridXSize;
   MaxY = _root.GridYSize;
   _root.TestLevel();
};
_root.EraseLevel();
_root.LoadCourseFromCode(_root.LoadedCode);
