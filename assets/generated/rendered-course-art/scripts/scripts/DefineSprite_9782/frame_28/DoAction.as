stop();
var result_lv = new LoadVars();
result_lv.onLoad = function(success)
{
   if(success)
   {
      play();
      _parent._parent.diffstuff.nextFrame();
      _parent._parent.Difficulty = result_lv.diff;
      _parent._parent.diff.text = "Difficulty: " + _parent._parent.Difficulty + "/5";
      _frame = Math.floor(Number(_parent._parent.Difficulty));
      _frame = Math.max(_frame,0);
      _frame = Math.min(_frame,5);
      if(_parent._parent.Difficulty > 4.5)
      {
         _frame = 5;
      }
      _parent._parent.diffclip.gotoAndStop(_frame + 1);
      _parent._parent.Votes2 = result_lv.votes;
      if(_parent._parent.Votes2 != "1")
      {
         _parent._parent.votes2.text = "Voted on " + _parent._parent.Votes2 + " times";
      }
      else
      {
         _parent._parent.votes2.text = "Voted on 1 time";
      }
   }
};
var send_lv = new LoadVars();
send_lv.id = _root.LDSelectedLevel;
send_lv.score = num;
send_lv.sendAndLoad("http://runouw.com/levels/leveldesigner/votediff.php",result_lv,"GET");
