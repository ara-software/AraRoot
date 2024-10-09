#include "TMath.h"
#include "RayTraceCorrelator.h"
#include "FFTtools.h"


std::unique_ptr<TGraph> RayTraceCorrelator::getNormalisedGraphByRMS(TGraph *grIn){
    Double_t rms=grIn->GetRMS(2);
    Double_t *xVals = grIn->GetX();
    Double_t *yVals = grIn->GetY();
    Int_t numPoints = grIn->GetN();
    Double_t *newY = new Double_t [numPoints];
    for(int i=0;i<numPoints;i++) {
        newY[i]=(yVals[i])/rms;
    }
    std::unique_ptr<TGraph> grOut{new TGraph(numPoints,xVals,newY)};
    delete [] newY;
    return grOut;    
}

// FIXME: Eventually re-write with unique pointers
double* RayTraceCorrelator::getCorrelation_NoNorm(int length, double * oldY1, double * oldY2) {

    FFTWComplex * theFFT1 = FFTtools::doFFT(length, oldY1);
    FFTWComplex * theFFT2 = FFTtools::doFFT(length, oldY2);

    int newLength = (length / 2) + 1;
    FFTWComplex * tempStep = new FFTWComplex[newLength];
    int no2 = length >> 1;
    for (int i = 0; i < newLength; i++) {
        double reFFT1 = theFFT1[i].re;
        double imFFT1 = theFFT1[i].im;
        double reFFT2 = theFFT2[i].re;
        double imFFT2 = theFFT2[i].im;

        //Real part of output 
        tempStep[i].re = (reFFT1 * reFFT2 + imFFT1 * imFFT2);
        //Imaginary part of output 
        tempStep[i].im = (imFFT1 * reFFT2 - reFFT1 * imFFT2);
    }
    double * theOutput = FFTtools::doInvFFT(length, tempStep);
    delete[] theFFT1;
    delete[] theFFT2;
    delete[] tempStep;
    return theOutput;
}

// FIXME: Eventually re-write with unique pointers
TGraph* RayTraceCorrelator::getCorrelationGraph_WFweight(TGraph * gr1, TGraph * gr2) {
    //Now we'll extend this up to a power of 2
    int length = gr1 -> GetN();
    int length2 = gr2 -> GetN();

    int N = int(TMath::Power(2, int(TMath::Log2(length)) + 2));
    if (N < length2)
        N = int(TMath::Power(2, int(TMath::Log2(length2)) + 2));

    //Will really assume that N's are equal for now
    int firstRealSamp = (N - length) / 2;

    double * oldY1 = new double[N];
    double * oldY2 = new double[N];

    double x, y;
    Double_t x2, y2;
    gr1 -> GetPoint(1, x2, y2);
    gr1 -> GetPoint(0, x, y);
    double deltaT = x2 - x;
    double firstX = x;

    gr2 -> GetPoint(0, x2, y2);
    double waveOffset = firstX - x2;

    int OffsetBin = (int)(waveOffset / deltaT);

    //    gr1->GetPoint(N/2,x2,y2);
    //    double offset=x-x2;
    //    std::cout << length << "\t" << length2 << "\n";

    for (int i = 0; i < N; i++) {

        if (i < firstRealSamp || i >= firstRealSamp + length)
            y = 0;
        else {
            gr1 -> GetPoint(i - firstRealSamp, x, y);
        }
        oldY1[i] = y;

        if (i < firstRealSamp || i >= firstRealSamp + length2)
            y = 0;
        else {
            gr2 -> GetPoint(i - firstRealSamp, x, y);
        }
        oldY2[i] = y;

    }

    //    offset+=waveOffset;

    double * xVals = new double[N];
    double * yVals = new double[N];

    int dBin;
    double Norm1 = 0.;
    double Norm2 = 0.;

    //double *corVals=FFTtools::getCorrelation(N,oldY1,oldY2);
    double * corVals = getCorrelation_NoNorm(N, oldY1, oldY2);
    for (int i = 0; i < N; i++) {

        Norm1 = 0.;
        Norm2 = 0.;

        if (i < N / 2) {
            //Positive
            xVals[i + (N / 2)] = (i * deltaT) + waveOffset;
            //yVals[i+(N/2)]=corVals[i];

            dBin = i + OffsetBin;

            if (dBin < 0) {
                for (int i = -(dBin); i < N; i++) {
                    Norm1 += oldY1[i] * oldY1[i];
                }
                for (int i = 0; i < N + (dBin); i++) {
                    Norm2 += oldY2[i] * oldY2[i];
                }
            } else { // dBin >= 0
                for (int i = 0; i < N - (dBin); i++) {
                    Norm1 += oldY1[i] * oldY1[i];
                }
                for (int i = (dBin); i < N; i++) {
                    Norm2 += oldY2[i] * oldY2[i];
                }
            }

            //cout<<"Norm1 : "<<Norm1<<", Norm2 : "<<Norm2<<endl;

            if (Norm1 > 0. && Norm2 > 0.)
                yVals[i + (N / 2)] = corVals[i] / (sqrt(Norm1) * sqrt(Norm2));
            else
                yVals[i + (N / 2)] = corVals[i];
        } else {
            //Negative
            xVals[i - (N / 2)] = ((i - N) * deltaT) + waveOffset;

            dBin = i - N + OffsetBin;

            if (dBin < 0) {
                for (int i = -(dBin); i < N; i++) {
                    Norm1 += oldY1[i] * oldY1[i];
                }
                for (int i = 0; i < N + (dBin); i++) {
                    Norm2 += oldY2[i] * oldY2[i];
                }
            } else { // dBin >= 0
                for (int i = 0; i < N - (dBin); i++) {
                    Norm1 += oldY1[i] * oldY1[i];
                }
                for (int i = (dBin); i < N; i++) {
                    Norm2 += oldY2[i] * oldY2[i];
                }
            }

            //cout<<"Norm1 : "<<Norm1<<", Norm2 : "<<Norm2<<endl;

            if (Norm1 > 0. && Norm2 > 0.)
                yVals[i - (N / 2)] = corVals[i] / (sqrt(Norm1) * sqrt(Norm2));
            else
                yVals[i - (N / 2)] = corVals[i];

        }
    }

    TGraph * grCor = new TGraph(N, xVals, yVals);
    delete[] oldY1;
    delete[] oldY2;
    delete[] xVals;
    delete[] yVals;
    delete[] corVals;

    return grCor;
}

