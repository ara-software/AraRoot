CREATE TABLE ARA01(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA02(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA03(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);


--chanNum,hole,antName,polType 
--polNum,daqChan,daqtrig,foam,foamchan
--xPos,yPos,zPos,tDelay
--xPos,yPos,zPos,tDelay --calibrated
--orientX,orientY,orientZ,highPass,lowPass,noiseFig


-- not clear which Hole corresponds to which FOAM. FOAM+T/B+H/V+daqChan OK
-- which hole which trigChan?

insert into ARA01 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 1, 1, 1, 4, "kBicone",
-4.03552152,7.5351132,-63.7909824,13.714,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 2, 4, "kBicone",
8.55701616,1.20463056,-55.7022,13.714,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 3, 4, "kBicone",
-9.74665056,-5.33259792,-54.7856664,13.714,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 25, 13, 3, 4, "kBicone",
 2.210659536,-10.78126368,-61.7863128,13.714,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA01 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 0, 0, 1, 2, "kBicone",
-4.035552,7.53505224,-82.663284,85.8187,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 2, 2, "kBicone",
8.5569552,1.204578744,-73.5835968,81.94,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 3, 2, "kBicone",
-9.74671152,-5.33265888,-73.6576632,85.8187,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 24, 12, 4, 2, "kBicone",
2.210616864,-10.78129416,-79.3632144,80.7578,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA01 VALUES(8, "BH1", "BHPol", "kHorizontal", 
4, 2, 2, 1, 1, "kQuadSlot",
-4.035552,7.53505224,-79.2595824,72.5466,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(9, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 2, 1, "kQuadSlot",
8.55698568,1.204587888,-70.1292984,68.668,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(10, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 3, 1, "kQuadSlot",
-9.74671152,-5.3326284,-70.2033648,72.5466,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(11, "BH4", "BHPol", "kHorizontal", 
7, 26, 14, 4, 1, "kQuadSlot",
2.210626008,-10.78129416,-76.213716,68.668,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA01 VALUES(12, "BH1", "THPol", "kHorizontal", 
0, 3, 3, 1, 3, "kQuadSlot",
-4.03552152,7.5351132,-61.7335824,2.19432,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(13 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 2, 3, "kQuadSlot",
8.55701616,1.204636656,-53.6448,2.19432,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(14 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 3, 3, "kQuadSlot",
-9.74665056,-5.33259792,-52.7282664,2.19432,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA01 VALUES(15 , "BH4", "THPol", "kHorizontal", 
3, 27, 15, 4, 3, "kQuadSlot",
2.210665632,-10.7812332,-59.7289128,2.19432,  
0,0,0,0,  
0,0,0,140,800,0);

--Surface Positions incorrect

insert into ARA01 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
57.8711568,25.9488432,1.156344144,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
-12.316968,-59.128152,0.80206596,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
-46.0638144,37.100256,0.649775688,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
-7.65517392,3.30881736,0.943410864,0,
0,0,0,0,
0,0,0,25,116,0);



-- ARA2 Information -- This is not correct yet!
-- FOAM+daqChan done+position+estimated delay

insert into ARA02 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 1, 1, 1, 2, "kBicone",
10.5874,2.3432,-171.9273,1035.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 2, 2, "kBicone",
4.8235, -10.3840, -170.4886,1035.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 3, 2, "kBicone",
-2.6820, 8.6845, -170.3862,1135.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 25, 13, 4, 2, "kBicone",
-7.7232,-4.4671,-170.6715,1035.44,  
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 0, 0, 1, 4, "kBicone",
10.5873,2.3428,-189.5020,1101.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 2, 4, "kBicone",
4.8234,-10.3844,-189.5419,1101.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 3, 4, "kBicone",
-2.6821,8.6841,-189.4392,1201.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 24, 12, 4, 4, "kBicone",
-7.7234,-4.4674,-189.5605,1101.41,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 3, 3, 1, 1, "kQuadSlot",
10.5874,2.3128,-167.4924,1027.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 2, 1, "kQuadSlot",
4.8235,-10.3840,-167.5701,1027.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 3, 1, "kQuadSlot",
-2.6820,8.6846,-167.2653,1127.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(11 , "BH4", "THPol", "kHorizontal", 
3, 27, 15, 4, 1, "kQuadSlot",
-7.7232,-4.4670,-167.7149,1027.24,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 2, 2, 1, 3, "kQuadSlot",
10.5873,2.3429,-186.5457,1093.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 2, 3, "kQuadSlot",
4.8234,-10.3844,-186.2567,1093.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 3, 3, "kQuadSlot",
-2.6821,8.6841,-186.3186,1193.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 26, 14, 4, 3, "kQuadSlot",
-7.7232,-4.4674,-186.2753,1093.2,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13


--Positions incorrect
insert into ARA02 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole"
,1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA02 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA02 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA02 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);

-- ARA03 information
--Position Done
--daqChan Mapping done
--chanNum done
--estimated cable delay

insert into ARA03 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.4113,-9.3933,-173.3937,1135.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6919,3.5089,-173.9734,1135.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 16, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.1054,1135.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.0990,-3.7098,-173.5476,1035.44,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.4111,-9.3936,-192.4470,1201.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6916,3.5086,-192.6981,1201.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 17, 8, 0, 0, "kBicone",
-2.0102,9.4120,-192.6658,1201.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.0992,-3.7101,-192.6009,1101.41,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.4114,-9.3932,-170.2728,1127.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6919,3.5090,-171.0168,1127.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 18, 11, 0, 0, "kQuadSlot",
-2.0100,9.4124,-172.6271,1127.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.0989,-3.7097,-170.5910,1027.24,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.4111,-9.3935,-189.1947,1193.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6917,3.5087,-189.7415,1193.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 19, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.7092,1193.2,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA03 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.0992,-3.7100,-189.4800,1093.2,
0,0,0,0,  
0,0,0,140,800,0);


--updated 15-07-13

--Positions incorrect

insert into ARA03 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);









