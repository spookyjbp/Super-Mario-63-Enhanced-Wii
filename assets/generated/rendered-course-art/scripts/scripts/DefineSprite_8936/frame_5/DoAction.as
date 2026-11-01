widt = 270;
heigh = 220;
coursetext = _root.GetCode();
_root.LoadLDCourse(0);
file1 = _root.level1Name + "\n" + "x:" + _root.level1GridX + "  y:" + _root.level1GridY;
file1BG = _root.level1BG;
file2 = _root.level2Name + "\n" + "x:" + _root.level2GridX + "  y:" + _root.level2GridY;
file2BG = _root.level2BG;
file3 = _root.level3Name + "\n" + "x:" + _root.level3GridX + "  y:" + _root.level3GridY;
file3BG = _root.level3BG;
FSave = function(filenum)
{
   trace("should save to file " + filenum);
   _root.SaveLDCourse(filenum);
   _root.LoadLDCourse(0);
   file1 = _root.level1Name + "\n" + "x:" + _root.level1GridX + "  y:" + _root.level1GridY;
   file1BG = _root.level1BG;
   file2 = _root.level2Name + "\n" + "x:" + _root.level2GridX + "  y:" + _root.level2GridY;
   file2BG = _root.level2BG;
   file3 = _root.level3Name + "\n" + "x:" + _root.level3GridX + "  y:" + _root.level3GridY;
   file3BG = _root.level3BG;
   _root.LoadCourseFromSave(filenum);
};
