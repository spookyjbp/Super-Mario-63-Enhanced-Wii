stop();
iD.text = _root.LDSelectedLevel;
comment.text = Comment;
name.text = unescape(Name);
author.text = "By: " + unescape(Author);
score.text = "Score: " + Score + "/5";
diff.text = "Difficulty: " + Difficulty + "/5";
_frame = Math.floor(Number(Difficulty));
_frame = Math.max(_frame,0);
_frame = Math.min(_frame,5);
if(Difficulty > 4.5)
{
   _frame = 5;
}
if(Votes1 != "1")
{
   votes1.text = "Voted on " + Votes1 + " times";
}
else
{
   votes1.text = "Voted on 1 time";
}
if(Votes2 != "1")
{
   votes2.text = "Voted on " + Votes2 + " times";
}
else
{
   votes2.text = "Voted on 1 time";
}
diffclip.gotoAndStop(_frame + 1);
if(Votes1 == "0")
{
   votes1.text = "No votes yet";
   score.text = "Score: ---";
}
if(Votes2 == "0")
{
   diffclip.gotoAndStop(7);
   votes2.text = "No votes yet";
   diff.text = "Difficulty: ---";
}
