<?php
$rundir=$_GET["rundir"];

echo "<table border=\"1\" align=\"center\" width=\"50%\">
<tr>
<td><a name=\"canAverageFFT\" align=\"center\"><h3>Average FFT</h3></a>
<a href=\"canAverageFFT.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canAverageFFT.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canWaveformRMSTime\" align=\"center\"><h3>Waveform RMS</h3></a>
<a href=\"canWaveformRMSTime.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canWaveformRMSTime.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canWaveformSNRTime\" align=\"center\"><h3>Waveform SNR</h3></a>
<a href=\"canWaveformSNRTime.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canWaveformSNRTime.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canAverageFFTTime\" align=\"center\"><h3>Average FFT v Time</h3></a>
<a href=\"canAverageFFTTime.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canAverageFFTTime.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canEventRate\" align=\"center\"><h3>Event Rate</h3></a>
<a href=\"canEventRate.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canEventRate.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canDeadTime\" align=\"center\"><h3>Dead Time</h3></a>
<a href=\"canDeadTime.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canDeadTime.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canEventNumber\" align=\"center\"><h3>Event Number</h3></a>
<a href=\"canEventNumber.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canEventNumber.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canPpsNum\" align=\"center\"><h3>PPS Num</h3></a>
<a href=\"canPpsNum.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canPpsNum.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canRcoCount\" align=\"center\"><h3>RCO Count</h3></a>
<a href=\"canRcoCount.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canRcoCount.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canRoVdd\" align=\"center\"><h3>ROVDD</h3></a>
<a href=\"canRoVdd.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canRoVdd.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canTrigTypeBit\" align=\"center\"><h3>Trigger type</h3></a>
<a href=\"canTrigTypeBit.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canTrigTypeBit.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canTrigPattern\" align=\"center\"><h3>Trigger Pattern</h3></a>
<a href=\"canTrigPattern.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canTrigPattern.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canUnixTimeUs\" align=\"center\"><h3>Sub Second Timing</h3></a>
<a href=\"canUnixTimeUs.php?rundir=$rundir\" target=\"_top\"> 
<img src=\"$rundir/canUnixTimeUs.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canTrigPatternTime\" align=\"center\"><h3>Trigger Pattern</h3></a>
<a href=\"canTrigPatternTime.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canTrigPatternTime.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
<tr>
<td><a name=\"canErrorFlag\" align=\"center\"><h3>Error  Flag</h3></a>
<a href=\"canErrorFlag.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canErrorFlag.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canPriority\" align=\"center\"><h3>Priority</h3></a>
<a href=\"canPriority.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canPriority.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
<td><a name=\"canCalibStatusBit\" align=\"center\"><h3>Calib Status Bit</h3></a>
<a href=\"canCalibStatusBit.php?rundir=$rundir\" target=\"_top\">
<img src=\"$rundir/canCalibStatusBit.png\" alt=\"Resized PNG graphic\" title=\"Click to view\" border=\"1\" width=\"158\" height=\"150\" hspace=\"10\" /></a>
</td>
</tr>
</table>"

?>


