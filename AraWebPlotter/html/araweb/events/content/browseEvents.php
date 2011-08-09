
<?php
function topDirList() {
Print("<h2>Event Directories</h2>\n");
// open this directory 
$myDirectory = opendir("all");
// get each entry
while($entryName = readdir($myDirectory)) {
	$dirArray[] = $entryName;
}

// close directory
closedir($myDirectory);

//	count elements in array
$indexCount	= count($dirArray);
//Print ("<p>$indexCount directories</p>\n");
$curURL=curPageURL();

for($index=0; $index < $indexCount; $index++) {
  if (substr("$dirArray[$index]", 0, 1) != "."){ // don't list hidden files
    Print("<p><a href=\"$curURL?subdir=$dirArray[$index]\">$dirArray[$index]</a></p>\n");
  }

 }
}

function getImageList($subdir1) {

  // open this directory 
  $myDirectory = opendir("all/$subdir1");
// // get each entry
  while($entryName = readdir($myDirectory)) {
    $fileArray[] = $entryName;
  }
  // close directory
  closedir($myDirectory);
  
  //	count elements in array
  //Print ("<p>$indexCount directories</p>\n");
  sort($fileArray);
  return $fileArray;
}

function subDirList($subdir1) {
  Print("<h2>Events in $subdir1</h2>\n");
  
  $fileArray=getImageList($subdir1);
  $indexCount	= count($fileArray);
  $curURL=curPageURL();
  
  for($index=0; $index < $indexCount; $index++) {
    if (substr("$fileArray[$index]", 0, 1) != "."){ // don't list hidden files
      Print("<p><a href=\"$curURL&index=$index\">$fileArray[$index]</a></p>\n");
    }    
  }
}


function subDirShow($subdir1,$index) {
  Print("<h2>Showing file $index from $subdir1</h2>\n");

  $fileArray=getImageList($subdir1);
  $indexCount	= count($fileArray);
  //	count elements in array
  $indexCount	= count($fileArray);
  //Print ("<p>$indexCount directories</p>\n");
  $curURL=curPageURL();
  
  if($index < $indexCount) {
    $curIndexStr="index=$index";
    if($index > 0) {
      $prevIndex=$index-1;
      $prevIndexStr="index=$prevIndex";
      $prevURL=str_replace($curIndexStr,$prevIndexStr,$curURL);
      Print("<p align=left><a href=\"$prevURL\">Previous Event</a></p>\n");
    }
    $nextIndex=$index+1;
    if($nextIndex<$indexCount) {
      $nextIndexStr="index=$nextIndex";
      $nextURL=str_replace($curIndexStr,$nextIndexStr,$curURL);
      Print("<p align=left><a href=\"$nextURL\">Next Event</a></p>\n");
      
    }
    Print("<img style=\"border:outset\" src=\"all/$subdir1/$fileArray[$index]\" alt=\"event\" />\n");
  }    
}


//Check if we are meant to look at as subdir or a list the subdirs
$subdir=$_GET["subdir"];
if($_GET["subdir"] === null) {
  topDirList();
 }
 else if(isset($_GET["subdir"])) {
   if(isset($_GET["index"])) {
     subDirShow($subdir,$_GET["index"]);
   }
   else {
     subDirList($subdir);
   }
 }
 else {
   topDirList();
 }


?>