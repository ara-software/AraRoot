<div class="vertical">
<?php
virtual("/monitor/leftMain.shtml");
?>
<!--#include virtual="/monitor/leftMain.shtml" -->
<!--#include virtual="/monitor/leftTimes.shtml" -->
<h2 class="navigation">Event Plots</h2>
<p class="navigation">
<a class=
"leftBar" title="Last Event" href="/monitor/events/lastEvent.shtml"
accesskey="C">Last Event</a>
<span class=
"spacer">&nbsp;&nbsp;</span>
<a class=
"leftBar" title="Browse Events" href="/monitor/events/browseEvents.php"
accesskey="C">Browse Events</a>
<span class=
"spacer">&nbsp;&nbsp;</span>
</p>

<p class="navigation">
<FORM NAME="nav1"><DIV>
<SELECT NAME="SelectURL" onChange="document.location.href=document.nav1.SelectURL.options[document.nav1.SelectURL.selectedIndex].value">
<OPTION VALUE="."
SELECTED>One Hour:
<OPTION VALUE="/monitor/events/canAverageFFTOneHour.shtml">
Average FFT
</SELECT><DIV>
</FORM>
</p>

<p class="navigation">
<FORM NAME="nav2"><DIV>
<SELECT NAME="SelectURL" onChange="document.location.href=document.nav2.SelectURL.options[document.nav2.SelectURL.selectedIndex].value">
<OPTION VALUE="."
SELECTED>Six Hours:
<OPTION VALUE="/monitor/events/canAverageFFTSixHours.shtml">
Average FFT
</SELECT><DIV>
</FORM>
</p>

<p class="navigation">
<FORM NAME="nav3"><DIV>
<SELECT NAME="SelectURL" onChange=
"document.location.href=
document.nav3.SelectURL.options[document.nav3.SelectURL.selectedIndex].value">
<OPTION VALUE="."
SELECTED>Twelve Hours:
<OPTION VALUE="/monitor/events/canAverageFFTTwelveHours.shtml">
Average FFT
</SELECT><DIV>
</FORM>
</p>



<p class="navigation">
<FORM NAME="nav4"><DIV>
<SELECT NAME="SelectURL" onChange="document.location.href=document.nav4.SelectURL.options[document.nav4.SelectURL.selectedIndex].value">
<OPTION VALUE="."
SELECTED>One Day:
<OPTION VALUE="/monitor/events/canAverageFFTOneDay.shtml">
Average FFT
</SELECT><DIV>
</FORM>
</p>

<p class="navigation">
<FORM NAME="nav5"><DIV>
<SELECT NAME="SelectURL" onChange="document.location.href=document.nav5.SelectURL.options[document.nav5.SelectURL.selectedIndex].value">
<OPTION VALUE="."
SELECTED>All:
<OPTION VALUE="/monitor/events/canAverageFFTAll.shtml">
Average FFT
</SELECT><DIV>
</FORM>
</p>





