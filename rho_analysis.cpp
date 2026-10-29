
#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"

using namespace std;

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

        track(float eta_in, float pT_in, float pTError_in, float phi_in, int charge_in, bool highPurity_in, float dz_in, float dzError_in, float dxy_in, float dxyError_in) {
            eta = eta_in;
            pT = pT_in;
            pTError = pTError_in;
            phi = phi_in;
            charge = charge_in;
            highPurity = highPurity_in;

            dz = dz_in;
            dzError = dzError_in;
            dxy = dxy_in;
            dxyError = dxyError_in;

            theta = 2*atan(exp(-eta));
            ptot = pT/sin(theta);
            pL = ptot*tanh(eta);

        }

        void set_mass(float mass_in) {
            m = mass_in;
            E = sqrt(ptot*ptot + m*m);
        }

        bool checkIsGood() {
            isGood = (highPurity == 1) &&
                (pTError/pT < 0.1) &&
                (dz/dzError < 3.0) &&
                (dxy/dxyError < 3.0) &&
                (eta < 2.4) &&
                (pT > 0.25);
            return isGood;
        }
};

track reconstruct(track track1, track track2) {
    float px = cos(track1.phi)*track1.pT + cos(track2.phi)*track2.pT;
    float py = sin(track1.phi)*track1.pT + sin(track2.phi)*track2.pT;
    float pT = sqrt(px*px + py*py);
    float pL = track1.pL + track2.pL;
    float ptot = sqrt(pT*pT + pL*pL);
    float eta = atanh(pL/ptot);
    int charge = track1.charge + track2.charge;
    float E = track1.E + track2.E;
    float m = sqrt(E*E - ptot*ptot);
    float phi = atan2(py, px);

    track reconstructed_track = track(eta, pT, -1, phi, charge, -1, -1, -1, -1, -1);
    reconstructed_track.set_mass(m);

    return reconstructed_track;
}

