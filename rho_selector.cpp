
#include <cmath>

#include "rho_selector.h"

using namespace std;

track::track() {
    eta = -1;
    pT = -1;
    pTError = -1;
    phi = -1;
    charge = -1;
    highPurity = -1;

    dz = -1;
    dzError = -1;
    dxy = -1;
    dxyError = -1;

    theta = -1;
    ptot = -1;
    pL = -1;
}

track::track(float eta_in, float pT_in, float pTError_in, float phi_in, int charge_in, bool highPurity_in, float dz_in, float dzError_in, float dxy_in, float dxyError_in) {
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

void track::set_mass(float mass_in) {
    m = mass_in;
    E = sqrt(ptot*ptot + m*m);
}

bool track::checkIsGood() {
    isGood = (highPurity == 1) &&
        (pTError/pT < 0.1) &&
        (dz/dzError < 3.0) &&
        (dxy/dxyError < 3.0) &&
        (eta < 2.4) &&
        (pT > 0.25);
    return isGood;
}


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

bool select_rho(rho_readers readers, track &rho_track, bool print) {
    float HFplus = 0;
    float HFminus = 0;

    for (int i=0; i<readers.HF_e->GetSize(); i++) {
        if (readers.HF_eta[0][i] > 0) {
            HFplus += readers.HF_e[0][i];
        }
        if (readers.HF_eta[0][i] < 0) {
            HFminus += readers.HF_e[0][i];
        }
    }

    if (HFplus > 9.2) {
        if (print) {cout << "HF+" << endl;}
        return false;
    }

    if (HFminus > 8.6) {
        if (print) {cout << "HF-" << endl;}
        return false;
    }

    if (*(*readers.nTrk) < 2) {
        if (print) {cout << "nTrk" << endl;}
        return false;
    }

    vector<track> tracks = {};
    for (int i=0; i<*(*readers.nTrk); i++) {
        int charge = (int)readers.trkCharge[0][i];
        tracks.push_back(track(readers.trkEta[0][i], readers.trkPt[0][i], readers.trkPtError[0][i], readers.trkPhi[0][i], charge, readers.highPurity[0][i], readers.trkDz[0][i], readers.trkDzErr[0][i], readers.trkDxy[0][i], readers.trkDxyErr[0][i]));
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
        if (print) {cout << "goodTracks: " << goodTracks.size() << endl;}
        return false;
    }
    if (goodTracks[0].charge == goodTracks[1].charge) {
        if (print) {cout << "charge" << endl;}
        return false;
    }

    rho_track = reconstruct(goodTracks[0], goodTracks[1]);

    if (rho_track.pT > 0.2) {
        if (print) {cout << "rho pT" << endl;}
        return false;
    }

    if (abs(rho_track.eta) > 2.0) {
        if (print) {cout << "rho eta" << endl;}
        return false;
    }

    return true;
}
