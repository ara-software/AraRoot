<?php
$fileURL=getJustBasenameURL();
#echo "<h2>Fred $fileURL</h2>";
$shtmlFile=preg_replace("/.php/",".shtml",$fileURL);
virtual("/monitor/headers/content/$shtmlFile");
?>