
#ifndef RHO_SELECTOR_H
#define RHO_SELECTOR_H

#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"

using namespace std;

const float pion_mass = 0.13957039;

class track {
    public:
        float eta;
        float pT;
        float pTError;
        float phi;
        int charge;
        bool highPurity;

        float dz;
        float dzError;
        float dxy;
        float dxyError;

        float theta;
        float pL;
        float ptot;
        float m;
        float E;
        bool isGood;

        track();

        track(float eta_in, float pT_in, float pTError_in, float phi_in, int charge_in, bool highPurity_in, float dz_in, float dzError_in, float dxy_in, float dxyError_in);

        void set_mass(float mass_in);

        bool checkIsGood();
};

track reconstruct(track track1, track track2);

struct rho_readers {
    TTreeReaderValue<Int_t> *nTrk;

    TTreeReaderArray<float> *trkEta;
    TTreeReaderArray<float> *trkPt;
    TTreeReaderArray<float> *trkPtError;
    TTreeReaderArray<float> *trkPhi;
    TTreeReaderArray<char> *trkCharge;
    TTreeReaderArray<bool> *highPurity;

    TTreeReaderArray<float> *trkDxy;
    TTreeReaderArray<float> *trkDxyErr;
    TTreeReaderArray<float> *trkDz;
    TTreeReaderArray<float> *trkDzErr;

    TTreeReaderArray<float> *HF_e;
    TTreeReaderArray<float> *HF_eta;

    rho_readers() {}

    rho_readers(
        TTreeReaderValue<Int_t> *nTrk_in,

        TTreeReaderArray<float> *trkEta_in,
        TTreeReaderArray<float> *trkPt_in,
        TTreeReaderArray<float> *trkPtError_in,
        TTreeReaderArray<float> *trkPhi_in,
        TTreeReaderArray<char> *trkCharge_in,
        TTreeReaderArray<bool> *highPurity_in,

        TTreeReaderArray<float> *trkDxy_in,
        TTreeReaderArray<float> *trkDxyErr_in,
        TTreeReaderArray<float> *trkDz_in,
        TTreeReaderArray<float> *trkDzErr_in,

        TTreeReaderArray<float> *HF_e_in,
        TTreeReaderArray<float> *HF_eta_in
    ) {
        nTrk = nTrk_in;

        trkEta = trkEta_in;
        trkPt = trkPt_in;
        trkPtError = trkPtError_in;
        trkPhi = trkPhi_in;
        trkCharge = trkCharge_in;
        highPurity = highPurity_in;

        trkDxy = trkDxy_in;
        trkDxyErr = trkDxyErr_in;
        trkDz = trkDz_in;
        trkDzErr = trkDzErr_in;

        HF_e = HF_e_in;
        HF_eta = HF_eta_in;
    }
};


bool select_rho(rho_readers readers, track &rho_track, bool print=false);

#endif