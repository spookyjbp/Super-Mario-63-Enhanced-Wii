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
         _parent.Total = Number(result_lv.total.split(" ")[0]);
         _parent.TotalPages = Math.ceil(_parent.Total / 8);
         if(result_lv.code !== undefined)
         {
            CreateList(result_lv.code);
         }
      }
      else
      {
         result_ta.text = "Error connecting to server.";
      }
   };
   var _loc4_ = new LoadVars();
   _loc4_.start = _root.Page * 8;
   _loc4_.length = 8;
   _loc4_.order = _root.Order;
   _loc4_.sendAndLoad("http://runouw.com/levels/leveldesigner/listlevels.php",result_lv,"POST");
   CreateList = function(code)
   {
      Results = code.split("|");
      _parent.LDResults = Results;
      i = 0;
      while(i < Results.length - 1)
      {
         RowStats = Results[i].split(",");
         this.attachMovie("LDPRow","Row" + i,this.getNextHighestDepth());
         this["Row" + i]._x = 0;
         this["Row" + i]._y = i * 20;
         this["Row" + i].iD.text = RowStats[0];
         this["Row" + i].num = RowStats[0];
         this["Row" + i].Name.text = RowStats[1];
         this["Row" + i].Author.text = RowStats[2];
         _score = Math.round(Number(RowStats[3]) * 100) / 100;
         if(_score == Math.round(_score))
         {
            _score += ".0";
         }
         if(Math.round(Number(RowStats[3]) * 10) / 10 == Math.round(Number(RowStats[3]) * 100) / 100)
         {
            _score += "0";
         }
         this["Row" + i].Score.text = _score + "/5";
         if(Number(RowStats[6]) < 3)
         {
            this["Row" + i].Score.text = "---";
         }
         _frame = Math.floor(Number(RowStats[4]));
         _frame = Math.max(_frame,0);
         _frame = Math.min(_frame,5);
         if(Number(RowStats[4]) > 4.5)
         {
            _frame = 5;
         }
         this["Row" + i].diffclip.gotoAndStop(_frame + 1);
         _score = Math.round(Number(RowStats[4]) * 10) / 10;
         if(_score == Math.round(_score))
         {
            _score += ".0";
         }
         this["Row" + i].Diff.text = _score + "/5";
         if(Number(RowStats[7]) < 3)
         {
            this["Row" + i].diffclip.gotoAndStop(7);
            this["Row" + i].Diff.text = "---";
         }
         i++;
      }
      nextFrame();
   };
};
