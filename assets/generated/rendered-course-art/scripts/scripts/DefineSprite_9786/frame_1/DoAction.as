stop();
antispam = 0;
onEnterFrame = function()
{
   antispam++;
   if(antispam == 16)
   {
      goload();
   }
};
goload = function()
{
   var result_lv = new LoadVars();
   result_lv.onLoad = function(success)
   {
      if(success)
      {
         trace("=" + result_lv);
         VotedScore = result_lv.votedscore;
         VotedDiff = result_lv.voteddiff;
         Name = result_lv.name;
         Author = result_lv.author;
         Score = result_lv.score;
         Difficulty = result_lv.difficulty;
         Votes1 = result_lv.votes1;
         Votes2 = result_lv.votes2;
         trace(result_lv.votedscore);
         Code = result_lv.code;
         _root.LoadedCode = Code;
         Comment = result_lv.comment;
         nextFrame();
      }
      else
      {
         result_ta.text = "Error connecting to server.";
      }
   };
   var _loc2_ = new LoadVars();
   _loc2_.id = _root.LDSelectedLevel;
   _root.LDSelectedLevel = _root.LDSelectedLevel;
   _loc2_.sendAndLoad("http://runouw.com/levels/leveldesigner/getstats.php",result_lv,"POST");
};
