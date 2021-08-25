#include "TMath.h"
#include "RayTraceCorrelator.h"
#include "FFTtools.h"

double RayTraceCorrelator::fastEvalForEvenSampling(TGraph * grIn, double xvalue) {
    Int_t numPoints = grIn -> GetN();
    if (numPoints < 2) return 0;
    Double_t * xVals = grIn -> GetX();
    Double_t * yVals = grIn -> GetY();
    Double_t dx = xVals[1] - xVals[0];
    if (dx <= 0) return 0;

    Int_t p0 = Int_t((xvalue - xVals[0]) / dx);
    if (p0 < 0) p0 = 0;
    if (p0 >= numPoints) p0 = numPoints - 2;
    return FFTtools::simpleInterploate(xVals[p0], yVals[p0], xVals[p0 + 1], yVals[p0 + 1], xvalue);
}

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