// FIXME: Eventually re-write with unique pointers
TGraph *RayTraceCorrelator::getCorrelationGraph_OSUNormalization(TGraph *gr1, TGraph *gr2){
    TGraph *corr = FFTtools::getCorrelationGraph(gr1,gr2);
    double RMS1 = gr1->GetRMS(2);
    double RMS2 = gr2->GetRMS(2);

    double t1i = gr1->GetX()[0];
    double t1f = gr1->GetX()[gr1->GetN()-1];
    double t2i = gr2->GetX()[0];
    double t2f = gr2->GetX()[gr2->GetN()-1];
    for(int corrsamp=0; corrsamp<corr->GetN(); corrsamp++){
        double lag, corrval;
        corr->GetPoint(corrsamp, lag, corrval);
        double t2i_new = t2i+lag;
        double t2f_new = t2f+lag;
        double integral_start=-1000000;
        double integral_stop=-500000;
        bool do_integral;
        if(
            t2i_new < t1i
            &&
            t2f_new < t1f
        )
            {
            integral_start = t1i;
            integral_stop = t2f_new;
            do_integral=true;
        }
        else if(
            t2i_new > t1i
            &&
            t2f_new > t1f
        )
            {
            integral_start = t2i_new;
            integral_stop = t1f;
            do_integral=true;
        }
        else if(
            t2i_new > t1i
            &&
            t2f_new < t1f
        )
            {
            integral_start = t2i_new;
            integral_stop = t2f_new;
            do_integral=true;

        }
        else if(
            t2i_new < t1i
            &&
            t2f_new > t1f
        ){
            integral_start = t1i;
            integral_stop = t1f;
            do_integral=true;
        }
        else if(
            (t2i_new-t1i)<0.0001
        ){
            integral_start = t1i;
            integral_stop = t1f;
        }
        double integral_gr1=0.;
        double integral_gr2=0.;
        int n_overlap_1=0;
        int n_overlap_2=0;
        if(do_integral){
            for(int samp1=0; samp1<gr1->GetN(); samp1++){
                double thisX, thisY;
                gr1->GetPoint(samp1,thisX,thisY);
                if(thisX>integral_start && thisX<integral_stop){
                    integral_gr1+=thisY*thisY;
                    n_overlap_1++;
                }
            }
            for(int samp2=0; samp2<gr2->GetN(); samp2++){
                double thisX, thisY;
                gr2->GetPoint(samp2, thisX, thisY);
                thisX+=lag;
                if(thisX>integral_start && thisX<integral_stop){
                    integral_gr2+=thisY*thisY;
                    n_overlap_2++;
                }
            }
            if(integral_gr1>0. && integral_gr2>0. && n_overlap_1>1){
                corrval*=1./(sqrt(n_overlap_1)*RMS1*RMS2);
            }
            else{
                corrval=0.;
            }
            corr->SetPoint(corrsamp,lag,corrval);
        }
        else{
            corr->SetPoint(corrsamp,lag,0.);
        }
    }
    return corr;
}
