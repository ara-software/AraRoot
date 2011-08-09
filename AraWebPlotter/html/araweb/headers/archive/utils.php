<?php
function modified($fileName) {
   $mtime=date("l jS F Y", filemtime("$fileName"));
   echo "<p class=copyright>";
   echo "Last modified:  $mtime";
   echo "</p>";
}

function printLink($title,$url,$key,$text) {
   if (basename($url) != basename($_SERVER['REQUEST_URI'])) {
      echo "<A class=leftBar title=\"$title\" HREF=\"$url\"
          ACCESSKEY=$key>$text</A>
          <span class=spacer>&nbsp;&nbsp;</span>\n";
   }
   else {
      echo "<div class=dummyLink>$text</div>";
   }
}

function findImages($dir) {
  if (ereg("^/~","$dir",$dummy)) {
   $fsdir=preg_replace("=^/~([^/]*)=", "/var/www/users/$1/public_html", $dir);
  }
  else {
   $fsdir=preg_replace("=^/=", "/var/www/html/", $dir);
  }
   if ($handle = opendir($fsdir)) {
      /* This is the correct way to loop over the directory. */
      while (false !== ($file = readdir($handle))) {
         if (ereg(".*\.(png)|(jpe?g)|(gif)$", "$file", $regs)) {
            $files[]="$dir/$file";
         }
      }
      closedir($handle);
   }
   return $files;
}

function bgStyle($imageDirectory) {
   $image=findImages($imageDirectory);
   $number=rand(0,count($image)-1);

   echo "<STYLE type=\"text/css\">body {background-image:  url($image[$number]);} </STYLE>";
}

function doPage($fileName) {
   $subject=basename("$fileName", ".html");
   $subject=basename("$subject", ".php");
   echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"
    \"http://www.w3.org/TR/html4/strict.dtd\"> 
    <HTML>
    <HEAD>";
   
   include("/monitor/styles.shtml");
   echo "<link rel=\"StyleSheet\" href=\"/monitor/styles/base.css\" type=\"text/css\" media=\"screen\" />
<link rel=\"StyleSheet\" href=\"/monitor/styles/default.css\" type=\"text/css\" media=\"screen\" title=\"RJN default\" />";


   echo "<TITLE>Header Page</TITLE>";

   echo "<META http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"> 
    </HEAD>
    <BODY>";

   include("top.shtml");

   echo "<DIV class=middle><DIV class=content>";
   include("$fileName");
   echo "<DIV class=copyright>";
   modified("$fileName");

   echo "</div></div></div>";

   include("left.php");

   echo "</body></html>";
}

function curPageURL() {
 $pageURL = 'http';
 if ($_SERVER["HTTPS"] == "on") {$pageURL .= "s";}
 $pageURL .= "://";
 if ($_SERVER["SERVER_PORT"] != "80") {
  $pageURL .= $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"].$_SERVER["REQUEST_URI"];
 } else {
  $pageURL .= $_SERVER["SERVER_NAME"].$_SERVER["REQUEST_URI"];
 }
 return $pageURL;
}

?>