int main() {

    cout << "starting" << endl;

    TFile* FSC_file(TFile::Open("data/FSC_data_0_cut.root"));
    TFile* ZDC_file(TFile::Open("data/ZDC_data_0_cut.root"));
    TFile* Tracker_file(TFile::Open("data/Tracker_data_0_cut.root"));
    TFile* HF_file(TFile::Open("data/HF_data_0_cut.root"));
    
    /*
    TFile* FSC_file(TFile::Open("data/FSC_data_0.root"));
    TFile* ZDC_file(TFile::Open("data/ZDC_data_0.root"));
    TFile* Tracker_file(TFile::Open("data/Tracker_data_0.root"));
    TFile* HF_file(TFile::Open("data/HF_data_0.root"));
    */

    //TTree* tree(Tracker_file->Get<TTree>("trackAnalyzer/trackTree"));
    //tree->Print();
    //return 0;

    TTreeReader FSC_reader("FSCAnalyzerHC/fscdigi", FSC_file);
    TTreeReader ZDC_reader("zdcanalyzer/zdcdigi", ZDC_file);
    TTreeReader ZDC_rechit_reader("zdcanalyzer/zdcrechit", ZDC_file);
    TTreeReader Tracker_reader("trackAnalyzer/trackTree", Tracker_file);
    TTreeReader HF_reader("rechitanalyzer/hf", HF_file);

    vector<TTreeReaderArray<Float_t>> FSC_charges;
    for (int i=0; i<6; i++) {
        string branch = "chargefCTs" + to_string(i);
        FSC_charges.emplace_back(FSC_reader, branch.c_str());
    }

    TTreeReaderValue<Float_t> ZDC_sumMinus(ZDC_rechit_reader, "sumMinus");;
    TTreeReaderValue<Float_t> ZDC_sumPlus(ZDC_rechit_reader, "sumPlus");;

    TTreeReaderValue<Int_t> nTrk(Tracker_reader, "nTrk");

    TTreeReaderArray<float> trkEta(Tracker_reader, "trkEta");
    TTreeReaderArray<float> trkPt(Tracker_reader, "trkPt");
    TTreeReaderArray<float> trkPtError(Tracker_reader, "trkPtError");
    TTreeReaderArray<float> trkPhi(Tracker_reader, "trkPhi");
    TTreeReaderArray<char> trkCharge(Tracker_reader, "trkCharge");
    TTreeReaderArray<bool> highPurity(Tracker_reader, "highPurity");

    TTreeReaderArray<float> trkDxy(Tracker_reader, "trkDxyAssociatedVtx");
    TTreeReaderArray<float> trkDxyErr(Tracker_reader, "trkDxyErrAssociatedVtx");
    TTreeReaderArray<float> trkDz(Tracker_reader, "trkDzAssociatedVtx");
    TTreeReaderArray<float> trkDzErr(Tracker_reader, "trkDzErrAssociatedVtx");


    TTreeReaderArray<float> HF_e(HF_reader, "e");
    TTreeReaderArray<float> HF_eta(HF_reader, "eta");


    TH1F* rho_mass_hist = new TH1F("rho_mass_hist", "rho_mass_hist", 100, 0, 2);

    const float pion_mass = 0.13957039;

    int count = 0;
    const int max_count = -1;
    const int chosen_ts = 2;
    while(FSC_reader.Next()) {

        int res = ZDC_rechit_reader.Next();
        if (res == 0) {
            throw exception();
        }
        res = Tracker_reader.Next();
        if (res == 0) {
            throw exception();
        }
        res = HF_reader.Next();
        if (res == 0) {
            throw exception();
        }

        float HFplus = 0;
        float HFminus = 0;

        for (int i=0; i<HF_e.GetSize(); i++) {
            if (HF_eta[i] > 0) {
                HFplus += HF_e[i];
            }
            if (HF_eta[i] < 0) {
                HFminus += HF_e[i];
            }
        }

        if (HFplus > 9.2) {
            cout << "HF+" << endl;
            continue;
        }

        if (HFminus > 8.6) {
            cout << "HF-" << endl;
            continue;
        }

        if (*nTrk < 2) {
            cout << "nTrk" << endl;
            continue;
        }

        if (*nTrk>10) {continue;}

        vector<track> tracks = {};
        for (int i=0; i<*nTrk; i++) {
            int charge = (int)trkCharge[i];
            tracks.push_back(track(trkEta[i], trkPt[i], trkPtError[i], trkPhi[i], charge, highPurity[i], trkDz[i], trkDzErr[i], trkDxy[i], trkDxyErr[i]));
        }

        vector<track> goodTracks = {};
        for (int i=0; i<tracks.size(); i++) {
            tracks[i].set_mass(pion_mass);
            bool isGood = tracks[i].checkIsGood();
            if (isGood) {
                goodTracks.push_back(tracks[i]);
            }
        }

        if (goodTracks.size() != 2) {
            cout << "goodTracks: " << goodTracks.size() << endl;
            continue;
        }
        if (goodTracks[0].charge == goodTracks[1].charge) {
            cout << "charge" << endl;
            continue;
        }

        track rho_track = reconstruct(goodTracks[0], goodTracks[1]);

        if (rho_track.pT > 0.2) {
            cout << "rho pT" << endl;
            continue;
        }

        if (abs(rho_track.eta) > 2.0) {
            cout << "rho eta" << endl;
            continue;
        }

        cout << "filling" << endl;
        cout << rho_track.m << endl;
        rho_mass_hist->Fill(rho_track.m);
    }

    TCanvas* rho_mass_canvas = new TCanvas("rho_mass_canvas", "reconstructed rho mass", 1000, 800);

    rho_mass_hist->GetXaxis()->SetTitle("rho mass [GeV]");
    rho_mass_hist->GetYaxis()->SetTitle("events");
    rho_mass_hist->SetTitle("reconstructed rho mass");

    rho_mass_hist->Draw("");

    rho_mass_canvas->Update();

    TLine* low_rho_mass = new TLine(0.65, 0, 0.65, rho_mass_canvas->GetUymax());
    low_rho_mass->SetLineWidth(2);
    low_rho_mass->Draw("SAME");

    TLine* high_rho_mass = new TLine(1.0, 0, 1.0, rho_mass_canvas->GetUymax());
    high_rho_mass->SetLineWidth(2);
    high_rho_mass->Draw("SAME");

    rho_mass_canvas->Print("figures/rho/tho_mass.png");

    cout << "finished" << endl;

    return 0;

}