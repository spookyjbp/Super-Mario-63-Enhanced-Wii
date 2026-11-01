widt = 270;
heigh = 220;
coursetext = "";
_root.LoadLDCourse(0);
file1 = _root.level1Name + "\n" + "x:" + _root.level1GridX + "  y:" + _root.level1GridY;
file1BG = _root.level1BG;
file2 = _root.level2Name + "\n" + "x:" + _root.level2GridX + "  y:" + _root.level2GridY;
file2BG = _root.level2BG;
file3 = _root.level3Name + "\n" + "x:" + _root.level3GridX + "  y:" + _root.level3GridY;
file3BG = _root.level3BG;
LoadCourse = function(filenum)
{
   _root.LDMenuFrame = 1;
   if(filenum == "code")
   {
      Text = coursetext;
      Text2 = Text.split(" ");
      Text3 = "";
      i = 0;
      while(i < Text2.length)
      {
         Text3 += String(Text2[i]);
         i++;
      }
      Text = Text3;
      linebreak = String("/n");
      Text2 = Text.split(String.fromCharCode(13));
      Text3 = "";
      i = 0;
      while(i < Text2.length)
      {
         Text3 += String(Text2[i]);
         i++;
      }
      Text = Text3;
      _root.LoadCourseFromCode(Text);
   }
};
FLoad = function(filenum)
{
   _root.LDMenuFrame = 1;
   trace("should load file " + filenum);
   _root.LoadCourseFromSave(filenum);
};
