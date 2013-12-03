
CREATE TABLE Testbed(
       chanId text primary key, 
       antDir text,   	      	   
       chanNum integer,		   
       daqChanType text,	   
       daqChanNum integer,	   
       highPassFilterMhz integer,	   
       lowPassFilterMhz integer,	   
       daqTrigChan integer,	   
       numLabChans integer,	   
       labChip text,		   
       labChans0 integer,	   
       labChans1 integer,	   
       isDiplexed integer,	   
       diplexedChans0 integer,	   
       diplexedChans1 integer,	   	
       preAmpNum text,	   
       avgNoiseFigure real,	   
       rcvrNum integer,		   
       designator text,
       antPolNum integer,
       antType text,
       polType text,
       locationName text,
       antLocation0 real,
       antLocation1 real,
       antLocation2 real,
       cableDelay real,
       debugHolePosition0 real,
       debugHolePosition1 real,
       debugHolePosition2 real,
       debugPreAmpDz real,
       debugHoelPositionZft real,
       debugHoelPositionZm real,
       debugTrueAsBuiltPosition0 real,
       debugTrueAsBuiltPosition1 real,
       debugTrueAsBuiltPosition2 real,
       debugCableDelay2 real,
       debugFeedPointDelay real,
       debugTotalCableDelay real,
       Comments text,
       antOrient0 text,
       antOrient1 text,
       antOrient2 text);


--This is the old data being put into the table

insert into Testbed VALUES( "0A1",	"kReceiver",	1,	"kDisconeChan",	1,	150,	1000,	8,	2,	"kA",	1,	"2",	0,	1,   0,		6,	73.3,	4,	"H1",	0,	"kBowtieSlot",	"kHorizontal",	"BH1",	9.19,	-4.97,	-20.61,	163.1,	9.19,	-4.97,	-20.4,	-0.213,	14.56,	0,	9.19,	-4.97,	-20.61,	152.0,	11.54,	163.1,	"H1 <-> V1", " ", " ", " ");


insert into Testbed VALUES(
"0A2",	"kReceiver",	2,	"kDisconeChan",	2,	150,	1000,	7,	2,	"kA",	3,	4,	0,	2, 0,		"7",	79.9,	2,	"H4",	3,	"kBowtieSlot",	"kHorizontal",	"BH5",	-9.53,	-4.68,	-29.8,	182.53,	-9.53,	-4.68,	30.2,	-0.213,	11.66,	-0.6,	-9.53,	-4.68,	-29.8,	170.993,	11.54,	182.53,	"H1 <-> V1", "", "", "");			

insert into Testbed VALUES(
"0A3",	"kReceiver",	3,	"kDisconeChan",	3,	150,	1000,	3,	2,	"kA",	5,	6,	0,	3, 0,		"2",	73.9,	3,	"V2",	1,	"kBicone",	"kVertical",	"BH2",	9.79,	5.46,	-22.59,	152.7,	9.79,	5.46,	-22.4,	-0.213,	14.44,	-0,	9.79,	5.46,	-22.59,	152.0,	1.12,	152.7, "", "", "", "");

insert into Testbed VALUES(				
"0A4",	"kReceiver",	4,	"kDisconeChan",	4,	150,	1000,	4,	2,	"kA",	7,	8,	0,	4, 0,		"4",	70.6,	1,	"V1",	0,	"kBicone",	"kVertical",	"BH1",	9.19,	-4.97,	-25.61,	172.11,	9.19,	-4.97,	-25.4,	-0.213,	14.56,	0,	9.19,	-4.97,	-25.61,	170.993,	1.12,	172.11,	"V1 <-> H1", "", "", "");			

insert into Testbed VALUES(
"0B1",	"kReceiver",	5,	"kDisconeChan",	5,	150,	1000,	5,	2,	"kB",	1,	2,	0,	5,	0,	"8",	78.3,	5,	"V3",	2,	"kBicone",	"kVertical",	"BH3",	0.03,	10.2,	-27.07,	172.11,	0.03,	10.2,	-27.4,	-0.213,	12.0,	-0.5,	0.03,	10.2,	-27.07,	170.993,	1.12,	172.11, "", "", "", "");


insert into Testbed VALUES(				
"0B2",	"kReceiver",	6,	"kDisconeChan",	6,	150,	1000,	6,	2,	"kB",	3,	4,	0,	6,	0,	"15",	76.9,	6,	"H2",	1,	"kBowtieSlot",	"kHorizontal",	"BH2",	9.79,	5.46,	-27.59,	182.53,	9.79,	5.46,	-27.4,	-0.213,	14.44,	-0,	9.79,	5.46,	-27.59,	170.993,	11.54,	182.53, "", "", "", "");				

