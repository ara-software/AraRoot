
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


-- Feb 7 2019
-- Add the source code for ICRR Station 1
-- This will probably never be used
-- But is necessary for some backwards compatibility
-- I recovered this using SQliteStudio from the AntennaInfo.sqlite file in branch 3.13

CREATE TABLE Station1 (
        chanId TEXT primary key,
        antDir TEXT,
        chanNum INTEGER,
        daqChanType TEXT,
        daqChanNum INTEGER,
        highPassFilterMhz INTEGER,
        lowPassFilterMhz INTEGER,
        daqTrigChan INTEGER,
        numLabChans INTEGER,
        labChip TEXT,
        labChans0 INTEGER,
        labChans1 INTEGER,
        isDiplexed INTEGER,
        diplexedChans0 INTEGER,
        diplexedChans1 INTEGER,
        preAmpNum TEXT,
        avgNoiseFigure REAL,
        rcvrNum INTEGER,
        designator TEXT,
        antPolNum INTEGER,
        antType TEXT,
        polType TEXT,
        locationName TEXT,
        antLocation0 REAL,
        antLocation1 REAL,
        antLocation2 REAL,
        cableDelay REAL,
        debugHolePosition0 REAL,
        debugHolePosition1 REAL,
        debugHolePosition2 REAL,
        debugPreAmpDz REAL,
        debugHolePositionZft REAL,
        debugHolePositionZm REAL,
        debugTrueAsBuiltPosition0 REAL,
        debugTrueAsBuiltPosition1 REAL,
        debugTrueAsBuiltPosition2 REAL,
        debugCableDelay2 REAL,
        debugFeedPointDelay REAL,
        debugTotalCableDelay REAL,
        Comments TEXT,
        antOrient0 TEXT,
        antOrient1 TEXT,
        antOrient2 TEXT);

