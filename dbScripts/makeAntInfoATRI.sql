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
9.9921,1.8342,-171.9690,15.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 2, 2, "kBicone",
4.2334,-10.8887,-170.4728,15.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 3, 2, "kBicone",
-3.2719,8.1801,-170.4008,15.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 25, 13, 4, 2, "kBicone",
-8.3140,-4.9722,-170.6297,15.44,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA02 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 0, 0, 1, 4, "kBicone",
9.9311,1.7818,-189.5437,81.41,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 2, 4, "kBicone",
4.1672,-10.9454,-189.5257,81.41,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 3, 4, "kBicone",
-3.3381,8.1233,-189.4538,81.41,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 24, 12, 4, 4, "kBicone",
-8.3796,-5.0286,-189.5184,81.41,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA02 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 3, 3, 1, 1, "kQuadSlot",
10.0075,1.8475,-167.5341,7.24,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 2, 1, "kQuadSlot",
4.2436,-10.8798,-167.5162,7.24,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 3, 1, "kQuadSlot",
-3.2611,8.1894,-167.2799,7.24,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(11 , "BH4", "THPol", "kHorizontal", 
3, 27, 15, 4, 1, "kQuadSlot",
-8.3038,-4.9634,-167.6731,7.24,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA02 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 2, 2, 1, 3, "kQuadSlot",
9.9414,1.7907,-186.5871,73.2,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 2, 3, "kQuadSlot",
4.1786,-10.9357,-186.2409,73.2,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 3, 3, "kQuadSlot",
-3.3272,8.1326,-186.3329,73.2,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 26, 14, 4, 3, "kQuadSlot",
-8.3682,-5.0188,-186.2335,73.2,  
0,0,0,0,  
0,0,0,140,800,0);



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
3.6661,-9.7544,-173.3912,15.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
9.9440,3.1465,-174.0246,15.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 16, 9, 0, 0, "kBicone",
-2.7584,9.0497,-174.1142,15.44, 
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.8447,-4.0713,-173.5031,15.44,  
0,0,0,0,  
0,0,0,140,800,0);


insert into ARA03 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
3.5840,-9.7944,-192.4442,81.41,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
9.8633,3.1073,-192.7490,81.41,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 17, 8, 0, 0, "kBicone",
-2.8384,9.0107,-192.6747,81.41,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.9269,-4.1113,-192.5561,81.41,  
0,0,0,0,  
0,0,0,140,800,0);


insert into ARA03 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
3.6796,-9.7478,-170.2704,7.24,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
9.9567,3.1528,-171.0680,7.24,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 18, 11, 0, 0, "kQuadSlot",
-2.7520,9.0528,-172.6362,7.24,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.8320,-4.0651,-170.5465,7.24,  
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA03 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
3.5980,-9.7876,-189.1920,73.2,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
9.8760,3.1134,-189.7924,73.2,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 19, 10, 0, 0, "kQuadSlot",
-2.8257,9.0169,-189.7181,73.2,  
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA03 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.9134,-4.1047,-189.4355,73.2,  
0,0,0,0,  
0,0,0,140,800,0);

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