insert into Testbed VALUES(
"0B3",	"kReceiver",	7,	"kDisconeChan",	7,	150,	1000,	2,	2,	"kB",	5,	6,	0,	7,	0,	"11",	80.1,	7,	"V4",	3,	"kBicone",	"kVertical",	"BH5",	-9.53,	-4.68,	-24.8,	152.7,	-9.53,	-4.68,	-25.2,	-0.213,	11.66,	-0.6,	-9.53,	-4.68,	-24.8,	152.0,	1.12,	152.7, 		"", "", "", "");

insert into Testbed VALUES(		
"0B4",	"kReceiver",	8,	"kDisconeChan",	8,	150,	1000,	1,	2,	"kB",	7,	8,	0,	8, 0,		"9",	79.3,	8,	"H3",	2,	"kBowtieSlot",	"kHorizontal",	"BH3",	0.03,	10.2,	-22.07,	163.1,	0.03,	10.2,	-22.4,	-0.213,	12.0,	-0.5,	0.03,	10.2,	-22.07,	152.0,	11.54,	163.1, "", "", "", "");
				

insert into Testbed VALUES(
"0C1",	"kReceiver",	9,	"kBatwingChan",	1,	100,	450,	8,	1,	"kC",	1,	0,	0,	9,	0,	"12",	80.1,	9,	"V5",	4,	"kDiscone",	"kVertical",	"SH1",	3.51,	-1.81,	-0.24,	95.193,	3.51,	-1.81,	-1,	0,	10.98,	-0.8,	3.51,	-1.81,	-0.24,	95.1934,	0.0,	95.193, "", "", "", "");				

insert into Testbed VALUES(
"0C2",	"kReceiver",	10,	"kBatwingChan",	2,	100,	450,	7,	1,	"kC",	2,	0,	0,	10,	0,	"10",	81.1,	10,	"H6",	5,	"kBatwing",	"kHorizontal",	"SH2",	-0.08,	4.09,	-0.29,	95.193,	-0.08,	4.09,	-1,	0,	11.18,	-0.7,	-0.08,	4.09,	-0.29,	95.1934,	0.0,	95.193, "", "", "", "");				

insert into Testbed VALUES(
"0C3",	"kReceiver",	11,	"kBatwingChan",	3,	100,	450,	3,	1,	"kC",	3,	0,	0,	11,	0,	"16",	82.3,	11,	"V6",	5,	"kDiscone",	"kVertical",	"SH3",	-3.37,	-1.88,	-0.29,	95.193,	-3.37,	-1.88,	-1,	0,	11.2,	-0.7,	-3.37,	-1.88,	-0.29,	95.1934,	0.0,	95.193, "", "", "", "");				

insert into Testbed VALUES(
"0C4",	"kReceiver",	12,	"kBatwingChan",	4,	100,	450,	4,	1,	"kC",	4,	0,	0,	12,	0,	"5",	84.2,	12,	"H5",	4,	"kBatwing",	"kHorizontal",	"SH1",	3.51,	-1.81,	-1.24,	95.193,	3.51,	-1.81,	-2,	0,	10.98,	-0.8,	3.51,	-1.81,	-1.24,	95.1934,	0.0,	95.193, "", "", "", "");				

insert into Testbed VALUES(
"0C5",	"kReceiver",	13,	"kBatwingChan",	5,	100,	450,	5,	1,	"kC",	5,	0,	0,	13,	0,	"3",	87.8,	13,	"H8",	7,	"kQuadSlot",	"kHorizontal",	"BH6",	2.96,	-10.44,	-26.44,	151.6,	2.96,	-10.44,	-26.5,	0,	14.26,	-0.1,	2.96,	-10.44,	-26.44,	152.0,	0.0,	151.6,	"replace surface discone V6", "", "", "");		

insert into Testbed VALUES(	
"0C6",	"kReceiver",	14,	"kBatwingChan",	6,	100,	450,	6,	1,	"kC",	6,	0,	0,	14,	0,	"13",	84.6,	14,	"H7",	6,	"kQuadSlot",	"kHorizontal",	"BH6",	2.96,	-10.44,	-30.44,	170.99,	2.96,	-10.44,	-30.5,	0,	14.26,	-0.1,	2.96,	-10.44,	-30.44,	170.993,	0.0,	170.99,	"replace surface BW H7", "", "", "");			

insert into Testbed VALUES(
"0C7",	"kReceiver",	15,	"kBatwingChan",	7,	25,	300,	2,	1,	"kC",	7,	0,	0,	15,	0,	"A",	0.0,	15,	"S1",	0,	"kFatDipole",	"kSurface",	"ST1",	5.71,	-19.45,	2.28,	108.74,	5.71,	-19.45,	-0.8,	0,	0.0,	-3.1,	5.7,	-19.45,	2.28,	108.745,	0.0,	108.74,	"oriented EW -8 deg", "", "", "");
			