INSERT INTO Station1  VALUES ('0A1', 'kReceiver', 1, 'kDisconeChan', 1, 140, 800, 1, 2, 'kA', 1, '2', 0, 1, NULL, '6', 73.3, 4, 'TV1', 0, 'kBicone', 'kVertical', 'BH1', '-4.035516896', '7.535106494', '-61.733665', 13.714, '9.19', '-4.97', '-20.4', '-0.213', 14.56, '0', '9.19', '-4.97', '-20.61', 152.0, 11.54, 163.1, 'H1 <-> V1', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A2', 'kReceiver', 2, 'kDisconeChan', 2, 140, 800, 2, 2, 'kA', 3, '4', 0, 2, NULL, '15', 76.9, 6, 'TV2', 1, 'kBicone', 'kVertical', 'BH2', '8.557013198', '1.204630773', '-55.70208182', 13.714, '9.79', '5.46', '-27.4', '-0.213', 14.44, '0', '9.79', '5.46', '-27.59', 170.993, 11.54, 182.53, '0', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A3', 'kReceiver', 3, 'kDisconeChan', 3, 140, 800, 3, 2, 'kA', 5, '6', 0, 3, NULL, '10', 81.1, 10, 'TV3', 2, 'kBicone', 'kVertical', 'BH3', '-9.746662207', '-5.332598124', '-54.78559446', 13.714, '-0.08', '4.09', '-1', '0', 11.18, '-0.7', '-0.08', '4.09', '-0.29', 95.1934, 0.0, 95.193, '0', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A4', 'kReceiver', 4, 'kDisconeChan', 4, 140, 800, 4, 2, 'kA', 7, '8', 0, 4, NULL, '13', 84.6, 14, 'TV4', 3, 'kBicone', 'kVertical', 'BH4', '2.210660341', '-10.78124952', '-61.78644499', 13.714, '2.96', '-10.44', '-30.5', '0', 14.26, '-0.1', '2.96', '-10.44', '-30.44', 170.993, 0.0, 170.99, 'replace surface BW H7', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A5', 'kReceiver', 5, 'kDisconeChan', 5, 140, 800, 5, 2, 'kB', 1, '2', 0, 5, NULL, '4', 70.6, 1, 'BV1', 4, 'kBicone', 'kVertical', 'BH1', '-4.035563384', '7.535051145', '-82.66326785', 85.8187, '9.19', '-4.97', '-25.4', '-0.213', 14.56, '0', '9.19', '-4.97', '-25.61', 170.993, 1.12, 172.11, 'V1 <-> H1', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A6', 'kReceiver', 6, 'kDisconeChan', 6, 140, 800, 6, 2, 'kB', 3, '4', 0, 6, NULL, '9', 79.3, 8, 'BV2', 5, 'kBicone', 'kVertical', 'BH2', '8.556970337', '1.204579772', '-73.58368423', 81.94, '0.03', '10.2', '-22.4', '-0.213', 12.0, '-0.5', '0.03', '10.2', '-22.07', 152.0, 11.54, 163.1, '0', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A7', 'kReceiver', 7, 'kDisconeChan', 7, 140, 800, 7, 2, 'kB', 5, '6', 0, 7, NULL, '5', 84.2, 12, 'BV3', 6, 'kBicone', 'kVertical', 'BH3', '-9.746708696', '-5.332653474', '-73.65779702', 85.8187, '3.51', '-1.81', '-2', '0', 10.98, '-0.8', '3.51', '-1.81', '-1.24', 95.1934, 0.0, 95.193, '0', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0A8', 'kReceiver', 8, 'kDisconeChan', 8, 140, 800, 8, 2, 'kB', 7, '8', 0, 8, NULL, 'B', 0.0, 16, 'BV4', 7, 'kBicone', 'kVertical', 'BH4', '2.210616658', '-10.78130154', '-79.36324738', 80.7578, '-16.7', '12', '-0.8', '0', 0.0, '-3.1', '-16.7', '12', '2.28', 108.745, 0.0, 108.74, 'oriented EW -30 deg', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0C1', 'kReceiver', 9, 'kBatwingChan', 1, 140, 800, 1, 1, 'kC', 1, '', 0, 9, NULL, '2', 73.9, 3, 'BH1', 0, 'kQuadSlot', 'kHorizontal', 'BH1', '-4.035554327', '7.535062288', '-79.25966739', 72.5466, '9.79', '5.46', '-22.4', '-0.213', 14.44, '0', '9.79', '5.46', '-22.59', 152.0, 1.12, 152.7, '0', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0C2', 'kReceiver', 10, 'kBatwingChan', 2, 140, 800, 2, 1, 'kC', 2, '', 0, 10, NULL, '11', 80.1, 7, 'BH2', 1, 'kQuadSlot', 'kHorizontal', 'BH2', '8.556978508', '1.204589403', '-70.12928378', 68.668, '-9.53', '-4.68', '-25.2', '-0.213', 11.66, '-0.6', '-9.53', '-4.68', '-24.8', 152.0, 1.12, 152.7, '0', 0.0, 0.0, 0.0);
INSERT INTO Station1  VALUES ('0C3', 'kReceiver', 11, 'kBatwingChan', 3, 140, 800, 3, 1, 'kC', 3, NULL, 0, 11, NULL, '16', 82.3, 11, 'BH3', 2, 'kQuadSlot', 'kHorizontal', 'BH3', '-9.746700525', '-5332643842', '-70.20339657', 72.5466, '-3.37', '-1.88', '-1', '0', 11.2, '-0.7', '-3.37', '-1.88', '-0.29', 95.1934, 0.0, 95.193, NULL, NULL, NULL, NULL);
INSERT INTO Station1  VALUES ('0C4', 'kReceiver', 12, 'kBatwingChan', 4, 140, 800, 4, 1, 'kC', 4, NULL, 0, 12, NULL, 'A', 0.0, 15, 'BH4', 3, 'kQuadSlot', 'kHorizontal', 'BH4', '2.210625406', '-10.7812907', '-76.21364695', 68.668, '5.71', '-19.45', '-0.8', '0', 0.0, '-3.1', '5.7', '-19.45', '2.28', 108.745, 0.0, 108.74, 'oriented EW -8 deg', NULL, NULL, NULL);
INSERT INTO Station1  VALUES ('0C5', 'kReceiver', 13, 'kBatwingChan', 5, 116, 800, '', 1, 'kC', 5, '', 1, 13, NULL, '6', 73.3, 4, 'TH1', 4, 'kQuadSlot', 'kHorizontal', 'BH1', '-4.035511338', '7.535113197', '-61.733665', 2.19432, '9.19', '-4.97', '-20.4', '-0.213', 14.56, '0', '9.19', '-4.97', '-20.61', 152.0, 11.54, 163.1, 'H1 <-> V1', NULL, NULL, NULL);
INSERT INTO Station1  VALUES ('0C6', 'kReceiver', 14, 'kBatwingChan', 6, 116, 800, '', 1, 'kC', 6, '', 1, 14, NULL, '8', 78.3, 5, 'TH2', 5, 'kQuadSlot', 'kHorizontal', 'BH2', '8.557018756', '1.204637475', '-53.64468153', 2.19432, '0.03', '10.2', '-27.4', '-0.213', 12.0, '-0.5', '0.03', '10.2', '-27.07', 170.993, 1.12, 172.11, NULL, NULL, NULL, NULL);
INSERT INTO Station1  VALUES ('0C7', 'kReceiver', 15, 'kBatwingChan', 7, 116, 800, '', 1, 'kC', 7, NULL, 1, 15, NULL, '12', 80.1, 9, 'TH3', 6, 'kQuadSlot', 'kHorizontal', 'BH3', '-9.74665665', '-5.332591422', '-52.72819417', 2.19432, '3.51', '-1.81', '-1', '0', 10.98, '-0.8', '3.51', '-1.81', '-0.24', 95.1934, 0.0, 95.193, NULL, NULL, NULL, NULL);
INSERT INTO Station1  VALUES ('0C8', 'kReceiver', 16, 'kBatwingChan', 8, 116, 800, '', 1, 'kC', 8, NULL, 1, 16, 16, '3', 87.8, 13, 'TH4', 7, 'kQuadSlot', 'kHorizontal', 'BH4', '2.210665899', '-10.78124281', '-59.7290447', 2.19432, '2.96', '-10.44', '-26.5', '0', 14.26, '-0.1', '2.96', '-10.44', '-26.44', 152.0, 0.0, 151.6, 'replace surface discone V6', NULL, NULL, NULL);
INSERT INTO Station1  VALUES ('0C9', 'kReceiver', 17, 'kBatwingChan', 5, 25, 116, '', 1, 'kC', 5, NULL, 1, NULL, NULL, NULL, NULL, NULL, 'SA1', 0, 'kQuadSlot', 'kSurface', 'S', '57.90371066', '25.98233713', '1.15635638', '', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0.041745408, -3.7182552, -0.103734108);
INSERT INTO Station1  VALUES ('0C10', 'kReceiver', 18, 'kBatwingChan', 6, 25, 116, '', 1, 'kC', 6, NULL, 1, NULL, NULL, NULL, NULL, NULL, 'SA2', 1, 'kQuadSlot', 'kSurface', 'S', '-12.29663928', '-59.10969575', '0.802053132', '', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, -1.48722588, -3.42235536, 0.183637428);
INSERT INTO Station1  VALUES ('0C11', 'kReceiver', 19, 'kBatwingChan', 7, 25, 116, '', 1, 'kC', 7, NULL, 1, NULL, NULL, NULL, NULL, NULL, 'SA3', 2, 'kFatDipole', 'kSurface', 'S', '-46.04578823', '37.11795551', '0.649757409', '', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, -1.602644496, -3.35011776, 0.054101086);
INSERT INTO Station1  VALUES ('0C12', 'kReceiver', 20, 'kBatwingChan', 8, 25, 116, '', 1, 'kC', 8, NULL, 1, NULL, NULL, NULL, NULL, NULL, 'SA4', 3, 'kFatDipole', 'kSurface', 'S', '-7.624187757', '3.34201688', '0.943424511', '', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '', -1.477749648, -3.49761048, 0.040583206);



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