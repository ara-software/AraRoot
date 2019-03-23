CREATE TABLE ARA01(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA02(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA03(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA03_2018(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA04(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA05(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA06(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);


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

-- Chan 0-15 X, Y, Z, and delay values adjusted by BAC Jan 11 2019

insert into ARA02 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 1, 1, 1, 2, "kBicone",
10.5874,2.3432,-170.247,19.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 2, 2, "kBicone",
4.85167,-10.3981,-170.347,19.8606,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 3, 2, "kBicone",
-2.58128,9.37815,-171.589,125.547,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 25, 13, 4, 2, "kBicone",
-7.84111,-4.05791,-175.377,22.5759,  
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 0, 0, 1, 4, "kBicone",
10.5873,2.3428,-189.502,93.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 2, 4, "kBicone",
4.85157,-10.3985,-189.4,93.8306,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 3, 4, "kBicone",
-2.58138,9.37775,-190.642,199.517,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 24, 12, 4, 4, "kBicone",
-7.84131,-4.05821,-194.266,96.5459,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 3, 3, 1, 1, "kQuadSlot",
10.5874,2.3128,-167.492,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 2, 1, "kQuadSlot",
4.85167,-10.3981,-167.428,7.66056,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 3, 1, "kQuadSlot",
-2.58128,9.37825,-168.468,113.347,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(11 , "BH4", "THPol", "kHorizontal", 
3, 27, 15, 4, 1, "kQuadSlot",
-7.84111,-4.05781,-172.42,10.3759,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 2, 2, 1, 3, "kQuadSlot",
10.5873,2.3429,-186.546,81.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 2, 3, "kQuadSlot",
4.85157,-10.3985,-186.115,81.6206,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 3, 3, "kQuadSlot",
-2.58138,9.37775,-187.522,187.307,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 26, 14, 4, 3, "kQuadSlot",
-7.84111,-4.05821,-190.981,84.3359,
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

-- Chan 0-15 X, Y, Z, and delay values adjusted by BAC Jan 11 2019

insert into ARA03 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.82341,-9.41804,-172.607,19.1537,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6429,3.40846,-176.209,126.249,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 16, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.105,19.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.14654,-3.933,-172.214,21.5614,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.82321,-9.41834,-191.66,93.1237,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6426,3.40816,-194.933,200.219,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 17, 8, 0, 0, "kBicone",
-2.0102,9.412,-192.666,93.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.14674,-3.9333,-191.267,95.5314,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.82351,-9.41794,-169.486,6.95369,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6429,3.40856,-173.252,114.049,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 18, 11, 0, 0, "kQuadSlot",
-2.01,9.4124,-170.617,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.14644,-3.9329,-169.257,9.36141,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.82321,-9.41824,-188.408,80.9137,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6427,3.40826,-191.977,188.009,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 19, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.709,81.2,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA03 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.14674,-3.9332,-188.146,83.3214,
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

--Update by UAL (01/25/2018). Here I have made the ARA03 2018 table where the channel mappings for channel 2 and channel 3 have been corrected. The correction has been by essentially switching the daqChanNum variable values between them. ARA02 did not have problems for channel mapping.
----------------------------------------------------------------------------------------

insert into ARA03_2018 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.82341,-9.41804,-172.607,19.1537,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6429,3.40846,-176.209,126.249,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.105,93.41,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA03_2018 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.14654,-3.933,-172.214,21.5614,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03_2018 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.82321,-9.41834,-191.66,93.1237,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6426,3.40816,-194.933,200.219,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 0, 0, "kBicone",
-2.0102,9.412,-192.666,19.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.14674,-3.9333,-191.267,95.5314,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03_2018 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.82351,-9.41794,-169.486,6.95369,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6429,3.40856,-173.252,114.049,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 0, 0, "kQuadSlot",
-2.01,9.4124,-170.617,81.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.14644,-3.9329,-169.257,9.36141,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03_2018 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.82321,-9.41824,-188.408,80.9137,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6427,3.40826,-191.977,188.009,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.709,7.24,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.14674,-3.9332,-188.146,83.3214,
0,0,0,0,  
0,0,0,140,800,0);


--updated 15-07-13

--Positions incorrect

insert into ARA03_2018 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03_2018 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03_2018 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03_2018 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);


--NEEDS TO BE CHECKED (Note made by UAL 01/25/2019)- ARA04 and ARA05 have the same cable delays and also antennas in one string have the same cable delays (this is true for all the strings).

-- ARA04 information
-- Updated by UAL (01/25/2019). Updated the X, Y and Z coordinates of the station, the foamChanNum, the channel mappings and the cable delays. I used Yue Pan's AntennaInfoV3.sqlite to get the right channel mappings (daqChanNum) and cable delays (which I am not sure about). Used this file: https://www.overleaf.com/read/bpvxgfrhqhzh  to get the surveyor coordinates which I then converted Ara Station Centric Coordinate system.

insert into ARA04 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 17, 1, 0, 2, "kBicone",
-12.6388,22.5182,-163.16,132.3,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 0, 2, "kBicone",
11.8342,5.40161,-159.5,132.3,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 1, 9, 0, 2, "kBicone",
7.93627,-21.847,-163.52,132.3,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 25, 13, 0, 2, "kBicone",
-13.183,-4.55271,-165.27,132.3,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA04 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 16, 0, 0, 4, "kBicone",
-12.6388,22.5182,-192.84,18.62,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 0, 4, "kBicone",
11.8342,5.40161,-189.23,18.62,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 0, 8, 0, 4, "kBicone",
7.93627,-21.847,-193.3,18.62,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 24, 12, 0, 4, "kBicone",
-13.183,-4.55271,-194.9,18.62,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA04 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 19, 3, 0, 1, "kQuadSlot",
-12.6388,22.5182,-160.15,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 0, 1, "kQuadSlot",
11.8342,5.40161,-156.54,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 3, 11, 0, 1, "kQuadSlot",
7.93627,-21.847,-160.54,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 27, 15, 0, 1, "kQuadSlot",
-13.183,-4.55271,-162.35,120.54,
0,0,0,0,  
0,0,0,140,800,0);


insert into ARA04 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 18, 2, 0, 3, "kQuadSlot",
-12.6388,22.5182,-189.89,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 0, 3, "kQuadSlot",
11.8342,5.40161,-186.16,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA04 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 2, 10, 0, 3, "kQuadSlot",
7.93627,-21.847,-190.3,6.86,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA04 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 26, 14, 0, 3, "kQuadSlot",
-13.183,-4.55271,-191.99,6.86,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA04 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA04 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA04 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA04 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);



-- ARA05 information
-- Updated by UAL (01/25/2019). Updated the X, Y and Z coordinates of the station, the foamChanNum, the channel mappings and the cable delays. I used Yue Pan's AntennaInfoV3.sqlite to get the right channel mappings (daqChanNum) and cable delays (which I am not sure about). Used this file: https://www.overleaf.com/read/bpvxgfrhqhzh  to get the surveyor coordinates which I then converted Ara Station Centric Coordinate system.  

insert into ARA05 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 1, 1, 0, 2, "kBicone",
23.8258,-1.6605,-165.46,132.3,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA05 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 0, 2, "kBicone",
-3.94569,-12.6275,-164.67,132.3,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 25, 9, 0, 2, "kBicone",
-15.4036,18.0452,-161.34,132.3,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 17, 13, 0, 2, "kBicone",
11.4825,25.1047,-147.93,132.3,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA05 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 0, 0, 0, 4, "kBicone",
23.8258,-1.6605,-195.32,18.62,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 0, 4, "kBicone",
-3.94569,-12.6275,-194.52,18.62,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 24, 8, 0, 4, "kBicone",
-15.4036,18.0452,-191.28,18.62,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 16, 12, 0, 4, "kBicone",
11.4825,25.1047,-177.72,18.62,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA05 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 3, 3, 0, 1, "kQuadSlot",
23.8258,-1.6605,-162.53,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 0, 1, "kQuadSlot",
-3.94569,-12.6275,-161.75,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 27, 11, 0, 1, "kQuadSlot",
-15.4036,18.0452,-158.39,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 19, 15, 0, 1, "kQuadSlot",
11.4825,25.1047,-145.01,120.54,
0,0,0,0,  
0,0,0,140,800,0);


insert into ARA05 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 2, 2, 0, 3, "kQuadSlot",
23.8258,-1.6605,-192.35,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 0, 3, "kQuadSlot",
-3.94569,-12.6275,-191.37,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 26, 10, 0, 3, "kQuadSlot",
-15.4036,18.0452,-188.12,6.86,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA05 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 18, 14, 0, 3, "kQuadSlot",
11.4825,25.1047,-174.74,6.86,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA05 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA05 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA05 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA05 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);




-- ARA06 information
-- Placeholder infomation... for now this is just a copy of the ARA03 info

insert into ARA06 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.4113,-9.3933,-173.3937,15.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6919,3.5089,-173.9734,115.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 16, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.1054,15.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.0990,-3.7098,-173.5476,15.44,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA06 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.4111,-9.3936,-192.4470,81.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6916,3.5086,-192.6981,181.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 17, 8, 0, 0, "kBicone",
-2.0102,9.4120,-192.6658,81.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.0992,-3.7101,-192.6009,81.41,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA06 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.4114,-9.3932,-170.2728,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6919,3.5090,-171.0168,107.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 18, 11, 0, 0, "kQuadSlot",
-2.0100,9.4124,-172.6271,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.0989,-3.7097,-170.5910,7.24,
0,0,0,0,  
0,0,0,140,800,0);


insert into ARA06 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.4111,-9.3935,-189.1947,73.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6917,3.5087,-189.7415,173.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 19, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.7092,73.2,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA06 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.0992,-3.7100,-189.4800,73.2,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA06 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA06 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA06 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA06 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);















