_root.File = "A";
_root.SaveFile = function()
{
   trace("File " + _root.File + " Saved!");
   _root.savedfiles = SharedObject.getLocal("SuperMarioSunshine128SavedFile");
   i = 0;
   while(i < 4)
   {
      if(i == 0)
      {
         file = "A";
      }
      if(i == 1)
      {
         file = "B";
      }
      if(i == 2)
      {
         file = "C";
      }
      if(i == 3)
      {
         file = "D";
      }
      if(_root.File == file)
      {
         _root["File" + file + "Star"] = deepObjectCopy(_root.Star);
         _root["File" + file + "Fludd"] = deepObjectCopy(_root.FluddArray);
         _root["File" + file + "StarCoin"] = deepObjectCopy(_root.StarCoin);
         _root["File" + file + "BowserKey1"] = deepObjectCopy(_root.BowserKey1);
         _root["File" + file + "BowserKey2"] = deepObjectCopy(_root.BowserKey2);
         _root["File" + file + "BowserKey3"] = deepObjectCopy(_root.BowserKey3);
         _root["File" + file + "DoorOpenedArray"] = deepObjectCopy(_root.DoorOpenedArray);
      }
      else
      {
         _root["File" + file + "Star"] = deepObjectCopy(_root.savedfiles.data["File" + file + "Star"]);
         _root["File" + file + "Fludd"] = deepObjectCopy(_root.savedfiles.data["File" + file + "Fludd"]);
         _root["File" + file + "StarCoin"] = deepObjectCopy(_root.savedfiles.data["File" + file + "StarCoin"]);
         _root["File" + file + "BowserKey1"] = deepObjectCopy(_root.savedfiles.data["File" + file + "BowserKey1"]);
         _root["File" + file + "BowserKey2"] = deepObjectCopy(_root.savedfiles.data["File" + file + "BowserKey2"]);
         _root["File" + file + "BowserKey3"] = deepObjectCopy(_root.savedfiles.data["File" + file + "BowserKey3"]);
         _root["File" + file + "DoorOpenedArray"] = deepObjectCopy(_root.savedfiles.data["File" + file + "DoorOpenedArray"]);
      }
      i++;
   }
   i = 0;
   while(i < 4)
   {
      if(i == 0)
      {
         file = "A";
      }
      if(i == 1)
      {
         file = "B";
      }
      if(i == 2)
      {
         file = "C";
      }
      if(i == 3)
      {
         file = "D";
      }
      _root.savedfiles.data["File" + file + "Star"] = deepObjectCopy(_root["File" + file + "Star"]);
      _root.savedfiles.data["File" + file + "Fludd"] = deepObjectCopy(_root["File" + file + "Fludd"]);
      _root.savedfiles.data["File" + file + "StarCoin"] = deepObjectCopy(_root["File" + file + "StarCoin"]);
      _root.savedfiles.data["File" + file + "BowserKey1"] = deepObjectCopy(_root["File" + file + "BowserKey1"]);
      _root.savedfiles.data["File" + file + "BowserKey2"] = deepObjectCopy(_root["File" + file + "BowserKey2"]);
      _root.savedfiles.data["File" + file + "BowserKey3"] = deepObjectCopy(_root["File" + file + "BowserKey3"]);
      _root.savedfiles.data["File" + file + "DoorOpenedArray"] = deepObjectCopy(_root["File" + file + "DoorOpenedArray"]);
      i++;
   }
   _root.savedfiles.data.FileExists = true;
   _root.savedfiles.flush();
   _root.CalculateFilesInfo();
};
_root.LoadFile = function()
{
   _root.savedfiles = SharedObject.getLocal("SuperMarioSunshine128SavedFile");
   i = 0;
   while(i < 4)
   {
      if(i == 0)
      {
         file = "A";
      }
      if(i == 1)
      {
         file = "B";
      }
      if(i == 2)
      {
         file = "C";
      }
      if(i == 3)
      {
         file = "D";
      }
      _root["File" + file + "Star"] = deepObjectCopy(_root.savedfiles.data["File" + file + "Star"]);
      _root["File" + file + "Fludd"] = deepObjectCopy(_root.savedfiles.data["File" + file + "Fludd"]);
      _root["File" + file + "StarCoin"] = deepObjectCopy(_root.savedfiles.data["File" + file + "StarCoin"]);
      _root["File" + file + "BowserKey1"] = deepObjectCopy(_root.savedfiles.data["File" + file + "BowserKey1"]);
      _root["File" + file + "BowserKey2"] = deepObjectCopy(_root.savedfiles.data["File" + file + "BowserKey2"]);
      _root["File" + file + "BowserKey3"] = deepObjectCopy(_root.savedfiles.data["File" + file + "BowserKey3"]);
      _root["File" + file + "DoorOpenedArray"] = deepObjectCopy(_root.savedfiles.data["File" + file + "DoorOpenedArray"]);
      i++;
   }
   _root.Star = deepObjectCopy(_root["File" + _root.File + "Star"]);
   _root.FluddArray = deepObjectCopy(_root["File" + _root.File + "Fludd"]);
   _root.StarCoin = deepObjectCopy(_root["File" + _root.File + "StarCoin"]);
   _root.BowserKey1 = deepObjectCopy(_root["File" + _root.File + "BowserKey1"]);
   _root.BowserKey2 = deepObjectCopy(_root["File" + _root.File + "BowserKey2"]);
   _root.BowserKey3 = deepObjectCopy(_root["File" + _root.File + "BowserKey3"]);
   _root.DoorOpenedArray = deepObjectCopy(_root["File" + _root.File + "DoorOpenedArray"]);
   if(_root.DoorOpenedArray == undefined)
   {
      _root.DoorOpenedArray = [];
      i = 0;
      while(i < 14)
      {
         _root.DoorOpenedArray.push(false);
         i++;
      }
   }
   _root.CalculateStars();
   _root.CalculateStarCoins();
   _root.CalculateFilesInfo();
   i = 1;
   while(i < 12)
   {
      _root["RecordCoin" + i] = _root.savedfiles.data["RecordCoin" + i];
      i++;
   }
   _root.savedfiles.flush();
};
_root.DeleteFile = function(file)
{
   _root.LoadFile();
   if(file == undefined)
   {
      _root.Star = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.FluddArray = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.FileAStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.FileBStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.FileCStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.FileDStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.FileAFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.FileBFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.FileCFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.FileDFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.savedfiles.data.FileAStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileBStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileCStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileDStar = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileAFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.savedfiles.data.FileBFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.savedfiles.data.FileCFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.savedfiles.data.FileDFludd = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.savedfiles.data.FileAStarCoin = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileBStarCoin = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileCStarCoin = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data.FileDStarCoin = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.BowserKey1 = false;
      _root.BowserKey2 = false;
      _root.BowserKey3 = false;
      _root.savedfiles.data.FileABowserKey1 = false;
      _root.savedfiles.data.FileBBowserKey1 = false;
      _root.savedfiles.data.FileCBowserKey1 = false;
      _root.savedfiles.data.FileDBowserKey1 = false;
      _root.savedfiles.data.FileABowserKey2 = false;
      _root.savedfiles.data.FileBBowserKey2 = false;
      _root.savedfiles.data.FileCBowserKey2 = false;
      _root.savedfiles.data.FileDBowserKey2 = false;
      _root.savedfiles.data.FileABowserKey3 = false;
      _root.savedfiles.data.FileBBowserKey3 = false;
      _root.savedfiles.data.FileCBowserKey3 = false;
      _root.savedfiles.data.FileDBowserKey3 = false;
      _root.DoorOpenedArray = [];
      i = 0;
      while(i < 14)
      {
         _root.DoorOpenedArray.push(false);
         i++;
      }
      _root.savedfiles.data.FileADoorOpenedArray = _root.DoorOpenedArray;
      _root.savedfiles.data.FileBDoorOpenedArray = _root.DoorOpenedArray;
      _root.savedfiles.data.FileCDoorOpenedArray = _root.DoorOpenedArray;
      _root.savedfiles.data.FileDDoorOpenedArray = _root.DoorOpenedArray;
   }
   else
   {
      _root.savedfiles.data["File" + file + "Star"] = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data["File" + file + "Fludd"] = ["",["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false],["",false,false,false]];
      _root.savedfiles.data["File" + file + "StarCoin"] = ["",false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false];
      _root.savedfiles.data["File" + file + "BowserKey1"] = false;
      _root.savedfiles.data["File" + file + "BowserKey2"] = false;
      _root.savedfiles.data["File" + file + "BowserKey3"] = false;
      _root.savedfiles.data["File" + file + "DoorOpenedArray"] = [];
      i = 0;
      while(i < 14)
      {
         _root.savedfiles.data["File" + file + "DoorOpenedArray"].push(false);
         i++;
      }
   }
   _root.m.Message("File " + file + " was Succesfully Deleted.");
   _root.LoadFile();
   _root.CalculateFilesInfo();
};
_root.DeleteHighScores = function()
{
   _root.savedfiles = SharedObject.getLocal("SuperMarioSunshine128SavedFile");
   _root.savedfiles.data.RecordCoin1 = 0;
   _root.savedfiles.data.RecordCoin2 = 0;
   _root.savedfiles.data.RecordCoin3 = 0;
   _root.savedfiles.data.RecordCoin4 = 0;
   _root.savedfiles.data.RecordCoin5 = 0;
   _root.savedfiles.data.RecordCoin6 = 0;
   _root.savedfiles.data.RecordCoin7 = 0;
   _root.savedfiles.data.RecordCoin8 = 0;
   _root.savedfiles.data.RecordCoin9 = 0;
   _root.savedfiles.data.RecordCoin10 = 0;
   _root.savedfiles.data.RecordCoin11 = 0;
   _root.RecordCoin1 = 0;
   _root.RecordCoin2 = 0;
   _root.RecordCoin3 = 0;
   _root.RecordCoin4 = 0;
   _root.RecordCoin5 = 0;
   _root.RecordCoin6 = 0;
   _root.RecordCoin7 = 0;
   _root.RecordCoin8 = 0;
   _root.RecordCoin9 = 0;
   _root.RecordCoin10 = 0;
   _root.RecordCoin11 = 0;
};
_root.SaveHighScores = function()
{
   _root.savedfiles = SharedObject.getLocal("SuperMarioSunshine128SavedFile");
   i = 1;
   while(i < 12)
   {
      _root.savedfiles.data["RecordCoin" + i] = _root["RecordCoin" + i];
      i++;
   }
};
_root.CopyFile = function(file1, file2)
{
   _root.File = file2;
   _root.LoadFile();
   _root.File = file1;
   _root.SaveFile();
   _root.m.Message("File " + file1 + " is now a duplicate of File " + file2 + ".","green");
   _root.CalculateFilesInfo();
   _root.LoadFile();
};
_root.savedfiles = SharedObject.getLocal("SuperMarioSunshine128SavedFile");
if(_root.savedfiles.data.FileExists == undefined && _root.savedfiles.data.FileExists !== true)
{
   _root.DeleteFile();
   _root.DeleteHighScores();
}
else
{
   _root.FileAStar = deepObjectCopy(_root.savedfiles.data.FileAStar);
   _root.FileBStar = deepObjectCopy(_root.savedfiles.data.FileBStar);
   _root.FileCStar = deepObjectCopy(_root.savedfiles.data.FileCStar);
   _root.FileDStar = deepObjectCopy(_root.savedfiles.data.FileDStar);
   _root.FileABowserKey1 = deepObjectCopy(_root.savedfiles.data.FileABowserKey1);
   _root.FileBBowserKey1 = deepObjectCopy(_root.savedfiles.data.FileBBowserKey1);
   _root.FileCBowserKey1 = deepObjectCopy(_root.savedfiles.data.FileCBowserKey1);
   _root.FileDBowserKey1 = deepObjectCopy(_root.savedfiles.data.FileDBowserKey1);
   _root.FileABowserKey2 = deepObjectCopy(_root.savedfiles.data.FileABowserKey2);
   _root.FileBBowserKey2 = deepObjectCopy(_root.savedfiles.data.FileBBowserKey2);
   _root.FileCBowserKey2 = deepObjectCopy(_root.savedfiles.data.FileCBowserKey2);
   _root.FileDBowserKey2 = deepObjectCopy(_root.savedfiles.data.FileDBowserKey2);
   _root.FileABowserKey3 = deepObjectCopy(_root.savedfiles.data.FileABowserKey3);
   _root.FileBBowserKey3 = deepObjectCopy(_root.savedfiles.data.FileBBowserKey3);
   _root.FileCBowserKey3 = deepObjectCopy(_root.savedfiles.data.FileCBowserKey3);
   _root.FileDBowserKey3 = deepObjectCopy(_root.savedfiles.data.FileDBowserKey3);
   if(_root.savedfiles.data.RecordCoin1 == undefined)
   {
      _root.DeleteHighScores();
   }
}
_root.LoadLDCourse(0);
_root.CalculateStars = function()
{
   count = 1;
   _root.TotalStars = 0;
   while(_root.Star[count] !== undefined)
   {
      if(_root.Star[count] == true)
      {
         _root.TotalStars = _root.TotalStars + 1;
      }
      count++;
   }
   if(_root.CheatAllShines == true)
   {
      _root.TotalStars = 64;
   }
};
_root.CalculateStarCoins = function()
{
   count = 1;
   _root.TotalStarCoins = 0;
   while(_root.StarCoin[count] !== undefined)
   {
      if(_root.StarCoin[count] == true)
      {
         _root.TotalStarCoins = _root.TotalStarCoins + 1;
      }
      count++;
   }
   if(_root.CheatAllShines == true)
   {
      _root.TotalStarCoins = 64;
   }
};
_root.CalculateFilesInfo = function()
{
   _root.MaxStarCoins = 0;
   i = 0;
   while(i < 4)
   {
      if(i == 0)
      {
         file = "A";
      }
      if(i == 1)
      {
         file = "B";
      }
      if(i == 2)
      {
         file = "C";
      }
      if(i == 3)
      {
         file = "D";
      }
      count = 1;
      _root["File" + file + "TotalStars"] = 0;
      while(_root["File" + file + "Star"][count] !== undefined)
      {
         if(_root["File" + file + "Star"][count] == true)
         {
            _root["File" + file + "TotalStars"]++;
         }
         count++;
      }
      count = 1;
      _root["File" + file + "TotalStarCoins"] = 0;
      while(_root["File" + file + "StarCoin"][count] !== undefined)
      {
         if(_root["File" + file + "StarCoin"][count] == true)
         {
            _root["File" + file + "TotalStarCoins"]++;
         }
         count++;
      }
      if(_root["File" + file + "TotalStarCoins"] > _root.MaxStarCoins)
      {
         _root.MaxStarCoins = _root["File" + file + "TotalStarCoins"];
      }
      i++;
   }
   trace(_root.MaxStarCoins);
};
_root.SaveLDCourse = function(num)
{
   _root.LDsavedfiles = SharedObject.getLocal("SuperMario63LDSavedFile");
   _root["level" + num + "Tiles"] = _root.deepObjectCopy(_root.TileArray);
   _root["level" + num + "GridX"] = _root.deepObjectCopy(_root.GridXSize);
   _root["level" + num + "GridY"] = _root.deepObjectCopy(_root.GridYSize);
   _root["level" + num + "Items"] = _root.deepObjectCopy(_root.LDItemArray);
   _root["level" + num + "BG"] = _root.deepObjectCopy(_root.LDCourseBG);
   _root["level" + num + "Music"] = _root.deepObjectCopy(_root.LDMusic);
   _root["level" + num + "Name"] = _root.deepObjectCopy(_root.LDCourseName);
   i = 1;
   while(i < 4)
   {
      _root.LDsavedfiles.data["level" + i + "Tiles"] = _root.deepObjectCopy(_root["level" + i + "Tiles"]);
      _root.LDsavedfiles.data["level" + i + "GridX"] = _root.deepObjectCopy(_root["level" + i + "GridX"]);
      _root.LDsavedfiles.data["level" + i + "GridY"] = _root.deepObjectCopy(_root["level" + i + "GridY"]);
      _root.LDsavedfiles.data["level" + i + "Items"] = _root.deepObjectCopy(_root["level" + i + "Items"]);
      _root.LDsavedfiles.data["level" + i + "BG"] = _root.deepObjectCopy(_root["level" + i + "BG"]);
      _root.LDsavedfiles.data["level" + i + "Music"] = _root.deepObjectCopy(_root["level" + i + "Music"]);
      _root.LDsavedfiles.data["level" + i + "Name"] = _root.deepObjectCopy(_root["level" + i + "Name"]);
      i++;
   }
};
_root.LoadLDCourse = function(num)
{
   _root.LDsavedfiles = SharedObject.getLocal("SuperMario63LDSavedFile");
   i = 1;
   while(i < 4)
   {
      _root["level" + i + "Tiles"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "Tiles"]);
      _root["level" + i + "GridX"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "GridX"]);
      _root["level" + i + "GridY"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "GridY"]);
      _root["level" + i + "Items"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "Items"]);
      _root["level" + i + "BG"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "BG"]);
      _root["level" + i + "Music"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "Music"]);
      _root["level" + i + "Name"] = _root.deepObjectCopy(_root.LDsavedfiles.data["level" + i + "Name"]);
      i++;
   }
   if(num !== 0)
   {
      _root.TileArray = _root.deepObjectCopy(_root["level" + num + "Tiles"]);
      _root.GridXSize = _root.deepObjectCopy(_root["level" + num + "GridX"]);
      _root.GridYSize = _root.deepObjectCopy(_root["level" + num + "GridY"]);
      _root.LDItemArray = _root.deepObjectCopy(_root["level" + num + "Items"]);
      _root.LDCourseBG = _root.deepObjectCopy(_root["level" + num + "BG"]);
      _root.LDMusic = _root.deepObjectCopy(_root["level" + num + "Music"]);
      _root.LDCourseName = _root.deepObjectCopy(_root["level" + num + "Name"]);
      if(_root.TileArray == undefined || isNaN(Number(_root.GridXSize)))
      {
         _root.EraseLevel();
      }
   }
   else
   {
      i = 1;
      while(i < 4)
      {
         if(_root["level" + i + "Tiles"] == undefined || isNaN(Number(_root["level" + i + "GridX"])))
         {
            _root["level" + i + "Name"] = "Untitled " + i;
            _root["level" + i + "GridX"] = 0;
            _root["level" + i + "GridY"] = 0;
            _root["level" + i + "BG"] = 1;
         }
         i++;
      }
   }
};
_root.StarNameArray = [];
_root.StarDescArray = [];
_root.StarNameArray[0] = "No Star Selected";
_root.StarDescArray[0] = "You are within the Castle Grounds";
_root.StarNameArray[1] = "Battle with King Bob-omb";
_root.StarDescArray[1] = "In this land, Goomba\'s and Bob-Ombs have always been rivals.  Ontop of Mount Bob-Omb, the King awaits any challenger, however, Goomba\'s have never proved strong enough.  Are you willing enough to risk a Battle with King Bob-Omb?";
_root.StarNameArray[2] = "Island in the Sky";
_root.StarDescArray[2] = "Theres an island in the sky; journey upwards, past turbulent winds, past flying koopas, past even a butterfly(!!!), to reach the Floating Island in the Sky";
_root.StarNameArray[3] = "The Five Silver Stars";
_root.StarDescArray[3] = "Sometimes, a Shine Sprite can break down into 5 Silver Stars and get scattered throughout the land.  You\'ll have to collect all 5 fragments, to recover it...";
_root.StarNameArray[4] = "Collect the 8 Red Coins";
_root.StarDescArray[4] = "For some unknown reason, when 8 different Red Coins come together, a Shine Sprite is formed...  Nobody understands why, but it works, so take advantage of it.";
_root.StarNameArray[5] = "The Secret of the Mountain";
_root.StarDescArray[5] = "Do I know where the Secret of the Mountain is?  Well, that\'s a Secret, in fact, it\'s so much a secret, that wheterh or not I know it is also a secret!  By the way, this discussion is also to be kept secret... Understand?";
_root.StarNameArray[6] = "The Snowmans Head";
_root.StarDescArray[6] = "Snowman\'s Land, as you can tell by the name, is named after the giant Snowman here.  On such an important landmark must lie a Shine Sprite.";
_root.StarNameArray[7] = "The Iceblock Maze";
_root.StarDescArray[7] = "To the East of Snowman\'s Land sits a majestic scuplture of ice..  But wait!!  There\'s a Shine Sprite in it!!";
_root.StarNameArray[8] = "Chief Chilly";
_root.StarDescArray[8] = "Near the Iceblock Maze, Chief Chilly awaits anyone who darse challenge him..  Who knows, maybe the reward for beating him is a Shine Sprite.";
_root.StarNameArray[9] = "Red-Coins of the Summit and Skies";
_root.StarDescArray[9] = "Somehwere above us floats those 8 maroonish coins.  If you collect all of them, then a Shine Sprite appears.";
_root.StarNameArray[10] = "The Secret of the Igloo";
_root.StarDescArray[10] = "Buried deep in the upper mountains of Snowman\'s Land lies an Igloo;  it may be small on the outside, but inside, ... well, that\'s a secret.";
_root.StarNameArray[11] = "The Underground Lake";
_root.StarDescArray[11] = "Find the pool hidden in Hazy Maze Cave and recover the Shine Sprite.";
_root.StarNameArray[12] = "The Deadly Toxic Maze";
_root.StarDescArray[12] = "Located under the Side Room, there exists a Maze so Toxic, no non-metallic creature could survive.  ";
_root.StarNameArray[13] = "The Caged Shine Sprite";
_root.StarDescArray[13] = "Maybe there\'s a way into that cage (perhaps from above).";
_root.StarNameArray[14] = "Excavating the 8 Red Coins";
_root.StarDescArray[14] = "Search high and low for these hidden red coins.";
_root.StarNameArray[15] = "The Secret of the Lake";
_root.StarDescArray[15] = "There must be a way to <i>Stomp</i> underwater, but how?";
_root.StarNameArray[16] = "Big Boo\'s Balcony";
_root.StarDescArray[16] = "I heard that Big Boo always keeps a Shine Sprite up on his balcony...  If only all those doors weren\'t locked, then you\'d be able to reach the top easily!  Oh wait, even with the doors unlocked, it\'ll still be a challenge...";
_root.StarNameArray[17] = "\'Switch\' to the Secret Room";
_root.StarDescArray[17] = "Get a Grip!  This ghostly Shine Sprite has been gobbled up by the Ghost\'s secret room.  By getting to the Ghostly Golden and Ghostly Green switches, by golly, you might get the garment, but be on the guard from the gruesome ghosts.";
_root.StarNameArray[18] = "Basement Shine Sprite";
_root.StarDescArray[18] = "Travel to the empty Basement and collect the lonely Shine Sprite there...  Don\'t worry about finding that poor thing, it\'s just sitting out there in the open... It\'s not as if it\'s a trap or anything...";
_root.StarNameArray[19] = "Hunting for Red-Coins";
_root.StarDescArray[19] = "Find the Hidden 8 Red-Coins in the Mansion";
_root.StarNameArray[20] = "The Secret of the Library";
_root.StarDescArray[20] = "The Library floats above your head in the Main Room... Just imagine what awaits inside it.";
_root.StarNameArray[21] = "The Bully\'s Tower";
_root.StarDescArray[21] = "The Bully-Toad War (abbreviated as: B.T.W.) is reaching it\'s climax!  The Blue Toad\'s now have the key to victory, but can you help them achieve it?";
_root.StarNameArray[22] = "Into the Volcano";
_root.StarDescArray[22] = "The Volcano has been rather volatile recently, perhaps due to a Shine Sprite?";
_root.StarNameArray[23] = "The Lava Wheel";
_root.StarDescArray[23] = "The Toad\'s Lava-Wheel, which power\'s their Lava Castle, appears to have Silver Stars in it!";
_root.StarNameArray[24] = "Red-Hot Red-Coins";
_root.StarDescArray[24] = "Around <i>the Steps</i> lie 8 Red Coins,  but can you even touch them without burning yourself?";
_root.StarNameArray[25] = "The Secret of the Fortress";
_root.StarDescArray[25] = "A Secret passage lies hidden inside the Blue Toad\'s Fortress... if only you could find it..";
_root.StarNameArray[26] = "Scale the Mountain";
_root.StarDescArray[26] = "Here in Tall Tall Mountain, anyone who can climb all the way to the top deserves a reward.  Your reward... is a Shine Sprite...";
_root.StarNameArray[27] = "Silver Stars in the Skies";
_root.StarDescArray[27] = "It\'s Silver, it\'s in a Star shape, and it\'s in the Sky!  Yay!";
_root.StarNameArray[28] = "Blast off to the Lonely Mushroom";
_root.StarDescArray[28] = "Utilize the prowess of thus Cannon to facilitate one\'s approach upon the golden disk awaiting atop mycelium.";
_root.StarNameArray[29] = "The Red Coins of the Mountain";
_root.StarDescArray[29] = "Go to the right and explore that sliver of mountainistic mountainside!";
_root.StarNameArray[30] = "The Secret of the Mountainside";
_root.StarDescArray[30] = "The Secret to Tall Tall Mountain... remains hidden near a Waterfall.  search around there and maybe you\'ll find it.";
_root.StarNameArray[31] = "Cruiser Crossin\' the Rainbow";
_root.StarDescArray[31] = "Ride the magic rainbow carpets all the way to beautiful blue sky and across fluffy clouds of cotton candy and golden concoctions of sweet riches to the magical floating ship in the sky!";
_root.StarNameArray[32] = "Tricky Triangles";
_root.StarDescArray[32] = "Whoever said math can\'t hurt is dead wrong;  these Triangles will flip, murder, squish, rotate, bite and scratch you until you\'re dead!  Well, maybe not all of those, but you get the idea...";
_root.StarNameArray[33] = "The Floating Mansion in the Sky";
_root.StarDescArray[33] = "For some reason, an old, cranky rich geezer decided to buy a giant floating house,  and he\'s left a Shine Sprite in there.";
_root.StarNameArray[34] = "The Maze of Red Coins";
_root.StarDescArray[34] = "OMG!  To the left is a Huge Maze!!  This gonna be so impossible!!  There are enemies, moving platforms, and -- Oh No -- Dead Ends!!!   AHHH!!!";
_root.StarNameArray[35] = "The Secret of the Triangle Tower";
_root.StarDescArray[35] = "That Blue-Brick tower t0 the right has a small ledge full of secrets, but it\'s too high to reach without Rocket Fludd...";
_root.StarNameArray[36] = "The Final Battle";
_root.StarDescArray[36] = "................It\'s Bowser\'s Castle.......   And it\'s freakin\' huge!";
_root.StarNameArray[37] = "Bowser\'s Castle Red-Coins";
_root.StarDescArray[37] = "";
_root.StarNameArray[38] = "The 16 Star Coin Prize";
_root.StarDescArray[38] = "";
_root.StarNameArray[39] = "Mushroom Kingdom";
_root.StarDescArray[39] = "This is Super Mario 63\'s Tutorial";
_root.StarNameArray[40] = "Bowser\'s Trap";
_root.StarDescArray[40] = "This is it!  Go and save the Princess from Bowser!";
_root.StarNameArray[41] = "Mushroom Kingdom 2";
_root.StarDescArray[41] = "The outskirts of the Mushroom Kingdom (also known here as Tutorial #2)";
_root.StarNameArray[42] = "Bowser\'s Second Trap";
_root.StarDescArray[42] = ".......... Bowser definitely didn\'t construct all of this just to stall you...  In fact, this Fortress looks hundreds of years old.........  But if Bowser didn\'t make this..........  then <i>who</i> made it?.......  And <i>why</i>?.......";
_root.StarNameArray[43] = "The 48 Star Coin Prize";
_root.StarDescArray[43] = "";
_root.StarNameArray[44] = "The Secret of the Mushroom";
_root.StarDescArray[44] = "Gotta Catch \'em all! 1-Up Mushroom!";
_root.StarNameArray[45] = "The Tidal Isles";
_root.StarDescArray[45] = "These tropical islands in the middle of the ocean are constantly battered with waves... be careful not to be swept away.";
_root.StarNameArray[46] = "Jolly Roger Bay";
_root.StarDescArray[46] = "Collect all of those 8 red coins!";
_root.StarNameArray[47] = "Frosty Fludd";
_root.StarDescArray[47] = "Even though Turbo Fludd is cold, it still runs at Turbo Speed.";
_root.StarNameArray[48] = "Secret of the Sky";
_root.StarDescArray[48] = "Welcome to the windswept, grassy hills of the Secret of the Sky!  Eight Red Coins can be found along here, and in the sky above.  Be sure to find them all.";
_root.StarNameArray[49] = "Thwomp\'s Castle";
_root.StarDescArray[49] = "\"Hey you Goomba!  Can you move under me? Good; just a little more to the right; little more, perfect...... SMASH!!  Mwahaha!\"";
_root.StarNameArray[50] = "Peach\'s Garden";
_root.StarDescArray[50] = "Isn\'t just so lovely here in Peach\'s Garden?";
_root.StarNameArray[51] = "Kamek\'s Castle";
_root.StarDescArray[51] = "So this is where Kamek lives;  It seems kind of small compared to how important Kamek is to Bowser...";
_root.StarNameArray[52] = "The Cave of Empuzzlement";
_root.StarDescArray[52] = "Be careful, this labryinth of Locked Cannons, Metal Blocks, Invisible Walls, Floating Platforms, Rocket Fludd, and Turbo Fludd will twist and mutilate your mind beyond sanity.";
_root.StarNameArray[53] = "The Magma Maze";
_root.StarDescArray[53] = "Somewhere in these molten caverns, ther lies an oasis of Water (and a Shine Sprite).";
_root.StarNameArray[54] = "A Galaxy of Stars";
_root.StarDescArray[54] = "Collect the five Silver Stars around this Galaxy;  from sunken in floating seas, spinning within planet\'s cores, to locked inside castles. ";
_root.StarNameArray[55] = "Inside the Pyramid";
_root.StarDescArray[55] = "The Ancent Pyramid of Shifting Sand Land is the burial site of the Koopa Pharoah, Yuuzgonadai.";
_root.StarNameArray[56] = "Trekking for 8 Red-Coins";
_root.StarDescArray[56] = "Some of the Red Coins are cursed, so that you\'ll need some Ancient Magic to reach some of them...";
_root.StarNameArray[57] = "Shining Atop the Ancient Pyramid";
_root.StarDescArray[57] = "The Shine Sprite glows brightly from on top of the Pyramid, but how to get there?";
_root.StarNameArray[58] = "The Top of the Town";
_root.StarDescArray[58] = "Wet-Dry World is called Wet-Dry World for a reason, use that to reach the top.";
_root.StarNameArray[59] = "Red Coins in the Underwater City";
_root.StarDescArray[59] = "A Long time ago, Wet-Dry World suffered from Global Warming, and the lower City sunk from rising water-levels.  Using the Cannon, find the lost city and the Red-Coins inside it.";
_root.StarNameArray[60] = "The 5 Secrets of the Shallows and Sky";
_root.StarDescArray[60] = "Five Silver Stars are hidden around this place... Now to find them....";
_root.StarNameArray[61] = "The Top of the Clock";
_root.StarDescArray[61] = "Make your way across this wacky world of Chronoic Chaos.";
_root.StarNameArray[62] = "The Flippin\' Red Coins";
_root.StarDescArray[62] = "Red Coins can be tricky, but did you know that they can actually flip you off (the platform)?";
_root.StarNameArray[63] = "The Pit of the Pendelums";
_root.StarDescArray[63] = "Swing across along the Giant Pendelums to reach this Shine Sprite.";
_root.StarNameArray[64] = "The Secret of the Shine Sprites";
_root.StarDescArray[64] = "";
_root.Restartcoins = function()
{
   _root["coindata1-1"] = [""];
   _root["coindata1-2"] = [""];
   _root["coindata1-3"] = [""];
   _root["coindata1-4"] = [""];
   _root["coindata1-5"] = [""];
   _root["coindata2-1"] = [""];
   _root["coindata2-2"] = [""];
   _root["coindata2-3"] = [""];
   _root["coindata2-4"] = [""];
   _root["coindata3-1"] = [""];
   _root["coindata3-2"] = [""];
   _root["coindata3-3"] = [""];
   _root["coindata3-4"] = [""];
   _root["coindata3-5"] = [""];
   _root["coindata3-6"] = [""];
   _root["coindata3-7"] = [""];
   _root["coindata3-8"] = [""];
   _root["coindata4-1"] = [""];
   _root["coindata4-2"] = [""];
   _root["coindata4-3"] = [""];
   _root["coindata4-4"] = [""];
   _root["coindata4-5"] = [""];
   _root["coindata4-6"] = [""];
   _root["coindata4-7"] = [""];
   _root["coindata4-8"] = [""];
   _root["coindata4-9"] = [""];
   _root["coindata4-10"] = [""];
   _root["coindata4-11"] = [""];
   _root["coindata5-1"] = [""];
   _root["coindata5-2"] = [""];
   _root["coindata5-3"] = [""];
   _root["coindata5-4"] = [""];
   _root["coindata5-5"] = [""];
   _root["coindata5-6"] = [""];
   _root["coindata5-7"] = [""];
   _root["coindata5-8"] = [""];
   _root["coindata5-9"] = [""];
   _root["coindata6-1"] = [""];
   _root["coindata6-2"] = [""];
   _root["coindata6-3"] = [""];
   _root["coindata6-4"] = [""];
   _root["coindata6-5"] = [""];
   _root["coindata6-6"] = [""];
   _root["coindata7-1"] = [""];
   _root["coindata7-2"] = [""];
   _root["coindata7-3"] = [""];
   _root["coindata7-4"] = [""];
   _root["coindata7-5"] = [""];
   _root["coindata8-1"] = [""];
   _root["coindata8-2"] = [""];
   _root["coindata8-3"] = [""];
   _root["coindata8-4"] = [""];
   _root["coindata8-5"] = [""];
   _root["coindata8-6"] = [""];
   _root["coindata8-7"] = [""];
   _root["coindata8-8"] = [""];
   _root["coindata8-9"] = [""];
   _root["coindata8-10"] = [""];
   _root["coindata8-11"] = [""];
   _root["coindata8-12"] = [""];
   _root["coindata8-13"] = [""];
   _root["coindata8-14"] = [""];
   _root["coindata8-15"] = [""];
   _root["coindata8-16"] = [""];
   _root["coindataBC-1"] = [""];
   _root["coindataBC-2"] = [""];
   _root["coindataBC-3"] = [""];
   _root["coindata8-E1-1"] = [""];
   _root["coindata8-E1-1-2"] = [""];
   _root["coindata8-E1-2"] = [""];
   _root["coindata8-E1-2-2"] = [""];
   _root["coindata8-E2-1"] = [""];
   _root["coindata8-E2-2"] = [""];
   _root["coindata8-E5-1"] = [""];
   _root["coindata8-E5-2"] = [""];
   _root["coindata8-E5-3"] = [""];
   _root["coindata9-01"] = [""];
   _root["coindata9-02"] = [""];
   _root["coindata9-03"] = [""];
   _root["coindata9-03-D"] = [""];
   _root["coindata9-03-2"] = [""];
   _root["coindata9-04"] = [""];
   _root["coindata9-05"] = [""];
   _root["coindata9-06"] = [""];
   _root["coindata9-07"] = [""];
   _root["coindata9-08"] = [""];
   _root["coindata9-09"] = [""];
   _root["coindata9-10"] = [""];
   _root["coindata9-11"] = [""];
   _root["coindataM1-1"] = [""];
   _root["coindataM1-2"] = [""];
   _root["coindataM2-1"] = [""];
   _root["coindataM2-2"] = [""];
   _root["coindataM2-3"] = [""];
   _root["coindataM3-1"] = [""];
   _root["coindataM3-2"] = [""];
   _root["coindataM3-3"] = [""];
   _root["coindataC-1"] = [""];
   _root["coindataC-2-2"] = [""];
   _root["coindataC-4-SC1"] = [""];
   _root.coindataLevelDesigner = [""];
   if(_root.LDhighestcoinrestart !== undefined)
   {
      i = - _root.LDhighestcoinrestart - 1;
      while(i < _root.LDhighestcoinrestart + 1)
      {
         _root["coindataLevelDesigner" + i] = [""];
         i++;
      }
   }
   _root.coindataLevel1 = [""];
   _root.hpcoincount = 0;
   _root.coincount = 0;
   _root.Coins = 0;
   _root.RedCoins = 0;
   _root.SilverStars = 0;
   _root.Invincible = false;
   _root.Metal = false;
   _root.Invisible = false;
   _root.WingCap = false;
   _root.PowerTimer = 0;
   _root.SaveFluddH = false;
   _root.SaveFluddR = false;
   _root.SaveFluddT = false;
   _root.CharHP = 8;
   _root.WaterHP = 8;
};
_root.Restartcoins();
_root.RestartFludd = function()
{
   _root.SaveFluddH = false;
   _root.SaveFluddR = false;
   _root.SaveFluddT = false;
   _root.Fluddpow = "";
};
