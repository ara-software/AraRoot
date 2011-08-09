
<?php
function topDirList() {
Print("<h2>Run Directories</h2>\n");
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
rsort($dirArray);
//	count elements in array
$indexCount	= count($dirArray);
//Print ("<p>$indexCount directories</p>\n");
$curURL=curPageURL();

for($index=0; $index < $indexCount; $index++) {
  if (substr("$dirArray[$index]", 0, 1) != "."){ // don't list hidden files
    Print("<p><a href=\"indexRun.php?rundir=$dirArray[$index]\">$dirArray[$index]</a></p>\n");
  }

 }
}





//Check if we are meant to look at as run or a list the runs
topDirList();

?>



