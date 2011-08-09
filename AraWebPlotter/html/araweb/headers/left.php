<div class="vertical">
<?php

function from_camel_case($str) {
   $str[0] = strtolower($str[0]);
   $func = create_function('$c', 'return " " . strtolower($c[1]);');
   return preg_replace_callback('/([A-Z])/', $func, $str);
 }

function getCanList($tag) {
   // open this directory 
   $myDirectory = opendir(".");
   // get each entry
   while($entryName = readdir($myDirectory)) {
     if (preg_match("/can/", "$entryName", $regs)) { 
       if (preg_match("/php/", "$entryName", $regs)) {  
	 if (preg_match($tag, "$entryName", $regs)) {     
	   $dirArray[] = $entryName;
#	   echo "<h2 class=\"navigation\">$entryName</h2>";
	 }	 

       }
     }
   }
   
   // close directory
   closedir($myDirectory);
   sort($dirArray);
   return $dirArray;
}

function doSideBar($tag,$desc,$nav) {
   $dirArray = getCanList($tag);
   $indexCount	= count($dirArray);
  # for($index=0; $index < $indexCount; $index++) {
  #   echo "<h2 class=\"navigation\">$dirArray[$index]</h2>";
  # }
     

   echo "<p class=\"navigation\">
<FORM NAME=\"$nav\"><DIV>
<SELECT NAME=\"SelectURL\" onChange=\"document.location.href=document.$nav.SelectURL.options[document.$nav.SelectURL.selectedIndex].value\">
<OPTION VALUE=\".\"
SELECTED>$desc:";
   for($index=0; $index < $indexCount; $index++) {
      if (substr("$dirArray[$index]", 0, 1) != "."){ // don't list hidden files
	Print("<OPTION VALUE=\"/monitor/headers/$dirArray[$index]\">");
	$patterns = array();
	$patterns[0]=$tag;
	$patterns[1]="/.php/";
	$patterns[2]="/can/";
	$replacements = array();
	$replacements[0] = "";
	$replacements[1] = "";
	$replacements[2] = "";
#	$printVer=preg_replace($tag," ",$dirArray[$index]); 
	$printVer=preg_replace($patterns,$replacements,$dirArray[$index]); 
#	$printVer2=from_camel_case($printVer);
	Print($printVer);
#	Print("$dirArray[$index]");
      }
   }
   echo "</SELECT></DIV>
</FORM>
</p>";
}

virtual("/monitor/leftMain.shtml");
echo "<h2 class=\"navigation\">Header Plots</h2>";
doSideBar("/OneHour/","One Hour","nav1");
doSideBar("/SixHours/","Six Hours","nav2");
doSideBar("/TwelveHours/","Twelve Hour","nav3");
doSideBar("/OneDay/","One Day","nav4");
doSideBar("/All/","All Time","nav5");
echo "</div>";
?>