insert into Testbed VALUES(
"0C8",	"kReceiver",	16,	"kBatwingChan",	8,	25,	300,	1,	1,	"kC",	8,	0,	0,	15,	0,	"B",	0.0,	16,	"S2",	1,	"kFatDipole",	"kSurface",	"ST2",	-16.7,	12,	2.28,	108.74,	-16.7,	12,	-0.8,	0,	0.0,	-3.1,	-16.7,	12,	2.28,	108.745,	0.0,	108.74,	"oriented EW -30 deg", "", "", "");			



-- Now let's just update the positions of the antennas

---- This is Jonathan's Version

-- UPDATE Testbed 
-- SET antLocation0=-8.421664, antLocation1=-4.424655, antLocation2=-20.49487
-- WHERE antPolNum='0' AND polType='kVertical';

-- UPDATE Testbed 
-- SET antLocation0=-8.421896, antLocation1=-4.429794, antLocation2=-25.49487
-- WHERE antPolNum='0' AND polType='kHorizontal';

-- UPDATE Testbed 
-- SET antLocation0=-0.425641, antLocation1=-11.14924, antLocation2=-22.49816
-- WHERE antPolNum='1' AND polType='kVertical';

-- UPDATE Testbed 
-- SET antLocation0=-0.425873, antLocation1=-11.15438, antLocation2=-27.49815
-- WHERE antPolNum='1' AND polType='kHorizontal';

-- UPDATE Testbed 
-- SET antLocation0=9.2192918, antLocation1=-6.180130, antLocation2=-27.72559
-- WHERE antPolNum='2' AND polType='kVertical';

-- UPDATE Testbed 
-- SET antLocation0=9.2195238, antLocation1=-6.174991, antLocation2=-22.72559
-- WHERE antPolNum='2' AND polType='kHorizontal';

-- UPDATE Testbed 
-- SET antLocation0=3.0199389, antLocation1=10.383029, antLocation2=25.57077
-- WHERE antPolNum='3' AND polType='kVertical';

-- UPDATE Testbed 
-- SET antLocation0=3.0197069, antLocation1=10.377889, antLocation2=-30.57077
-- WHERE antPolNum='3' AND polType='kHorizontal';


---This is Carl and Amy's version (will be better)
--UPDATE Testbed
--SET  antLocation0=-8.4193682,	antLocation1=-4.4039331, antLocation2=-20.498074
--WHERE chanId='0A1';

--UPDATE Testbed
--SET antLocation0= 3.02253691,	antLocation1=10.4088892, antLocation2=-30.558942
--WHERE chanId='0A2';

--UPDATE Testbed
--SET antLocation0=-0.4232361,	antLocation1=-11.12648,	antLocation2=-22.507315
--WHERE chanId='0A3';

--UPDATE Testbed
--SET antLocation0=-8.4193377,	antLocation1=-4.4039331, antLocation2=-25.498074
--WHERE chanId='0A4';



--UPDATE Testbed
--SET antLocation0=9.22196754, antLocation1=-6.152022, antLocation2=-27.72924
--WHERE chanId='0B1';

--UPDATE Testbed
--SET antLocation0=-0.4232148, antLocation1=-11.12648, antLocation2=-27.507315
--WHERE chanId='0B2';

--UPDATE Testbed
--SET antLocation0=3.02251558, antLocation1=10.4089197, antLocation2=-25.55879
--WHERE chanId='0B3';

--UPDATE Testbed
--SET antLocation0=9.22196754, antLocation1=-6.152022, antLocation2=-22.72924
--WHERE chanId='0B4';

--UPDATE Testbed
--SET antLocation0=-2.4813127, antLocation1=-1.7493112, antLocation2=-1.2148352
--WHERE chanId='0C1';

--UPDATE Testbed
--SET antLocation0=4.3936309, antLocation1=-2.4089745, antLocation2=-1.1910242
--WHERE chanId='0C2';

--UPDATE Testbed
--SET antLocation0=1.58057997, antLocation1=3.80024628, antLocation2=-1.1910242
--WHERE chanId='0C3';

--UPDATE Testbed
--SET antLocation0=-2.4813097, antLocation1=-1.7493142, antLocation2=-2.2148352
--WHERE chanId='0C4';

--UPDATE Testbed
--SET antLocation0=-9.074932, antLocation1=3.8573658, antLocation2=-26.411072
--WHERE chanId='0C5';

--UPDATE Testbed
--SET antLocation0=-9.074932, antLocation1=3.8573658, antLocation2=-30.411053
--WHERE chanId='0C6';

--UPDATE Testbed
--SET antLocation0=0, antLocation1=0, antLocation2=0
--WHERE chanId='0C7';

--UPDATE Testbed
--SET antLocation0=0, antLocation1=0, antLocation2=0
--WHERE chanId='0C8';