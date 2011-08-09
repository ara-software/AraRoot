<div class="vertical">
<?php
virtual("/monitor/leftMain.shtml");

// open this directory 
$myDirectory = opendir(".");
// get each entry
while($entryName = readdir($myDirectory)) {
  if (ereg("^run", "$entryName", $regs)) {
	$dirArray[] = $entryName;
  }
}

// close directory
closedir($myDirectory);
sort($dirArray);
$indexCount	= count($dirArray);
$curURL=curPageURL();
$parts=Explode('?', $curURL);
$snippedURL=$parts[0];
echo "<h2 class=\"navigation\">Housekeeping Plots</h2>";


echo "<p class=\"navigation\">
<FORM NAME=\"nav1\"><DIV>
<SELECT NAME=\"SelectURL\" onChange=\"document.location.href=document.nav1.SelectURL.options[document.nav1.SelectURL.selectedIndex].value\">
<OPTION VALUE=\".\"
SELECTED>Run:";

for($index=0; $index < $indexCount; $index++) {
  if (substr("$dirArray[$index]", 0, 1) != "."){ // don't list hidden files
    Print("<OPTION VALUE=\"$snippedURL?rundir=$dirArray[$index]\">");
    Print("$dirArray[$index]");
  }
 }

echo "</SELECT></DIV>
</FORM>
</p>
</div>";
?>
