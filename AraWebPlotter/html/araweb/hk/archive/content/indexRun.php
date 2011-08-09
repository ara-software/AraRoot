<?php
$rundir=$_GET["rundir"];



echo "<table border=\"1\" align=\"center\" width=\"50%\">
<tr>
<td><a name=\"canTemp\" align=\"center\"><h3>Temperatures</h3></a>
<a href=\"canTemp.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canTemp.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canRfpDiscone\" align=\"center\"><h3>RF Power Discone</h3></a>
<a href=\"canRfpDiscone.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canRfpDiscone.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canRfpBatwing\" align=\"center\"><h3>RF Power Batwing</h3></a>
<a href=\"canRfpBatwing.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canRfpBatwing.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canSclDiscone\" align=\"center\"><h3>Scaler - Discone</h3></a>
<a href=\"canSclDiscone.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canSclDiscone.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canSclBatMinus\" align=\"center\"><h3>Scaler - Batwing-</h3></a>
<a href=\"canSclBatMinus.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canSclBatMinus.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canSclBatPlus\" align=\"center\"><h3>Scaler - Batwing+</h3></a>
<a href=\"canSclBatPlus.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canSclBatPlus.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canSclGlobal\" align=\"center\"><h3>Scaler - Global</h3></a>
<a href=\"canSclGlobal.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canSclGlobal.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canSclTrigL1\" align=\"center\"><h3>Scaler - L1 Trigger</h3></a>
<a href=\"canSclTrigL1.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canSclTrigL1.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canDac\" align=\"center\"><h3>DAC</h3></a>
<a href=\"canDac.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canDac.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
</table>";
?>


